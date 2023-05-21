#include <vector>

#include "UiTable.h"


int main() {
    std::vector<TableInfo> tableData = {
        {"Abc", "Doe", 40},
        {"Bac", "Doe", 35},
        {"Cba", "Smith", 55},
        {"Bac", "Smith", 60},
        {"Abc", "Smith", 50},
        {"Cba", "Doe", 45},
        {"Abc", "Jones", 30},
        {"Bac", "Jones", 25},
        {"Cba", "Jones", 20},
    };

    UITable table(tableData);
    table.drawTable();
    table.waitForInput();

    return 0;
}