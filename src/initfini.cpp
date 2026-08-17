//
// Created by Trent Tanchin on 12/9/25.
//

#include "initfini.h"

#include <filesystem>
#include <stdexcept>
#include <unistd.h>
#include <sys/stat.h>

#include "libabii.h"

namespace abii
{
static void dump_memmaps()
{
    const auto pid = std::to_string(getpid());
    const auto tid = std::to_string(gettid());

    const auto fname = abii_logdir + "/" + program_invocation_short_name + "_" + pid + "_" + tid + ".maps";

    auto map_logstream = std::ofstream(fname, std::ios::app);
    if (!map_logstream.is_open())
        throw std::runtime_error("Could not open " + fname);

    const std::ifstream maps("/proc/self/maps");
    map_logstream << maps.rdbuf() << std::endl;
}

__attribute__((constructor))
static void abii_init()
{
    DISABLE_OVERRIDES
#ifdef BIT32
    abii_stream << "Loading 32-bit ABII in process: " << getpid() << "..." << std::endl << std::endl;
#else
    abii_stream << "Loading 64-bit ABII in process: " << getpid() << "..." << std::endl << std::endl;
#endif

    dump_memmaps();

    abii_syms = getenv("ABII_SYMS");

    ENABLE_OVERRIDES
}

__attribute__((destructor))
static void abii_destructor()
{
    DISABLE_OVERRIDES
    const auto pid = std::to_string(getpid());
    const auto tid = std::to_string(gettid());

    const auto fname = abii_logdir + "/" + program_invocation_short_name + "_" + pid + "_" + tid + ".log";

    mkdir(abii_logdir.c_str(), 0775);
    auto stream = std::ofstream(fname, std::ios::app);

#ifndef BIT32
    stream << "Unloading 64-bit ABII in process: " << getpid() << "..." << std::endl;
#else
    stream << "Unloading 32-bit ABII in process: " << getpid() << "..." << std::endl;
#endif
}
} // namespace abii
