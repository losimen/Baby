//
// Created by losimen on 5/21/23.
//

#include "UiTable.h"

void UITable::drawTableData() {
    for (int i = 0; i < data.size(); i++) {
        int x = 1;

        mvwprintw(window, i + 3, x, "%s", data[i].firstName.substr(0, widths[0]).c_str());
        x += widths[0] + 1;

        mvwprintw(window, i + 3, x, "%s", data[i].lastName.substr(0, widths[1]).c_str());
        x += widths[1] + 2;

        mvwprintw(window, i + 3, x, "%s", std::to_string(data[i].age).c_str());
    }
}

void UITable::initHeader() {
    int x = 1;
    for (int i = 0; i < headers.size(); i++) {
        mvwprintw(window, 1, x, "%s", headers[i].c_str());
        x += widths[i] + 1;
    }

    mvwhline(window, 2, 1, '-', getmaxx(window) - 2);
}

void UITable::sortData(int col, bool sortDirection) {
    std::sort(data.begin(), data.end(), [&](const TableInfo& a, const TableInfo& b) {
        bool condition = false;

        if (col == 0)
        {
            condition = sortDirection ? (a.firstName > b.firstName) : (a.firstName < b.firstName);
        }
        else if (col == 1)
        {
            condition = sortDirection ? (a.lastName > b.lastName) : (a.lastName < b.lastName);
        }
        else if (col == 2)
        {
            condition = sortDirection ? (a.age > b.age) : (a.age < b.age);
        }

        return condition;
    });
}

void UITable::redrawHeader(int col) {
    int x = 1;
    for (int i = 0; i < headers.size(); i++) {
        if (i == col)
        {
            wattron(window, A_REVERSE);
        }

        mvwprintw(window, 1, x, "%s", headers[i].c_str());
        wattroff(window, A_REVERSE);
        x += widths[i] + 1;
    }

    // redraw the horizontal line under the header
    mvwhline(window, 2, 1, '-', getmaxx(window) - 2);
}

UITable::UITable(const std::vector<TableInfo> &data) {
    initscr();
    noecho();

    this->data = data;
    this->window = newwin((int)data.size() + 3, std::accumulate(widths.begin(), widths.end(), 1) + 1, 1, 1);
}

void UITable::drawTable() {
    this->initHeader();
    this->drawTableData();

    refresh();
    wrefresh(window);
}

void UITable::waitForInput() {
    std::vector<bool> sortDirections(headers.size(), false);
    int ch;

    while ((ch = getch()) != 'q')
    {
        if (ch >= '1' && ch <= '0' + headers.size())
        {
            int col = ch - '1';
            this->sortData(col, sortDirections[col-1]);

            sortDirections[col-1] = !sortDirections[col-1];

            this->drawTableData();
            this->redrawHeader(col);

            wrefresh(window);
        }
    }
}

UITable::~UITable() {
    endwin();
    delwin(window);
}
