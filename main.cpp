#include <sstream>
#include <algorithm>

#include "SystemInfo.h"
#include "UiTable.h"

//
//void writeProcessListToFile(ProcessList &processList)
//{
//    std::ofstream file;
//    file.open("processes.txt");
//
//    for (auto &process : processList) {
//        file << process.PID << " " << process.name << " " << process.cpuUsage << " " << process.memUsage << std::endl;
//    }
//
//    file.close();
//}

void readProcessListFromFile(ProcessList &processList)
{
    std::ifstream file;
    file.open("processes-t.txt");

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        Process process;
        iss >> process.PID >> process.name >> process.cpuUsage >> process.memUsage;
        processList.push_back(process);
    }

    file.close();
}


int main()
{
    using std::cout;
    using std::endl;

    SystemInfo systemInfo;
    ProcessList processList;

    readProcessListFromFile(processList);

    //    std::sort(processList.begin(), processList.end(), [](const Process& a, const Process& b) {
//        return a.cpuUsage > b.cpuUsage;
//    });
//    std::cout << "Process list len = " << processList.size() << std::endl;



    UITable table(processList);
    table.drawTable();
    table.waitForInput();

    return 0;
}