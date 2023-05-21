//
// Created by losimen on 5/21/23.
//

#include "UiTable.h"

void UITable::drawTableData() {
    int maxRows = LINES - 3;
    int dataSize = static_cast<int>(data.size());
    int endRow = startOffset + maxRows - 1;

    if (startOffset < 0) {
        startOffset = 0;
        endRow = startOffset + maxRows - 1;
    }

    if (currentRow < 0) {
        currentRow = 0;
    }

    if (endRow >= dataSize) {
        endRow = dataSize - 1;
        startOffset = endRow - maxRows + 1;
    }

    if (currentRow < startOffset) {
        startOffset = currentRow;
        endRow = startOffset + maxRows - 1;
    }

    if (currentRow > endRow) {
        endRow = currentRow;
        startOffset = endRow - maxRows + 1;
    }

    for (int i = startOffset; i <= endRow; i++) {
        wmove(window, i - startOffset + 3, 1);
        wclrtoeol(window);

        if (i == currentRow)
            wattron(window, COLOR_PAIR(1));

        int x = 1;

        mvwprintw(window, i - startOffset + 3, x, "%s", std::to_string(data[i].PID).c_str());
        x += widths[0];

        mvwprintw(window, i - startOffset + 3, x, "%s", data[i].name.substr(0, widths[1]).c_str());
        x += widths[1];

        mvwprintw(window, i - startOffset + 3, x, "%s", std::to_string(data[i].cpuUsage).c_str());
        x += widths[2];

        mvwprintw(window, i - startOffset + 3, x, "%s", std::to_string(data[i].memUsage).c_str());

        if (i == currentRow)
            wattroff(window, COLOR_PAIR(1));
    }
}


std::string UITable::addLeadingZeros(const std::string& str, size_t n) {
    size_t precision = n - std::min(n, str.size());
    return std::string(precision, '0').append(str);
}


void UITable::sortData(int col, bool sortDirection) {
    if (col == 1)
    {
        std::sort(data.begin(), data.end(), [sortDirection](const Process& a, const Process& b) {
            if (sortDirection)
                return a.PID < b.PID;
            else
                return a.PID > b.PID;
        });
    }
    else if (col == 2)
    {
        std::sort(data.begin(), data.end(), [sortDirection](const Process& a, const Process& b) {
            if (sortDirection)
                return a.name < b.name;
            else
                return a.name > b.name;
        });
    }
    else if (col == 3)
    {
        std::sort(data.begin(), data.end(), [sortDirection](const Process& a, const Process& b) {
            if (sortDirection)
                return a.cpuUsage < b.cpuUsage;
            else
                return a.cpuUsage > b.cpuUsage;
        });
    }
    else if (col == 4)
    {
        std::sort(data.begin(), data.end(), [sortDirection](const Process& a, const Process& b) {
            if (sortDirection)
                return a.memUsage < b.memUsage;
            else
                return a.memUsage > b.memUsage;
        });
    }
}

void UITable::drawHeader(int col) {
    int x = 1;
    for (int i = 0; i < headers.size(); i++) {
        if (i == col)
            wattron(window, A_REVERSE);

        mvwprintw(window, 1, x, "%s", headers[i].c_str());
        wattroff(window, A_REVERSE);
        x += widths[i];
    }

    // Redraw the horizontal line under the header
    mvwhline(window, 2, 1, '-', getmaxx(window) - 2);
}

UITable::UITable(const ProcessList &data) {
    initscr();
    mousemask(ALL_MOUSE_EVENTS, nullptr);
    noecho();
    start_color();

    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);

    keypad(window, TRUE);

    this->data = data;
    this->window = newwin((int)data.size() + 3, std::accumulate(widths.begin(), widths.end(), 1) + 1, 1, 1);
}

void UITable::drawTable() {
    this->drawHeader(-1);
    this->drawTableData();

    refresh();
    wrefresh(window);
}

void UITable::waitForInput() {
    std::vector<bool> sortDirections(headers.size(), false);
    int ch;

    while ((ch = getch()) != 'q') {
        int col = ch - '0';
        if (col >= 1 && col < 9)
        {
            sortDirections[col] = !sortDirections[col];
            this->sortData(col, sortDirections[col]);
            startOffset = 0;
            currentRow = 0;
        }

        if (ch == 'u')
        {
            currentRow--;
            col = -1;
        }
        else if (ch == 'd')
        {
            currentRow++;
            col = -1;
        }

        this->drawTableData();
        this->drawHeader(col-1);

        wrefresh(window);
    }
}

UITable::~UITable() {
    endwin();
    delwin(window);
}
