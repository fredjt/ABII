//
// Created by Trent Tanchin on 5/19/24.
//

#ifndef ABII_UTILS_H
#define ABII_UTILS_H

#include <cassert>
#include <string>
#include <unicode/unistr.h>
#include <unicode/ustream.h>

namespace abii
{
inline bool starts_with(const std::string& str, const char& prefix)
{
    return str.find(prefix, 0) == 0;
}

inline bool ends_with(const std::string& str, const char& suffix)
{
    return str.back() == suffix;
}

enum strip_side
{
    FRONT,
    BACK
};

inline std::string strip(std::string str, const char& chr, const strip_side side)
{
    switch (side)
    {
    case FRONT:
        while (starts_with(str, chr))
            str.erase(0, 1);
    case BACK:
        while (ends_with(str, chr))
            str.pop_back();
    }
    return str;
}

inline std::vector<std::string> get_lines(const std::string& str)
{
    std::stringstream ss(str), ret_ss;
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(ss, line))
        lines.push_back(line);
    return lines;
}

inline std::vector<std::tuple<int, int, bool>> findLCS(const std::vector<std::string>& lines1,
                                                       const std::vector<std::string>& lines2)
{
    const auto m = lines1.size();
    const auto n = lines2.size();
    std::vector lcsLength(m + 1, std::vector(n + 1, 0));

    for (auto i = 1; i <= m; ++i)
    {
        for (auto j = 1; j <= n; ++j)
        {
            auto arg1 = lines1[i - 1].substr(0, lines1[i - 1].find(':'));
            auto arg2 = lines2[j - 1].substr(0, lines2[j - 1].find(':'));
            if (arg1 == arg2)
                lcsLength[i][j] = lcsLength[i - 1][j - 1] + 1;
            else
                lcsLength[i][j] = std::max(lcsLength[i - 1][j], lcsLength[i][j - 1]);
        }
    }

    std::vector<std::tuple<int, int, bool>> lcs;
    auto i = m, j = n;
    while (i > 0 && j > 0)
    {
        std::string arg1 = lines1[i - 1].substr(0, lines1[i - 1].find(':'));
        std::string arg2 = lines2[j - 1].substr(0, lines2[j - 1].find(':'));
        if (arg1 == arg2)
        {
            lcs.emplace_back(i - 1, j - 1, lines1[i - 1] == lines2[j - 1]);
            --i;
            --j;
        }
        else if (lcsLength[i - 1][j] > lcsLength[i][j - 1])
            --i;
        else
            --j;
    }

    std::ranges::reverse(lcs);
    return lcs;
}

inline std::string wide_to_narrow_char(const wchar_t wide_char)
{
    const icu::UnicodeString unicode_str(static_cast<const UChar>(wide_char));

    std::string narrow_string;
    unicode_str.toUTF8String(narrow_string);

    return narrow_string;
}

inline std::string wide_to_narrow_str(const std::wstring& wide_string)
{
    std::stringstream ss;
    for (const auto wchar : wide_string)
        ss << wide_to_narrow_char(wchar);
    return ss.str();
}
}

#endif //ABII_UTILS_H
