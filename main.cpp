#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <map>
#include <tuple>

#include "ProcessDispatcher.h"

struct ProcessStat {
    long unsigned int utime_ticks;
    long int cutime_ticks;
    long unsigned int stime_ticks;
    long int cstime_ticks;
    long unsigned int vsize; // virtual memory size in bytes
    long unsigned int rss; //Resident  Set  Size in bytes

    long unsigned int cpu_total_time;
};

/*
 * read /proc data into the passed struct ProcessStat
 * returns 0 on success, -1 on error
*/
int get_usage(const pid_t pid, struct ProcessStat* result) {
    //convert  pid to string
    char pid_s[20];
    snprintf(pid_s, sizeof(pid_s), "%d", pid);
    char stat_filepath[30] = "/proc/"; strncat(stat_filepath, pid_s,
                                               sizeof(stat_filepath) - strlen(stat_filepath) -1);
    strncat(stat_filepath, "/stat", sizeof(stat_filepath) -
                                    strlen(stat_filepath) -1);

    FILE *fpstat = fopen(stat_filepath, "r");
    if (fpstat == NULL) {
        perror("FOPEN ERROR ");
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
    if (fstat == NULL) {
        perror("FOPEN ERROR ");
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

    for(int i=0; i < 10;i++)
        result->cpu_total_time += cpu_time[i];

    return 0;
}


/*
* calculates the elapsed CPU usage between 2 measuring points. in percent
*/
void calc_cpu_usage_pct(const struct ProcessStat* cur_usage,
                        const struct ProcessStat* last_usage,
                        double* ucpu_usage, double* scpu_usage)
{
    const long unsigned int total_time_diff = cur_usage->cpu_total_time -
                                              last_usage->cpu_total_time;

    *ucpu_usage = 100.f * ((double)((cur_usage->utime_ticks + cur_usage->cutime_ticks)
                          - (last_usage->utime_ticks + last_usage->cutime_ticks))
                         / (double) total_time_diff);

    *scpu_usage = 100.f * ((double)(((cur_usage->stime_ticks + cur_usage->cstime_ticks)
                           - (last_usage->stime_ticks + last_usage->cstime_ticks))) /
                         (double) total_time_diff);
}


void calcCpuUsage()
{
    ProcessDispatcher processDispatcher;
    auto listOf = ProcessDispatcher::getListOfProcesses();

    std::vector<std::tuple<Process, ProcessStat>> usageStat;

    for (auto &process: listOf)
    {
        ProcessStat usage {};

        get_usage(process.PID, &usage);
        usageStat.emplace_back(process, usage);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::vector<Process> newProcessList;

    for (auto &stat: usageStat)
    {
        auto process = std::get<0>(stat);
        auto pStat = std::get<1>(stat);

        ProcessStat currentUsage {};
        get_usage(process.PID, &currentUsage);

        double ucpu, scpu;

        calc_cpu_usage_pct(&currentUsage, &pStat, &ucpu, &scpu);
        process.cpuUsage = std::max(ucpu, scpu);

        newProcessList.emplace_back(process);
    }

    std::sort(newProcessList.begin(), newProcessList.end(), [](const Process &a, const Process &b){
        return a.cpuUsage > b.cpuUsage;
    });

    for (auto &process: newProcessList)
    {
        std::cout << process.PID << "| " << process.name << "| " << process.cpuUsage << "%" << std::endl;
    }
}

void process_mem_usage(int pId, double& vm_usage, double& resident_set)
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
    vm_usage     = (vsize / 1024.0)/100;
    resident_set = (rss * page_size_kb)/100;
}

int main()
{
    using std::cout;
    using std::endl;

    double vm, rss;
    for (const auto &process: ProcessDispatcher::getListOfProcesses())
    {
        process_mem_usage(process.PID, vm, rss);
        cout << "PID: " << process.PID << " | VM: " << std::fixed << vm << "; RSS: " << rss << "\n";
    }


}
