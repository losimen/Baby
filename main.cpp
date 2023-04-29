#include <iostream>
#include "ProcessDispatcher.h"


int main()
{
    auto processList = ProcessDispatcher::getListOfProcesses();

    for (auto &process: processList)
    {
        std::cout << process.PID << " | " << process.name << std::endl << std::endl;
    }

    return 0;
}