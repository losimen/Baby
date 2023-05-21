#ifndef SERVER_TYPES_H
#define SERVER_TYPES_H

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>


struct ProcessStat {
    long unsigned int utime_ticks;
    long int cutime_ticks;
    long unsigned int stime_ticks;
    long int cstime_ticks;
    long unsigned int vsize; // virtual memory size in bytes
    long unsigned int rss; //Resident  Set  Size in bytes
    double cpuUsage;

    long unsigned int cpu_total_time;
};



struct Process {
    int PID;
    std::string name;
    double cpuUsage;

    double vSize;
    double rss;
    double memUsage;
};

struct ProcessStat {
    int PID;

    long unsigned int utime_ticks;
    long int cutime_ticks;
    long unsigned int stime_ticks;
    long int cstime_ticks;
    long unsigned int vsize; // virtual memory size in bytes
    long unsigned int rss; //Resident  Set  Size in bytes

    long unsigned int cpu_total_time;
};

typedef std::vector<Process> ProcessList;

#endif //LISTENER_H