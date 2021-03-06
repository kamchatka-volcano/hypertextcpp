#pragma once
#include <string>

namespace htcpp::utils{

std::string toPascalCase(const std::string& name);
std::string toSnakeCase(const std::string& name);
std::string toLowerCase(const std::string& name);

}
