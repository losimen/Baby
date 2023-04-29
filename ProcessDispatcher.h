#ifndef PROCESS_DISPATCHER_H
#define PROCESS_DISPATCHER_H


#include <string>
#include <iostream>
#include <vector>
#include <signal.h>
#include <cstring>
#include <dirent.h>
#include <sys/types.h>
#include <fstream>

#include "Types.h"


class ProcessDispatcher {
private:
    static bool _charStartsWith(const char *a, const char *b);
    static std::string _parseProcessFileLine(std::string lineToParse);

    static const std::string PROC_DIR;
    static const std::string PROC_STAT_FILE;
    static const std::string PROC_STATUS_FILE;

public:
    static ProcessList getListOfProcesses();

    static Process getProcessInfo(const int PID);
    static std::string getProcessName(const int PID);
    static bool isProcessExists(const int PID);
    static bool killProcess(const int PID);
};

#endif //PROCESS_DISPATCHER_H