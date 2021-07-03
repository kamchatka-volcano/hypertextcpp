#pragma once
#include "idocumentnode.h"
#include <map>
#include <memory>

namespace htcpp{
class StreamReader;

class NodeReader{
public:
    std::unique_ptr<IDocumentNode> readTopLevelNode(StreamReader& stream);
    std::unique_ptr<IDocumentNode> readTagAttributeNode(StreamReader& stream);
    std::unique_ptr<IDocumentNode> readTagContentNode(StreamReader& stream);
    std::unique_ptr<IDocumentNode> readSectionNode(StreamReader& stream);

private:
    std::unique_ptr<IDocumentNode> readCommonNodes(StreamReader& stream);

private:
    std::map<std::string, std::string> macrosMap_;
};



}
