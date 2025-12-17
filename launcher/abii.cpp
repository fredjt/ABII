//
// Created by Trent Tanchin on 5/17/24.
//

#include <cstdlib>
#include <docopt.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <sys/stat.h>

static constexpr auto HELP = R"(
ABII - Application Binary Interface Interceptor

Usage: abii <plugin> <syms> [--searchpath <searchpath>] <program>...

Options:
    -h --help                     Show this screen.
    --version                     Show the version number.
    --searchpath <searchpath>     Additional colon-separated plugin search path.
)";

static constexpr auto DATA_PATH = "/usr/share/abii/";
static const auto BASE_PATH = std::string(DATA_PATH) + "plugins/";
static const std::vector<std::string> ARCHS = {"32", "64"};
static const auto ASSEMBLY_TEMPLATE = std::string(DATA_PATH) + "template.S";
static constexpr auto TMPDIR = "/tmp/abii/";
static constexpr auto HOOKS_LIB = "libabii_hooks.so";

std::vector<std::string_view> splitCSV(std::string_view str)
{
    std::vector<std::string_view> csv_v;
    size_t pos, start = 0;

    while ((pos = str.find(',', start)) != std::string_view::npos)
    {
        csv_v.emplace_back(str.substr(start, pos - start));
        start = pos + 1;
    }

    csv_v.emplace_back(str.substr(start));
    return csv_v;
}

void processFile(const std::string& file, const std::string& destFile, const std::string& token,
                 const std::string& replacement)
{
    std::ifstream in(file);
    std::string text((std::istreambuf_iterator(in)), std::istreambuf_iterator<char>());

    size_t pos = 0;
    while ((pos = text.find(token, pos)) != std::string::npos)
    {
        text.replace(pos, token.size(), replacement);
        pos += replacement.size();
    }

    std::ofstream(destFile) << text;
}

int main(const int argc, char** argv)
{
    std::map<std::string, docopt::value> args =
        docopt::docopt(HELP, {argv + 1, argv + argc}, true, "ABII v0.0.1");

    std::vector<const char*> launch_args;
    for (const auto& arg : args["<program>"].asStringList())
        launch_args.push_back(arg.c_str());

    const char* old_ld_library_path = getenv("LD_LIBRARY_PATH");
    const char* old_ld_preload = getenv("LD_PRELOAD");

    std::string ld_library_path;
    for (const auto& arch : ARCHS)
        ld_library_path += BASE_PATH + arch + "/:";

    if (args["--searchpath"])
        ld_library_path += args["--searchpath"].asString();

    std::string ld_preload = "lib" + args["<plugin>"].asString() + ".so";
    const auto syms = splitCSV(args["<syms>"].asString());

    std::filesystem::create_directories(TMPDIR);

    for (const auto& arch : ARCHS)
    {
        std::string tmpdir = TMPDIR + arch + "/";
        mkdir(tmpdir.c_str(), 0700);
        ld_library_path += ":" + tmpdir;
        std::vector<std::string> objfiles;
        for (const auto& sym : syms)
        {
            const auto asmfile = tmpdir + std::string(sym) + ".S";
            processFile(ASSEMBLY_TEMPLATE, asmfile, "@SYMBOL@", std::string(sym));

            const auto objfile = tmpdir + std::string(sym) + ".o";
            objfiles.push_back(objfile);

            system(("as --" + arch + " " + asmfile + " -o " + objfile).c_str());
        }

        std::stringstream objfiles_ss;
        for (const auto& objfile : objfiles)
            objfiles_ss << objfile << " ";

        const auto sofile = tmpdir + HOOKS_LIB;

        system(("gcc -m" + arch + " -shared " + objfiles_ss.str() + " -o " + sofile).c_str());
    }

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
    setenv("LD_PRELOAD", (std::string(HOOKS_LIB) + ":" + ld_preload.c_str()).c_str(), 1);

    std::cout << "LD_LIBRARY_PATH=" << ld_library_path << std::endl;
    std::cout << "LD_PRELOAD=" << ld_preload << std::endl;
    std::cout << "Capturing calls to " << args["<syms>"].asString() << std::endl;

    execvp(launch_args[0], const_cast<char* const*>(launch_args.data()));
    return 0;
}
