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
        arg_(arg), name_(name), recurse_(flags & RECURSE), os_(os) {}

    explicit ArgPrinter(T*&& arg, const std::string& name = "", std::ostream* os = &abii_stream, const int flags = 3) :
        arg_(rval_arg_), rval_arg_(arg), name_(name), recurse_(flags & RECURSE), os_(os) {}

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
    [[nodiscard]] std::string enum_printer(const V& arg) const
    {
        return enum_printers_.contains(depth_)
                   ? enum_printers_.find(depth_)->second(static_cast<const void*>(&arg))
                   : "";
    }

    [[nodiscard]] std::function<std::string(const void*)> get_enum_printer() const
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

    [[nodiscard]] bool get_recurse() const { return recurse_; }
    void set_recurse(const bool recurse) { recurse_ = recurse; }
    [[nodiscard]] std::ostream* get_os() const override { return os_; }
    void set_os(std::ostream* os) override { os_ = os; }

    [[nodiscard]] std::string get_value() const override
    {
        std::stringstream ss;
        ss << static_cast<const void*>(arg_);
        return ss.str();
    }

    void print_arg() override;

    ArgPrinter(T*& arg, const std::string& name, const size_t previous_depth,
               const std::map<size_t, std::function<std::string(const void*)>>& enum_printers,
               std::ostream* os = &abii_stream, const int flags = 1)
        : arg_(arg), name_(name), enum_printers_(enum_printers), depth_(previous_depth + 1),
          recurse_(flags & RECURSE), os_(os) {}

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
    bool recurse_ = true;
    std::ostream* os_;
    static size_t def_len_;
};

template <typename T>
size_t ArgPrinter<T*>::def_len_ = 0;

/**
 * Template specialization of template class ArgPrinter for volatile-qualified pointer types
 *
 * @tparam T Underlying type of object pointed to by the object this class holds
 *
 * @struct ArgPrinter libabii.h
 */
template <typename T>
struct ArgPrinter<volatile T*>final : VirtArgPrinter
{
    explicit ArgPrinter(volatile T*& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(arg), name_(name),
                                               recurse_(flags & RECURSE), os_(os) {}

    explicit ArgPrinter(volatile T*&& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(rval_arg_), rval_arg_(arg), name_(name),
                                               recurse_(flags & RECURSE), os_(os) {}

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
    [[nodiscard]] std::string enum_printer(const volatile V& arg) const
    {
        return enum_printers_.contains(depth_)
                   ? enum_printers_.find(depth_)->second(static_cast<const volatile void*>(&arg))
                   : "";
    }

    [[nodiscard]] std::function<std::string(const volatile void*)> get_enum_printer() const
    {
        return enum_printers_.contains(depth_) ? enum_printers_.find(depth_)->second : nullptr;
    }

    template <typename V>
    void set_enum_printer_(const std::function<std::string(V)>& enum_printer, size_t depth = 0)
    {
        enum_printers_.insert({
            depth,
            [enum_printer](const volatile void* arg) -> std::string
            {
                return enum_printer(*static_cast<const volatile V*>(arg));
            }
        });
    }

    [[nodiscard]] bool get_recurse() const { return recurse_; }
    void set_recurse(const bool recurse) { recurse_ = recurse; }
    [[nodiscard]] std::ostream* get_os() const override { return os_; }
    void set_os(std::ostream* os) override { os_ = os; }

    [[nodiscard]] std::string get_value() const override
    {
        std::stringstream ss;
        ss << static_cast<const volatile void*>(arg_);
        return ss.str();
    }

    void print_arg() override;

    ArgPrinter(volatile T*& arg, const std::string& name, const size_t previous_depth,
               const std::map<size_t, std::function<std::string(const volatile void*)>>& enum_printers,
               std::ostream* os = &abii_stream, const int flags = 1)
        : arg_(arg), name_(name), enum_printers_(enum_printers), depth_(previous_depth + 1),
          recurse_(flags & RECURSE), os_(os) {}

private:
    volatile T*& arg_;
    volatile T* rval_arg_ = nullptr;
    std::string name_;
    ReferenceType* len_ = new Reference(def_len_);
    std::function<bool(size_t)> end_test_ = [&](const int i) { return i < len_->get_ref(); };
    std::map<size_t, std::function<std::string(const volatile void*)>> enum_printers_;
    size_t depth_ = 0;
    bool recurse_ = true;
    std::ostream* os_;
    static size_t def_len_;
};

template <typename T>
size_t ArgPrinter<volatile T*>::def_len_ = 0;

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
                        const int flags = 3) : arg_(arg), name_(name),
                                               recurse_(flags & RECURSE), os_(os) {}

    explicit ArgPrinter(const T*&& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(rval_arg_), rval_arg_(arg), name_(name),
                                               recurse_(flags & RECURSE), os_(os) {}

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
                   ? enum_printers_.find(depth_)->second(static_cast<const void*>(&arg))
                   : "";
    }

    [[nodiscard]] std::function<std::string(const void*)> get_enum_printer() const
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

    [[nodiscard]] bool get_recurse() const { return recurse_; }
    void set_recurse(const bool recurse) { recurse_ = recurse; }
    [[nodiscard]] std::ostream* get_os() const override { return os_; }
    void set_os(std::ostream* os) override { os_ = os; }

    [[nodiscard]] std::string get_value() const override
    {
        std::stringstream ss;
        ss << static_cast<const void*>(arg_);
        return ss.str();
    }

    void print_arg() override;

    ArgPrinter(const T*& arg, const std::string& name, const size_t previous_depth,
               const std::map<size_t, std::function<std::string(const void*)>>& enum_printers,
               std::ostream* os = &abii_stream, const int flags = 1)
        : arg_(arg), name_(name), enum_printers_(enum_printers), depth_(previous_depth + 1),
          recurse_(flags & RECURSE), os_(os) {}

private:
    const T*& arg_;
    const T* rval_arg_ = nullptr;
    std::string name_;
    ReferenceType* len_ = new Reference(def_len_);
    std::function<bool(size_t)> end_test_ = [&](const int i) { return i < len_->get_ref(); };
    std::map<size_t, std::function<std::string(const void*)>> enum_printers_;
    size_t depth_ = 0;
    bool recurse_ = true;
    std::ostream* os_;
    static size_t def_len_;
};

template <typename T>
size_t ArgPrinter<const T*>::def_len_ = 0;

/**
 * Template specialization of template class ArgPrinter for const-volatile-qualified pointer types
 *
 * @tparam T Underlying type of object pointed to by the object this class holds
 *
 * @struct ArgPrinter libabii.h
 */
