//
// Created by Trent Tanchin on 5/18/24.
//

#ifndef LIBABII_H
#define LIBABII_H

#include <algorithm>
#include <cstring>
#include <cxxabi.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <quadmath.h>
#include <sstream>
#include <unistd.h>
#include <vector>

#include "Logger.h"
#include "utils.h"

#define ENABLE_OVERRIDES abii::redirect = true;
#define DISABLE_OVERRIDES abii::redirect = false;

#ifndef NDEBUG
#define TRACE_LOGGER auto abii_logger = Logger(__func__);
#else
#define TRACE_LOGGER
#endif

#define OVERRIDE_PREFIX(real_func) \
    if (abii::redirect) \
    { \
        DISABLE_OVERRIDES \
        TRACE_LOGGER \
        if ((real_func) == nullptr) \
        { \
            (real_func) = (decltype(real_func)) dlsym(RTLD_NEXT, __func__); \
            if ((real_func) == nullptr) \
                std::cerr << "Error in `dlsym`: " << dlerror() << std::endl; \
        } \
        abii::prefix = ""; \
        const auto abii_args = new abii::ArgsPrinter();

#define OVERRIDE_SUFFIX(real_func, ret) \
        abii_args->print_args(); \
        abii::abii_stream << std::endl; \
        delete abii_args; \
        ENABLE_OVERRIDES \
        return ret; \
    } \
    if ((real_func) == nullptr) \
    { \
        (real_func) = (decltype(real_func)) dlsym(RTLD_NEXT, __func__); \
        if ((real_func) == nullptr) \
            std::cerr << "Error in `dlsym`: " << dlerror() << std::endl; \
    }

#define OVERRIDE_STREAM_PREFIX \
    const auto abii_old_prefix = abii::prefix; \
    abii::prefix = abii::prefix + "\t"; \
    os << std::endl; \
    const auto abii_args = new abii::ArgsPrinter();

#define OVERRIDE_STREAM_SUFFIX \
    abii_args->print_args(); \
    abii::prefix = abii_old_prefix; \
    return os;

#define OVERRIDE_VARIADIC_PREFIX(real_func, fmt) \
    OVERRIDE_PREFIX(real_func) \
        va_list abii_vargs; \
        const auto abii_bi_vargs = __builtin_apply_args();

#define OVERRIDE_VARIADIC_SUFFIX(real_func, fmt) \
        va_start(abii_vargs, fmt); \
        abii_args->print_args(); \
        va_end(abii_vargs); \
        abii::abii_stream << std::endl; \
        ENABLE_OVERRIDES \
        __builtin_return(abii_ret); \
    } \
    if ((real_func) == nullptr) \
    { \
        (real_func) = (decltype(real_func)) dlsym(RTLD_NEXT, __func__); \
        if ((real_func) == nullptr) \
            std::cerr << "Error in `dlsym`: " << dlerror() << std::endl; \
    } \
    const auto abii_bi_vargs = __builtin_apply_args();

#define OVERRIDE_VALIST_PREFIX(real_func, fmt, valist) \
    OVERRIDE_PREFIX(real_func) \
        va_list abii_vargs; \
        va_copy(abii_vargs, valist);

#define OVERRIDE_VALIST_SUFFIX(real_func, ret, fmt) \
    OVERRIDE_SUFFIX(real_func, ret)

#define DUMP_VARIADIC_ARGS(str, fmt) \
    va_start(abii_vargs, fmt); \
    (str) = abii::print_variadic_args(fmt, abii_vargs, OUTPUT_DECLARATION_ARGS); \
    va_end(abii_vargs);

#define PUSH_VARIADIC_ARGS(printer_name, format, ...) \
    va_start(abii_vargs, format); \
    auto (printer_name) = new ArgPrinter(abii_vargs, "..."); \
    (printer_name)->set_fmt(format); \
    (printer_name)->set_va_list_printer(__VA_ARGS__); \
    abii_args->push_arg(printer_name); \
    va_end(abii_vargs);

#define DUMP_VALIST_ARGS(str, fmt, valist) \
    va_copy(abii_vargs, valist); \
    (str) = abii::print_variadic_args(fmt, abii_vargs, OUTPUT_DECLARATION_ARGS);

