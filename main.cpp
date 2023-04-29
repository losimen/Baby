//Last modified: 18/11/12 19:13:35(CET) by Fabian Holler
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <chrono>

#include "ProcessDispatcher.h"

struct pstat {
    long unsigned int utime_ticks;
    long int cutime_ticks;
    long unsigned int stime_ticks;
    long int cstime_ticks;
    long unsigned int vsize; // virtual memory size in bytes
    long unsigned int rss; //Resident  Set  Size in bytes

    long unsigned int cpu_total_time;
};

/*
 * read /proc data into the passed struct pstat
 * returns 0 on success, -1 on error
*/
int get_usage(const pid_t pid, struct pstat* result) {
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
    bzero(result, sizeof(struct pstat));
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
void calc_cpu_usage_pct(const struct pstat* cur_usage,
                        const struct pstat* last_usage,
                        double* ucpu_usage, double* scpu_usage)
{
    const long unsigned int total_time_diff = cur_usage->cpu_total_time -
                                              last_usage->cpu_total_time;

    *ucpu_usage = 100 * (((cur_usage->utime_ticks + cur_usage->cutime_ticks)
                          - (last_usage->utime_ticks + last_usage->cutime_ticks))
                         / (double) total_time_diff);

    *scpu_usage = 100 * ((((cur_usage->stime_ticks + cur_usage->cstime_ticks)
                           - (last_usage->stime_ticks + last_usage->cstime_ticks))) /
                         (double) total_time_diff);
}

int main()
{
    ProcessDispatcher processDispatcher;
    auto listOf = ProcessDispatcher::getListOfProcesses();

    for (auto &process: listOf)
    {
        pstat startUsage;
        pstat endUsage;

        double ucpu, scpu;

        if (process.PID != 4420)
        {
            continue;
        }
        get_usage(process.PID, &startUsage);
        std::this_thread::sleep_for(std::chrono::milliseconds(7));
        get_usage(process.PID, &endUsage);

        calc_cpu_usage_pct(&endUsage, &startUsage, &ucpu, &scpu);
        std::cout << process.PID << " " << process.name << " | " << ucpu << " " << scpu << std::endl;
    }
//    get_usage();

    return 0;
}