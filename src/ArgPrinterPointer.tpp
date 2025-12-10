//
// Created by Trent Tanchin on 7/14/24.
//

#ifndef ARGPRINTERPOINTER_H
#define ARGPRINTERPOINTER_H

#include <dirent.h>

namespace abii
{
/**
 * Template specialization of template class ArgPrinter for pointer types
 *
 * @tparam T Underlying type of object pointed to by the object this class holds
 *
 * @struct ArgPrinter libabii.h
 */
template <typename T>
struct ArgPrinter<T*>final : VirtArgPrinter
{
    explicit ArgPrinter(T*& arg, const std::string& name = "", std::ostream* os = &abii_stream, const int flags = 3) :
        arg_(arg), name_(name), print_endl_(flags & PRINT_ENDL), recurse_(flags & RECURSE), os_(os) {}

    explicit ArgPrinter(T*&& arg, const std::string& name = "", std::ostream* os = &abii_stream, const int flags = 3) :
        arg_(rval_arg_), rval_arg_(arg), name_(name), print_endl_(flags & PRINT_ENDL), recurse_(flags & RECURSE),
        os_(os) {}

    ~ArgPrinter() override = default;

    [[nodiscard]] std::string get_name() const override { return name_; }
    void set_name(const std::string& name) override { name_ = name; }

    template <typename V>
    [[nodiscard]] V get_len() const { return len_->get_ref(); }

    template <typename V>
    void set_len(V& len) { len_ = new Reference<V>(len); }

    [[nodiscard]] std::string get_fmt() const { return fmt_; }
    void set_fmt(const std::string& fmt) { fmt_ = fmt; }

    [[nodiscard]] std::function<bool(size_t)> get_end_test() const { return end_test_; }

    void set_end_test(const std::function<bool(size_t)>& end_test) { end_test_ = end_test; }

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

    [[nodiscard]] std::string va_list_printer(const char* fmt, va_list args, const size_t size = 0) const
    {
        return va_list_printers_.contains(depth_) ? va_list_printers_.find(depth_)->second(fmt, args, size) : "";
    }

    [[nodiscard]] std::function<std::string(const char*, va_list, size_t)> get_va_list_printer() const
    {
        return va_list_printers_.contains(depth_) ? va_list_printers_.find(depth_)->second : nullptr;
    }

    void set_va_list_printer(const std::function<std::string(const char*, va_list, size_t)>& va_list_printer,
                             const size_t size = 0)
    {
        va_list_printers_.insert({
            depth_,
            [va_list_printer](const char* fmt, va_list args, const size_t size) -> std::string
            {
                return va_list_printer(fmt, args, size);
            }
        });
        va_list_printer_buf_size_ = size;
    }

    [[nodiscard]] bool get_print_endl() const override { return print_endl_; }
    void set_print_endl(const bool print_endl) override { print_endl_ = print_endl; }
    [[nodiscard]] bool get_recurse() const { return recurse_; }
    void set_recurse(const bool recurse) { recurse_ = recurse; }
    [[nodiscard]] std::ostream* get_os() const override { return os_; }
    void set_os(std::ostream* os) override { os_ = os; }

    [[nodiscard]] std::string get_value() const override
    {
        std::stringstream ss;
        ss << arg_;
        return ss.str();
    }

    void print_arg() override;

