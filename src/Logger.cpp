//
// Created by Trent Tanchin on 7/31/25.
//

#include "Logger.h"

#include <sys/stat.h>

thread_local std::ofstream Logger::ofstream_;

__attribute__((constructor))
void LoggerInit()
{
    mkdir("abii_log", 0775);
    Logger::ofstream_ = std::ofstream(LOG_DIR "/###TRACE_LOG###.log", std::ios::app);
}
