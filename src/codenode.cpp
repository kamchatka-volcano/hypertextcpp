#include "codenode.h"
#include "streamreader.h"
#include "errors.h"
#include "utils.h"
#include <gsl/assert>

namespace htcpp{

CodeNode::CodeNode(const std::string& name,
                   char idToken,
                   char openToken,
                   char closeToken,
                   bool isGlobalScoped)
    : name_(name)
    , idToken_(idToken)
    , openToken_(openToken)
    , closeToken_(closeToken)
    , isGlobalScoped_(isGlobalScoped)
{
}

void CodeNode::load(StreamReader& stream)
{
    auto nodePosInfo = stream.positionInfo();
    Expects(stream.read(2) == std::string{} + idToken_ + openToken_);

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
    throw TemplateError{nodePosInfo += " " + name_ + " isn't closed with '" + closeToken_ + "'"};
}

std::string CodeNode::docTemplate()
{
    return (std::string{} + idToken_ + openToken_) + content_ + closeToken_;
}

std::string CodeNode::docRenderingCode()
{
    return utils::preprocessRawStrings(content_);
}

std::string ExpressionNode::docRenderingCode()
{
    return "out << (" + utils::preprocessRawStrings(content_) + ");";
}

bool CodeNode::isGlobalScoped()
{
    return isGlobalScoped_;
}

}
