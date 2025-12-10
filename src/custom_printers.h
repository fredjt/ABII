//
// Created by Trent Tanchin on 10/4/24.
//

#ifndef CUSTOM_PRINTERS_H
#define CUSTOM_PRINTERS_H

#include <cstdarg>
#include <mqueue.h>
#include <sstream>
#include <string>

#include "libabii.h"

#define CUSTOM_PRINT_PREFIX \
std::stringstream ss; \
bool first = true; \
int n = 0; \
const auto args = new ArgsPrinter();

#define CUSTOM_PRINT_SUFFIX \
args->print_args(); \
delete args; \
return ss.str();

#define FUNCTION_ARGS_FMT(type) \
    std::stringstream ss1; \
    ss1 << "[" << n++ << "]"; \
    args->push_arg(new abii::ArgPrinter<type>(va_arg(vargs, type), ss1.str(), &ss));

namespace abii
{
// TODO: Support flags and widths for OUTPUT_FUNCTION_ARGS
inline std::string print_variadic_args_printf(const char* fmt, va_list vargs_ro, size_t /*size*/)
{
    CUSTOM_PRINT_PREFIX
    va_list vargs;
    va_copy(vargs, vargs_ro);
    const char* p = fmt;
    while (*p)
    {
        if (*p == '%')
        {
            // ReSharper disable once CppPossiblyErroneousEmptyStatements
            while (*++p && (*p < 65 || (*p > 90 && *p < 97) || *p > 122));
            switch (*p)
            {
            case 'a':
                {
                    FUNCTION_ARGS_FMT(double)
                    break;
                }
            case 'A':
                {
                    FUNCTION_ARGS_FMT(double)
                    break;
                }
            case 'c':
                {
                    FUNCTION_ARGS_FMT(int)
                    break;
                }
            case 'C':
                {
                    FUNCTION_ARGS_FMT(int)
                    break;
                }
            case 'd':
                {
                    FUNCTION_ARGS_FMT(int)
                    break;
                }
            case 'e':
                {
                    FUNCTION_ARGS_FMT(double)
                    break;
                }
            case 'E':
                {
                    FUNCTION_ARGS_FMT(double)
                    break;
                }
            case 'f':
                {
                    FUNCTION_ARGS_FMT(double)
                    break;
                }
            case 'F':
                {
                    FUNCTION_ARGS_FMT(double)
                    break;
                }
            case 'g':
                {
                    FUNCTION_ARGS_FMT(double)
                    break;
                }
            case 'G':
                {
                    FUNCTION_ARGS_FMT(double)
                    break;
                }
            case 'h':
                {
                    ++p;
                    switch (*p)
                    {
                    case 'c':
                        {
                            FUNCTION_ARGS_FMT(int)
                            break;
                        }
                    case 'C':
                        {
                            FUNCTION_ARGS_FMT(int)
                            break;
                        }
                    case 'd':
                        {
                            FUNCTION_ARGS_FMT(int)
                            break;
                        }
                    case 'h':
                        {
                            ++p;
                            switch (*p)
                            {
                            case 'd':
                                {
                                    FUNCTION_ARGS_FMT(int)
                                    break;
                                }
                            case 'i':
                                {
                                    FUNCTION_ARGS_FMT(int)
                                    break;
                                }
                            case 'n':
                                {
                                    if (const auto val = va_arg(vargs, signed char*); val != nullptr)
                                        *val = p - fmt; // NOLINT(*-narrowing-conversions)
                                    break;
                                }
                            case 'o':
                                {
                                    FUNCTION_ARGS_FMT(int)
                                    break;
                                }
                            case 'u':
                                {
                                    FUNCTION_ARGS_FMT(int)
                                    break;
                                }
                            case 'x':
                                {
                                    FUNCTION_ARGS_FMT(int)
                                    break;
                                }
                            case 'X':
                                {
                                    FUNCTION_ARGS_FMT(int)
                                    break;
                                }
                            default:
                                break;
                            }
                            break;
                        }
                    case 'i':
                        {
                            FUNCTION_ARGS_FMT(int)
                            break;
                        }
                    case 'n':
                        {
                            if (const auto val = va_arg(vargs, short*); val != nullptr)
                                *val = p - fmt; // NOLINT(*-narrowing-conversions)
                            break;
                        }
                    case 'o':
                        {
                            FUNCTION_ARGS_FMT(int)
                            break;
                        }
                    case 's':
                        {
                            FUNCTION_ARGS_FMT(const char*)
                            break;
                        }
                    case 'S':
                        {
                            FUNCTION_ARGS_FMT(const char*)
                            break;
                        }
                    case 'u':
                        {
                            FUNCTION_ARGS_FMT(int)
                            break;
                        }
                    case 'x':
                        {
                            FUNCTION_ARGS_FMT(int)
                            break;
                        }
                    case 'X':
                        {
                            FUNCTION_ARGS_FMT(int)
                            break;
                        }
                    default:
                        break;
                    }
                    break;
                }
            case 'i':
                {
                    FUNCTION_ARGS_FMT(int)
                    break;
                }
            case 'j':
                {
                    ++p;
                    switch (*p)
                    {
                    case 'd':
                        {
                            FUNCTION_ARGS_FMT(long long)
                            break;
                        }
                    case 'i':
                        {
                            FUNCTION_ARGS_FMT(long long)
                            break;
                        }
                    case 'n':
                        {
                            if (const auto val = va_arg(vargs, long long*); val != nullptr)
                                *val = p - fmt;
                            break;
                        }
                    case 'o':
                        {
                            FUNCTION_ARGS_FMT(unsigned long long)
                            break;
                        }
                    case 'u':
                        {
                            FUNCTION_ARGS_FMT(unsigned long long)
                            break;
                        }
                    case 'x':
                        {
                            FUNCTION_ARGS_FMT(unsigned long long)
                            break;
                        }
                    case 'X':
                        {
                            FUNCTION_ARGS_FMT(unsigned long long)
                            break;
                        }
                    default:
                        break;
                    }
                    break;
                }
            case 'l':
                {
                    ++p;
                    switch (*p)
                    {
                    case 'a':
                        {
                            FUNCTION_ARGS_FMT(double)
                            break;
                        }
                    case 'A':
                        {
                            FUNCTION_ARGS_FMT(double)
                            break;
                        }
                    case 'c':
                        {
                            FUNCTION_ARGS_FMT(int)
                            break;
                        }
                    case 'C':
                        {
                            FUNCTION_ARGS_FMT(int)
                            break;
                        }
                    case 'd':
                        {
                            FUNCTION_ARGS_FMT(long)
                            break;
                        }
                    case 'e':
                        {
                            FUNCTION_ARGS_FMT(double)
                            break;
                        }
                    case 'E':
                        {
                            FUNCTION_ARGS_FMT(double)
                            break;
                        }
                    case 'f':
                        {
                            FUNCTION_ARGS_FMT(double)
                            break;
                        }
                    case 'F':
                        {
                            FUNCTION_ARGS_FMT(double)
                            break;
                        }
                    case 'g':
                        {
                            FUNCTION_ARGS_FMT(double)
                            break;
                        }
                    case 'G':
                        {
                            FUNCTION_ARGS_FMT(double)
                            break;
                        }
                    case 'i':
                        {
                            FUNCTION_ARGS_FMT(long)
                            break;
                        }
                    case 'l':
                        {
                            ++p;
                            switch (*p)
                            {
                            case 'd':
                                {
                                    FUNCTION_ARGS_FMT(long long)
                                    break;
                                }
                            case 'i':
                                {
                                    FUNCTION_ARGS_FMT(long long)
                                    break;
                                }
                            case 'n':
                                {
                                    if (const auto val = va_arg(vargs, long long*); val != nullptr)
                                        *val = p - fmt;
                                    break;
                                }
                            case 'o':
                                {
                                    FUNCTION_ARGS_FMT(unsigned long long)
                                    break;
                                }
                            case 'u':
                                {
                                    FUNCTION_ARGS_FMT(unsigned long long)
                                    break;
                                }
                            case 'x':
                                {
                                    FUNCTION_ARGS_FMT(unsigned long long)
                                    break;
                                }
                            case 'X':
                                {
                                    FUNCTION_ARGS_FMT(unsigned long long)
                                    break;
                                }
                            default:
                                break;
                            }
                            break;
                        }
                    case 'n':
                        {
                            if (const auto val = va_arg(vargs, long*); val != nullptr)
                                *val = p - fmt;
                            break;
                        }
                    case 'o':
                        {
                            FUNCTION_ARGS_FMT(unsigned long long)
                            break;
                        }
                    case 's':
                        {
                            FUNCTION_ARGS_FMT(const wchar_t*)
                            break;
                        }
                    case 'S':
                        {
                            FUNCTION_ARGS_FMT(const wchar_t*)
                            break;
                        }
                    case 'u':
                        {
                            FUNCTION_ARGS_FMT(unsigned long long)
                            break;
                        }
                    case 'x':
                        {
                            FUNCTION_ARGS_FMT(unsigned long long)
                            break;
                        }
                    case 'X':
                        {
                            FUNCTION_ARGS_FMT(unsigned long long)
                            break;
                        }
                    default:
                        break;
                    }
                    break;
                }
            case 'L':
                {
                    ++p;
                    switch (*p)
                    {
                    case 'a':
                        {
                            FUNCTION_ARGS_FMT(long double)
                            break;
                        }
                    case 'A':
                        {
                            FUNCTION_ARGS_FMT(long double)
                            break;
                        }
                    case 'e':
                        {
                            FUNCTION_ARGS_FMT(long double)
                            break;
                        }
                    case 'E':
                        {
                            FUNCTION_ARGS_FMT(long double)
                            break;
                        }
                    case 'f':
                        {
                            FUNCTION_ARGS_FMT(long double)
                            break;
                        }
                    case 'F':
                        {
                            FUNCTION_ARGS_FMT(long double)
                            break;
                        }
                    case 'g':
                        {
                            FUNCTION_ARGS_FMT(long double)
                            break;
                        }
                    case 'G':
                        {
                            FUNCTION_ARGS_FMT(long double)
                            break;
                        }
                    default:
                        break;
                    }
                    break;
                }
            case 'n':
                {
                    if (const auto val = va_arg(vargs, int*); val != nullptr)
                        *val = p - fmt; // NOLINT(*-narrowing-conversions)
                    break;
                }
            case 'o':
                {
                    FUNCTION_ARGS_FMT(unsigned)
                    break;
                }
            case 'p':
                {
                    FUNCTION_ARGS_FMT(const void*)
                    break;
                }
            case 's':
                {
                    FUNCTION_ARGS_FMT(const char*)
                    break;
                }
            case 'S':
                {
                    FUNCTION_ARGS_FMT(const wchar_t*)
                    break;
                }
            case 't':
                {
                    ++p;
                    switch (*p)
                    {
                    case 'd':
                        {
                            FUNCTION_ARGS_FMT(long)
                            break;
                        }
                    case 'i':
                        {
                            FUNCTION_ARGS_FMT(long)
                            break;
                        }
                    case 'n':
                        {
                            if (const auto val = va_arg(vargs, long*); val != nullptr)
                                *val = p - fmt;
                            break;
                        }
                    case 'o':
                        {
                            FUNCTION_ARGS_FMT(unsigned long long)
                            break;
                        }
                    case 'u':
                        {
                            FUNCTION_ARGS_FMT(unsigned long long)
                            break;
                        }
                    case 'x':
                        {
                            FUNCTION_ARGS_FMT(unsigned long long)
                            break;
                        }
                    case 'X':
                        {
                            FUNCTION_ARGS_FMT(unsigned long long)
                            break;
                        }
                    default:
                        break;
                    }
                    break;
                }
            case 'u':
                {
                    FUNCTION_ARGS_FMT(unsigned)
                    break;
                }
            case 'x':
                {
                    FUNCTION_ARGS_FMT(unsigned)
                    break;
                }
            case 'X':
                {
                    FUNCTION_ARGS_FMT(unsigned)
                    break;
                }
            case 'z':
                {
                    ++p;
                    switch (*p)
                    {
                    case 'd':
                        {
                            FUNCTION_ARGS_FMT(long)
                            break;
                        }
                    case 'i':
                        {
                            FUNCTION_ARGS_FMT(long)
                            break;
                        }
                    case 'n':
                        {
                            if (const auto val = va_arg(vargs, size_t *); val != nullptr)
                                *val = p - fmt;
                            break;
                        }
                    case 'o':
                        {
                            FUNCTION_ARGS_FMT(unsigned long long)
                            break;
                        }
                    case 'u':
                        {
                            FUNCTION_ARGS_FMT(unsigned long long)
                            break;
                        }
                    case 'x':
                        {
                            FUNCTION_ARGS_FMT(unsigned long long)
                            break;
                        }
                    case 'X':
                        {
                            FUNCTION_ARGS_FMT(unsigned long long)
                            break;
                        }
                    default:
                        break;
                    }
                    break;
                }
            case 'Z':
                {
                    FUNCTION_ARGS_FMT(const void*)
                    break;
                }
            default:
                break;
            }
        }
        ++p;
    }
    va_end(vargs);
    CUSTOM_PRINT_SUFFIX
}
}

#endif //CUSTOM_PRINTERS_H
