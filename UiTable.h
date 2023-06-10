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

#include "Types.h"


class UITable
{
private:
    ProcessList data;
    double cpuLoad;
    double memUsage;

    int startOffset = 0;
    int currentRow = 0;

    WINDOW *window = nullptr;
    std::vector<std::string> headers = {"PID", "Name", "CPU %", "Mem %"};
    std::vector<int> widths = {10, 15, 10, 7};

    void drawTableData();

    std::string getBar(double cpuLoad);

    void drawHeader(int col, bool sortDirection);

    void sortData(int col, bool sortDirection);
public:
    explicit UITable();

    void drawTable();
    void waitForInput();

    ~UITable();
};



#endif //BABY_UITABLE_H
