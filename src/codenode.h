#pragma once
#include "document_node_interface_access.h"
#include "idocumentnode.h"
#include "idocumentnoderenderer.h"
#include "nodeextension.h"
#include <optional>

namespace htcpp {

class CodeNode {
public:
    CodeNode(std::string nodeTypeName, char idToken, char openToken, char closeToken, StreamReader& stream);
    const std::string& content() const;
    const std::optional<NodeExtension>& extension() const;

private:
    void load(StreamReader& stream);

private:
    std::string nodeTypeName_;
    char idToken_;
    char openToken_;
    char closeToken_;
    std::string content_;
    std::optional<NodeExtension> extension_;
};

class ExpressionNode : public IDocumentNode,
                       public IDocumentNodeRenderer {
    DOCUMENT_NODE_INTERFACE_ACCESS(IDocumentNodeRenderer)
public:
    explicit ExpressionNode(StreamReader& stream);
    std::string renderingCode() const override;

private:
    CodeNode impl_;
};

class StatementNode : public IDocumentNode,
                      public IDocumentNodeRenderer {
    DOCUMENT_NODE_INTERFACE_ACCESS(IDocumentNodeRenderer)

public:
    explicit StatementNode(StreamReader& stream);
    std::string renderingCode() const override;

private:
    CodeNode impl_;
};

class GlobalStatementNode : public IDocumentNode,
                            public IDocumentNodeRenderer {
    DOCUMENT_NODE_INTERFACE_ACCESS(IDocumentNodeRenderer)
public:
    explicit GlobalStatementNode(StreamReader& stream);
    std::string renderingCode() const override;

private:
    CodeNode impl_;
};

}
