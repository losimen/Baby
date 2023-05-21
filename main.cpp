#include <sstream>
#include <algorithm>

#include "SystemInfo.h"


int main()
{
    using std::cout;
    using std::endl;

    SystemInfo systemInfo;
    auto &processList = systemInfo.getListOfProcesses();

    std::sort(processList.begin(), processList.end(), [](const Process& a, const Process& b) {
        return a.cpuUsage > b.cpuUsage;
    });

    for (const auto &process: processList)
    {
        cout << process.PID << "| " << process.name << "| " << process.cpuUsage << "| " << std::fixed << process.memUsage << "%" << std::endl;
    }

    return 0;
}