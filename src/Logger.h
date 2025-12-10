//
// Created by Trent Tanchin on 7/31/25.
//

#ifndef ABII_LOGGER_H
#define ABII_LOGGER_H

#include <fstream>

#define LOG_DIR "abii_log"

class Logger
{
    static thread_local std::ofstream ofstream_;
    const char* scope_;

public:
    Logger() = delete;
    explicit Logger(const char* scope) : scope_(scope) { ofstream_ << "Entering " << scope_ << std::endl; }
    ~Logger() { ofstream_ << "Exiting " << scope_ << std::endl; }
    friend void LoggerInit();
};

#endif //ABII_LOGGER_H
