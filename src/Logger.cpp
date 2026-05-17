//
// Created by Trent Tanchin on 7/31/25.
//

#include "Logger.h"

#include <sys/stat.h>

thread_local std::ofstream Logger::ofstream_;
