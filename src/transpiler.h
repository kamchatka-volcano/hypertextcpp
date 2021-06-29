#pragma once
#include <filesystem>
#include <string>
#include <map>

namespace fs = std::filesystem;

namespace htcpp{

std::string transpileToSingleHeaderRendererClass(const fs::path& filePath,
                                                 const std::string& className);
std::string transpileToSharedLibRendererClass(const fs::path& filePath);
}
