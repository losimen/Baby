//
// Created by losimen on 5/21/23.
//

#include "UiTable.h"

void UITable::drawTableData() {
    int maxRows = LINES - 3;
    int dataSize = data.size();
    int endRow = startOffset + maxRows - 1;

    if (startOffset < 0)
        startOffset = 0;

    if (endRow > dataSize)
        startOffset = dataSize - maxRows + 1;

    for (int i = startOffset; i <= endRow; i++) {
        // Check if the current index is valid
        if (i >= 0 && i < dataSize) {
            int x = 1;

            mvwprintw(window, i - startOffset + 3, x, "%s", addLeadingZeros(std::to_string(data[i].PID), 5).c_str());
            x += widths[0];

            mvwprintw(window, i - startOffset + 3, x, "%s", data[i].name.substr(0, widths[1]).c_str());
            x += widths[1];

            mvwprintw(window, i - startOffset + 3, x, "%s", std::to_string(data[i].cpuUsage).c_str());
            x += widths[2];

            mvwprintw(window, i - startOffset + 3, x, "%s", std::to_string(data[i].memUsage).c_str());
        }
    }
}


std::string UITable::addLeadingZeros(std::string str, size_t n) {
    int precision = n - std::min(n, str.size());
    return std::string(precision, '0').append(str);
}


void UITable::sortData(int col, bool sortDirection) {
    // Implement your sorting logic here based on the specified column and sort direction
    // Update the `data` vector accordingly
}

void UITable::drawHeader(int col) {
    int x = 1;
    for (int i = 0; i < headers.size(); i++) {
        if (i == col)
        {
            wattron(window, A_REVERSE);
        }

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

        if (ch == 'u') {
            startOffset--;
            this->drawTableData();
        }
        else if (ch == 'd') {
            startOffset++;
            this->drawTableData();
        }
        this->drawHeader(-1);


        wrefresh(window);
    }
}

UITable::~UITable() {
    endwin();
    delwin(window);
}
