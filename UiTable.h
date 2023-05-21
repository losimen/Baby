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

#include "TableInfo.h"


class UITable
{
private:
    std::vector<TableInfo> data;
    WINDOW *window = nullptr;
    std::vector<std::string> headers = {"First Name", "Last Name", "Age"};
    std::vector<int> widths = {12, 12, 4};

    void drawTableData();

    void initHeader();
    void redrawHeader(int col);

    void sortData(int col, bool sortDirection);
public:
    explicit UITable(const std::vector<TableInfo> &data);

    void drawTable();
    void waitForInput();

    ~UITable();
};



#endif //BABY_UITABLE_H
