#include <iostream>
#include <sys/sysctl.h>
#include <libproc.h>
#include <mach/mach_host.h>


int main() {
    int mib[4];
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_ALL;
    mib[3] = 0;

    size_t numBytes;
    sysctl(mib, 4, NULL, &numBytes, NULL, 0);

    auto *procList = (struct kinfo_proc*) malloc(numBytes);
    sysctl(mib, 4, procList, &numBytes, NULL, 0);

    int numProcs = numBytes / sizeof(struct kinfo_proc);
    std::cout << "Number of processes: " << numProcs << std::endl;

    for (int i = 0; i < numProcs; ++i) {
        pid_t pid = procList[i].kp_proc.p_pid;

        struct proc_taskinfo task_info;
        int task_info_size = sizeof(task_info);
        if (proc_pidinfo(pid, PROC_PIDTASKINFO, 0, &task_info, task_info_size) <= 0) {
            continue;
        }

        host_cpu_load_info_data_t cpu_load;
        mach_msg_type_number_t cpu_load_size = HOST_CPU_LOAD_INFO_COUNT;
        if (host_statistics64(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info64_t)&cpu_load, &cpu_load_size) != KERN_SUCCESS) {
            continue;
        }
        std::cout << "Process name: " << procList[i].kp_proc.p_comm << std::endl;
        std::cout << "Process ID: " << pid << std::endl;

        uint64_t total_ticks = cpu_load.cpu_ticks[CPU_STATE_USER] + cpu_load.cpu_ticks[CPU_STATE_SYSTEM] + cpu_load.cpu_ticks[CPU_STATE_NICE] + cpu_load.cpu_ticks[CPU_STATE_IDLE];

        double cpu_usage = (((double)task_info.pti_total_user + (double)task_info.pti_total_system) / total_ticks) / 100;

        std::cout << "Process " << pid << " CPU usage: " << cpu_usage << "%" << std::endl << std::endl;
    }

    return 0;
}
