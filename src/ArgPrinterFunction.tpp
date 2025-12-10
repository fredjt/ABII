//
// Created by Trent Tanchin on 7/14/24.
//

#ifndef ARGPRINTERFUNCTION_H
#define ARGPRINTERFUNCTION_H

namespace abii
{
/**
 * Template specialization of template class ArgPrinter for function pointers
 *
 * @tparam Ret Return type of the function
 * @tparam Args Arguments of the function
 *
 * @struct ArgPrinter libabii.h
 */
template <typename Ret, typename... Args>
struct ArgPrinter<Ret(*)(Args...)>final : VirtArgPrinter
{
    typedef Ret (*ArgPrinterFunc)(Args...);

    explicit ArgPrinter(ArgPrinterFunc& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 1) : arg_(arg), name_(name), print_endl_(flags & PRINT_ENDL), os_(os) {}

    explicit ArgPrinter(ArgPrinterFunc&& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 1) : arg_(rval_arg_), rval_arg_(arg), name_(name),
                                               print_endl_(flags & PRINT_ENDL), os_(os) {}

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

    ArgPrinter(ArgPrinterFunc& arg, const std::string& name, const size_t previous_depth,
               const std::map<size_t, std::function<std::string(const void*)>>& enum_printers,
               std::ostream* os = &abii_stream, const int flags = 1)
        : arg_(arg), name_(name), enum_printers_(enum_printers), depth_(previous_depth + 1),
          print_endl_(flags & PRINT_ENDL), os_(os) {}

private:
    ArgPrinterFunc& arg_;
    ArgPrinterFunc rval_arg_ = nullptr;
    std::string name_{};
    std::map<size_t, std::function<std::string(const void*)>> enum_printers_;
    size_t depth_ = 0;
    bool print_endl_ = true;
    std::ostream* os_;
};

/**
 * Template specialization of template class ArgPrinter for function pointers
 *
 * @tparam Ret Return type of the function
 * @tparam Args Arguments of the function
 *
 * @struct ArgPrinter libabii.h
 */
template <typename Ret, typename... Args>
struct ArgPrinter<Ret(* const)(Args...)>final : VirtArgPrinter
{
    typedef Ret (*ArgPrinterFunc)(Args...);

    explicit ArgPrinter(ArgPrinterFunc const& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 1) : arg_(arg), name_(name), print_endl_(flags & PRINT_ENDL), os_(os) {}

    explicit ArgPrinter(ArgPrinterFunc const&& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 1) : arg_(rval_arg_), rval_arg_(arg), name_(name),
                                               print_endl_(flags & PRINT_ENDL), os_(os) {}

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

    ArgPrinter(ArgPrinterFunc const& arg, const std::string& name, const size_t previous_depth,
               const std::map<size_t, std::function<std::string(const void*)>>& enum_printers,
               std::ostream* os = &abii_stream, const int flags = 1)
        : arg_(arg), name_(name), enum_printers_(enum_printers), depth_(previous_depth + 1),
          print_endl_(flags & PRINT_ENDL), os_(os) {}

private:
    ArgPrinterFunc const& arg_;
    ArgPrinterFunc const rval_arg_ = nullptr;
    std::string name_{};
    std::map<size_t, std::function<std::string(const void*)>> enum_printers_;
    size_t depth_ = 0;
    bool print_endl_ = true;
    std::ostream* os_;
};

/**
 * ArgPrinter<void* const>::print_arg() - Prints a void pointer to @code *stream @endcode
 */
template <typename Ret, typename... Args>
void ArgPrinter<Ret(*)(Args...)>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << reinterpret_cast<void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(reinterpret_cast<void*>(arg_)); !name.empty())
        *os_ << " (" << name << ")";
    if (print_endl_)
        *os_ << std::endl;
}

/**
 * ArgPrinter<void* const>::print_arg() - Prints a void pointer to @code *stream @endcode
 */
template <typename Ret, typename... Args>
void ArgPrinter<Ret(* const)(Args...)>::print_arg()
{
    *os_ << prefix << name_ << ": (" << get_type(arg_) << ") " << reinterpret_cast<void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(reinterpret_cast<void*>(arg_)); !name.empty())
        *os_ << " (" << name << ")";
    if (print_endl_)
        *os_ << std::endl;
}
}

#endif //ARGPRINTERFUNCTION_H