#define PUSH_VALIST_ARGS(printer_name, format, valist, name, ...) \
    va_copy(abii_vargs, valist); \
    auto (printer_name) = new ArgPrinter(abii_vargs, name); \
    (printer_name)->set_fmt(format); \
    (printer_name)->set_va_list_printer(__VA_ARGS__); \
    abii_args->push_arg(printer_name);

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

namespace abii
{
struct VirtArgPrinter
{
    virtual ~VirtArgPrinter() = default;
    [[nodiscard]] virtual std::string get_name() const = 0;
    virtual void set_name(const std::string& name) = 0;
    [[nodiscard]] virtual std::ostream* get_os() const = 0;
    virtual void set_os(std::ostream* os) = 0;
    [[nodiscard]] virtual bool get_print_endl() const = 0;
    virtual void set_print_endl(bool print_endl) = 0;
    [[nodiscard]] virtual std::string get_value() const = 0;
    virtual void print_arg() = 0;
};

typedef std::string pre_fmtd_str;
typedef std::vector<VirtArgPrinter*> ArgsVector;

template<typename T = unsigned long long>
using defines_map = std::vector<std::pair<T, std::string>>;

extern thread_local bool redirect;
extern thread_local std::string prefix;
extern thread_local std::vector<uintptr_t> used_addrs;
extern thread_local std::ofstream abii_stream;

inline std::ostream& operator<<(std::ostream& os, const wchar_t& wc)
{
    const auto str = wide_to_narrow_char(wc);
    return os << str;
}

#if __HAVE_FLOAT128
inline std::ostream& operator<<(std::ostream& os, const _Float128& f)
{
    char buf[128];
    quadmath_snprintf(buf, sizeof(buf), "%.36Qg", f);
    return os << buf;
}
#endif

inline void* _get_real_symbol(const char* symbol_name)
{
    std::ifstream maps("/proc/self/maps");
    if (!maps.is_open())
    {
        std::cerr << "Failed to open /proc/self/maps\n";
        return nullptr;
    }

    std::string line;
    std::vector<std::string> candidate_paths;

    while (std::getline(maps, line))
    {
        if (auto pos = line.find("libc.so"); pos != std::string::npos)
        {
            std::istringstream iss(line);
            std::string path;
            if (std::string addr, perms, offset, dev, inode; !(iss >> addr >> perms >> offset >> dev >> inode))
                continue;
            std::getline(iss, path);
            // Remove leading spaces
            if (size_t start = path.find_first_not_of(' '); start != std::string::npos)
                path = path.substr(start);
            candidate_paths.push_back(path);
        }
    }

    for (auto& path: candidate_paths)
    {
        void* handle = dlopen(path.c_str(), RTLD_NOW | RTLD_NOLOAD);
        if (!handle)
            continue;

        if (void* sym = dlsym(handle, symbol_name))
        {
            dlclose(handle);
            return sym;
        }
        dlclose(handle);
    }

    return nullptr;
}

template<typename T>
bool bomb_detector(T* ptr, size_t size = 0)
{
    if (ptr == nullptr)
        return false;

    int pipefds[2];
    if (pipe(pipefds) == -1)
    {
        std::cerr << "Pipe creation failed" << std::endl;
        return false;
    }

    const int flags = fcntl(pipefds[1], F_GETFL, 0);
    fcntl(pipefds[1], F_SETFL, flags | O_NONBLOCK);

    const ssize_t nbytes = write(pipefds[1], (void*) ptr, size != 0 ? size : sizeof(T));

    close(pipefds[0]);
    close(pipefds[1]);

    return nbytes == (size != 0 ? size : sizeof(T));
}

template<>
inline bool bomb_detector<char>(char* ptr, size_t size)
{
    if (ptr == nullptr)
        return false;

    do
    {
        int pipefds[2];
        if (pipe(pipefds) == -1)
        {
            std::cerr << "Pipe creation failed" << std::endl;
            return false;
        }

        const int flags = fcntl(pipefds[1], F_GETFL, 0);
        fcntl(pipefds[1], F_SETFL, flags | O_NONBLOCK);
        const ssize_t nbytes = write(pipefds[1], ptr, size != 0 ? size : sizeof(char));
        close(pipefds[0]);
        close(pipefds[1]);
        if (nbytes != (size != 0 ? size : sizeof(char)))
            return false;
        if (*ptr == 0)
            break;
    } while (*ptr++ != 0 && size == 0);

    return true;
}

template<>
inline bool bomb_detector<const char>(const char* ptr, size_t size)
{
    if (ptr == nullptr)
        return false;

    do
    {
        int pipefds[2];
        if (pipe(pipefds) == -1)
        {
            std::cerr << "Pipe creation failed" << std::endl;
            return false;
        }

        const int flags = fcntl(pipefds[1], F_GETFL, 0);
        fcntl(pipefds[1], F_SETFL, flags | O_NONBLOCK);
        const ssize_t nbytes = write(pipefds[1], ptr, size != 0 ? size : sizeof(const char));
        close(pipefds[0]);
        close(pipefds[1]);
        if (nbytes != (size != 0 ? size : sizeof(const char)))
            return false;
        if (*ptr == 0)
            break;
    } while (*ptr++ != 0 && size == 0);

    return true;
}

inline std::string demangle(const std::string& name)
{
    const auto t = __cxxabiv1::__cxa_demangle(name.c_str(), nullptr, nullptr, nullptr);
    if (t == nullptr)
        return name;
    return t;
}

template<typename T>
std::string get_type([[maybe_unused]] T& i) requires std::is_polymorphic_v<T>
{
    const void* vptr = *(void**) &i;
    if (const void* rtti = vptr ? ((void**) vptr)[-1] : nullptr; rtti != nullptr)
        return demangle(typeid(i).name());
    return demangle(typeid(T).name());
}

template<typename T>
std::string get_type([[maybe_unused]] T& i) requires (!std::is_polymorphic_v<T>)
{
    return demangle(typeid(i).name());
}

template<typename T>
std::string get_type([[maybe_unused]] const T& i) requires (std::is_const_v<T> && std::is_polymorphic_v<T>)
{
    const void* vptr = *(void**) &i;
    if (const void* rtti = vptr ? ((void**) vptr)[-1] : nullptr; rtti != nullptr)
        return demangle(typeid(i).name());
    return demangle(typeid(T).name());
}

template<typename T>
std::string get_type([[maybe_unused]] const T& i) requires (std::is_const_v<T> && !std::is_polymorphic_v<T>)
{
    return demangle(typeid(i).name());
}

inline std::string get_symbol_name(const void* ptr)
{
    if (ptr == nullptr)
        return "";
    Dl_info info;
    if (dladdr(ptr, &info) && info.dli_sname != nullptr && info.dli_saddr != nullptr)
        return demangle(info.dli_sname);
    return "";
}

template<typename T, typename... defines_maps>
std::string print_enum_entry(const T v, const defines_maps&... maps)
{
    std::stringstream ss;
    auto first = true;

    auto search_in_map = [&](const auto& defines) {
        for (const auto& [define, str]: defines)
            if (v == (T) define)
            {
                ss << (first ? "" : " & ") << str;
                first = false;
            }
    };

    (search_in_map(maps), ...);

    if (first)
        return "";
    return ss.str();
}

template<typename... defines_maps>
std::string print_enum_entry(const char* v, const defines_maps&... maps)
{
    std::stringstream ss;
    auto first = true;

    auto search_in_map = [&](const auto& defines) {
        for (const auto& [define, str]: defines)
            if (strcmp(v, (const char*) define) == 0)
            {
                ss << (first ? "" : " & ") << str;
                first = false;
            }
    };

    (search_in_map(maps), ...);

    if (first)
        return "";
    return ss.str();
}

template<typename T, typename... defines_maps>
std::string print_or_enum_entries(const T v, const defines_maps&... maps)
{
    std::stringstream ss;
    auto first = true;

    auto search_in_map = [&](const auto& defines) {
        for (const auto& [define, str]: defines)
            if (v == (T) define || ((T) define > 0 && v >= (T) define && (v & (T) define) == (T) define))
            {
                ss << (first ? "" : " | ") << str;
                first = false;
            }
    };

    (search_in_map(maps), ...);

    if (first)
        return "";
    return ss.str();
}

inline void replace_all(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Move past the replaced part to avoid infinite loop
    }
}

