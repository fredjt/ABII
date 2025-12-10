//
// Created by Trent Tanchin on 7/14/24.
//

#ifndef ARGPRINTER_H
#define ARGPRINTER_H

#include <fstream>
#include <functional>
#include <map>
#include <utility>

#define PRINT_ENDL 0x1
#define RECURSE 0x2
#define set_enum_printer(enum_printer, obj_or_type) set_enum_printer_<decltype(obj_or_type)>(enum_printer<decltype(obj_or_type)>) // NOLINT(*-macro-parentheses)



namespace abii
{
/**
 * Primary template class for ArgPrinter
 *
 * @tparam T Type of object to be printed
 *
 * @struct ArgPrinter ArgPrinter.tpp
 */
template <typename T>
struct ArgPrinter final : VirtArgPrinter
{
    explicit ArgPrinter(T& arg, const std::string& name = "", std::ostream* os = &abii_stream, const int flags = 1) :
        arg_(arg), name_(name), print_endl_(flags & PRINT_ENDL), os_(os) {}

    explicit ArgPrinter(T&& arg, const std::string& name = "", std::ostream* os = &abii_stream, const int flags = 1) :
        arg_(rval_arg_), rval_arg_(arg), name_(name), print_endl_(flags & PRINT_ENDL), os_(os) {}

    ~ArgPrinter() override = default;

    [[nodiscard]] std::string get_name() const override { return name_; }
    void set_name(const std::string& name) override { name_ = name; }

    template <typename V>
    [[nodiscard]] std::string enum_printer(V& arg) const
    {
        return enum_printers_.contains(depth_)
                   ? enum_printers_.find(depth_)->second(reinterpret_cast<const void*>(&arg))
                   : "";
    }

    [[nodiscard]] std::function<std::string(void*)> get_enum_printer() const
    {
        return enum_printers_.contains(depth_) ? enum_printers_.find(depth_)->second : nullptr;
    }

    template <typename V>
    void set_enum_printer_(const std::function<std::string(V)>& enum_printer, size_t depth = 0)
    {
        enum_printers_.insert({
            depth, [enum_printer](const void* arg) -> std::string { return enum_printer(*static_cast<const V*>(arg)); }
        });
    }

    [[nodiscard]] bool get_print_endl() const override { return print_endl_; }
    void set_print_endl(const bool print_endl) override { print_endl_ = print_endl; }
    [[nodiscard]] std::ostream* get_os() const override { return os_; }
    void set_os(std::ostream* os) override { os_ = os; }

    [[nodiscard]] std::string get_value() const override
    {
        std::stringstream ss;
        ss << arg_;
        return ss.str();
    }

    void print_arg() override;

    ArgPrinter(T& arg, const std::string& name, const size_t previous_depth,
               const std::map<size_t, std::function<std::string(const void*)>>& enum_printers,
               std::ostream* os = &abii_stream, const int flags = 1) : arg_(arg), name_(name),
                                                                       enum_printers_(enum_printers),
                                                                       depth_(previous_depth + 1),
                                                                       print_endl_(flags & PRINT_ENDL), os_(os) {}

private:
    T& arg_;
    T rval_arg_;
    std::string name_;
    std::map<size_t, std::function<std::string(const void*)>> enum_printers_;
    size_t depth_ = 0;
    bool print_endl_ = true;
    std::ostream* os_;
};

/**
 * Template specialization of template class ArgPrinter for const-qualified types
 *
 * @tparam T Type of object to be printed
 *
 * @struct ArgPrinter libabii.h
 */
template <typename T>
struct ArgPrinter<const T>final : VirtArgPrinter
{
    explicit ArgPrinter(const T& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 1) : arg_(arg), name_(name), print_endl_(flags & PRINT_ENDL), os_(os) {}

    explicit ArgPrinter(const T&& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 1) : arg_(rval_arg_), rval_arg_(arg), name_(name),
                                               print_endl_(flags & PRINT_ENDL), os_(os) {}

    ~ArgPrinter() override = default;

    [[nodiscard]] std::string get_name() const override { return name_; }
    void set_name(const std::string& name) override { name_ = name; }

    template <typename V>
    [[nodiscard]] std::string enum_printer(V& arg) const
    {
        return enum_printers_.contains(depth_)
                   ? enum_printers_.find(depth_)->second(reinterpret_cast<const void*>(&arg))
                   : "";
    }

    [[nodiscard]] std::function<std::string(void*)> get_enum_printer() const
    {
        return enum_printers_.contains(depth_) ? enum_printers_.find(depth_)->second : nullptr;
    }

    template <typename V>
    void set_enum_printer_(const std::function<std::string(V)>& enum_printer, size_t depth = 0)
    {
        enum_printers_.insert({
            depth, [enum_printer](const void* arg) -> std::string { return enum_printer(*static_cast<const V*>(arg)); }
        });
    }

