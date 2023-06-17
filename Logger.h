//
// Created by losimen on 6/17/23.
//

#ifndef BABY_LOGGER_H
#define BABY_LOGGER_H

#include <fstream>

static std::ofstream m_out("log.txt");

class Logger {
public:
    static void log(const std::string& message)
    {
        m_out << message << std::endl;
    }
};


#endif //BABY_LOGGER_H
