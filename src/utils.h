#pragma once
#include <string>

namespace htcpp{

bool isTagEmptyElement(const std::string& tagName);
std::string preprocessRawStrings(const std::string& cppCode);

}
