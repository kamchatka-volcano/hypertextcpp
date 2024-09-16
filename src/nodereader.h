#pragma once
#include "idocumentnode.h"
#include <map>
#include <memory>

namespace htcpp{
    class GlobalStatementNode;
    class ProcedureNode;
    class StreamReader;

    std::unique_ptr<IDocumentNode> readTagAttributeNode(StreamReader& stream);
    std::unique_ptr<IDocumentNode> readTagContentNode(StreamReader& stream);
    std::unique_ptr<IDocumentNode> readNonTagNode(StreamReader& stream);
    std::unique_ptr<IDocumentNode> readGlobalStatement(StreamReader& stream);
    std::unique_ptr<ProcedureNode> readProcedure(StreamReader& stream);
}
