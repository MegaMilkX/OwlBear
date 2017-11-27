#ifndef UTIL_H
#define UTIL_H

#include <windows.h>

#include <vector>
#include <string>

inline std::vector<std::string> find_files(const std::string& filename)
{
    std::vector<std::string> result;

    WIN32_FIND_DATAA find_data;
    HANDLE h;
    long long time = 0;

    h = FindFirstFileA(filename.c_str(), &find_data);
    if (h == NULL)
    {
        return result;
    }

    result.push_back(std::string(find_data.cFileName));

    while (FindNextFileA(h, &find_data))
    {
        result.push_back(std::string(find_data.cFileName));
    }

    return result;
}

inline std::vector<std::string> split(const std::string& str, const std::string& delim)
{
    std::string s = str;
    std::vector<std::string> tokens;

    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delim)) != std::string::npos)
    {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delim.length());
    }

    tokens.push_back(s);

    return tokens;
}

#endif
