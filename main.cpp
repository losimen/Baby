#include <iostream>
#include <sys/sysctl.h>
#include <cstring>
#include <libproc.h>
#include <mach/task_info.h>
#include <mach/task.h>
#include <mach/mach_init.h>
#include <chrono>
#include <thread>


int main() {
    int mib[4];
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_ALL;
    mib[3] = 0;

    size_t numBytes;
    sysctl(mib, 4, NULL, &numBytes, NULL, 0);

    struct kinfo_proc *procList = (struct kinfo_proc*) malloc(numBytes);
    sysctl(mib, 4, procList, &numBytes, NULL, 0);

    int numProcs = numBytes / sizeof(struct kinfo_proc);
    std::cout << "Number of processes: " << numProcs << std::endl;

    for (int i = 0; i < numProcs; ++i) {
        pid_t pid = procList[i].kp_proc.p_pid;

        std::cout << "Process name: " << procList[i].kp_proc.p_comm << std::endl;
        std::cout << "Process ID: " << pid << std::endl;


        auto get_cpu_time = [](){
            auto start = std::chrono::high_resolution_clock::now();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            auto end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double> elapsed = end - start;
            return elapsed.count();
        };

        rusage usage;
        std::cout << "elsf" << RUSAGE_SELF << std::endl;
        getrusage(RUSAGE_SELF, &usage);

        double user_time = (double)usage.ru_utime.tv_sec + (double)usage.ru_utime.tv_usec / 1000000;
        double sys_time = (double)usage.ru_stime.tv_sec + (double)usage.ru_stime.tv_usec / 1000000;
        double total_time = user_time + sys_time;

        double cpu_usage = total_time * 100 / get_cpu_time();
        std::cout << "CPU usage: " << cpu_usage << "%\n";
    }

    return 0;
}
