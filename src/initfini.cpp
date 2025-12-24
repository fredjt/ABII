//
// Created by Trent Tanchin on 12/9/25.
//

#include <filesystem>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <sys/stat.h>

#include "libabii.h"

namespace abii
{
std::string get_logfname()
{
    const auto pid = std::to_string(getpid());
    const auto tid = std::to_string(gettid());

    return std::string(getenv("HOME")) + "/abii_log/" + program_invocation_short_name + "_" + pid + "_" + tid;
}

__attribute__((constructor))
void abii_init()
{
    mkdir((std::string(getenv("HOME")) + "/abii_log").c_str(), 0775);

    abii_stream = std::ofstream(get_logfname() + ".log", std::ios::app);
    if (!abii_stream.is_open())
        throw std::runtime_error("Could not open " + get_logfname() + ".log");
#ifdef BIT32
    abii_stream << "Loading 32-bit ABII in process: " << getpid() << " thread: " << gettid() << "..."
        << std::endl << std::endl;
#else
    abii_stream << "Loading 64-bit ABII in process: " << getpid() << " thread: " << gettid() << "..."
        << std::endl << std::endl;
#endif

    auto map_logstream = std::ofstream(get_logfname() + ".maps", std::ios::app);
    if (!map_logstream.is_open())
        throw std::runtime_error("Could not open " + get_logfname() + ".maps");

    const std::ifstream maps("/proc/self/maps");
    map_logstream << maps.rdbuf() << std::endl;
    ENABLE_OVERRIDES
}

__attribute__((destructor))
static void abii_destructor()
{
    DISABLE_OVERRIDES
    std::ofstream os(get_logfname() + ".log", std::ios::app);
#ifndef BIT32
    os << "Unloading 64-bit ABII in process: " << getpid() << " thread: " << gettid() << "..." << std::endl;
#else
    os << "Unloading 32-bit ABII in process: " << getpid() << " thread: " << gettid() << "..." << std::endl;
#endif
    os.flush();
    os.close();
    if (abii_stream.is_open())
    {
        abii_stream.flush();
        abii_stream.close();
    }
}
} // namespace abii
