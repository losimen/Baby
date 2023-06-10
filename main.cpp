#include <sstream>
#include <algorithm>

#include "SystemInfo.h"
#include "UiTable.h"


void writeProcessListToFile(ProcessList &processList)
{
    std::ofstream file;
    file.open("processes.txt");

    for (auto &process : processList) {
        file << process.PID << " " << process.name << " " << process.cpuUsage << " " << process.memUsage << std::endl;
    }

    file.close();
}

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
    UITable table;
    table.drawTable();
    table.waitForInput();

    return 0;
}