template <typename T>
struct ArgPrinter<const volatile T*>final : VirtArgPrinter
{
    explicit ArgPrinter(const volatile T*& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(arg), name_(name),
                                               recurse_(flags & RECURSE), os_(os) {}

    explicit ArgPrinter(const volatile T*&& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(rval_arg_), rval_arg_(arg), name_(name),
                                               recurse_(flags & RECURSE), os_(os) {}

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
    [[nodiscard]] std::string enum_printer(const volatile V& arg) const
    {
        return enum_printers_.contains(depth_)
                   ? enum_printers_.find(depth_)->second(static_cast<const volatile void*>(&arg))
                   : "";
    }

    [[nodiscard]] std::function<std::string(const volatile void*)> get_enum_printer() const
    {
        return enum_printers_.contains(depth_) ? enum_printers_.find(depth_)->second : nullptr;
    }

    template <typename V>
    void set_enum_printer_(const std::function<std::string(V)>& enum_printer, size_t depth = 0)
    {
        enum_printers_.insert({
            depth,
            [enum_printer](const volatile void* arg) -> std::string
            {
                return enum_printer(*static_cast<const volatile V*>(arg));
            }
        });
    }

    [[nodiscard]] bool get_recurse() const { return recurse_; }
    void set_recurse(const bool recurse) { recurse_ = recurse; }
    [[nodiscard]] std::ostream* get_os() const override { return os_; }
    void set_os(std::ostream* os) override { os_ = os; }

    [[nodiscard]] std::string get_value() const override
    {
        std::stringstream ss;
        ss << static_cast<const volatile void*>(arg_);
        return ss.str();
    }

    void print_arg() override;

    ArgPrinter(const volatile T*& arg, const std::string& name, const size_t previous_depth,
               const std::map<size_t, std::function<std::string(const volatile void*)>>& enum_printers,
               std::ostream* os = &abii_stream, const int flags = 1)
        : arg_(arg), name_(name), enum_printers_(enum_printers), depth_(previous_depth + 1),
          recurse_(flags & RECURSE), os_(os) {}

private:
    const volatile T*& arg_;
    const volatile T* rval_arg_ = nullptr;
    std::string name_;
    ReferenceType* len_ = new Reference(def_len_);
    std::function<bool(size_t)> end_test_ = [&](const int i) { return i < len_->get_ref(); };
    std::map<size_t, std::function<std::string(const volatile void*)>> enum_printers_;
    size_t depth_ = 0;
    bool recurse_ = true;
    std::ostream* os_;
    static size_t def_len_;
};

template <typename T>
size_t ArgPrinter<const volatile T*>::def_len_ = 0;

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
                        const int flags = 3) : arg_(arg), name_(name),
                                               recurse_(flags & RECURSE), os_(os) {}

    explicit ArgPrinter(T* const&& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(rval_arg_), rval_arg_(arg), name_(name),
                                               recurse_(flags & RECURSE), os_(os) {}

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
                   ? enum_printers_.find(depth_)->second(static_cast<const void*>(&arg))
                   : "";
    }

    [[nodiscard]] std::function<std::string(const void*)> get_enum_printer() const
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

    [[nodiscard]] bool get_recurse() const { return recurse_; }
    void set_recurse(const bool recurse) { recurse_ = recurse; }
    [[nodiscard]] std::ostream* get_os() const override { return os_; }
    void set_os(std::ostream* os) override { os_ = os; }

    [[nodiscard]] std::string get_value() const override
    {
        std::stringstream ss;
        ss << static_cast<const void*>(arg_);
        return ss.str();
    }

    void print_arg() override;

    ArgPrinter(T* const& arg, const std::string& name, const size_t previous_depth,
               const std::map<size_t, std::function<std::string(const void*)>>& enum_printers,
               std::ostream* os = &abii_stream, const int flags = 1)
        : arg_(arg), name_(name), enum_printers_(enum_printers), depth_(previous_depth + 1),
          recurse_(flags & RECURSE), os_(os) {}

private:
    T* const& arg_;
    T* const rval_arg_ = nullptr;
    std::string name_;
    ReferenceType* len_ = new Reference(def_len_);
    std::function<bool(size_t)> end_test_ = [&](const int i) { return i < len_->get_ref(); };
    std::map<size_t, std::function<std::string(const void*)>> enum_printers_;
    size_t depth_ = 0;
    bool recurse_ = true;
    std::ostream* os_ = &abii_stream;
    static size_t def_len_;
};

template <typename T>
size_t ArgPrinter<T* const>::def_len_ = 0;

/**
 * Template specialization of template class ArgPrinter for pointer types
 *
 * @tparam T Underlying type of object pointed to by the object this class holds
 *
 * @struct ArgPrinter libabii.h
 */
template <typename T>
struct ArgPrinter<volatile T* const>final : VirtArgPrinter
{
    explicit ArgPrinter(volatile T* const& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(arg), name_(name),
                                               recurse_(flags & RECURSE), os_(os) {}

    explicit ArgPrinter(volatile T* const&& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(rval_arg_), rval_arg_(arg), name_(name),
                                               recurse_(flags & RECURSE), os_(os) {}

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
    [[nodiscard]] std::string enum_printer(const volatile V& arg) const
    {
        return enum_printers_.contains(depth_)
                   ? enum_printers_.find(depth_)->second(static_cast<const volatile void*>(&arg))
                   : "";
    }

    [[nodiscard]] std::function<std::string(const volatile void*)> get_enum_printer() const
    {
        return enum_printers_.contains(depth_) ? enum_printers_.find(depth_)->second : nullptr;
    }

    template <typename V>
    void set_enum_printer_(const std::function<std::string(V)>& enum_printer, size_t depth = 0)
    {
        enum_printers_.insert({
            depth,
            [enum_printer](const volatile void* arg) -> std::string
            {
                return enum_printer(*static_cast<const volatile V*>(arg));
            }
        });
    }

    [[nodiscard]] bool get_recurse() const { return recurse_; }
    void set_recurse(const bool recurse) { recurse_ = recurse; }
    [[nodiscard]] std::ostream* get_os() const override { return os_; }
    void set_os(std::ostream* os) override { os_ = os; }

    [[nodiscard]] std::string get_value() const override
    {
        std::stringstream ss;
        ss << static_cast<const volatile void*>(arg_);
        return ss.str();
    }

    void print_arg() override;

    ArgPrinter(volatile T* const& arg, const std::string& name, const size_t previous_depth,
               const std::map<size_t, std::function<std::string(const volatile void*)>>& enum_printers,
               std::ostream* os = &abii_stream, const int flags = 1)
        : arg_(arg), name_(name), enum_printers_(enum_printers), depth_(previous_depth + 1),
          recurse_(flags & RECURSE), os_(os) {}

private:
    volatile T* const& arg_;
    volatile T* const rval_arg_ = nullptr;
    std::string name_;
    ReferenceType* len_ = new Reference(def_len_);
    std::function<bool(size_t)> end_test_ = [&](const int i) { return i < len_->get_ref(); };
    std::map<size_t, std::function<std::string(const volatile void*)>> enum_printers_;
    size_t depth_ = 0;
    bool recurse_ = true;
    std::ostream* os_ = &abii_stream;
    static size_t def_len_;
};

template <typename T>
size_t ArgPrinter<volatile T* const>::def_len_ = 0;

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
                        const int flags = 3) : arg_(arg), name_(name),
                                               recurse_(flags & RECURSE), os_(os) {}

