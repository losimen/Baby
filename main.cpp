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

    std::sort(processList.begin(), processList.end(), [](const Process& a, const Process& b) {
        return a.cpuUsage > b.cpuUsage;
    });

    std::vector<TableInfo> tableData = {
            {"Abc", "Doe", 40},
            {"Bac", "Doe", 35},
            {"Cba", "Smith", 55},
            {"Bac", "Smith", 60},
            {"Abc", "Smith", 50},
            {"Cba", "Doe", 45},
            {"Abc", "Jones", 30},
            {"Bac", "Jones", 25},
            {"Cba", "Jones", 20},
    };

    UITable table(tableData);
    table.drawTable();
    table.waitForInput();

    return 0;
}