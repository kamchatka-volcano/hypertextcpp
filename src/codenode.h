#pragma once
#include "idocumentnode.h"

namespace htcpp{

class CodeNode : public IDocumentNode{
public:
    CodeNode(char idToken,
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
    char idToken_;
    char openToken_;
    char closeToken_;
    bool isGlobalScoped_;
};

class ExpressionNode : public CodeNode
{
public:
    ExpressionNode() :
        CodeNode('$', '(', ')', false)
    {}
    std::string docRenderingCode() override;
};

class RenderCodeNode : public CodeNode
{
public:
    RenderCodeNode() :
        CodeNode('$', '{', '}', false)
    {}
};

class GlobalCodeNode : public CodeNode
{
public:
    GlobalCodeNode() :
        CodeNode('#', '{', '}', true)
    {}
};

}
