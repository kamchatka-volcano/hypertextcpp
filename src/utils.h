#pragma once
#include <string>
#include <vector>
#include <memory>

namespace htcpp{
class IDocumentNode;
class ProcedureNode;
struct StreamReaderPosition;
}

namespace htcpp::utils{

bool isTagEmptyElement(const std::string& tagName);
std::string transformRawStrings(const std::string& cppCode, const StreamReaderPosition& position);
bool isBlank(const std::string& str);
void trimBlankLines(std::string& str);

void consumeReadAttributesText(std::string& readText, std::vector<std::unique_ptr<IDocumentNode>>& nodes);
void consumeReadText(std::string& readText, std::vector<std::unique_ptr<IDocumentNode>>& nodes,
        IDocumentNode* newNode = nullptr);

void replaceElementsWithIdsToProcedures(
        std::unique_ptr<IDocumentNode>& node,
        std::vector<std::unique_ptr<ProcedureNode>>& procedureNodes);
}
