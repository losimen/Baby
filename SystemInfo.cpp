#include "SystemInfo.h"

const std::string SystemInfo::PROC_DIR = "/proc";
const std::string SystemInfo::PROC_STAT_FILE = "/stat";
const std::string SystemInfo::PROC_STATUS_FILE = "/status";


bool SystemInfo::charStartsWith(const char *a, const char *b) {
    return strncmp(a, b, strlen(b)) == 0;
}


std::string SystemInfo::parseProcessFileLine(std::string lineToParse) {
    std::string result;
    lineToParse = lineToParse.substr(lineToParse.rfind(':')+1);

    for (auto el: lineToParse)
        if (isalpha(el))
            result += el;

    return result;
}


bool SystemInfo::isProcessExists(const int PID) {
    try
    {
        SystemInfo::getProcessInfo(PID);
        return true;
    }
    catch (const std::runtime_error& e)
    {
        return false;
    }
}


ProcessList SystemInfo::getListOfProcesses() {
    ProcessList processList;
    struct dirent *entry;
    DIR *dir = opendir(PROC_DIR.c_str());
    processList.clear();

    while ((entry = readdir(dir)) != NULL) {
        int PID = atoi(entry->d_name);
        if (PID != 0) {
            Process processInfo = getProcessInfo(PID);
            processList.push_back(processInfo);
        }
    }

    calcCpuUsage(processList);
    calcMemUsage(processList);

    return processList;
}


Process SystemInfo::getProcessInfo(const int PID) {
    const std::string filePath = PROC_DIR + "/" + std::to_string(PID) + PROC_STAT_FILE;
    Process process;

    std::ifstream procStatFile(filePath);
    std::string content( (std::istreambuf_iterator<char>(procStatFile) ),
                         (std::istreambuf_iterator<char>()) );
    std::string delimiter = " ";

    size_t pos;
    std::vector<std::string> tokens {};

    while ((pos = content.find(delimiter)) != std::string::npos) {
        tokens.push_back(content.substr(0, pos));
        content.erase(0, pos + delimiter.length());
    }

    process.PID = PID;
    process.name = SystemInfo::getProcessName(process.PID);

    return process;
}


std::string SystemInfo::getProcessName(const int PID)
{
    const int BUFFER_SIZE = 1000;
    char buffer[BUFFER_SIZE];

    const std::string filePath = PROC_DIR + "/" + std::to_string(PID) + PROC_STATUS_FILE;
    FILE *procStatFile = fopen(filePath.c_str(), "r");

    if (procStatFile == nullptr)
        return "None";

    while(fgets(buffer, BUFFER_SIZE, procStatFile)) {
        if (charStartsWith(buffer, "Name"))
        {
            fclose(procStatFile);
            return parseProcessFileLine(buffer);
        }
    }

    return "None";
}


bool SystemInfo::killProcess(const int PID) {
    if (!SystemInfo::isProcessExists(PID))
    {
        return false;
    }

    kill(PID, SIGTERM);
    return true;
}

void SystemInfo::calcCpuUsage(ProcessList &processList) {
    std::vector<ProcessStat> usageStat;
    for (auto &process: processList)
    {
        ProcessStat usage {};

        getUsage(process.PID, &usage);
        usageStat.emplace_back(usage);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));


    for (auto &stat: usageStat)
    {
        auto process = std::find_if(processList.begin(), processList.end(), [&](const Process &p) {
            return p.PID == stat.PID;
        });

        ProcessStat currentUsage {};
        getUsage(stat.PID, &currentUsage);

        double ucpu, scpu;

        calcCpuUsagePct(&currentUsage, &stat, &ucpu, &scpu);
        process->cpuUsage = std::max(ucpu, scpu);
    }
}

int SystemInfo::getUsage(const pid_t pid, struct ProcessStat *result) {
    //convert  pid to string
    char pid_s[20];
    snprintf(pid_s, sizeof(pid_s), "%d", pid);
    char stat_filepath[30] = "/proc/"; strncat(stat_filepath, pid_s,
                                               sizeof(stat_filepath) - strlen(stat_filepath) -1);
    strncat(stat_filepath, "/stat", sizeof(stat_filepath) -
                                    strlen(stat_filepath) -1);

    FILE *fpstat = fopen(stat_filepath, "r");
    if (fpstat == nullptr) {
        return -1;
    }

    //read values from /proc/pid/stat
    bzero(result, sizeof(struct ProcessStat));
    long int rss_pages;
    if (fscanf(fpstat, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu"
                       "%lu %ld %ld %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %lu",
               &result->utime_ticks, &result->stime_ticks,
               &result->cutime_ticks, &result->cstime_ticks, &rss_pages) == EOF) {
        fclose(fpstat);
        return -1;
    }
    fclose(fpstat);
    result->rss = rss_pages * getpagesize();

    //read+calc cpu total time from /proc/stat
    FILE *fstat = fopen("/proc/stat", "r");
    if (fstat == nullptr) {
        return -1;
    }

    long unsigned int cpu_time[10];
    bzero(cpu_time, sizeof(cpu_time));
    if (fscanf(fstat, "%*s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
               &cpu_time[0], &cpu_time[1], &cpu_time[2], &cpu_time[3],
               &cpu_time[4], &cpu_time[5], &cpu_time[6], &cpu_time[7],
               &cpu_time[8], &cpu_time[9]) == EOF) {
        fclose(fstat);
        return -1;
    }

    fclose(fstat);

    for(unsigned long i : cpu_time)
        result->cpu_total_time += i;

    result->PID = pid;

    return 0;
}


