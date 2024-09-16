#pragma once
#include "document_node_interface_access.h"
#include "idocumentnode.h"
#include "idocumentnoderenderer.h"
#include <memory>
#include <string>
#include <vector>

namespace htcpp {
class StreamReader;

class ProcedureNode : public IDocumentNode,
                      public IDocumentNodeRenderer {
    DOCUMENT_NODE_INTERFACE_ACCESS(IDocumentNodeRenderer)

public:
    ProcedureNode(std::string procedureName, StreamReader& stream);
    const std::string& name() const;

    std::string renderingCode() const override;

private:
    void load(StreamReader& stream);

private:
    std::string procedureName_;
    std::vector<std::unique_ptr<IDocumentNode>> contentNodes_;
};

} //namespace htcpp
