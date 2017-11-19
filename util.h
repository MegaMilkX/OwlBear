#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>

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
