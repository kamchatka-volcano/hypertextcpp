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
                   bool isGlobal,
                   StreamReader& stream)
    : name_(name)
    , idToken_(idToken)
    , openToken_(openToken)
    , closeToken_(closeToken)
    , isGlobal_(isGlobal)
{
    load(stream);
}

void CodeNode::load(StreamReader& stream)
{
    auto nodePosInfo = stream.positionInfo();
    Expects(stream.read(2) == std::string{} + idToken_ + openToken_);
    extension_ = readNodeExtension(stream);

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
                if (openParenthesisNum == 0){
                    const auto extensionPos = stream.positionInfo();
                    const auto closingTokenExtension = readNodeExtension(stream);
                    if (!closingTokenExtension.isEmpty()){
                        if (!extension_.isEmpty())
                            throw TemplateError{extensionPos + " " + name_ + " can't have multiple extensions"};
                        extension_ = closingTokenExtension;
                        extensionIsOnClosingToken_ = true;
                    }
                    return;
                }
            }
        }
        content_ += res;
    }
    throw TemplateError{nodePosInfo += " " + name_ + " isn't closed with '" + closeToken_ + "'"};
}

std::string CodeNode::docTemplate()
{
    auto result = std::string{};
    result += std::string{} + idToken_ + openToken_;
    if (!extensionIsOnClosingToken_)
        result += extension_.docTemplate();
    result += content_ + closeToken_;
    if (extensionIsOnClosingToken_)
        result += extension_.docTemplate();
    return result;
}

std::string CodeNode::docRenderingCode()
{
    return utils::preprocessRawStrings(content_);
}

std::string ExpressionNode::docRenderingCode()
{
    auto result = std::string{};
    if (!extension_.isEmpty()){
        if (extension_.type() == NodeExtension::Type::Conditional)
            result += "if (" + extension_.content() + "){ ";
        else if (extension_.type() == NodeExtension::Type::Loop)
            result += "for (" + extension_.content() + "){ ";
    }
    result += "out << (" + utils::preprocessRawStrings(content_) + ");";
    if (!extension_.isEmpty() && extension_.type() != NodeExtension::Type::Prototype)
        result += " } ";
    return result;
}

bool CodeNode::isGlobal()
{
    return isGlobal_;
}

}
