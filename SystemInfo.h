#ifndef PROCESS_DISPATCHER_H
#define PROCESS_DISPATCHER_H

#include <string>
#include <iostream>
#include <vector>
#include <signal.h>
#include <cstring>
#include <dirent.h>
#include <sys/types.h>
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <tuple>
#include <sstream>
#include <algorithm>

#include "Types.h"


class SystemInfo {
private:
    ProcessList processList;

    static bool _charStartsWith(const char *a, const char *b);
    static std::string _parseProcessFileLine(std::string lineToParse);

    int getUsage(const pid_t pid, struct ProcessStat* result);
    void calcCpuUsagePct(const struct ProcessStat* cur_usage,
                         const struct ProcessStat* last_usage,
                         double* ucpu_usage, double* scpu_usage);

    void processMemUsage(int pId, double& vm_usage, double& resident_set);
    int getTotalMemKb();

    void calcCpuUsage();
    void calcMemUsage();

    static const std::string PROC_DIR;
    static const std::string PROC_STAT_FILE;
    static const std::string PROC_STATUS_FILE;

public:
    ProcessList &getListOfProcesses();

    Process getProcessInfo(const int PID);
    std::string getProcessName(const int PID);
    bool isProcessExists(const int PID);
    bool killProcess(const int PID);
};

#endif //PROCESS_DISPATCHER_H