//
// Created by losimen on 5/21/23.
//

#include "UiTable.h"

void UITable::drawTableData(int &startRow, int &numRows) {
    int maxRows = getmaxy(window) - 3; // Calculate the maximum number of visible rows
    int dataSize = data.size();
    int endRow = startRow + numRows - 1;

    // Adjust the start and end row based on the available data and visible rows
    if (startRow < 0) {
        startRow = 0;
    }
    if (endRow >= dataSize) {
        endRow = dataSize - 1;
    }
    if (endRow >= startRow + maxRows) {
        startRow = endRow - maxRows + 1;
    }

    numRows = endRow - startRow + 1;

    for (int i = startRow; i <= endRow; i++) {
        // Check if the current index is valid
        if (i >= 0 && i < dataSize) {
            int x = 1;

            mvwprintw(window, i - startRow + 3, x, "%s", std::to_string(data[i].PID).c_str());
            x += widths[0];

            mvwprintw(window, i - startRow + 3, x, "%s", data[i].name.substr(0, widths[1]).c_str());
            x += widths[1];

            mvwprintw(window, i - startRow + 3, x, "%s", std::to_string(data[i].cpuUsage).c_str());
            x += widths[2];

            mvwprintw(window, i - startRow + 3, x, "%s", std::to_string(data[i].memUsage).c_str());
        }
    }
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
    noecho();
    scrollok(stdscr, TRUE); // Enable scrolling for the standard screen

    this->data = data;
    this->window = newwin((int)data.size() + 3, std::accumulate(widths.begin(), widths.end(), 1) + 1, 1, 1);
    scrollok(window, TRUE); // Enable scrolling for the table window

    this->initHeader(); // Initialize the header only once in the constructor
}

void UITable::drawTable() {
    this->initHeader();
    int a = 0;
    int size = data.size();
    this->drawTableData(a, size);

    refresh();
    wrefresh(window);
}

void UITable::waitForInput() {
    std::vector<bool> sortDirections(headers.size(), false);
    int ch;
    int startRow = 0;
    int numRows = data.size(); // Calculate the number of visible rows

    while ((ch = getch()) != 'q') {
        if (ch == KEY_UP) {
            startRow--;
        } else if (ch == KEY_DOWN) {
            startRow++;
        } else if (ch >= '1' && ch <= '0' + headers.size()) {
            int col = ch - '1';
            this->sortData(col, sortDirections[col-1]);
            sortDirections[col-1] = !sortDirections[col-1];
            startRow = 0; // Reset the start row when sorting to show the sorted data from the beginning
        }

        this->drawTableData(startRow, numRows);
//        this->redrawHeader(-1); // Redraw the header without highlighting any column

        wrefresh(window);
    }
}

UITable::~UITable() {
    endwin();
    delwin(window);
}
