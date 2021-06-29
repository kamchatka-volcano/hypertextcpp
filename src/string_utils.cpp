//#include "string_utils.h"
//#include <algorithm>

//namespace str{

//bool endsWith(const std::string &str, const std::string &val)
//{
//    auto res = std::find_end(str.begin(), str.end(), val.begin(), val.end());
//    if (res == str.end())
//        return false;
//    return std::distance(res, str.end()) == static_cast<int>(val.size());
//}

//std::string before(const std::string &str, const std::string& val)
//{
//    auto res = str.find(val);
//    if (res == std::string::npos)
//        return "";
//    return std::string(str.begin(), str.begin() + static_cast<int>(res));
//}

//std::string after(const std::string &str, const std::string& val)
//{
//    auto res = str.find(val);
//    if (res == std::string::npos)
//        return "";
//    return std::string(str.begin() + static_cast<int>(res + val.size()), str.end());
//}

//std::string replace(const std::string &str, const std::string &subStr, const std::string &val)
//{
//    std::string res = std::string(str);
//    auto pos = res.find(subStr);
//    while (pos != std::string::npos){
//        res.replace(pos, subStr.size(), val);
//        pos = res.find(subStr, pos + val.size());
//    }
//    return res;
//}

//std::string trimmed(const std::string& str)
//{
//    auto res = std::string(str);
//    res.erase(res.begin(), std::find_if(res.begin(), res.end(), [](int ch) {
//        return !std::isspace(ch);
//    }));
//    res.erase(std::find_if(res.rbegin(), res.rend(), [](int ch) {
//        return !std::isspace(ch);
//    }).base(), res.end());
//    return res;
//}

//std::string lastWord(const std::string& str)
//{
//    auto it = std::find_if(str.rbegin(), str.rend(), [](int ch) {
//        return std::isspace(ch);
//    });
//    if (it.base() == str.end())
//        return str;
//    return std::string(it.base(), str.end());
//}

//std::string chopWord(const std::string& str)
//{
//    auto it = std::find_if(str.rbegin(), str.rend(), [](int ch) {
//        return std::isspace(ch);
//    });
//    if (it.base() == str.end())
//        return str;
//    return std::string(str.begin(), it.base());
//}

//std::vector<std::string> split(const std::string& str, const std::string& delim, bool trim)
//{
//    std::vector<std::string> result;
//    std::size_t delimPos = 0;
//    while (delimPos != std::string::npos){
//        auto prevDelimPos = delimPos;
//        if (delimPos != 0){
//            delimPos += 1;
//            prevDelimPos += 1;
//        }
//        delimPos = str.find(delim, delimPos);
//        std::string part = std::string(str,prevDelimPos,delimPos - prevDelimPos);
//        if (trim)
//            part = trimmed(part);
//        if (trim || !part.empty())
//            result.push_back(part);
//    }
//    return result;
//}

//}

