#include "nameutils.h"
#include <algorithm>

namespace htcpp::utils{
namespace{
std::string formatName(const std::string& name)
{
    auto result = name;
    //remove front non-alphabet characters
    result.erase(result.begin(), std::find_if(result.begin(), result.end(),
        [](int ch){
            return std::isalpha(ch);
        })
    );
    //remove back non-alphabet and non-digit characters
    result.erase(std::find_if(result.rbegin(), result.rend(),
        [](int ch){ return std::isalnum(ch);}).base(), result.end());
    return result;
}

std::string replace(const std::string &str, const std::string &subStr, const std::string &val)
{
    std::string res = std::string(str);
    auto pos = res.find(subStr);
    while (pos != std::string::npos){
        res.replace(pos, subStr.size(), val);
        pos = res.find(subStr, pos + val.size());
    }
    return res;
}

}

std::string toPascalCase(const std::string& name)
{
    auto result = std::string{};
    auto prevCharNonAlpha = false;
    auto formattedName = formatName(name);
    if (!formattedName.empty())
        formattedName[0] = static_cast<char>(std::toupper(formattedName[0]));
    for (auto ch : formattedName){
        if (!std::isalpha(ch)){
            if (std::isdigit(ch))
                result.push_back(ch);
            if (!result.empty())
                prevCharNonAlpha = true;
            continue;
        }
        if (prevCharNonAlpha)
            ch = static_cast<char>(std::toupper(ch));
        result.push_back(ch);
        prevCharNonAlpha = false;
    }
    return result;
}

std::string toSnakeCase(const std::string& name)
{
    auto result = std::string{};
    auto formattedName = formatName(replace(name, "-", "_"));
    if (!formattedName.empty())
        formattedName[0] = static_cast<char>(std::tolower(formattedName[0]));
    for (auto ch : formattedName){
        if (std::isupper(ch) && !result.empty()){
            result.push_back('_');
            result.push_back(static_cast<char>(std::tolower(ch)));
        }
        else
            result.push_back(ch);
    }
    return result;
}

std::string toLowerCase(const std::string& name)
{
    auto result = std::string{};
    for (auto ch : formatName(name)){
        if (std::isalnum(ch))
            result.push_back(static_cast<char>(std::tolower(ch)));
    }
    return result;
}

}
