#include "ProcessDispatcher.h"

const std::string ProcessDispatcher::PROC_DIR = "/proc";
const std::string ProcessDispatcher::PROC_STAT_FILE = "/stat";
const std::string ProcessDispatcher::PROC_STATUS_FILE = "/status";


bool ProcessDispatcher::_charStartsWith(const char *a, const char *b) {
    return strncmp(a, b, strlen(b)) == 0;
}


std::string ProcessDispatcher::_parseProcessFileLine(std::string lineToParse) {
    std::string result;
    lineToParse = lineToParse.substr(lineToParse.rfind(':')+1);

    for (auto el: lineToParse)
        if (isalpha(el))
            result += el;

    return result;
}


bool ProcessDispatcher::isProcessExists(const int PID) {
    try
    {
        ProcessDispatcher::getProcessInfo(PID);
        return true;
    }
    catch (const std::runtime_error& e)
    {
        return false;
    }
}


ProcessList ProcessDispatcher::getListOfProcesses() {
    ProcessList listOfProcesses;

    struct dirent *entry;
    DIR *dir = opendir(PROC_DIR.c_str());

    if (dir == NULL) {
        throw std::runtime_error("Cannot get process list");
    }

    while ((entry = readdir(dir)) != NULL) {
        int PID = atoi(entry->d_name);
        if (PID != 0) {
            Process processInfo = getProcessInfo(PID);
            listOfProcesses.push_back(processInfo);
        }
    }

    closedir(dir);
    return listOfProcesses;
}


Process ProcessDispatcher::getProcessInfo(const int PID) {
    const int BUFFER_SIZE = 1000;
    ssize_t read;

    const std::string filePath = PROC_DIR + "/" + std::to_string(PID) + PROC_STAT_FILE;
    Process process;

    std::ifstream procStatFile(filePath);

    if (!procStatFile.is_open())
        throw std::runtime_error("Process doesn't exist");

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
    process.name = ProcessDispatcher::getProcessName(process.PID);
    process.cpuUsage = 0;

    return process;
}


std::string ProcessDispatcher::getProcessName(const int PID)
{
    const int BUFFER_SIZE = 1000;
    char buffer[BUFFER_SIZE];
    ssize_t read;

    const std::string filePath = PROC_DIR + "/" + std::to_string(PID) + PROC_STATUS_FILE;
    FILE *procStatFile = fopen(filePath.c_str(), "r");

    if (procStatFile == nullptr)
        throw std::runtime_error("Process doesn't exist");

    while(fgets(buffer, BUFFER_SIZE, procStatFile)) {
        if (_charStartsWith(buffer, "Name"))
        {
            fclose(procStatFile);
            return _parseProcessFileLine(buffer);
        }
    }

    return "None";
}


bool ProcessDispatcher::killProcess(const int PID) {
    if (!ProcessDispatcher::isProcessExists(PID)) {
        return false;
    }

    kill(PID, SIGTERM);
    return true;
}


