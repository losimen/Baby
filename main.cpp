#include <iostream>
#include <sys/sysctl.h>
#include <cstring>
#include <libproc.h>
#include <mach/task_info.h>
#include <mach/task.h>
#include <mach/mach_init.h>

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

    mach_port_t machPort = mach_host_self();
    natural_t processorCount;
    processor_info_array_t processorInfo;
    mach_msg_type_number_t processorInfoCount;

    task_info_data_t taskInfo;
    mach_msg_type_number_t taskInfoCount = TASK_INFO_MAX;

    for (int i = 0; i < numProcs; ++i) {
        std::cout << "Process name: " << procList[i].kp_proc.p_comm << std::endl;
        std::cout << "Process ID: " << procList[i].kp_proc.p_pid << std::endl;

        pid_t pid = procList[i].kp_proc.p_pid;
        int ret = task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&taskInfo, &taskInfoCount);
        if (ret != KERN_SUCCESS) {
            std::cerr << "Failed to get basic task info for process " << pid << std::endl;
            break;
        }

        double cpuUsage = ((double)taskInfo[0] / (double)TH_USAGE_SCALE) * 100.0;
        std::cout << "CPU usage: " << cpuUsage << "%" << std::endl;

        std::cout << std::endl;
    }

    return 0;
}