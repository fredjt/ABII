//
// Created by Trent Tanchin on 7/14/24.
//

#ifndef ARGPRINTERARRAY_H
#define ARGPRINTERARRAY_H

namespace abii
{
/**
 * Template specialization of template class ArgPrinter for pointer types
 *
 * @tparam T Underlying type of object pointed to by the object this class holds
 *
 * @struct ArgPrinter libabii.h
 */
template <typename T, size_t N>
struct ArgPrinter<T[N]>final : VirtArgPrinter
{
    explicit ArgPrinter(T (&arg)[N], const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(arg), name_(name), print_endl_(flags & PRINT_ENDL),
                                               recurse_(flags & RECURSE), os_(os) {}

    explicit ArgPrinter(T (&&arg)[N], const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(rval_arg_), rval_arg_(arg), name_(name),
                                               print_endl_(flags & PRINT_ENDL), recurse_(flags & RECURSE), os_(os) {}

    ~ArgPrinter() override = default;

    [[nodiscard]] std::string get_name() const override { return name_; }
    void set_name(const std::string& name) override { name_ = name; }

    template <typename V>
    [[nodiscard]] V get_len() const { return len_->get_ref(); }

    template <typename V>
    void set_len(V& len) { len_ = new Reference<V>(len); }

    [[nodiscard]] std::string get_fmt() const { return fmt_; }
    void set_fmt(const std::string& fmt) { fmt_ = fmt; }

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

    ArgPrinter(T (&arg)[N], const std::string& name, const size_t previous_depth,
               const std::map<size_t, std::function<std::string(const void*)>>& enum_printers,
               std::ostream* os = &abii_stream, const int flags = 1)
        : arg_(arg), name_(name), enum_printers_(enum_printers), depth_(previous_depth + 1),
          print_endl_(flags & PRINT_ENDL), recurse_(flags & RECURSE), os_(os) {}

private:
    T (&arg_)[N];
    T rval_arg_[N];
    std::string name_{};
    ReferenceType* len_ = new Reference(def_len_);
    std::string fmt_{};
    std::map<size_t, std::function<std::string(const void*)>> enum_printers_;
    std::map<size_t, std::function<std::string(const char*, va_list, size_t)>> va_list_printers_;
    size_t va_list_printer_buf_size_ = 0;
    size_t depth_ = 0;
    bool print_endl_ = true;
    bool recurse_ = true;
    std::ostream* os_;
    static size_t def_len_;
};

template <typename T, size_t N>
size_t ArgPrinter<T[N]>::def_len_ = N;

/**
 * Template specialization of template class ArgPrinter for __locale_data pointer types
 *
 * @tparam N Number of elements in the array
 *
 * @struct ArgPrinter libabii.h
 */
template <size_t N>
struct ArgPrinter<__locale_data* const[N]>final : VirtArgPrinter
{
    explicit ArgPrinter(__locale_data* const (&arg)[N], const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(arg), name_(name), print_endl_(flags & PRINT_ENDL),
                                               recurse_(flags & RECURSE), os_(os) {}

    explicit ArgPrinter(__locale_data* const (&&arg)[N], const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(rval_arg_), rval_arg_(arg), name_(name),
                                               print_endl_(flags & PRINT_ENDL), recurse_(flags & RECURSE), os_(os) {}

    [[nodiscard]] std::string get_name() const override { return name_; }
    void set_name(const std::string& name) override { name_ = name; }

    template <typename V>
    [[nodiscard]] V get_len() const { return len_->get_ref(); }

    template <typename V>
    void set_len(V& len) { len_ = new Reference<V>(len); }

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
        ss << arg_;
        return ss.str();
    }

    void print_arg() override;

    ArgPrinter(__locale_data* const (&arg)[N], const std::string& name, const size_t previous_depth,
               const std::map<size_t, std::function<std::string(const void*)>>& enum_printers,
               std::ostream* os = &abii_stream, const int flags = 1)
        : arg_(arg), name_(name), enum_printers_(enum_printers), depth_(previous_depth + 1),
          print_endl_(flags & PRINT_ENDL), recurse_(flags & RECURSE), os_(os) {}

private:
    __locale_data* const (&arg_)[N];
    __locale_data* const rval_arg_[N] = nullptr;
    std::string name_;
    ReferenceType* len_ = new Reference(def_len_);
    std::map<size_t, std::function<std::string(const void*)>> enum_printers_;
    size_t depth_ = 0;
    bool print_endl_ = true;
    bool recurse_ = true;
    std::ostream* os_;
    static size_t def_len_;
};

template <size_t N>
size_t ArgPrinter<__locale_data* const[N]>::def_len_ = N;

/**
 * Template specialization of template class ArgPrinter for const-qualified pointer types
 *
 * @tparam T Underlying type of object pointed to by the object this class holds
 *
 * @struct ArgPrinter libabii.h
 */
template <typename T, size_t N>
struct ArgPrinter<const T[N]>final : VirtArgPrinter
{
    explicit ArgPrinter(const T (&arg)[N], const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(arg), name_(name), print_endl_(flags & PRINT_ENDL),
                                               recurse_(flags & RECURSE), os_(os) {}

    explicit ArgPrinter(const T (&&arg)[N], const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(rval_arg_), rval_arg_(arg), name_(name),
                                               print_endl_(flags & PRINT_ENDL), recurse_(flags & RECURSE), os_(os) {}

    [[nodiscard]] std::string get_name() const override { return name_; }
    void set_name(const std::string& name) override { name_ = name; }

    template <typename V>
    [[nodiscard]] V get_len() const { return len_->get_ref(); }

    template <typename V>
    void set_len(V& len) { len_ = new Reference<V>(len); }

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
        ss << arg_;
        return ss.str();
    }

    void print_arg() override;

    ArgPrinter(const T (&arg)[N], const std::string& name, const size_t previous_depth,
               const std::map<size_t, std::function<std::string(const void*)>>& enum_printers,
               std::ostream* os = &abii_stream, const int flags = 1)
        : arg_(arg), name_(name), enum_printers_(enum_printers), depth_(previous_depth + 1),
          print_endl_(flags & PRINT_ENDL), recurse_(flags & RECURSE), os_(os) {}

private:
    const T (&arg_)[N];
    const T rval_arg_[N];
    std::string name_;
    ReferenceType* len_ = new Reference(def_len_);
    std::map<size_t, std::function<std::string(const void*)>> enum_printers_;
    size_t depth_ = 0;
    bool print_endl_ = true;
    bool recurse_ = true;
    std::ostream* os_;
    static size_t def_len_;
};

template <typename T, size_t N>
size_t ArgPrinter<const T[N]>::def_len_ = N;

/**
 * Template specialization of template class ArgPrinter for pointer types
 *
 * @tparam T Underlying type of object pointed to by the object this class holds
 *
 * @struct ArgPrinter libabii.h
 */
template <typename T>
struct ArgPrinter<T[0]>final : VirtArgPrinter
{
    explicit ArgPrinter(T (&arg)[0], const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 1) : arg_(arg), name_(name), print_endl_(flags & PRINT_ENDL), os_(os) {}

    explicit ArgPrinter(T (&&arg)[0], const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 1) : arg_(rval_arg_), rval_arg_(arg), name_(name),
                                               print_endl_(flags & PRINT_ENDL), os_(os) {}

    [[nodiscard]] std::string get_name() const override { return name_; }
    void set_name(const std::string& name) override { name_ = name; }

    template <typename V>
    [[nodiscard]] V get_len() const { return len_->get_ref(); }

    template <typename V>
    void set_len(V& len) { len_ = new Reference<V>(len); }

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

    ArgPrinter(T (&arg)[0], const std::string& name, const size_t previous_depth,
               const std::map<size_t, std::function<std::string(const void*)>>& enum_printers,
               std::ostream* os = &abii_stream, const int flags = 1)
        : arg_(arg), name_(name), enum_printers_(enum_printers), depth_(previous_depth + 1),
          print_endl_(flags & PRINT_ENDL), os_(os) {}

private:
    T (&arg_)[0];
    T rval_arg_[0];
    std::string name_;
    ReferenceType* len_ = new Reference(def_len_);
    std::map<size_t, std::function<std::string(const void*)>> enum_printers_;
    size_t depth_ = 0;
    bool print_endl_ = true;
    std::ostream* os_;
    static size_t def_len_;
};

template <typename T>
size_t ArgPrinter<T[0]>::def_len_ = 0;

/**
 * Template specialization of template class ArgPrinter for const-qualified pointer types
 *
 * @tparam T Underlying type of object pointed to by the object this class holds
 *
 * @struct ArgPrinter libabii.h
 */
template <typename T>
struct ArgPrinter<const T[0]>final : VirtArgPrinter
{
    explicit ArgPrinter(const T (&arg)[0], const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 1) : arg_(arg), name_(name), print_endl_(flags & PRINT_ENDL), os_(os) {}

    explicit ArgPrinter(const T (&&arg)[0], const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 1) : arg_(rval_arg_), rval_arg_(arg), name_(name),
                                               print_endl_(flags & PRINT_ENDL), os_(os) {}

    [[nodiscard]] std::string get_name() const override { return name_; }
    void set_name(const std::string& name) override { name_ = name; }

    template <typename V>
    [[nodiscard]] V get_len() const { return len_->get_ref(); }

    template <typename V>
    void set_len(V& len) { len_ = new Reference<V>(len); }

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

    ArgPrinter(const T (&arg)[0], const std::string& name, const size_t previous_depth,
               const std::map<size_t, std::function<std::string(const void*)>>& enum_printers,
               std::ostream* os = &abii_stream, const int flags = 1)
        : arg_(arg), name_(name), enum_printers_(enum_printers), depth_(previous_depth + 1),
          print_endl_(flags & PRINT_ENDL), os_(os) {}

private:
    const T (&arg_)[0];
    const T rval_arg_[0];
    std::string name_;
    ReferenceType* len_ = new Reference(def_len_);
    std::map<size_t, std::function<std::string(const void*)>> enum_printers_;
    size_t depth_ = 0;
    bool print_endl_ = true;
    std::ostream* os_;
    static size_t def_len_;
};

template <typename T>
size_t ArgPrinter<const T[0]>::def_len_ = 0;

/*
 * ArgPrinter<T[N]>::print_arg() definition and specializations
 */

/**
 * ArgPrinter<T[N]>::print_arg() - Prints a pointer to an array and its contents to @code *stream @endcode
 *
 * @tparam T The type of the object pointed to to be printed
 * @tparam N The number of elements in the array
 */
template <typename T, size_t N>
void ArgPrinter<T[N]>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << reinterpret_cast<void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (depth_ != -1)
        --depth_;
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
            for (auto i = 0; i < N; ++i)
            {
                std::stringstream ss;
                ss << name_ << "[" << i << "]";
                auto next = new ArgPrinter<T>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                if (i == N - 1)
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

#ifndef BIT32
template <>
inline void ArgPrinter<va_list>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ")";
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
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
    else if (print_endl_)
        *os_ << std::endl;
}
#endif

/*
 * ArgPrinter<const T[N]>::print_arg() definition and specializations
 */

/**
 * ArgPrinter<const T*>::print_arg() - Prints a pointer and its contents to @code *stream @endcode
 *
 * @tparam T The type of the object pointed to to be printed
 */
template <typename T, size_t N>
void ArgPrinter<const T[N]>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << reinterpret_cast<const void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (depth_ != -1)
        --depth_;
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
            for (auto i = 0; i < N; ++i)
            {
                std::stringstream ss;
                ss << name_ << "[" << i << "]";
                auto next = new ArgPrinter<const T>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                if (i == N - 1)
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

template <size_t N>
void ArgPrinter<__locale_data* const[N]>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << reinterpret_cast<void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (depth_ != -1)
        --depth_;
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
            for (auto i = 0; i < N; ++i)
            {
                std::stringstream ss;
                ss << name_ << "[" << i << "]";
                auto next = new ArgPrinter<void* const>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                if (i == N - 1)
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

/*
 * ArgPrinter<T[0]>::print_arg() definition and specializations
 */

/**
 * ArgPrinter<T*>::print_arg() - Prints a pointer to an empty array to @code *stream @endcode
 *
 * @tparam T The type of the object pointed to to be printed
 */
template <typename T>
void ArgPrinter<T[0]>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << reinterpret_cast<void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (print_endl_)
        *os_ << std::endl;
}

/*
 * ArgPrinter<const T[0]>::print_arg() definition and specializations
 */

/**
 * ArgPrinter<const T*>::print_arg() - Prints a pointer and its contents to @code *stream @endcode
 *
 * @tparam T The type of the object pointed to to be printed
 */
template <typename T>
void ArgPrinter<const T[0]>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << reinterpret_cast<const void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (print_endl_)
        *os_ << std::endl;
}
}

#endif //ARGPRINTERARRAY_H