inline std::string print_diff(const std::string& arg1, const std::string& arg2)
{
    std::stringstream ss;
    const auto lines1 = get_lines(arg1);
    const auto lines2 = get_lines(arg2);

    auto lcs = findLCS(lines1, lines2);

    auto i = 0, j = 0;
    auto first = true;

    auto print_line = [&](const std::string& line, const std::string& prefix) {
        if (!first) ss << std::endl;
        first = false;
        ss << prefix << line;
    };

    for (const auto& [idx1, idx2, identical]: lcs)
    {
        while (i < idx1 && j < idx2)
        {
            print_line(lines1[i++] + " --> " + strip(lines2[j++], '\t', FRONT), "");
        }
        while (i < idx1)
        {
            print_line(lines1[i++] + " --> ", "");
        }
        while (j < idx2)
        {
            std::string temp_prefix;
            for (auto k = 0; lines2[j][k] == '\t'; ++k) temp_prefix += '\t';
            print_line(" --> " + strip(lines2[j++], '\t', FRONT), temp_prefix);
        }
        if (!identical)
        {
            print_line(lines1[idx1] + " --> " + strip(lines2[idx2], '\t', FRONT), "");
        } else
        {
            print_line(lines1[idx1], "");
        }
        ++i;
        ++j;
    }

    while (i < lines1.size() || j < lines2.size())
    {
        if (i < lines1.size())
        {
            print_line(lines1[i++] + " --> ", "");
        }
        if (j < lines2.size())
        {
            std::string temp_prefix;
            for (auto k = 0; lines2[j][k] == '\t'; ++k) temp_prefix += '\t';
            print_line(" --> " + strip(lines2[j++], '\t', FRONT), temp_prefix);
        }
    }

    if (ends_with(arg1, '\n') || ends_with(arg2, '\n')) ss << std::endl;
    auto str = ss.str();
    return ss.str();
}

