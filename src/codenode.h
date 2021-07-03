#pragma once
#include "idocumentnode.h"

namespace htcpp{

class CodeNode : public IDocumentNode{
public:
    CodeNode(const std::string& name,
             char idToken,
             char openToken,
             char closeToken,
             bool isGlobalScoped);
    void load(StreamReader& stream) override;
    std::string docTemplate() override;
    std::string docRenderingCode() override;
    bool isGlobalScoped() override;

protected:
    std::string content_;

private:
    std::string name_;
    char idToken_;
    char openToken_;
    char closeToken_;
    bool isGlobalScoped_;
};

class ExpressionNode : public CodeNode
{
public:
    ExpressionNode() :
        CodeNode("Expression ", '$', '(', ')', false)
    {}
    std::string docRenderingCode() override;
};

class RenderStatementNode : public CodeNode
{
public:
    RenderStatementNode() :
        CodeNode("Render statement", '$', '{', '}', false)
    {}
};

class GlobalStatementNode : public CodeNode
{
public:
    GlobalStatementNode() :
        CodeNode("Global statement", '#', '{', '}', true)
    {}
};

}