    explicit ArgPrinter(const T* const&& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(rval_arg_), rval_arg_(arg), name_(name),
                                               recurse_(flags & RECURSE), os_(os) {}

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
                   ? enum_printers_.find(depth_)->second(static_cast<const void*>(&arg))
                   : "";
    }

    [[nodiscard]] std::function<std::string(const void*)> get_enum_printer() const
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

    [[nodiscard]] bool get_recurse() const { return recurse_; }
    void set_recurse(const bool recurse) { recurse_ = recurse; }
    [[nodiscard]] std::ostream* get_os() const override { return os_; }
    void set_os(std::ostream* os) override { os_ = os; }

    [[nodiscard]] std::string get_value() const override
    {
        std::stringstream ss;
        ss << static_cast<const void*>(arg_);
        return ss.str();
    }

    void print_arg() override;

    ArgPrinter(const T* const& arg, const std::string& name, const size_t previous_depth,
               const std::map<size_t, std::function<std::string(const void*)>>& enum_printers,
               std::ostream* os = &abii_stream, const int flags = 1)
        : arg_(arg), name_(name), enum_printers_(enum_printers), depth_(previous_depth + 1),
          recurse_(flags & RECURSE), os_(os) {}

private:
    const T* const& arg_;
    const T* const rval_arg_ = nullptr;
    std::string name_;
    ReferenceType* len_ = new Reference(def_len_);
    std::function<bool(size_t)> end_test_ = [&](const int i) { return i < len_->get_ref(); };
    std::map<size_t, std::function<std::string(const void*)>> enum_printers_;
    size_t depth_ = 0;
    bool recurse_ = true;
    std::ostream* os_ = &abii_stream;
    static size_t def_len_;
};

template <typename T>
size_t ArgPrinter<const T* const>::def_len_ = 0;

/**
 * Template specialization of template class ArgPrinter for const-volatile-qualified pointer types
 *
 * @tparam T Underlying type of object pointed to by the object this class holds
 *
 * @struct ArgPrinter libabii.h
 */
