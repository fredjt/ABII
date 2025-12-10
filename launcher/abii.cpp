//
// Created by Trent Tanchin on 5/17/24.
//

#include <cstdlib>
#include <docopt.h>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

static constexpr auto HELP = R"(
ABII - Application Binary Interface Interceptor

Usage: abii <plugin> [--searchpath <searchpath>] <program> [<args>...]

Options:
    -h --help                     Show this screen.
    --version                     Show the version number.
    --searchpath <searchpath>     Additional colon-separated plugin search path.
)";

static constexpr auto BASE_PATH = "/usr/share/abii/plugins/";
static const std::vector<std::string> ARCHS = {"32", "64"};

int main(const int argc, char** argv)
{
    std::map<std::string, docopt::value> args =
        docopt::docopt(HELP, {argv + 1, argv + argc}, true, "ABII v0.0.1");

    std::vector<const char*> launch_args;
    const char* program = args["<program>"].asString().c_str();
    launch_args.push_back(program);
    for (const auto& arg : args["<args>"].asStringList())
        launch_args.push_back(arg.c_str());

    const char* old_ld_library_path = getenv("LD_LIBRARY_PATH");
    const char* old_ld_preload = getenv("LD_PRELOAD");

    std::string ld_library_path;
    for (const auto& arch : ARCHS)
        ld_library_path += BASE_PATH + arch + "/:";

    if (args["--searchpath"])
        ld_library_path += args["--searchpath"].asString();

    std::string ld_preload = args["<plugin>"].asString() + ".so";
    if (old_ld_library_path != nullptr)
    {
        ld_library_path += ":";
        ld_library_path += old_ld_library_path;
    }

    if (old_ld_preload != nullptr)
    {
        ld_preload += ":";
        ld_preload += old_ld_preload;
    }

    setenv("LD_LIBRARY_PATH", ld_library_path.c_str(), 1);
    setenv("LD_PRELOAD", ld_preload.c_str(), 1);

    std::cout << "LD_LIBRARY_PATH=" << ld_library_path << std::endl;
    std::cout << "LD_PRELOAD=" << ld_preload << std::endl;

    execvp(launch_args[0], const_cast<char* const*>(launch_args.data()));
    return 0;
}