    [[nodiscard]] bool get_print_endl() const override { return print_endl_; }
    void set_print_endl(const bool print_endl) override { print_endl_ = print_endl; }
    [[nodiscard]] std::ostream* get_os() const override { return os_; }
    void set_os(std::ostream* os) override { os_ = os; }

    [[nodiscard]] std::string get_value() const override
    {
        std::stringstream ss;
        ss << arg_;
        return ss.str();
    }

    void print_arg() override;

    // internal usage
    ArgPrinter(const T& arg, const std::string& name, const size_t previous_depth,
               const std::map<size_t, std::function<std::string(const void*)>>& enum_printers,
               std::ostream* os = &abii_stream, const int flags = 1) : arg_(arg), name_(name),
                                                                       enum_printers_(enum_printers),
                                                                       depth_(previous_depth + 1),
                                                                       print_endl_(flags & PRINT_ENDL), os_(os) {}

private:
    const T& arg_;
    const T rval_arg_ = arg_;
    std::string name_;
    std::map<size_t, std::function<std::string(const void*)>> enum_printers_;
    size_t depth_ = 0;
    bool print_endl_ = true;
    std::ostream* os_;
};

/*
 * ArgPrinter<T>::print_arg() definition and specializations
 */

/**
 * ArgPrinter<T>::print_arg() - Prints an object's contents to @code *stream @endcode
 *
 * @tparam T The type of the object to be printed
 */
template <typename T>
void ArgPrinter<T>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<char>::print_arg() - Prints a char and a string representation of the char to @code *stream @endcode
 */
template <>
inline void ArgPrinter<char>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<unsigned int>(static_cast<unsigned char>(
        arg_));
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (arg_ == 8)
        *os_ << " {BS}";
    else if (arg_ == '\n')
        *os_ << " {\\n}";
    else
        *os_ << " {" << arg_ << "}";

    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<char>::print_arg() - Prints a char and a string representation of the char to @code *stream @endcode
 */
template <>
inline void ArgPrinter<wchar_t>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<unsigned int>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (arg_ == 8)
        *os_ << " {BS}";
    else if (arg_ == '\n')
        *os_ << " {\\n}";
    else
        *os_ << " {" << arg_ << "}";

    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<char>::print_arg() - Prints an unsigned char and a string representation of the unsigned char to @code *stream @endcode
 */
template <>
inline void ArgPrinter<unsigned char>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<unsigned int>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (arg_ == 8)
        *os_ << " {BS}";
    else if (arg_ == '\n')
        *os_ << " {\\n}";
    else
        *os_ << " {" << arg_ << "}";

    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<int>::print_arg() - Prints a pre-formatted string to @code *stream @endcode
 */
template <>
inline void ArgPrinter<pre_fmtd_str>::print_arg()
{
    *os_ << prefix << arg_;
    if (print_endl_)
        *os_ << std::endl;
}

/*
 * ArgPrinter<T>::print_arg() definition and specializations
 */

/**
 * ArgPrinter<const T>::print_arg() - Prints an object's contents to @code *stream @endcode
 *
 * @tparam T The type of the object to be printed
 */
template <typename T>
void ArgPrinter<const T>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";

    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<char>::print_arg() - Prints a char and a string representation of the char to @code *stream @endcode
 */
template <>
inline void ArgPrinter<const char>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<unsigned int>(static_cast<unsigned char>(
        arg_));
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (arg_ == 8)
        *os_ << " {BS}";
    else if (arg_ == '\n')
        *os_ << " {\\n}";
    else
        *os_ << " {" << arg_ << "}";

    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<char>::print_arg() - Prints a char and a string representation of the char to @code *stream @endcode
 */
template <>
inline void ArgPrinter<const wchar_t>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<unsigned int>(static_cast<unsigned char>(
        arg_));
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (arg_ == 8)
        *os_ << " {BS}";
    else if (arg_ == '\n')
        *os_ << " {\\n}";
    else
        *os_ << " {" << arg_ << "}";

    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<char>::print_arg() - Prints an unsigned char and a string representation of the unsigned char to @code *stream @endcode
 */
template <>
inline void ArgPrinter<const unsigned char>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<unsigned int>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (arg_ == 8)
        *os_ << " {BS}";
    else if (arg_ == '\n')
        *os_ << " {\\n}";
    else
        *os_ << " {" << arg_ << "}";

    if (print_endl_)
        *os_ << std::endl;
}
}

#include "ArgPrinterPointer.tpp"
// ReSharper disable once CppWrongIncludesOrder
#include "ArgPrinterArray.tpp"
#include "ArgPrinterFunction.tpp"

#endif //ARGPRINTER_H