template <typename T>
struct ArgPrinter<const volatile T* const>final : VirtArgPrinter
{
    explicit ArgPrinter(const volatile T* const& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(arg), name_(name),
                                               recurse_(flags & RECURSE), os_(os) {}

    explicit ArgPrinter(const volatile T* const&& arg, const std::string& name = "", std::ostream* os = &abii_stream,
                        const int flags = 3) : arg_(rval_arg_), rval_arg_(arg), name_(name),
                                               recurse_(flags & RECURSE), os_(os) {}

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
    [[nodiscard]] std::string enum_printer(const volatile V& arg) const
    {
        return enum_printers_.contains(depth_)
                   ? enum_printers_.find(depth_)->second(static_cast<const volatile void*>(&arg))
                   : "";
    }

    [[nodiscard]] std::function<std::string(const volatile void*)> get_enum_printer() const
    {
        return enum_printers_.contains(depth_) ? enum_printers_.find(depth_)->second : nullptr;
    }

    template <typename V>
    void set_enum_printer_(const std::function<std::string(V)>& enum_printer, size_t depth = 0)
    {
        enum_printers_.insert({
            depth,
            [enum_printer](const volatile void* arg) -> std::string
            {
                return enum_printer(*static_cast<const volatile V*>(arg));
            }
        });
    }

    [[nodiscard]] bool get_recurse() const { return recurse_; }
    void set_recurse(const bool recurse) { recurse_ = recurse; }
    [[nodiscard]] std::ostream* get_os() const override { return os_; }
    void set_os(std::ostream* os) override { os_ = os; }

    [[nodiscard]] std::string get_value() const override
    {
        std::stringstream ss;
        ss << static_cast<const void*>(arg_);
        return ss.str();
    }

    void print_arg() override;

    ArgPrinter(const volatile T* const& arg, const std::string& name, const size_t previous_depth,
               const std::map<size_t, std::function<std::string(const volatile void*)>>& enum_printers,
               std::ostream* os = &abii_stream, const int flags = 1)
        : arg_(arg), name_(name), enum_printers_(enum_printers), depth_(previous_depth + 1),
          recurse_(flags & RECURSE), os_(os) {}

private:
    const volatile T* const& arg_;
    const volatile T* const rval_arg_ = nullptr;
    std::string name_;
    ReferenceType* len_ = new Reference(def_len_);
    std::function<bool(size_t)> end_test_ = [&](const int i) { return i < len_->get_ref(); };
    std::map<size_t, std::function<std::string(const volatile void*)>> enum_printers_;
    size_t depth_ = 0;
    bool recurse_ = true;
    std::ostream* os_ = &abii_stream;
    static size_t def_len_;
};

template <typename T>
size_t ArgPrinter<const volatile T* const>::def_len_ = 0;

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
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(static_cast<void*>(arg_)); !name.empty())
        *os_ << " (" << name << ")";
    if (recurse_ && bomb_detector(arg_, len_->get_ref()))
    {
        if constexpr (std::is_same_v<T, unsigned char>)
        {
            if (auto len = len_->get_ref() == 0 ? strlen(reinterpret_cast<const char*>(arg_)) : len_->get_ref(); len >
                MAX_PRINT_BYTES)
            {
                std::stringstream fname_ss;
                fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                    << data_file_suffix;
                auto os = std::ofstream(fname_ss.str());
                os.write(reinterpret_cast<const char*>(arg_), static_cast<std::streamsize>(len));
                os.close();
                *os_ << std::endl << prefix << '\t' << fname_ss.str();
                prefix.pop_back();
                return;
            }
        }
        if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
            *os_ << std::endl << prefix << '\t' << "[RECURSION]";
        else
        {
            used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
            if (len_->get_ref() != 0)
                for (auto i = 0; end_test_(i); ++i)
                {
                    std::stringstream ss;
                    ss << name_ << "[" << i << "]";
                    const auto next = new ArgPrinter<T>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                    next->print_arg();
                }
            else
            {
                const auto next = new ArgPrinter<T>(*arg_, "*" + name_, depth_, enum_printers_, os_);
                next->print_arg();
            }
            used_addrs.pop_back();
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<char*>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ")";
#ifdef BIT32
    if (!fmt_.empty())
    {
        std::stringstream ss;
        ss << va_list_printer(fmt_.c_str(), arg_, va_list_printer_buf_size_);
        std::string str = ss.str();
        *os_ << str;
    }
    else
    {



#endif
    *os_ << " " << static_cast<void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        if (auto len = len_->get_ref() == 0 ? strlen(arg_) : len_->get_ref(); len > MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                << data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(arg_, static_cast<std::streamsize>(len));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            std::string arg = arg_;
            replace_all(arg, "\n", "\\n");
            *os_ << " {" << arg << "}";
            if (recurse_)
            {
                if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                    *os_ << std::endl << prefix << '\t' << "[RECURSION]";
                else
                {
                    used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                    for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                    {
                        std::stringstream ss;
                        ss << name_ << "[" << i << "]";
                        const auto next = new ArgPrinter<char>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                        if (i == len_->get_ref() - 1 || (len_->get_ref() == 0 && arg_[i] == 0))
                            next->print_arg();
                    }
                    used_addrs.pop_back();
                }
            }
        }
    }
#ifdef BIT32
    }
#endif
    prefix.pop_back();
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<wchar_t*>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        if (auto len = len_->get_ref() == 0 ? std::wcslen(arg_) * sizeof(wchar_t) : len_->get_ref() * sizeof(wchar_t);
            len > MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                << data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(reinterpret_cast<const char*>(arg_), static_cast<std::streamsize>(len));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            auto arg = wide_to_narrow_str(arg_);
            replace_all(arg, "\n", "\\n");
            *os_ << " {" << arg << "}";
            if (recurse_)
            {
                if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                    *os_ << std::endl << prefix << '\t' << "[RECURSION]";
                else
                {
                    used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                    for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                    {
                        std::stringstream ss;
                        ss << name_ << "[" << i << "]";
                        const auto next = new ArgPrinter<wchar_t>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                        next->print_arg();
                    }
                    used_addrs.pop_back();
                }
            }
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<FILE*>::print_arg() - Prints a FILE pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<FILE*>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    prefix.pop_back();
}

/**
 * ArgPrinter<FILE*>::print_arg() - Prints a DIR pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<DIR*>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    prefix.pop_back();
}

/**
 * ArgPrinter<void*>::print_arg() - Prints a void pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<void*>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (recurse_ && len_->get_ref() != 0 &&
        bomb_detector(static_cast<const volatile std::byte*>(arg_), len_->get_ref()))
    {
        if (len_->get_ref() > MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++ <<
                data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(static_cast<const char*>(arg_), static_cast<std::streamsize>(len_->get_ref()));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            *os_ << std::endl;
            hexdump(*os_, arg_, len_->get_ref());
        }
    }
    prefix.pop_back();
}

/*
 * ArgPrinter<volatile T*>::print_arg() definition and specializations
 */

/**
 * ArgPrinter<volatile T*>::print_arg() - Prints a pointer and its contents to @code *stream @endcode
 *
 * @tparam T The type of the object pointed to to be printed
 */
template <typename T>
void ArgPrinter<volatile T*>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<volatile void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(static_cast<volatile void*>(arg_)); !name.empty())
        *os_ << " (" << name << ")";
    if (recurse_ && bomb_detector(arg_, len_->get_ref()))
    {
        if constexpr (std::is_same_v<T, unsigned char>)
        {
            if (auto len = len_->get_ref() == 0
                               ? strlen(reinterpret_cast<const char*>(const_cast<const unsigned char*>(arg_)))
                               : len_->get_ref(); len > MAX_PRINT_BYTES)
            {
                std::stringstream fname_ss;
                fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                    << data_file_suffix;
                auto os = std::ofstream(fname_ss.str());
                os.write(reinterpret_cast<const char*>(const_cast<const unsigned char*>(arg_)),
                         static_cast<std::streamsize>(len));
                os.close();
                *os_ << std::endl << prefix << '\t' << fname_ss.str();
                prefix.pop_back();
                return;
            }
        }
        if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
            *os_ << std::endl << prefix << '\t' << "[RECURSION]";
        else
        {
            used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
            if (len_->get_ref() != 0)
                for (auto i = 0; end_test_(i); ++i)
                {
                    std::stringstream ss;
                    ss << name_ << "[" << i << "]";
                    const auto next = new ArgPrinter<volatile T>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                    next->print_arg();
                }
            else
            {
                const auto next = new ArgPrinter<volatile T>(*arg_, "*" + name_, depth_, enum_printers_, os_);
                next->print_arg();
            }
            used_addrs.pop_back();
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<volatile char*>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<volatile void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        if (auto len = len_->get_ref() == 0 ? strlen(const_cast<const char*>(arg_)) : len_->get_ref(); len >
            MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                << data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(const_cast<const char*>(arg_), static_cast<std::streamsize>(len));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            std::string arg = const_cast<const char*>(arg_);
            replace_all(arg, "\n", "\\n");
            *os_ << " {" << arg << "}";
            if (recurse_)
            {
                if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                    *os_ << std::endl << prefix << '\t' << "[RECURSION]";
                else
                {
                    used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                    for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                    {
                        std::stringstream ss;
                        ss << name_ << "[" << i << "]";
                        const auto next = new ArgPrinter<volatile char>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                        next->print_arg();
                    }
                    used_addrs.pop_back();
                }
            }
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<volatile wchar_t*>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<volatile void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        if (auto len = len_->get_ref() == 0
                           ? std::wcslen(const_cast<const wchar_t*>(arg_)) * sizeof(wchar_t)
                           : len_->get_ref() * sizeof(wchar_t); len > MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                << data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(reinterpret_cast<const char*>(const_cast<const wchar_t*>(arg_)),
                     static_cast<std::streamsize>(len));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            auto arg = wide_to_narrow_str(const_cast<const wchar_t*>(arg_));
            replace_all(arg, "\n", "\\n");
            *os_ << " {" << arg << "}";
            if (recurse_)
            {
                if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                    *os_ << std::endl << prefix << '\t' << "[RECURSION]";
                else
                {
                    used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                    for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                    {
                        std::stringstream ss;
                        ss << name_ << "[" << i << "]";
                        const auto next = new ArgPrinter<volatile wchar_t>(
                            arg_[i], ss.str(), depth_, enum_printers_, os_);
                        next->print_arg();
                    }
                    used_addrs.pop_back();
                }
            }
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<void*>::print_arg() - Prints a void pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<volatile void*>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (recurse_ && len_->get_ref() != 0 &&
        bomb_detector(static_cast<const volatile std::byte*>(arg_), len_->get_ref()))
    {
        if (len_->get_ref() > MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++ <<
                data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(static_cast<const char*>(const_cast<const void*>(arg_)),
                     static_cast<std::streamsize>(len_->get_ref()));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            *os_ << std::endl;
            hexdump(*os_, arg_, len_->get_ref());
        }
    }
    prefix.pop_back();
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
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<const void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(static_cast<const void*>(arg_)); !name.empty())
        *os_ << " (" << name << ")";
    if (recurse_ && bomb_detector(arg_, len_->get_ref()))
    {
        if constexpr (std::is_same_v<T, unsigned char>)
        {
            if (auto len = len_->get_ref() == 0 ? strlen(reinterpret_cast<const char*>(arg_)) : len_->get_ref(); len >
                MAX_PRINT_BYTES)
            {
                std::stringstream fname_ss;
                fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                    << data_file_suffix;
                auto os = std::ofstream(fname_ss.str());
                os.write(reinterpret_cast<const char*>(arg_), static_cast<std::streamsize>(len));
                os.close();
                *os_ << std::endl << prefix << '\t' << fname_ss.str();
                prefix.pop_back();
                return;
            }
        }
        if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
            *os_ << std::endl << prefix << '\t' << "[RECURSION]";
        else
        {
            used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
            if (len_->get_ref() != 0)
                for (auto i = 0; end_test_(i); ++i)
                {
                    std::stringstream ss;
                    ss << name_ << "[" << i << "]";
                    const auto next = new ArgPrinter<const T>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                    next->print_arg();
                }
            else
            {
                const auto next = new ArgPrinter<const T>(*arg_, "*" + name_, depth_, enum_printers_, os_);
                next->print_arg();
            }
            used_addrs.pop_back();
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<const char*>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<const void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        if (auto len = len_->get_ref() == 0 ? strlen(arg_) : len_->get_ref(); len > MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                << data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(arg_, static_cast<std::streamsize>(len));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            std::string arg = arg_;
            replace_all(arg, "\n", "\\n");
            *os_ << " {" << arg << "}";
            if (recurse_)
            {
                if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                    *os_ << std::endl << prefix << '\t' << "[RECURSION]";
                else
                {
                    used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                    for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                    {
                        std::stringstream ss;
                        ss << name_ << "[" << i << "]";
                        const auto next = new ArgPrinter<const char>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                        next->print_arg();
                    }
                    used_addrs.pop_back();
                }
            }
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<const wchar_t*>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<const void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        if (auto len = len_->get_ref() == 0 ? std::wcslen(arg_) * sizeof(wchar_t) : len_->get_ref() * sizeof(wchar_t);
            len > MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                << data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(reinterpret_cast<const char*>(arg_), static_cast<std::streamsize>(len));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            auto arg = wide_to_narrow_str(arg_);
            replace_all(arg, "\n", "\\n");
            *os_ << " {" << arg << "}";
            if (recurse_)
            {
                if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                    *os_ << std::endl << prefix << '\t' << "[RECURSION]";
                else
                {
                    used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                    for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                    {
                        std::stringstream ss;
                        ss << name_ << "[" << i << "]";
                        const auto next = new ArgPrinter<const wchar_t>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                        next->print_arg();
                    }
                    used_addrs.pop_back();
                }
            }
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<void*>::print_arg() - Prints a void pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<const void*>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (recurse_ && len_->get_ref() != 0 &&
        bomb_detector(static_cast<const volatile std::byte*>(arg_), len_->get_ref()))
    {
        if (len_->get_ref() > MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++ <<
                data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(static_cast<const char*>(arg_), static_cast<std::streamsize>(len_->get_ref()));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            *os_ << std::endl;
            hexdump(*os_, arg_, len_->get_ref());
        }
    }
    prefix.pop_back();
}

/*
 * ArgPrinter<const volatile T*>::print_arg() definition and specializations
 */

/**
 * ArgPrinter<const volatile T*>::print_arg() - Prints a pointer and its contents to @code *stream @endcode
 *
 * @tparam T The type of the object pointed to to be printed
 */
template <typename T>
void ArgPrinter<const volatile T*>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<const volatile void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(static_cast<const volatile void*>(arg_)); !name.empty())
        *os_ << " (" << name << ")";
    if (recurse_ && bomb_detector(arg_, len_->get_ref()))
    {
        if constexpr (std::is_same_v<T, unsigned char>)
        {
            if (auto len = len_->get_ref() == 0
                               ? strlen(reinterpret_cast<const char*>(const_cast<const unsigned char*>(arg_)))
                               : len_->get_ref(); len > MAX_PRINT_BYTES)
            {
                std::stringstream fname_ss;
                fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                    << data_file_suffix;
                auto os = std::ofstream(fname_ss.str());
                os.write(reinterpret_cast<const char*>(const_cast<const unsigned char*>(arg_)),
                         static_cast<std::streamsize>(len));
                os.close();
                *os_ << std::endl << prefix << '\t' << fname_ss.str();
                prefix.pop_back();
                return;
            }
        }
        if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
            *os_ << std::endl << prefix << '\t' << "[RECURSION]";
        else
        {
            used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
            if (len_->get_ref() != 0)
                for (auto i = 0; end_test_(i); ++i)
                {
                    std::stringstream ss;
                    ss << name_ << "[" << i << "]";
                    const auto next = new ArgPrinter<const volatile T>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                    next->print_arg();
                }
            else
            {
                const auto next = new ArgPrinter<const volatile T>(*arg_, "*" + name_, depth_, enum_printers_, os_);
                next->print_arg();
            }
            used_addrs.pop_back();
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<const volatile char*>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<const volatile void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        if (auto len = len_->get_ref() == 0 ? strlen(const_cast<const char*>(arg_)) : len_->get_ref(); len >
            MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                << data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(const_cast<const char*>(arg_), static_cast<std::streamsize>(len));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            std::string arg = const_cast<const char*>(arg_);
            replace_all(arg, "\n", "\\n");
            *os_ << " {" << arg << "}";
            if (recurse_)
            {
                if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                    *os_ << std::endl << prefix << '\t' << "[RECURSION]";
                else
                {
                    used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                    for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                    {
                        std::stringstream ss;
                        ss << name_ << "[" << i << "]";
                        const auto next = new ArgPrinter<const volatile char>(
                            arg_[i], ss.str(), depth_, enum_printers_, os_);
                        next->print_arg();
                    }
                    used_addrs.pop_back();
                }
            }
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<const volatile wchar_t*>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<const volatile void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        if (auto len = len_->get_ref() == 0
                           ? std::wcslen(const_cast<const wchar_t*>(arg_)) * sizeof(wchar_t)
                           : len_->get_ref() * sizeof(wchar_t); len > MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                << data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(reinterpret_cast<const char*>(const_cast<const wchar_t*>(arg_)),
                     static_cast<std::streamsize>(len));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            auto arg = wide_to_narrow_str(const_cast<const wchar_t*>(arg_));
            replace_all(arg, "\n", "\\n");
            *os_ << " {" << arg << "}";
            if (recurse_)
            {
                if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                    *os_ << std::endl << prefix << '\t' << "[RECURSION]";
                else
                {
                    used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                    for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                    {
                        std::stringstream ss;
                        ss << name_ << "[" << i << "]";
                        const auto next = new ArgPrinter<const volatile wchar_t>(
                            arg_[i], ss.str(), depth_, enum_printers_, os_);
                        next->print_arg();
                    }
                    used_addrs.pop_back();
                }
            }
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<void*>::print_arg() - Prints a void pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<const volatile void*>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (recurse_ && len_->get_ref() != 0 &&
        bomb_detector(static_cast<const volatile std::byte*>(arg_), len_->get_ref()))
    {
        if (len_->get_ref() > MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++ <<
                data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(static_cast<const char*>(const_cast<const void*>(arg_)),
                     static_cast<std::streamsize>(len_->get_ref()));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            *os_ << std::endl;
            hexdump(*os_, arg_, len_->get_ref());
        }
    }
    prefix.pop_back();
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
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<const void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(static_cast<const void*>(arg_)); !name.empty())
        *os_ << " (" << name << ")";
    if (recurse_ && bomb_detector(arg_, len_->get_ref()))
    {
        if constexpr (std::is_same_v<T, unsigned char>)
        {
            if (auto len = len_->get_ref() == 0 ? strlen(reinterpret_cast<const char*>(arg_)) : len_->get_ref(); len >
                MAX_PRINT_BYTES)
            {
                std::stringstream fname_ss;
                fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                    << data_file_suffix;
                auto os = std::ofstream(fname_ss.str());
                os.write(reinterpret_cast<const char*>(arg_), static_cast<std::streamsize>(len));
                os.close();
                *os_ << std::endl << prefix << '\t' << fname_ss.str();
                prefix.pop_back();
                return;
            }
        }
        if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
            *os_ << std::endl << prefix << '\t' << "[RECURSION]";
        else
        {
            used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
            if (len_->get_ref() != 0)
                for (auto i = 0; end_test_(i); ++i)
                {
                    std::stringstream ss;
                    ss << name_ << "[" << i << "]";
                    const auto next = new ArgPrinter<T>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                    next->print_arg();
                }
            else
            {
                const auto next = new ArgPrinter<T>(*arg_, "*" + name_, depth_, enum_printers_, os_);
                next->print_arg();
            }
            used_addrs.pop_back();
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<char* const>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<const void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        if (auto len = len_->get_ref() == 0 ? strlen(arg_) : len_->get_ref(); len > MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                << data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(arg_, static_cast<std::streamsize>(len));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            std::string arg = arg_;
            replace_all(arg, "\n", "\\n");
            *os_ << " {" << arg << "}";
            if (recurse_)
            {
                if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                    *os_ << std::endl << prefix << '\t' << "[RECURSION]";
                else
                {
                    used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                    for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                    {
                        std::stringstream ss;
                        ss << name_ << "[" << i << "]";
                        const auto next = new ArgPrinter<char>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                        next->print_arg();
                    }
                    used_addrs.pop_back();
                }
            }
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<wchar_t* const>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<const void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        if (auto len = len_->get_ref() == 0 ? std::wcslen(arg_) * sizeof(wchar_t) : len_->get_ref() * sizeof(wchar_t);
            len > MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                << data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(reinterpret_cast<const char*>(arg_), static_cast<std::streamsize>(len));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            auto arg = wide_to_narrow_str(arg_);
            replace_all(arg, "\n", "\\n");
            *os_ << " {" << arg << "}";
            if (recurse_)
            {
                if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                    *os_ << std::endl << prefix << '\t' << "[RECURSION]";
                else
                {
                    used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                    for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                    {
                        std::stringstream ss;
                        ss << name_ << "[" << i << "]";
                        const auto next = new ArgPrinter<wchar_t>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                        next->print_arg();
                    }
                    used_addrs.pop_back();
                }
            }
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<FILE* const>::print_arg() - Prints a FILE pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<FILE* const>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    prefix.pop_back();
}

/**
 * ArgPrinter<DIR* const>::print_arg() - Prints a DIR pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<DIR* const>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    prefix.pop_back();
}

/**
 * ArgPrinter<void* const>::print_arg() - Prints a void pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<void* const>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (recurse_ && len_->get_ref() != 0 &&
        bomb_detector(static_cast<const volatile std::byte*>(arg_), len_->get_ref()))
    {
        if (len_->get_ref() > MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++ <<
                data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(static_cast<const char*>(arg_), static_cast<std::streamsize>(len_->get_ref()));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            *os_ << std::endl;
            hexdump(*os_, arg_, len_->get_ref());
        }
    }
    prefix.pop_back();
}

/*
 * ArgPrinter<volatile T* const>::print_arg() definition and specializations
 */

/**
 * ArgPrinter<volatile T*>::print_arg() - Prints a pointer and its contents to @code *stream @endcode
 *
 * @tparam T The type of the object pointed to to be printed
 */
template <typename T>
void ArgPrinter<volatile T* const>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<const volatile void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(static_cast<const volatile void*>(arg_)); !name.empty())
        *os_ << " (" << name << ")";
    if (recurse_ && bomb_detector(arg_, len_->get_ref()))
    {
        if constexpr (std::is_same_v<T, unsigned char>)
        {
            if (auto len = len_->get_ref() == 0
                               ? strlen(reinterpret_cast<const char*>(const_cast<const unsigned char*>(arg_)))
                               : len_->get_ref(); len > MAX_PRINT_BYTES)
            {
                std::stringstream fname_ss;
                fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                    << data_file_suffix;
                auto os = std::ofstream(fname_ss.str());
                os.write(reinterpret_cast<const char*>(const_cast<const unsigned char*>(arg_)),
                         static_cast<std::streamsize>(len));
                os.close();
                *os_ << std::endl << prefix << '\t' << fname_ss.str();
                prefix.pop_back();
                return;
            }
        }
        if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
            *os_ << std::endl << prefix << '\t' << "[RECURSION]";
        else
        {
            used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
            if (len_->get_ref() != 0)
                for (auto i = 0; end_test_(i); ++i)
                {
                    std::stringstream ss;
                    ss << name_ << "[" << i << "]";
                    const auto next = new ArgPrinter<volatile T>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                    next->print_arg();
                }
            else
            {
                const auto next = new ArgPrinter<volatile T>(*arg_, "*" + name_, depth_, enum_printers_, os_);
                next->print_arg();
            }
            used_addrs.pop_back();
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<volatile char* const>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<const volatile void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        if (auto len = len_->get_ref() == 0 ? strlen(const_cast<const char*>(arg_)) : len_->get_ref(); len >
            MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                << data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(const_cast<const char*>(arg_), static_cast<std::streamsize>(len));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            std::string arg = const_cast<char* const>(arg_);
            replace_all(arg, "\n", "\\n");
            *os_ << " {" << arg << "}";
            if (recurse_)
            {
                if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                    *os_ << std::endl << prefix << '\t' << "[RECURSION]";
                else
                {
                    used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                    for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                    {
                        std::stringstream ss;
                        ss << name_ << "[" << i << "]";
                        const auto next = new ArgPrinter<volatile char>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                        next->print_arg();
                    }
                    used_addrs.pop_back();
                }
            }
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<volatile wchar_t* const>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<const volatile void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        if (auto len = len_->get_ref() == 0
                           ? std::wcslen(const_cast<const wchar_t*>(arg_)) * sizeof(wchar_t)
                           : len_->get_ref() * sizeof(wchar_t); len > MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                << data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(reinterpret_cast<const char*>(const_cast<const wchar_t*>(arg_)),
                     static_cast<std::streamsize>(len));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            auto arg = wide_to_narrow_str(const_cast<wchar_t* const>(arg_));
            replace_all(arg, "\n", "\\n");
            *os_ << " {" << arg << "}";
            if (recurse_)
            {
                if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                    *os_ << std::endl << prefix << '\t' << "[RECURSION]";
                else
                {
                    used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                    for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                    {
                        std::stringstream ss;
                        ss << name_ << "[" << i << "]";
                        const auto next = new ArgPrinter<volatile wchar_t>(
                            arg_[i], ss.str(), depth_, enum_printers_, os_);
                        next->print_arg();
                    }
                    used_addrs.pop_back();
                }
            }
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<FILE* const>::print_arg() - Prints a FILE pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<volatile FILE* const>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    prefix.pop_back();
}

/**
 * ArgPrinter<DIR* const>::print_arg() - Prints a DIR pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<volatile DIR* const>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    prefix.pop_back();
}

/**
 * ArgPrinter<void* const>::print_arg() - Prints a void pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<volatile void* const>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (recurse_ && len_->get_ref() != 0 &&
        bomb_detector(static_cast<const volatile std::byte*>(arg_), len_->get_ref()))
    {
        if (len_->get_ref() > MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++ <<
                data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(static_cast<const char*>(const_cast<const void*>(arg_)),
                     static_cast<std::streamsize>(len_->get_ref()));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            *os_ << std::endl;
            hexdump(*os_, arg_, len_->get_ref());
        }
    }
    prefix.pop_back();
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
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<const void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(static_cast<const void*>(arg_)); !name.empty())
        *os_ << " (" << name << ")";
    if (recurse_ && bomb_detector(arg_, len_->get_ref()))
    {
        if constexpr (std::is_same_v<T, unsigned char>)
        {
            if (auto len = len_->get_ref() == 0 ? strlen(reinterpret_cast<const char*>(arg_)) : len_->get_ref(); len >
                MAX_PRINT_BYTES)
            {
                std::stringstream fname_ss;
                fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                    << data_file_suffix;
                auto os = std::ofstream(fname_ss.str());
                os.write(reinterpret_cast<const char*>(arg_), static_cast<std::streamsize>(len));
                os.close();
                *os_ << std::endl << prefix << '\t' << fname_ss.str();
                prefix.pop_back();
                return;
            }
        }
        if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
            *os_ << std::endl << prefix << '\t' << "[RECURSION]";
        else
        {
            used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
            if (len_->get_ref() != 0)
                for (auto i = 0; end_test_(i); ++i)
                {
                    std::stringstream ss;
                    ss << name_ << "[" << i << "]";
                    const auto next = new ArgPrinter<const T>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                    next->print_arg();
                }
            else
            {
                const auto next = new ArgPrinter<const T>(*arg_, "*" + name_, depth_, enum_printers_, os_);
                next->print_arg();
            }
            used_addrs.pop_back();
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<const char* const>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<const void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        if (auto len = len_->get_ref() == 0 ? strlen(arg_) : len_->get_ref(); len > MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                << data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(arg_, static_cast<std::streamsize>(len));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            std::string arg = arg_;
            replace_all(arg, "\n", "\\n");
            *os_ << " {" << arg << "}";
            if (recurse_)
            {
                if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                    *os_ << std::endl << prefix << '\t' << "[RECURSION]";
                else
                {
                    used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                    for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                    {
                        std::stringstream ss;
                        ss << name_ << "[" << i << "]";
                        const auto next = new ArgPrinter<const char>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                        next->print_arg();
                    }
                    used_addrs.pop_back();
                }
            }
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<const wchar_t* const>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<const void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        if (auto len = len_->get_ref() == 0 ? std::wcslen(arg_) * sizeof(wchar_t) : len_->get_ref() * sizeof(wchar_t);
            len > MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                << data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(reinterpret_cast<const char*>(arg_), static_cast<std::streamsize>(len));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            auto arg = wide_to_narrow_str(arg_);
            replace_all(arg, "\n", "\\n");
            *os_ << " {" << arg << "}";
            if (recurse_)
            {
                if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                    *os_ << std::endl << prefix << '\t' << "[RECURSION]";
                else
                {
                    used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                    for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                    {
                        std::stringstream ss;
                        ss << name_ << "[" << i << "]";
                        const auto next = new ArgPrinter<const wchar_t>(
                            arg_[i], ss.str(), depth_, enum_printers_, os_);
                        next->print_arg();
                    }
                    used_addrs.pop_back();
                }
            }
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<const void* const>::print_arg() - Prints a void pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<const void* const>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (recurse_ && len_->get_ref() != 0 &&
        bomb_detector(static_cast<const volatile std::byte*>(arg_), len_->get_ref()))
    {
        if (len_->get_ref() > MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++ <<
                data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(static_cast<const char*>(arg_), static_cast<std::streamsize>(len_->get_ref()));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            *os_ << std::endl;
            hexdump(*os_, arg_, len_->get_ref());
        }
    }
    prefix.pop_back();
}

/*
 * ArgPrinter<const volatile T* const>::print_arg() definition and specializations
 */

/**
 * ArgPrinter<const volatile T* const>::print_arg() - Prints a pointer and its contents to @code *stream @endcode
 *
 * @tparam T The type of the object pointed to to be printed
 */
template <typename T>
void ArgPrinter<const volatile T* const>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<const volatile void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(static_cast<const volatile void*>(arg_)); !name.empty())
        *os_ << " (" << name << ")";
    if (recurse_ && bomb_detector(arg_, len_->get_ref()))
    {
        if constexpr (std::is_same_v<T, unsigned char>)
        {
            if (auto len = len_->get_ref() == 0
                               ? strlen(reinterpret_cast<const char*>(const_cast<const unsigned char*>(arg_)))
                               : len_->get_ref(); len > MAX_PRINT_BYTES)
            {
                std::stringstream fname_ss;
                fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                    << data_file_suffix;
                auto os = std::ofstream(fname_ss.str());
                os.write(reinterpret_cast<const char*>(const_cast<const unsigned char*>(arg_)),
                         static_cast<std::streamsize>(len));
                os.close();
                *os_ << std::endl << prefix << '\t' << fname_ss.str();
                prefix.pop_back();
                return;
            }
        }
        if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
            *os_ << std::endl << prefix << '\t' << "[RECURSION]";
        else
        {
            used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
            if (len_->get_ref() != 0)
                for (auto i = 0; end_test_(i); ++i)
                {
                    std::stringstream ss;
                    ss << name_ << "[" << i << "]";
                    const auto next = new ArgPrinter<const volatile T>(arg_[i], ss.str(), depth_, enum_printers_, os_);
                    next->print_arg();
                }
            else
            {
                const auto next = new ArgPrinter<const volatile T>(*arg_, "*" + name_, depth_, enum_printers_, os_);
                next->print_arg();
            }
            used_addrs.pop_back();
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<const volatile char* const>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<const volatile void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        if (auto len = len_->get_ref() == 0 ? strlen(const_cast<const char*>(arg_)) : len_->get_ref(); len >
            MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                << data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(const_cast<const char*>(arg_), static_cast<std::streamsize>(len));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            std::string arg = const_cast<const char* const>(arg_);
            replace_all(arg, "\n", "\\n");
            *os_ << " {" << arg << "}";
            if (recurse_)
            {
                if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                    *os_ << std::endl << prefix << '\t' << "[RECURSION]";
                else
                {
                    used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                    for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                    {
                        std::stringstream ss;
                        ss << name_ << "[" << i << "]";
                        const auto next = new ArgPrinter<const volatile char>(
                            arg_[i], ss.str(), depth_, enum_printers_, os_);
                        next->print_arg();
                    }
                    used_addrs.pop_back();
                }
            }
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<char*>::print_arg() - Prints a char* pointer and its contents to @code *stream @endcode
 */
template <>
inline void ArgPrinter<const volatile wchar_t* const>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << static_cast<const volatile void*>(arg_);
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (bomb_detector(arg_, len_->get_ref()))
    {
        if (auto len = len_->get_ref() == 0
                           ? std::wcslen(const_cast<const wchar_t*>(arg_)) * sizeof(wchar_t)
                           : len_->get_ref() * sizeof(wchar_t); len > MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++
                << data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(reinterpret_cast<const char*>(const_cast<const wchar_t*>(arg_)),
                     static_cast<std::streamsize>(len));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            auto arg = wide_to_narrow_str(const_cast<const wchar_t* const>(arg_));
            replace_all(arg, "\n", "\\n");
            *os_ << " {" << arg << "}";
            if (recurse_)
            {
                if (std::ranges::find(used_addrs, reinterpret_cast<uintptr_t>(arg_)) != used_addrs.end())
                    *os_ << std::endl << prefix << '\t' << "[RECURSION]";
                else
                {
                    used_addrs.push_back(reinterpret_cast<uintptr_t>(arg_));
                    for (auto i = 0; len_->get_ref() == 0 ? i == 0 || arg_[i - 1] != 0 : end_test_(i); ++i)
                    {
                        std::stringstream ss;
                        ss << name_ << "[" << i << "]";
                        const auto next = new ArgPrinter<const volatile wchar_t>(
                            arg_[i], ss.str(), depth_, enum_printers_, os_);
                        next->print_arg();
                    }
                    used_addrs.pop_back();
                }
            }
        }
    }
    prefix.pop_back();
}

/**
 * ArgPrinter<const void* const>::print_arg() - Prints a void pointer to @code *stream @endcode
 */
template <>
inline void ArgPrinter<const volatile void* const>::print_arg()
{
    prefix += '\t';
    *os_ << std::endl << prefix << name_ << ": (" << get_type(arg_) << ") " << arg_;
    if (enum_printers_.contains(depth_))
        *os_ << " [" << enum_printer(arg_) << "]";
    if (const auto name = get_symbol_name(arg_); !name.empty())
        *os_ << " (" << name << ")";
    if (recurse_ && len_->get_ref() != 0 &&
        bomb_detector(static_cast<const volatile std::byte*>(arg_), len_->get_ref()))
    {
        if (len_->get_ref() > MAX_PRINT_BYTES)
        {
            std::stringstream fname_ss;
            fname_ss << abii_logdir << "/" << data_file_prefix << getpid() << "_" << gettid() << "_" << file_num++ <<
                data_file_suffix;
            auto os = std::ofstream(fname_ss.str());
            os.write(static_cast<const char*>(const_cast<const void*>(arg_)),
                     static_cast<std::streamsize>(len_->get_ref()));
            os.close();
            *os_ << std::endl << prefix << '\t' << fname_ss.str();
        }
        else
        {
            *os_ << std::endl;
            hexdump(*os_, arg_, len_->get_ref());
        }
    }
    prefix.pop_back();
}
}

#endif //ARGPRINTERPOINTER_H
