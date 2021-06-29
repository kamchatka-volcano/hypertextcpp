#include "codenode.h"
#include "streamreader.h"
#include "errors.h"
#include "utils.h"

namespace htcpp{

CodeNode::CodeNode(char idToken,
                   char openToken,
                   char closeToken,
                   bool isGlobalScoped)
    : idToken_(idToken)
    , openToken_(openToken)
    , closeToken_(closeToken)
    , isGlobalScoped_(isGlobalScoped)
{}

void CodeNode::load(StreamReader& stream)
{
    if (stream.peek(2) != std::string{} + idToken_ + openToken_)
        throw TemplateError{"Invalid rendered expression"};
    stream.skip(2);

    auto openParenthesisNum = 1;
    auto insideString = false;
    while(!stream.atEnd()){
        auto res = stream.read();
        if (res == "`")
            insideString = !insideString;

        if (!insideString){
            if (res.front() == openToken_)
                openParenthesisNum++;
            else if (res.front() == closeToken_){
                openParenthesisNum--;
                if (openParenthesisNum == 0)
                    return;
            }
        }
        content_ += res;
    }
    throw TemplateError{"Rendered expression is unclosed"};
}

std::string CodeNode::docTemplate()
{
    return (std::string{} + idToken_ + openToken_) + content_ + closeToken_;
}

std::string CodeNode::docRenderingCode()
{
    return preprocessRawStrings(content_);
}

std::string ExpressionNode::docRenderingCode()
{
    return "out << (" + preprocessRawStrings(content_) + ");";
}

bool CodeNode::isGlobalScoped()
{
    return isGlobalScoped_;
}

}
