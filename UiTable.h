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
    WINDOW *window = nullptr;
    std::vector<std::string> headers = {"PID", "Name", "CPU %", "Mem %"};
    std::vector<int> widths = {5, 15, 10, 7};

    void drawTableData();

    void initHeader();
    void redrawHeader(int col);

    void sortData(int col, bool sortDirection);
public:
    explicit UITable(const ProcessList &data);

    void drawTable();
    void waitForInput();

    ~UITable();
};



#endif //BABY_UITABLE_H
