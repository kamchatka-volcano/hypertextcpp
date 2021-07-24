#pragma once
#include <string>
#include <vector>
#include <memory>

namespace htcpp{
class IDocumentNode;
}

namespace htcpp::utils{

bool isTagEmptyElement(const std::string& tagName);
std::string preprocessRawStrings(const std::string& cppCode);
bool isBlank(const std::string& str);
void trimBlankLines(std::string& str);

void consumeReadAttributesText(std::string& readText, std::vector<std::unique_ptr<IDocumentNode>>& nodes);
void consumeReadText(std::string& readText, std::vector<std::unique_ptr<IDocumentNode>>& nodes, IDocumentNode* newNode = nullptr);
}
