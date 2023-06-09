#ifndef PROCESS_DISPATCHER_H
#define PROCESS_DISPATCHER_H

#include <string>
#include <iostream>
#include <vector>
#include <csignal>
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

#define CP_USER   0
#define CP_NICE   1
#define CP_SYS    2
#define CP_IDLE 3
#define CP_STATES 4

enum BYTE_UNITS
{
    BYTES = 0,
    KILOBYTES = 1,
    MEGABYTES = 2,
    GIGABYTES = 3
};

struct MemoryStatus
{
    float usedMem;
    float totalMem;
    float memUsage;
};

template <class T>
inline T convert_unit( T num, int to, int from = BYTES)
{
    for( ; from < to; from++)
    {
        num /= 1024;
    }
    return num;
}


class SystemInfo {
private:
    static bool charStartsWith(const char *a, const char *b);
    static std::string parseProcessFileLine(std::string lineToParse);

    static int getUsage(pid_t pid, struct ProcessStat* result);
    static void calcCpuUsagePct(const struct ProcessStat* cur_usage,
                         const struct ProcessStat* last_usage,
                         double* ucpu_usage, double* scpu_usage);

    static void processMemUsage(int pId, double& vm_usage, double& resident_set);
    static int getTotalMemKb();

    static void calcCpuUsage(ProcessList &processList);
    static void calcMemUsage(ProcessList &processList);

    static std::string getProcessName(int PID);
    static Process getProcessInfo(int PID);

    static const std::string PROC_DIR;
    static const std::string PROC_STAT_FILE;
    static const std::string PROC_STATUS_FILE;

public:
    static ProcessList getListOfProcesses();

    static float getCpuLoad(unsigned cpu_usage_delay);
    static void getMemStatus(MemoryStatus & status);
    static bool killProcess(int PID);
};

#endif //PROCESS_DISPATCHER_H