    ArgPrinter(T*& arg, const std::string& name, const size_t previous_depth,
               const std::map<size_t, std::function<std::string(const void*)>>& enum_printers,
               std::ostream* os = &abii_stream, const int flags = 1)
        : arg_(arg), name_(name), enum_printers_(enum_printers), depth_(previous_depth + 1),
          print_endl_(flags & PRINT_ENDL), recurse_(flags & RECURSE), os_(os) {}

private:
    T*& arg_;
    T* rval_arg_ = nullptr;
    std::string name_;
    ReferenceType* len_ = new Reference(def_len_);
    std::string fmt_;
    std::function<bool(size_t)> end_test_ = [&](const int i) { return i < len_->get_ref(); };
    std::map<size_t, std::function<std::string(const void*)>> enum_printers_;
    std::map<size_t, std::function<std::string(const char*, va_list, size_t)>> va_list_printers_;
    size_t va_list_printer_buf_size_ = 0;
    size_t depth_ = 0;
    bool print_endl_ = true;
    bool recurse_ = true;
    std::ostream* os_;
    static size_t def_len_;
};

template <typename T>
size_t ArgPrinter<T*>::def_len_ = 0;

/**
 * Template specialization of template class ArgPrinter for const-qualified pointer types
 *
 * @tparam T Underlying type of object pointed to by the object this class holds
 *
 * @struct ArgPrinter libabii.h
 */
template <typename T>
struct ArgPrinter<const T*>final : VirtArgPrinter
{
    explicit ArgPrinter(const T*& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(arg), name_(name), print_endl_(flags & PRINT_ENDL),
                                               recurse_(flags & RECURSE), os_(os) {}

    explicit ArgPrinter(const T*&& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(rval_arg_), rval_arg_(arg), name_(name),
                                               print_endl_(flags & PRINT_ENDL), recurse_(flags & RECURSE), os_(os) {}

    ~ArgPrinter() override = default;

    [[nodiscard]] std::string get_name() const override { return name_; }
    void set_name(const std::string& name) override { name_ = name; }

    template <typename V>
    [[nodiscard]] V get_len() const { return len_->get_ref(); }

    template <typename V>
    void set_len(V& len) { len_ = new Reference<V>(len); }

    [[nodiscard]] std::function<bool(size_t)> get_end_test() const { return end_test_; }

    void set_end_test(const std::function<bool(size_t)>& end_test) { end_test_ = end_test; }

    template <typename V>
    [[nodiscard]] std::string enum_printer(const V& arg) const
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
    [[nodiscard]] bool get_recurse() const { return recurse_; }
    void set_recurse(const bool recurse) { recurse_ = recurse; }
    [[nodiscard]] std::ostream* get_os() const override { return os_; }
    void set_os(std::ostream* os) override { os_ = os; }

    [[nodiscard]] std::string get_value() const override
    {
        std::stringstream ss;
        ss << reinterpret_cast<const void*>(arg_);
        return ss.str();
    }

    void print_arg() override;

    ArgPrinter(const T*& arg, const std::string& name, const size_t previous_depth,
               const std::map<size_t, std::function<std::string(const void*)>>& enum_printers,
               std::ostream* os = &abii_stream, const int flags = 1)
        : arg_(arg), name_(name), enum_printers_(enum_printers), depth_(previous_depth + 1),
          print_endl_(flags & PRINT_ENDL), recurse_(flags & RECURSE), os_(os) {}

private:
    const T*& arg_;
    const T* rval_arg_ = nullptr;
    std::string name_;
    ReferenceType* len_ = new Reference(def_len_);
    std::function<bool(size_t)> end_test_ = [&](const int i) { return i < len_->get_ref(); };
    std::map<size_t, std::function<std::string(const void*)>> enum_printers_;
    size_t depth_ = 0;
    bool print_endl_ = true;
    bool recurse_ = true;
    std::ostream* os_;
    static size_t def_len_;
};

template <typename T>
size_t ArgPrinter<const T*>::def_len_ = 0;

/**
 * Template specialization of template class ArgPrinter for pointer types
 *
 * @tparam T Underlying type of object pointed to by the object this class holds
 *
 * @struct ArgPrinter libabii.h
 */
template <typename T>
struct ArgPrinter<T* const>final : VirtArgPrinter
{
    explicit ArgPrinter(T* const& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(arg), name_(name), print_endl_(flags & PRINT_ENDL),
                                               recurse_(flags & RECURSE), os_(os) {}

    explicit ArgPrinter(T* const&& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(rval_arg_), rval_arg_(arg), name_(name),
                                               print_endl_(flags & PRINT_ENDL), recurse_(flags & RECURSE), os_(os) {}

    ~ArgPrinter() override = default;

    [[nodiscard]] std::string get_name() const override { return name_; }
    void set_name(const std::string& name) override { name_ = name; }

    template <typename V>
    [[nodiscard]] V get_len() const { return len_->get_ref(); }

    template <typename V>
    void set_len(V& len) { len_ = new Reference<V>(len); }

    [[nodiscard]] std::function<bool(size_t)> get_end_test() const { return end_test_; }

    void set_end_test(const std::function<bool(size_t)>& end_test) { end_test_ = end_test; }

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
    [[nodiscard]] bool get_recurse() const { return recurse_; }
    void set_recurse(const bool recurse) { recurse_ = recurse; }
    [[nodiscard]] std::ostream* get_os() const override { return os_; }
    void set_os(std::ostream* os) override { os_ = os; }

    [[nodiscard]] std::string get_value() const override
    {
        std::stringstream ss;
        ss << reinterpret_cast<void*>(arg_);
        return ss.str();
    }

    void print_arg() override;

    ArgPrinter(T* const& arg, const std::string& name, const size_t previous_depth,
               const std::map<size_t, std::function<std::string(const void*)>>& enum_printers,
               std::ostream* os = &abii_stream, const int flags = 1)
        : arg_(arg), name_(name), enum_printers_(enum_printers), depth_(previous_depth + 1),
          print_endl_(flags & PRINT_ENDL), recurse_(flags & RECURSE), os_(os) {}

private:
    T* const& arg_;
    T* const rval_arg_ = nullptr;
    std::string name_;
    ReferenceType* len_ = new Reference(def_len_);
    std::function<bool(size_t)> end_test_ = [&](const int i) { return i < len_->get_ref(); };
    std::map<size_t, std::function<std::string(const void*)>> enum_printers_;
    size_t depth_ = 0;
    bool print_endl_ = true;
    bool recurse_ = true;
    std::ostream* os_ = &abii_stream;
    static size_t def_len_;
};

template <typename T>
size_t ArgPrinter<T* const>::def_len_ = 0;

/**
 * Template specialization of template class ArgPrinter for const-qualified pointer types
 *
 * @tparam T Underlying type of object pointed to by the object this class holds
 *
 * @struct ArgPrinter libabii.h
 */
template <typename T>
struct ArgPrinter<const T* const>final : VirtArgPrinter
{
    explicit ArgPrinter(const T* const& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(arg), name_(name), print_endl_(flags & PRINT_ENDL),
                                               recurse_(flags & RECURSE), os_(os) {}

    explicit ArgPrinter(const T* const&& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(rval_arg_), rval_arg_(arg), name_(name),
                                               print_endl_(flags & PRINT_ENDL), recurse_(flags & RECURSE), os_(os) {}

    ~ArgPrinter() override = default;

    [[nodiscard]] std::string get_name() const override { return name_; }
    void set_name(const std::string& name) override { name_ = name; }

    template <typename V>
    [[nodiscard]] V get_len() const { return len_->get_ref(); }

    template <typename V>
    void set_len(V& len) { len_ = new Reference<V>(len); }

    [[nodiscard]] std::function<bool(size_t)> get_end_test() const { return end_test_; }

    void set_end_test(const std::function<bool(size_t)>& end_test) { end_test_ = end_test; }

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
    [[nodiscard]] bool get_recurse() const { return recurse_; }
    void set_recurse(const bool recurse) { recurse_ = recurse; }
    [[nodiscard]] std::ostream* get_os() const override { return os_; }
    void set_os(std::ostream* os) override { os_ = os; }

    [[nodiscard]] std::string get_value() const override
    {
        std::stringstream ss;
        ss << reinterpret_cast<const void*>(arg_);
        return ss.str();
    }

    void print_arg() override;

    ArgPrinter(const T* const& arg, const std::string& name, const size_t previous_depth,
               const std::map<size_t, std::function<std::string(const void*)>>& enum_printers,
               std::ostream* os = &abii_stream, const int flags = 1)
        : arg_(arg), name_(name), enum_printers_(enum_printers), depth_(previous_depth + 1),
          print_endl_(flags & PRINT_ENDL), recurse_(flags & RECURSE), os_(os) {}

private:
    const T* const& arg_;
    const T* const rval_arg_ = nullptr;
    std::string name_;
    ReferenceType* len_ = new Reference(def_len_);
    std::function<bool(size_t)> end_test_ = [&](const int i) { return i < len_->get_ref(); };
    std::map<size_t, std::function<std::string(const void*)>> enum_printers_;
    size_t depth_ = 0;
    bool print_endl_ = true;
    bool recurse_ = true;
    std::ostream* os_ = &abii_stream;
    static size_t def_len_;
};

template <typename T>
size_t ArgPrinter<const T* const>::def_len_ = 0;

/*
 * ArgPrinter<T*>::print_arg() definition and specializations
 */

/**
 * ArgPrinter<T*>::print_arg() - Prints a pointer and its contents to @code *os_ @endcode
 *
 * @tparam T The type of the object pointed to to be printed
 */
template <typename T>
void ArgPrinter<T*>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << reinterpret_cast<void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (depth_ != -1)
        --depth_;
    if (const auto name = get_symbol_name(reinterpret_cast<void*>(arg_)); !name.empty())
        *os_ << " (" << name << ")";
    if (recurse_ && bomb_detector(arg_, len_->get_ref()))
    {
        *os_ << std::endl;
        const auto old_prefix = prefix;
        prefix += "\t";
        if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
            *os_ << prefix << "[RECURSION]";
        else
        {
            used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
            if (len_->get_ref() != 0)
                for (auto i = 0; end_test_(i); ++i)
                {
                    std::stringstream ss;
                    ss << name_ << "[" << i << "]";
                    const auto next = new ArgPrinter<T>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                    if (!end_test_(i + 1))
                        next->set_print_endl(false);

                    next->print_arg();
                }
            else
            {
                const auto next = new ArgPrinter<T>(*arg_, "*" + name_, depth_, enum_printers_, os_);
                next->set_print_endl(false);

                next->print_arg();
            }
            used_addrs.pop_back();
        }
        prefix = old_prefix;
    }
    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<char*>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ")";
#ifdef BIT32
    if (!fmt_.empty())
    {
        const auto old_prefix = prefix;
        prefix += "\t";
        std::stringstream ss;
        ss << va_list_printer(fmt_.c_str(), arg_, va_list_printer_buf_size_);
        prefix = old_prefix;
        if (std::string str = ss.str(); !str.empty())
        {
            if (!print_endl_)
                str.pop_back();
            *os_ << std::endl << str;
        }
    }
    else
    {



#endif
    *os_ << " " << reinterpret_cast<void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (depth_ != -1)
        --depth_;
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        std::string arg = arg_;
        replace_all(arg, "\n", "\\n");
        *os_ << " {" << arg << "}";
        if (recurse_)
        {
            *os_ << std::endl;
            const auto old_prefix = prefix;
            prefix += "\t";
            if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                *os_ << prefix << "[RECURSION]";
            else
            {
                used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                {
                    std::stringstream ss;
                    ss << name_ << "[" << i << "]";
                    const auto next = new ArgPrinter<char>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                    if (i == len_->get_ref() - 1 || (len_->get_ref() == 0 && arg_[i] == 0))
                        next->set_print_endl(false);

                    next->print_arg();
                }
                used_addrs.pop_back();
            }
            prefix = old_prefix;
        }
    }
    if (print_endl_)
        *os_ << std::endl;
#ifdef BIT32
    }
#endif
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<wchar_t*>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << reinterpret_cast<void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (depth_ != -1)
        --depth_;
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        auto arg = wide_to_narrow_str(arg_);
        replace_all(arg, "\n", "\\n");
        *os_ << " {" << arg << "}";
        if (recurse_)
        {
            *os_ << std::endl;
            const auto old_prefix = prefix;
            prefix += "\t";
            if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                *os_ << prefix << "[RECURSION]";
            else
            {
                used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                {
                    std::stringstream ss;
                    ss << name_ << "[" << i << "]";
                    const auto next = new ArgPrinter<wchar_t>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                    if (i == len_->get_ref() - 1 || (len_->get_ref() == 0 && arg_[i] == 0))
                        next->set_print_endl(false);

                    next->print_arg();
                }
                used_addrs.pop_back();
            }
            prefix = old_prefix;
        }
    }
    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<FILE*>::print_arg() - Prints a FILE pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<FILE*>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<FILE*>::print_arg() - Prints a DIR pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<DIR*>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<void*>::print_arg() - Prints a void pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<void*>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (print_endl_)
        *os_ << std::endl;
}

/*
 * ArgPrinter<const T*>::print_arg() definition and specializations
 */

/**
 * ArgPrinter<const T*>::print_arg() - Prints a pointer and its contents to @code *stream @endcode
 *
 * @tparam T The type of the object pointed to to be printed
 */
template <typename T>
void ArgPrinter<const T*>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << reinterpret_cast<const void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (depth_ != -1)
        --depth_;
    if (const auto name = get_symbol_name(reinterpret_cast<const void*>(arg_)); !name.empty())
        *os_ << " (" << name << ")";
    if (recurse_ && bomb_detector(arg_, len_->get_ref()))
    {
        *os_ << std::endl;
        const auto old_prefix = prefix;
        prefix += "\t";
        if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
            *os_ << prefix << "[RECURSION]";
        else
        {
            used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
            if (len_->get_ref() != 0)
                for (auto i = 0; end_test_(i); ++i)
                {
                    std::stringstream ss;
                    ss << name_ << "[" << i << "]";
                    const auto next = new ArgPrinter<const T>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                    if (!end_test_(i + 1))
                        next->set_print_endl(false);

                    next->print_arg();
                }
            else
            {
                const auto next = new ArgPrinter<const T>(*arg_, "*" + name_, depth_, enum_printers_, os_);
                next->set_print_endl(false);

                next->print_arg();
            }
            used_addrs.pop_back();
        }
        prefix = old_prefix;
    }
    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<const char*>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << reinterpret_cast<const void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (depth_ != -1)
        --depth_;
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        std::string arg = arg_;
        replace_all(arg, "\n", "\\n");
        *os_ << " {" << arg << "}";
        if (recurse_)
        {
            *os_ << std::endl;
            const auto old_prefix = prefix;
            prefix += "\t";
            if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                *os_ << prefix << "[RECURSION]";
            else
            {
                used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                {
                    std::stringstream ss;
                    ss << name_ << "[" << i << "]";
                    const auto next = new ArgPrinter<const char>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                    if (i == len_->get_ref() - 1 || (len_->get_ref() == 0 && arg_[i] == 0))
                        next->set_print_endl(false);

                    next->print_arg();
                }
                used_addrs.pop_back();
            }
            prefix = old_prefix;
        }
    }
    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<const wchar_t*>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << reinterpret_cast<const void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (depth_ != -1)
        --depth_;
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        auto arg = wide_to_narrow_str(arg_);
        replace_all(arg, "\n", "\\n");
        *os_ << " {" << arg << "}";
        if (recurse_)
        {
            *os_ << std::endl;
            const auto old_prefix = prefix;
            prefix += "\t";
            if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                *os_ << prefix << "[RECURSION]";
            else
            {
                used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                {
                    std::stringstream ss;
                    ss << name_ << "[" << i << "]";
                    const auto next = new ArgPrinter<const wchar_t>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                    if (i == len_->get_ref() - 1 || (len_->get_ref() == 0 && arg_[i] == 0))
                        next->set_print_endl(false);

                    next->print_arg();
                }
                used_addrs.pop_back();
            }
            prefix = old_prefix;
        }
    }
    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<void*>::print_arg() - Prints a void pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<const void*>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (print_endl_)
        *os_ << std::endl;
}

/*
 * ArgPrinter<T* const>::print_arg() definition and specializations
 */

/**
 * ArgPrinter<T*>::print_arg() - Prints a pointer and its contents to @code *stream @endcode
 *
 * @tparam T The type of the object pointed to to be printed
 */
template <typename T>
void ArgPrinter<T* const>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << reinterpret_cast<void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (depth_ != -1)
        --depth_;
    if (const auto name = get_symbol_name(reinterpret_cast<void*>(arg_)); !name.empty())
        *os_ << " (" << name << ")";
    if (recurse_ && bomb_detector(arg_, len_->get_ref()))
    {
        *os_ << std::endl;
        const auto old_prefix = prefix;
        prefix += "\t";
        if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
            *os_ << prefix << "[RECURSION]";
        else
        {
            used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
            if (len_->get_ref() != 0)
                for (auto i = 0; end_test_(i); ++i)
                {
                    std::stringstream ss;
                    ss << name_ << "[" << i << "]";
                    const auto next = new ArgPrinter<T>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                    if (!end_test_(i + 1))
                        next->set_print_endl(false);

                    next->print_arg();
                }
            else
            {
                const auto next = new ArgPrinter<T>(*arg_, "*" + name_, depth_, enum_printers_, os_);
                next->set_print_endl(false);

                next->print_arg();
            }
            used_addrs.pop_back();
        }
        prefix = old_prefix;
    }
    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<char* const>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << reinterpret_cast<void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (depth_ != -1)
        --depth_;
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        std::string arg = arg_;
        replace_all(arg, "\n", "\\n");
        *os_ << " {" << arg << "}";
        if (recurse_)
        {
            *os_ << std::endl;
            const auto old_prefix = prefix;
            prefix += "\t";
            if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                *os_ << prefix << "[RECURSION]";
            else
            {
                used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                {
                    std::stringstream ss;
                    ss << name_ << "[" << i << "]";
                    const auto next = new ArgPrinter<char>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                    if (i == len_->get_ref() - 1 || (len_->get_ref() == 0 && arg_[i] == 0))
                        next->set_print_endl(false);

                    next->print_arg();
                }
                used_addrs.pop_back();
            }
            prefix = old_prefix;
        }
    }
    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<wchar_t* const>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << reinterpret_cast<void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (depth_ != -1)
        --depth_;
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        auto arg = wide_to_narrow_str(arg_);
        replace_all(arg, "\n", "\\n");
        *os_ << " {" << arg << "}";
        if (recurse_)
        {
            *os_ << std::endl;
            const auto old_prefix = prefix;
            prefix += "\t";
            if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                *os_ << prefix << "[RECURSION]";
            else
            {
                used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                {
                    std::stringstream ss;
                    ss << name_ << "[" << i << "]";
                    const auto next = new ArgPrinter<wchar_t>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                    if (i == len_->get_ref() - 1 || (len_->get_ref() == 0 && arg_[i] == 0))
                        next->set_print_endl(false);

                    next->print_arg();
                }
                used_addrs.pop_back();
            }
            prefix = old_prefix;
        }
    }
    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<FILE* const>::print_arg() - Prints a FILE pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<FILE* const>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<DIR* const>::print_arg() - Prints a DIR pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<DIR* const>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<void* const>::print_arg() - Prints a void pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<void* const>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<void* const>::print_arg() - Prints a void pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<volatile void* const>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(const_cast<void*>(arg_)); !name.empty())
        *os_ << " (" << name << ")";
    if (print_endl_)
        *os_ << std::endl;
}

