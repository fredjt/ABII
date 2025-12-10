//
// Created by Trent Tanchin on 7/6/24.
//

#include "libabii.h"

#include <sys/stat.h>

namespace abii
{
thread_local bool redirect = false;
thread_local std::string prefix;
thread_local std::vector<uintptr_t> used_addrs = {};
thread_local std::ofstream abii_stream;
}
