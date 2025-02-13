#pragma once
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
public:
    ControlFlowStatementNode(ControlFlowStatementNodeType, NodeExtension nodeExtension);
    std::string renderingCode() const override;

private:
    ControlFlowStatementNodeType type_;
    NodeExtension nodeExtension_;
};

} //namespace htcpp