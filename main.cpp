#include <ncurses.h>
#include <algorithm>
#include <vector>
#include <string>
#include <numeric>


int main() {
    // initialize ncurses
    initscr();

    // create some sample data
    std::vector<std::vector<std::string>> data = {
            {"John", "Doe", "40"},
            {"Jane", "Doe", "35"},
            {"Bob", "Smith", "55"},
            {"Alice", "Jones", "25"}
    };

    // create a vector of column names
    std::vector<std::string> headers = {"First Name", "Last Name", "Age"};

    // create a vector of column widths
    std::vector<int> widths = {12, 12, 4};

    // create a vector of column sort directions
    std::vector<bool> sort_directions(headers.size(), false);

    // create a window to display the table
    WINDOW* window = newwin(data.size() + 2, std::accumulate(widths.begin(), widths.end(), 1) + 1, 1, 1);

    // draw the table header
    int x = 1;
    for (int i = 0; i < headers.size(); i++) {
        mvwprintw(window, 1, x, headers[i].c_str());
        x += widths[i] + 1;
    }

    // draw a horizontal line under the header
    mvwhline(window, 2, 1, '-', getmaxx(window) - 2);

    // draw the table data
    for (int i = 0; i < data.size(); i++) {
        x = 1;
        for (int j = 0; j < data[i].size(); j++) {
            mvwprintw(window, i + 3, x, data[i][j].c_str());
            x += widths[j] + 1;
        }
    }

    // refresh the screen to display the window and its contents
    refresh();
    wrefresh(window);

    // wait for a key press
    int ch;
    while ((ch = getch()) != 'q') {
        // if a key other than 'q' is pressed, check if it corresponds to a column header
        if (ch >= '1' && ch <= '0' + headers.size()) {
            // convert the key to a column index
            int col = ch - '1';

            // sort the data by the selected column
            std::sort(data.begin(), data.end(), [&](const std::vector<std::string>& a, const std::vector<std::string>& b) {
                if (sort_directions[col]) {
                    return a[col] < b[col];
                } else {
                    return a[col] > b[col];
                }
            });

            // flip the sort direction for the selected column
            sort_directions[col] = !sort_directions[col];

            // redraw the table data
            for (int i = 0; i < data.size(); i++) {
                x = 1;
                for (int j = 0; j < data[i].size(); j++) {
                    mvwprintw(window, i + 3, x, data[i][j].c_str());
                    x += widths[j] + 1;
                }
            }
            // highlight the selected column header to indicate the sort direction
            x = 1;
            for (int i = 0; i < headers.size(); i++) {
                if (i == col) {
                    wattron(window, A_REVERSE);
                }
                mvwprintw(window, 1, x, headers[i].c_str());
                wattroff(window, A_REVERSE);
                x += widths[i] + 1;
            }

            // redraw the horizontal line under the header
            mvwhline(window, 2, 1, '-', getmaxx(window) - 2);

            // refresh the screen to display the updated window and its contents
            wrefresh(window);
        }

    }

    endwin();

    return 0;
}