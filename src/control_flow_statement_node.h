#pragma once
#include "document_node_interface_access.h"
#include "idocumentnode.h"
#include "idocumentnoderenderer.h"
#include "nodeextension.h"
#include <sfun/optional_ref.h>
#include <gsl/assert>

namespace htcpp {

enum class ControlFlowStatementNodeType {
    Open,
    Close
};

class ControlFlowStatementNode : public IDocumentNode,
                                 public IDocumentNodeRenderer {
    DOCUMENT_NODE_INTERFACE_ACCESS(IDocumentNodeRenderer)

public:
    ControlFlowStatementNode(ControlFlowStatementNodeType, NodeExtension nodeExtension);
    std::string renderingCode() const override;

private:
    ControlFlowStatementNodeType type_;
    NodeExtension nodeExtension_;
};

} //namespace htcpp