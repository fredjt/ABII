//
// Created by Trent Tanchin on 12/9/25.
//

#include <stdexcept>
#include <string>
#include <unistd.h>
#include <sys/stat.h>

#include "libabii.h"

namespace abii
{
std::string get_logfname()
{
    auto pid = std::to_string(getpid());
    auto tid = std::to_string(gettid());
    auto path = "/proc/" + pid + "/comm";
    std::ifstream fcomm(path);
    if (!fcomm.is_open())
        throw std::runtime_error("Could not open " + path);

    std::string comm;
    std::getline(fcomm, comm);
    return std::string(getenv("HOME")) + "/abii_log/" + comm + "_" + pid + "_" + tid + ".txt";
}

__attribute__((constructor))
void abii_init()
{
    mkdir((std::string(getenv("HOME")) + "/abii_log").c_str(), 0775);

    abii_stream = std::ofstream(get_logfname(), std::ios::app);
    if (!abii_stream.is_open())
        throw std::runtime_error("Could not open " + get_logfname());
#ifndef BIT32
    abii_stream << "Loading 64-bit ABII in process: " << getpid() << " thread: " << gettid() << "..."
        << std::endl << std::endl;
#else
    abii_stream << "Loading 32-bit ABII in process: " << getpid() << " thread: " << gettid() << "..."
        << std::endl << std::endl;
#endif
    ENABLE_OVERRIDES
}

__attribute__((destructor))
static void abii_destructor()
{
    DISABLE_OVERRIDES
    std::ofstream os(get_logfname(), std::ios::app);
#ifndef BIT32
    os << "Unloading 64-bit ABII in process: " << getpid() << " thread: " << gettid() << "..."
        << std::endl;
#else
    os << "Unloading 32-bit ABII in process: " << getpid() << " thread: " << gettid() << "..."
        << std::endl;
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
