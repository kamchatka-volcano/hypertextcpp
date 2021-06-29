#pragma once
#include "idocumentnode.h"
#include <memory>

namespace htcpp{
class StreamReader;

std::unique_ptr<IDocumentNode> readTopLevelNode(StreamReader& stream);
std::unique_ptr<IDocumentNode> readTagAttributeNode(StreamReader& stream);
std::unique_ptr<IDocumentNode> readTagContentNode(StreamReader& stream);
std::unique_ptr<IDocumentNode> readSectionNode(StreamReader& stream);

}
