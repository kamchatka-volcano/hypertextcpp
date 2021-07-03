#pragma once
#include <string>

namespace htcpp::utils{

bool isTagEmptyElement(const std::string& tagName);
std::string preprocessRawStrings(const std::string& cppCode);
bool stringHasContent(const std::string& str);

}