/*
 * ArgPrinter<const T* const>::print_arg() definition and specializations
 */

/**
 * ArgPrinter<const T*>::print_arg() - Prints a pointer and its contents to @code *stream @endcode
 *
 * @tparam T The type of the object pointed to to be printed
 */
template <typename T>
void ArgPrinter<const T* const>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << reinterpret_cast<const void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (depth_ != -1)
        --depth_;
    if (const auto name = get_symbol_name(reinterpret_cast<const void*>(arg_)); !name.empty())
        *os_ << " (" << name << ")";
    if (recurse_ && bomb_detector(arg_, len_->get_ref()))
    {
        *os_ << std::endl;
        const auto old_prefix = prefix;
        prefix += "\t";
        if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
            *os_ << prefix << "[RECURSION]";
        else
        {
            used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
            if (len_->get_ref() != 0)
                for (auto i = 0; end_test_(i); ++i)
                {
                    std::stringstream ss;
                    ss << name_ << "[" << i << "]";
                    const auto next = new ArgPrinter<const T>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                    if (!end_test_(i + 1))
                        next->set_print_endl(false);

                    next->print_arg();
                }
            else
            {
                const auto next = new ArgPrinter<const T>(*arg_, "*" + name_, depth_, enum_printers_, os_);
                next->set_print_endl(false);

                next->print_arg();
            }
            used_addrs.pop_back();
        }
        prefix = old_prefix;
    }
    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<const char* const>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << reinterpret_cast<const void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (depth_ != -1)
        --depth_;
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        std::string arg = arg_;
        replace_all(arg, "\n", "\\n");
        *os_ << " {" << arg << "}";
        if (recurse_)
        {
            *os_ << std::endl;
            const auto old_prefix = prefix;
            prefix += "\t";
            if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                *os_ << prefix << "[RECURSION]";
            else
            {
                used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                {
                    std::stringstream ss;
                    ss << name_ << "[" << i << "]";
                    const auto next = new ArgPrinter<const char>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                    if (i == len_->get_ref() - 1 || (len_->get_ref() == 0 && arg_[i] == 0))
                        next->set_print_endl(false);

                    next->print_arg();
                }
                used_addrs.pop_back();
            }
            prefix = old_prefix;
        }
    }
    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<const wchar_t* const>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << reinterpret_cast<const void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (depth_ != -1)
        --depth_;
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (recurse_)
    {
        *os_ << std::endl;
        const auto old_prefix = prefix;
        prefix += "\t";
        if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
            *os_ << prefix << "[RECURSION]";
        else
        {
            auto arg = wide_to_narrow_str(arg_);
            replace_all(arg, "\n", "\\n");
            *os_ << " {" << arg << "}";
            if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) == used_addrs.end())
            {
                used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                {
                    std::stringstream ss;
                    ss << name_ << "[" << i << "]";
                    const auto next = new ArgPrinter<const wchar_t>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                    if (i == len_->get_ref() - 1 || (len_->get_ref() == 0 && arg_[i] == 0))
                        next->set_print_endl(false);

                    next->print_arg();
                }
                used_addrs.pop_back();
            }
            prefix = old_prefix;
        }
    }
    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<const void* const>::print_arg() - Prints a void pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<const void* const>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (print_endl_)
        *os_ << std::endl;
}
}

#endif //ARGPRINTERPOINTER_H