struct ArgsPrinter
{
    void push_arg(VirtArgPrinter* arg)
    {
        std::stringstream ss;
        std::ostream* os = arg->get_os();
        arg->set_os(&ss);
        arg->print_arg();
        args_.emplace_back(arg, ss.str(), os);
    }

    void push_func(VirtArgPrinter* arg)
    {
        func_ = arg;
        func_->set_print_endl(false);
        func_->print_arg();
        prefix += '\t';
    }

    void push_return(VirtArgPrinter* ret)
    {
        ret_val_ = ret->get_value();
        ret_ = ret;
    }

    void print_args()
    {
        if (func_ != nullptr)
        {
            if (!ret_val_.empty())
                *func_->get_os() << " = " << ret_val_;
            *func_->get_os() << std::endl;
        }
        std::ranges::for_each(args_, [&](const auto& arg) {
            std::stringstream ss2;
            std::get<0>(arg)->set_os(&ss2);
            std::get<0>(arg)->print_arg();

            *std::get<2>(arg) << print_diff(std::get<1>(arg), ss2.str());
        });
        if (ret_ != nullptr)
            ret_->print_arg();
    }

    std::string ret_val_;
    VirtArgPrinter* func_ = nullptr;
    VirtArgPrinter* ret_ = nullptr;
    std::vector<std::tuple<VirtArgPrinter*, std::string, std::ostream*>> args_{};
};

inline void print_args(ArgsVector args)
{
    std::ranges::for_each(args, [&](const auto& arg) { arg->print_arg(); });
}

struct ReferenceType
{
    virtual ~ReferenceType() = default;
    virtual size_t get_ref() = 0;
};

template<typename T>
struct Reference final : ReferenceType
{
    explicit Reference(T& ref) : ref_(ref) {}

    size_t get_ref() override
    {
        return ref_;
    }

private:
    T& ref_;
};
}

#include "ArgPrinter.tpp"

#endif //LIBABII_H
