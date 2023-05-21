//
// Created by losimen on 5/21/23.
//

#include "UiTable.h"

void UITable::drawTableData(int &startOffset) {
    int maxRows = LINES - 3;
//    std::cout << "maxRows = " << maxRows << std::endl;
    int dataSize = data.size();
    int endRow = startOffset + maxRows - 1;

    // Validate the startRow and numRows
    if (startOffset < 0) {
        startOffset = 0;
    }
    // validate for max value
    if (endRow > dataSize) {
        startOffset = dataSize - maxRows + 1;
    }

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


void UITable::initHeader() {
    // Check if the header has already been drawn
    if (headerDrawn) {
        return;
    }

    int x = 1;
    for (int i = 0; i < headers.size(); i++) {
        mvwprintw(window, 1, x, "%s", headers[i].c_str());
        x += widths[i];
    }

    mvwhline(window, 2, 1, '-', getmaxx(window) - 2);

    // Set headerDrawn to true after drawing the header
    headerDrawn = true;
}

void UITable::sortData(int col, bool sortDirection) {
    // Implement your sorting logic here based on the specified column and sort direction
    // Update the `data` vector accordingly
}

void UITable::redrawHeader(int col) {
    int x = 1;
    for (int i = 0; i < headers.size(); i++) {
        if (i == col) {
            wattron(window, A_REVERSE);
        }

        mvwprintw(window, 1, x, "%s", headers[i].c_str());
        wattroff(window, A_REVERSE);
        x += widths[i] + 1;
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

    this->initHeader(); // Initialize the header only once in the constructor
}

void UITable::drawTable() {
    this->initHeader();
    int a = 0;
    this->drawTableData(a);

    refresh();
    wrefresh(window);
}

void UITable::waitForInput() {
    std::vector<bool> sortDirections(headers.size(), false);
    int ch;
    int startRow = 0;

    while ((ch = getch()) != 'q') {
        if (ch == 'u') {
            startRow--;
            this->drawTableData(startRow);
        }
        if (ch == 'd') {
            startRow++;
            this->drawTableData(startRow);
        }
//        this->redrawHeader(-1); // Redraw the header without highlighting any column

        wrefresh(window);
    }
}

UITable::~UITable() {
    endwin();
    delwin(window);
}
