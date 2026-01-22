//
// Created by Trent Tanchin on 5/17/24.
//

#include <cstdlib>
#include <cstring>
#include <elf.h>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <sys/stat.h>

#include "CLI11.hpp"

static constexpr auto DATA_PATH = "/usr/share/abii/";
static const auto PLUGIN_PATH = std::string(DATA_PATH) + "plugins/";
static const auto SYMS_PATH = std::string(DATA_PATH) + "syms/";
static const std::vector<std::string> ARCHS = {"32", "64"};
static constexpr auto TMPDIR = "/tmp/abii/";
static constexpr auto HOOKS_LIB = "libabii_hooks.so";

std::vector<std::string_view> splitStr(std::string_view str, const char sep = ' ')
{
    std::vector<std::string_view> csv_v;
    size_t pos, start = 0;

    while ((pos = str.find(sep, start)) != std::string_view::npos)
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

int get_elf_class(const char* lib_path)
{
    const int fd = open(lib_path, O_RDONLY);
    if (fd < 0)
        return -1;

    unsigned char ident[EI_NIDENT];
    if (read(fd, ident, EI_NIDENT) != EI_NIDENT)
    {
        close(fd);
        return -1;
    }
    close(fd);

    if (memcmp(ident, ELFMAG, SELFMAG) != 0)
        return -1;

    return ident[EI_CLASS];
}

int main(const int argc, char** argv)
{
    CLI::App app{"ABII - Application Binary Interface Interceptor"};
    argv = app.ensure_utf8(argv);
    app.set_version_flag("-v,--version", "ABII v" PROJECT_VERSION);

    std::string plugin;
    std::string searchpath;
    std::string symbols;
    std::string program;
    std::vector<std::string> arguments;

    auto list_syms = app.add_subcommand("list-syms", "List functions available for interception in <plugin>");
    auto intercept_cmd = app.add_subcommand("run", "Run <program> <args>... and intercept <syms> using <plugin>");

    app.add_option("--searchpath", searchpath, "Additional colon-separated plugin search path");
    app.add_option("<plugin>", plugin, "Plugin from which to list syms")->required();

    intercept_cmd->add_option("<syms>", symbols, "Comma-separated list of symbols to be intercepted")->required();
    intercept_cmd->add_option("<program>", program, "The program to be captured")->required();
    intercept_cmd->add_option("<args>", arguments, "Arguments passed to <program>");

    CLI11_PARSE(app, argc, argv);

    if (*list_syms)
    {
        std::ifstream symsf(SYMS_PATH + plugin);
        std::cout << symsf.rdbuf() << std::endl;
        return 0;
    }

    std::vector<const char*> launch_args;
    launch_args.push_back(program.c_str());
    for (const auto& arg : arguments)
        launch_args.push_back(arg.c_str());

    const char* old_ld_library_path = getenv("LD_LIBRARY_PATH");
    const char* old_ld_preload = getenv("LD_PRELOAD");

    std::string ld_library_path;

    if (app.count("--searchpath"))
        ld_library_path += searchpath;

    for (const auto& arch : ARCHS)
        ld_library_path += std::string(":") + TMPDIR + arch + "/:" + PLUGIN_PATH + arch + "/";

    std::string ld_preload = std::string(HOOKS_LIB) + ":lib" + plugin + ".so";

    const auto syms = splitStr(symbols, ',');
    const auto ld_lib_paths = splitStr(ld_library_path, ':');

    for (const auto& arch : ARCHS)
    {
        std::filesystem::path plugin_path;
        for (const auto& path : ld_lib_paths)
        {
            const auto lib = std::string(path) + "/lib" + plugin + ".so";
            if (std::filesystem::exists(path) && ((arch == "32" && get_elf_class(lib.c_str()) == ELFCLASS32)
                || (arch == "64" && get_elf_class(lib.c_str()) == ELFCLASS64)))
            {
                plugin_path = lib;
                break;
            }
        }
        if (plugin_path == "")
            throw std::runtime_error("ERROR: Cannot find " + arch + "-bit lib" + plugin + ".so to link with!");

        std::string tmpdir = TMPDIR + arch + "/";
        std::filesystem::create_directories(tmpdir.c_str());

        std::vector<std::string> objfiles;
        for (const auto& sym : syms)
        {
            const auto asmfile = tmpdir + std::string(sym) + ".S";
            processFile(std::string(DATA_PATH) + "asm-stubs/" + arch + "/stub-" + arch + ".S", asmfile, "@SYMBOL@",
                        std::string(sym));

            const auto objfile = tmpdir + std::string(sym) + ".o";
            objfiles.push_back(objfile);

            system(("gcc -m" + arch + " -c -fPIC " + asmfile + " -o " + objfile).c_str());
        }

        std::stringstream objfiles_ss;
        for (const auto& objfile : objfiles)
            objfiles_ss << objfile << " ";

        const auto sofile = tmpdir + HOOKS_LIB;

        system(
            ("gcc -m" + arch + " -shared " + objfiles_ss.str() + " " + plugin_path.string() + " -o " + sofile).c_str());
    }

    if (old_ld_library_path != nullptr)
        ld_library_path += std::string(":") + old_ld_library_path;

    if (old_ld_preload != nullptr)
        ld_preload += std::string(":") + old_ld_preload;

    setenv("LD_LIBRARY_PATH", ld_library_path.c_str(), 1);
    setenv("LD_PRELOAD", ld_preload.c_str(), 1);

#ifndef NDEBUG
    std::cout << "LD_LIBRARY_PATH=" << ld_library_path << std::endl;
    std::cout << "LD_PRELOAD=" << ld_preload << std::endl;
#endif

    std::cout << "[ABII] Capturing calls to " << symbols << std::endl;

    execvp(launch_args[0], const_cast<char* const*>(launch_args.data()));

    std::cerr << "[ABII] ERROR: execvp(): Failed to launch program " << launch_args[0] << std::endl;
    return 0;
}
