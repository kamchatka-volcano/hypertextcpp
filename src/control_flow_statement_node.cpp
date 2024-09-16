#include "control_flow_statement_node.h"

namespace htcpp {
ControlFlowStatementNode::ControlFlowStatementNode(ControlFlowStatementNodeType type, NodeExtension nodeExtension)
    : type_{type}
    , nodeExtension_{std::move(nodeExtension)}
{
}

std::string ControlFlowStatementNode::renderingCode() const
{
    if (type_ == ControlFlowStatementNodeType::Open) {
        if (nodeExtension_.type() == NodeExtension::Type::Conditional)
            return "if (" + nodeExtension_.content() + "){ ";
        else if (nodeExtension_.type() == NodeExtension::Type::Loop)
            return "for (" + nodeExtension_.content() + "){ ";
    }

    return " } ";
}

} //namespace htcpp
