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
    int startOffset = 0;

    WINDOW *window = nullptr;
    std::vector<std::string> headers = {"PID", "Name", "CPU %", "Mem %"};
    std::vector<int> widths = {10, 15, 10, 7};

    std::string addLeadingZeros(std::string str, size_t n);

    void drawTableData();

    void drawHeader(int col);

    void sortData(int col, bool sortDirection);
public:
    explicit UITable(const ProcessList &data);

    void drawTable();
    void waitForInput();

    ~UITable();
};



#endif //BABY_UITABLE_H
