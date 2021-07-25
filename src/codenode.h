#pragma once
#include "idocumentnode.h"
#include "nodeextension.h"

namespace htcpp{

class CodeNode : public IDocumentNode{
public:
    CodeNode(std::string nodeTypeName,
             char idToken,
             char openToken,
             char closeToken,
             StreamReader& stream);

    std::string renderingCode() override;

protected:
    std::string content_;
    NodeExtension extension_;

private:
    void load(StreamReader& stream);

private:
    std::string nodeTypeName_;
    char idToken_;
    char openToken_;
    char closeToken_;
};

class ExpressionNode : public CodeNode
{
public:
    explicit ExpressionNode(StreamReader& stream) :
        CodeNode("Expression", '$', '(', ')', stream)
    {}
    std::string renderingCode() override;
};

class StatementNode : public CodeNode
{
public:
    explicit StatementNode(StreamReader& stream) :
        CodeNode("Statement", '$', '{', '}', stream)
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
