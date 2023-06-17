//
// Created by losimen on 5/21/23.
//

#include "UiTable.h"
#include "SystemInfo.h"

void UITable::drawTableData() {
    int maxRows = LINES - 5;
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
        startOffset = currentRow.load();
        endRow = startOffset + maxRows - 1;
    }

    if (currentRow > endRow) {
        endRow = currentRow;
        startOffset = endRow - maxRows + 1;
    }

    for (int i = startOffset; i <= endRow; i++) {
        wmove(window, i - startOffset + 5, 1);
        wclrtoeol(window);

        if (i == currentRow)
            wattron(window, COLOR_PAIR(1));

        int x = 1;

        mvwprintw(window, i - startOffset + 5, x, "%s", std::to_string(data[i].PID).c_str());
        x += widths[0];

        mvwprintw(window, i - startOffset + 5, x, "%s", data[i].name.substr(0, widths[1]).c_str());
        x += widths[1];

        mvwprintw(window, i - startOffset + 5, x, "%s", std::to_string(data[i].cpuUsage).c_str());
        x += widths[2];

        mvwprintw(window, i - startOffset + 5, x, "%s", std::to_string(data[i].memUsage).c_str());

        if (i == currentRow)
            wattroff(window, COLOR_PAIR(1));
    }
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


void UITable::drawHeader() {
    bool sortDirection = sortDirections[col];

    int innerCol = col-1;
    if (innerCol > 0)
    {
        innerCol = col-1;
    }

    wclrtoeol(window);

    std::string cpuRow = "CPU " + getBar(cpuLoad) + " " + std::to_string(cpuLoad).substr(0, 4) + "%";
    std::string memRow = "MEM " + getBar(memUsage) + " " + std::to_string(memUsage).substr(0, 4) + "%";

    int x = 1;
    mvwprintw(window, 0, x, "%s", cpuRow.c_str());
    mvwprintw(window, 1, x, "%s", memRow.c_str());

    mvwhline(window, 2, 1, '-', getmaxx(window) - 2);

    x = 1;
    for (int i = 0; i < headers.size(); i++) {
        if (i == innerCol) {
            wattron(window, COLOR_PAIR(2));

            if (sortDirection)
                mvwprintw(window, 3, x, "%s ^", headers[i].c_str());
            else
                mvwprintw(window, 3, x, "%s v", headers[i].c_str());
        } else {
            mvwprintw(window, 3, x, "%s  ", headers[i].c_str());
        }

        wattroff(window, COLOR_PAIR(2));
        x += widths[i];
    }

    mvwhline(window, 4, 1, '-', getmaxx(window) - 2);
}


UITable::UITable() {
    initscr();
    noecho();
    start_color();

    init_pair(1, COLOR_BLACK, COLOR_CYAN);
    init_pair(2, COLOR_BLACK, COLOR_MAGENTA);

    keypad(window, TRUE);

    sortDirections = std::vector<bool>(headers.size(), false);

    dataThread = std::thread(&UITable::updateTableDataThread, this);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    this->window = newwin((int)data.size() + 3, std::accumulate(widths.begin(), widths.end(), 1) + 1, 1, 1);
}


void UITable::drawTable() {
    this->drawHeader();
    this->drawTableData();

    refresh();
    wrefresh(window);
}


void UITable::waitForInput() {
    int ch;

    while ((ch = getch()) != 'q') {
        col = ch - '0';
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
        this->drawHeader();

        wrefresh(window);
    }

    dataThread.join();
}


UITable::~UITable() {
    endwin();
    delwin(window);
}


std::string UITable::getBar(double cpuLoad)
{
    const int maxBars = 10;
    const double maxLoad = 100.0;
    double barCount = (cpuLoad / maxLoad) * maxBars;

    std::string bar;
    for (int i = 0; i < maxBars; ++i)
    {
        if (i < barCount)
            bar += "|";
        else
            bar += " ";
    }
    return bar;
}


void UITable::updateTableData() {
    cpuLoad = SystemInfo::getCpuLoad(100000);
    MemoryStatus memStatus;
    SystemInfo::getMemStatus(memStatus);
    memUsage = memStatus.memUsage;

    this->data = SystemInfo::getListOfProcesses();
}


void UITable::updateTableDataThread() {
    while (true) {
        this->updateTableData();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