void SystemInfo::calcCpuUsagePct(const struct ProcessStat *cur_usage, const struct ProcessStat *last_usage,
                                 double *ucpu_usage, double *scpu_usage) {
    const long unsigned int total_time_diff = cur_usage->cpu_total_time -
                                              last_usage->cpu_total_time;

    *ucpu_usage = 100.f * ((double)((cur_usage->utime_ticks + cur_usage->cutime_ticks)
                                    - (last_usage->utime_ticks + last_usage->cutime_ticks))
                           / (double) total_time_diff);

    *scpu_usage = 100.f * ((double)(((cur_usage->stime_ticks + cur_usage->cstime_ticks)
                                     - (last_usage->stime_ticks + last_usage->cstime_ticks))) /
                           (double) total_time_diff);
}

void SystemInfo::processMemUsage(int pId, double &vm_usage, double &resident_set)
{
    using std::ios_base;
    using std::ifstream;
    using std::string;

    vm_usage     = 0.0;
    resident_set = 0.0;

    string a("/proc/" + std::to_string(pId) + "/stat");
    ifstream stat_stream(a,ios_base::in);

    string pid, comm, state, ppid, pgrp, session, tty_nr;
    string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    string utime, stime, cutime, cstime, priority, nice;
    string O, itrealvalue, starttime;

    unsigned long vsize;
    long rss;

    stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
                >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
                >> utime >> stime >> cutime >> cstime >> priority >> nice
                >> O >> itrealvalue >> starttime >> vsize >> rss;

    stat_stream.close();

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    vm_usage     = ((double)vsize / 1024.0)/100;
    resident_set = (float)(rss * page_size_kb)/100;
}

int SystemInfo::getTotalMemKb() {
    // Get total physical memory size (in KB) from /proc/meminfo
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    int totalMemKb = 0;
    while (std::getline(meminfo, line)) {
        if (line.compare(0, 9, "MemTotal:") == 0) {
            std::istringstream iss(line.substr(9));
            iss >> totalMemKb;
            break;
        }
    }
    meminfo.close();

    return totalMemKb;
}

void SystemInfo::calcMemUsage(ProcessList &processList)
{
    int totalMemKb = getTotalMemKb();
    for (auto &process: processList)
    {
        double vm, rss;

        processMemUsage(process.PID, vm, rss);

        process.vSize = vm;
        process.rss = rss;
        process.memUsage = (rss / totalMemKb) * 10000;
    }
}

float SystemInfo::getCpuLoad(unsigned int cpu_usage_delay)
{
    std::string line;
    size_t substr_start = 0;
    size_t substr_len;

    unsigned long long stats[CP_STATES];

    std::ifstream stat_file( "/proc/stat" );
    getline( stat_file, line );
    stat_file.close();

    substr_len = line.find_first_of( " ", 3 );
    for( unsigned i=0; i < 4; i++ )
    {
        substr_start = line.find_first_not_of( " ", substr_len );
        substr_len   = line.find_first_of( " ", substr_start );
        stats[i] = std::stoll( line.substr( substr_start, substr_len ) );
    }

    usleep( cpu_usage_delay );

    stat_file.open( "/proc/stat" );
    getline( stat_file, line );
    stat_file.close();

    substr_len = line.find_first_of( " ", 3 );
    for( unsigned i=0; i < 4; i++ )
    {
        substr_start = line.find_first_not_of( " ", substr_len );
        substr_len   = line.find_first_of    ( " ", substr_start );
        stats[i] = std::stoll( line.substr( substr_start, substr_len ) ) - stats[i];
    }

    return static_cast<float>(
                   stats[CP_USER] + stats[CP_NICE] + stats[CP_SYS]
           ) / static_cast<float>(
                   stats[CP_USER] + stats[CP_NICE] + stats[CP_SYS] + stats[CP_IDLE]
           ) * 100.0;
}

void SystemInfo::getMemStatus(MemoryStatus &status)
{
    std::string line;
    std::string substr;
    size_t substr_start;
    size_t substr_len;

    unsigned int total_mem;
    unsigned int used_mem;

    std::ifstream memory_info("/proc/meminfo");

    while( std::getline( memory_info, line ) )
    {
        substr_start = 0;
        substr_len = line.find_first_of( ':' );
        substr = line.substr( substr_start, substr_len );
        substr_start = line.find_first_not_of( " ", substr_len + 1 );
        substr_len = line.find_first_of( 'k' ) - substr_start;
        if( substr.compare( "MemTotal" ) == 0 )
        {
            // get total memory
            total_mem = stoi( line.substr( substr_start, substr_len ) );
        }
        else if( substr.compare( "MemFree" ) == 0 )
        {
            used_mem = total_mem - stoi( line.substr( substr_start, substr_len ) );
        }
        else if( substr.compare( "Shmem" ) == 0 )
        {
            used_mem += stoi( line.substr( substr_start, substr_len ) );
        }
        else if( substr.compare( "Buffers" ) == 0 ||
                 substr.compare( "Cached" ) == 0  ||
                 substr.compare( "SReclaimable" ) == 0 )
        {
            used_mem -= stoi( line.substr( substr_start, substr_len ) );
        }
    }

    status.usedMem = convert_unit(static_cast< float >( used_mem ), MEGABYTES, KILOBYTES);
    status.totalMem = convert_unit(static_cast< float >( total_mem ), MEGABYTES, KILOBYTES);
    status.memUsage = (status.usedMem / status.totalMem) * 100.0;
}
