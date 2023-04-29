#ifndef SERVER_TYPES_H
#define SERVER_TYPES_H

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>


struct Process {
    int PID;
    std::string name;
};

typedef std::vector<Process> ProcessList;

#endif //LISTENER_H