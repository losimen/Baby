#include <sstream>
#include <algorithm>

#include "SystemInfo.h"
#include "UiTable.h"


int main()
{
    using std::cout;
    using std::endl;

    SystemInfo systemInfo;
    auto &processList = systemInfo.getListOfProcesses();

    //    std::sort(processList.begin(), processList.end(), [](const Process& a, const Process& b) {
//        return a.cpuUsage > b.cpuUsage;
//    });

    UITable table(processList);
    table.drawTable();
    table.waitForInput();

    return 0;
}