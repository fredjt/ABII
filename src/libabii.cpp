//
// Created by Trent Tanchin on 7/6/24.
//

#include "libabii.h"

#include <sys/stat.h>

namespace abii
{
thread_local bool redirect = true;
thread_local std::string prefix;
thread_local std::vector<uintptr_t> used_addrs = {};
thread_local std::ofstream abii_stream{
    []
    {
        DISABLE_OVERRIDES
        const auto pid = std::to_string(getpid());
        const auto tid = std::to_string(gettid());

        const auto fname = abii_logdir + "/" + program_invocation_short_name + "_" + pid + "_" + tid + ".log";

        mkdir(abii_logdir.c_str(), 0775);
        auto stream = std::ofstream(fname, std::ios::app);
        ENABLE_OVERRIDES
        return stream;
    }()
};
}
