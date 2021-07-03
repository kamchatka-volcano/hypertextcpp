#pragma once
#include "idocumentnode.h"
#include "nodeextension.h"

namespace htcpp{

class CodeNode : public IDocumentNode{
public:
    CodeNode(const std::string& name,
             char idToken,
             char openToken,
             char closeToken,
             bool isGlobal,
             StreamReader& stream);
    std::string docTemplate() override;
    std::string docRenderingCode() override;
    bool isGlobal() override;

protected:
    std::string content_;
    NodeExtension extension_;

private:
    void load(StreamReader& stream);

private:
    std::string name_;
    char idToken_;
    char openToken_;
    char closeToken_;
    bool isGlobal_;
    bool extensionIsOnClosingToken_ = false;
};

class ExpressionNode : public CodeNode
{
public:
    ExpressionNode(StreamReader& stream) :
        CodeNode("Expression ", '$', '(', ')', false, stream)
    {}
    std::string docRenderingCode() override;
};

class RenderStatementNode : public CodeNode
{
public:
    RenderStatementNode(StreamReader& stream) :
        CodeNode("Render statement", '$', '{', '}', false, stream)
    {}    
};

class GlobalStatementNode : public CodeNode
{
public:
    GlobalStatementNode(StreamReader& stream) :
        CodeNode("Global statement", '#', '{', '}', true, stream)
    {}
};

}
