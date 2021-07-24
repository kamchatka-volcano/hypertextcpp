#pragma once
#include "idocumentnode.h"
#include "nodeextension.h"

namespace htcpp{

class CodeNode : public IDocumentNode{
public:
    CodeNode(std::string name,
             char idToken,
             char openToken,
             char closeToken,
             StreamReader& stream);
    std::string docTemplate() override;
    std::string docRenderingCode() override;    

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
    bool extensionIsOnClosingToken_ = false;
};

class ExpressionNode : public CodeNode
{
public:
    explicit ExpressionNode(StreamReader& stream) :
        CodeNode("Expression ", '$', '(', ')', stream)
    {}
    std::string docRenderingCode() override;
};

class RenderStatementNode : public CodeNode
{
public:
    explicit RenderStatementNode(StreamReader& stream) :
        CodeNode("Render statement", '$', '{', '}', stream)
    {}    
};

class GlobalStatementNode : public CodeNode
{
public:
    explicit GlobalStatementNode(StreamReader& stream) :
        CodeNode("Global statement", '#', '{', '}', stream)
    {}
};

}
