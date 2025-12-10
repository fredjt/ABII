//
// Created by Trent Tanchin on 11/26/25.
//

#define BOOST_TEST_MODULE abii_tests

#include <libabii.h>
#include <boost/test/included/unit_test.hpp>

#include "custom_printers.h"

#define TEST_TYPE(type, init_val)                               \
{                                                               \
    const auto pi_args = new abii::ArgsPrinter();               \
    type arg = init_val;                                        \
    pi_args->push_arg(new abii::ArgPrinter(arg, "arg", &std::cout));  \
    BOOST_CHECK_NO_THROW(pi_args->print_args());                \
    delete pi_args;                                             \
}

struct test_struct
{
    bool a = BOOL_MAX;
    short b = SHRT_MAX;
    int c = INT_MAX;
    long d = LONG_MAX;
    long long e = LONG_LONG_MAX;
    float f = FLT_MAX;
    double g = DBL_MAX;
    long double h = LDBL_MAX;
    void* i = &std::cout;
    int* j = nullptr;
    test_struct* k;
    int* l = new int(1);
};

std::ostream& operator<<(std::ostream& os, const test_struct& obj)
{
    OVERRIDE_STREAM_PREFIX
    abii_args->push_arg(new abii::ArgPrinter(obj.a, "a", &os));
    abii_args->push_arg(new abii::ArgPrinter(obj.b, "b", &os));
    abii_args->push_arg(new abii::ArgPrinter(obj.c, "c", &os));
    abii_args->push_arg(new abii::ArgPrinter(obj.d, "d", &os));
    abii_args->push_arg(new abii::ArgPrinter(obj.e, "e", &os));
    abii_args->push_arg(new abii::ArgPrinter(obj.f, "f", &os));
    abii_args->push_arg(new abii::ArgPrinter(obj.g, "g", &os));
    abii_args->push_arg(new abii::ArgPrinter(obj.h, "h", &os));
    abii_args->push_arg(new abii::ArgPrinter(obj.i, "i", &os));
    abii_args->push_arg(new abii::ArgPrinter(obj.j, "j", &os));
    abii_args->push_arg(new abii::ArgPrinter(obj.k, "k", &os));
    abii_args->push_arg(new abii::ArgPrinter(obj.l, "l", &os, RECURSE));
    OVERRIDE_STREAM_SUFFIX
}

const char* va_func(const char* fmt, ...)
{
    TRACE_LOGGER
    abii::prefix = "";
    const auto abii_args = new abii::ArgsPrinter();
    va_list abii_vargs;
    abii::pre_fmtd_str str = "va_func(fmt, ...)";
    abii_args->push_func(new abii::ArgPrinter(str, "", &std::cout));

    abii_args->push_arg(new abii::ArgPrinter(fmt, "__fmt", &std::cout));

    va_start(abii_vargs, fmt);
    auto (printer) = new abii::ArgPrinter(abii_vargs, "...", &std::cout);
    printer->set_fmt(fmt);
    printer->set_va_list_printer(abii::print_variadic_args_printf);
    abii_args->push_arg(printer);
    va_end(abii_vargs);

    auto msg = "Test return";

    abii_args->push_return(new abii::ArgPrinter(msg, "return", &std::cout));

    va_start(abii_vargs, fmt);
    abii_args->print_args();
    va_end(abii_vargs);
    abii::abii_stream << std::endl;
    delete abii_args;
    return msg;
}

BOOST_AUTO_TEST_CASE(test_bool)
{
    auto abii_logger = Logger("test_bool");
    TEST_TYPE(bool, 0)
    TEST_TYPE(bool, BOOL_MAX)
}

BOOST_AUTO_TEST_CASE(test_char)
{
    auto abii_logger = Logger("test_char");
    TEST_TYPE(char, CHAR_MIN)
    TEST_TYPE(char, 0)
    TEST_TYPE(char, CHAR_MAX)
}

BOOST_AUTO_TEST_CASE(test_short)
{
    auto abii_logger = Logger("test_short");
    TEST_TYPE(short, SHRT_MIN)
    TEST_TYPE(short, 0)
    TEST_TYPE(short, SHRT_MAX)
}

BOOST_AUTO_TEST_CASE(test_int)
{
    auto abii_logger = Logger("test_int");
    TEST_TYPE(int, INT_MIN)
    TEST_TYPE(int, 0)
    TEST_TYPE(int, INT_MAX)
}

BOOST_AUTO_TEST_CASE(test_long)
{
    auto abii_logger = Logger("test_long");
    TEST_TYPE(long, LONG_MIN)
    TEST_TYPE(long, 0)
    TEST_TYPE(long, LONG_MAX)
}

BOOST_AUTO_TEST_CASE(test_long_long)
{
    auto abii_logger = Logger("test_long_long");
    TEST_TYPE(long long, LONG_LONG_MIN)
    TEST_TYPE(long long, 0)
    TEST_TYPE(long long, LONG_LONG_MAX)
}

BOOST_AUTO_TEST_CASE(test_float)
{
    auto abii_logger = Logger("test_float");
    TEST_TYPE(float, FLT_MIN)
    TEST_TYPE(float, 0)
    TEST_TYPE(float, FLT_MAX)
}

BOOST_AUTO_TEST_CASE(test_double)
{
    auto abii_logger = Logger("test_double");
    TEST_TYPE(double, DBL_MIN)
    TEST_TYPE(double, 0)
    TEST_TYPE(double, DBL_MAX)
}

BOOST_AUTO_TEST_CASE(test_long_double)
{
    auto abii_logger = Logger("test_long_double");
    TEST_TYPE(long double, LDBL_MIN)
    TEST_TYPE(long double, 0)
    TEST_TYPE(long double, LDBL_MAX)
}

BOOST_AUTO_TEST_CASE(test_intptr_t)
{
    auto abii_logger = Logger("test_intptr_t");
    TEST_TYPE(intptr_t, INTPTR_MIN)
    TEST_TYPE(intptr_t, 0)
    TEST_TYPE(intptr_t, INTPTR_MAX)
}

BOOST_AUTO_TEST_CASE(test_intmax_t)
{
    auto abii_logger = Logger("test_intmax_t");
    TEST_TYPE(intmax_t, INTMAX_MIN)
    TEST_TYPE(intmax_t, 0)
    TEST_TYPE(intmax_t, INTMAX_MAX)
}

BOOST_AUTO_TEST_CASE(test_test_struct)
{
    auto abii_logger = Logger("test_test_struct");
    const auto pi_args = new abii::ArgsPrinter();
    test_struct arg;
    arg.k = &arg;
    auto argp = &arg;
    pi_args->push_arg(new abii::ArgPrinter(arg, "arg", &std::cout));
    pi_args->push_arg(new abii::ArgPrinter(argp, "argp", &std::cout));
    arg.a = false;
    BOOST_CHECK_NO_THROW(pi_args->print_args());
    delete pi_args;
}

BOOST_AUTO_TEST_CASE(test_va_func)
{
    va_func("Test va_func: %d, %s, %f\n", 42, "Hello, World!", 3.14);
}
