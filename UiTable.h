//
// Created by losimen on 5/21/23.
//

#ifndef BABY_UITABLE_H
#define BABY_UITABLE_H

#include <ncurses.h>
#include <algorithm>
#include <vector>
#include <string>
#include <numeric>
#include <iostream>
#include <atomic>
#include <thread>

#include "Types.h"


class UITable
{
private:
    std::thread dataThread;

    ProcessList data;
    std::atomic<double> cpuLoad;
    std::atomic<double> memUsage;

    std::atomic<int> startOffset = 0;
    std::atomic<int> currentRow = 0;

    std::atomic<int> col = -1;
    std::vector<bool> sortDirections;
    std::atomic<int> sortCol = -1;

    WINDOW *window = nullptr;
    std::vector<std::string> headers = {"PID", "Name", "CPU %", "Mem %"};
    std::vector<int> widths = {16, 15, 10, 10};

    void drawTableData();
    void updateTableData();
    void updateTableDataThread();
    void refreshWindow();

    std::string getBar(double cpuLoad);

    void drawHeader();

    void sortData(int col, bool sortDirection);
public:
    explicit UITable();

    void drawTable();
    void waitForInput();

    ~UITable();
};



#endif //BABY_UITABLE_H
