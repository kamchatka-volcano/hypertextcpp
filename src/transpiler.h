#pragma once
#include <filesystem>
#include <string>
#include <map>

namespace htcpp{
namespace fs = std::filesystem;

std::string transpileToSingleHeaderRendererClass(const fs::path& filePath,
                                                 const std::string& className);
std::string transpileToSharedLibRendererClass(const fs::path& filePath);
}
