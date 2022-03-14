#include "codenode.h"
#include "streamreader.h"
#include "errors.h"
#include "utils.h"
#include <utility>
#include <gsl/gsl>

namespace htcpp{

CodeNode::CodeNode(std::string nodeTypeName,
                   char idToken,
                   char openToken,
                   char closeToken,                   
                   StreamReader& stream)
    : nodeTypeName_(std::move(nodeTypeName))
    , idToken_(idToken)
    , openToken_(openToken)
    , closeToken_(closeToken)    
{
    load(stream);
}

void CodeNode::load(StreamReader& stream)
{
    auto nodePos = stream.position();
    auto openSeq = stream.read(2);
    Expects(openSeq == std::string{} + idToken_ + openToken_);
    extension_ = readNodeExtension(stream);

    auto openParenthesisNum = 1;
    auto insideString = char{0};
    auto lastStringPos = StreamReaderPosition{};
    while(!stream.atEnd()){
        auto res = stream.read();
        if (!insideString){
            if (res == "'" || res == "`" || res == "\"")
                insideString = res.front();
            lastStringPos = stream.position();
        }
        else if (res.front() == insideString) {
            insideString = 0;
            lastStringPos = stream.position();
        }

        if (!insideString){
            if (res.front() == openToken_)
                openParenthesisNum++;
            else if (res.front() == closeToken_){
                openParenthesisNum--;
                if (openParenthesisNum == 0){
                    const auto extensionPos = stream.position();
                    const auto closingTokenExtension = readNodeExtension(stream);
                    if (!closingTokenExtension.isEmpty()){
                        if (!extension_.isEmpty())
                            throw TemplateError{extensionPos, nodeTypeName_ + " can't have multiple extensions"};
                        extension_ = closingTokenExtension;
                    }
                    if (utils::isBlank(content_))
                        throw TemplateError(nodePos, nodeTypeName_ + " can't be empty");

                    content_ = utils::transformRawStrings(content_, nodePos);
                    return;
                }
            }
        }
        content_ += res;
    }
    if (insideString) {
        if (insideString != '\'')
            throw TemplateError{lastStringPos, std::string{"String isn't closed with '"} + insideString + "'"};
        else
            throw TemplateError{lastStringPos, "Char literal isn't closed with \"'\""};
    }

    throw TemplateError{nodePos, nodeTypeName_ + " isn't closed with '" + closeToken_ + "'"};
}

std::string CodeNode::renderingCode()
{
    return content_;
}

std::string ExpressionNode::renderingCode()
{
    auto result = std::string{};
    if (!extension_.isEmpty()){
        if (extension_.type() == NodeExtension::Type::Conditional)
            result += "if (" + extension_.content() + "){ ";
        else if (extension_.type() == NodeExtension::Type::Loop)
            result += "for (" + extension_.content() + "){ ";
    }
    result += "out << (" + content_ + ");";
    if (!extension_.isEmpty())
        result += " } ";
    return result;
}

}
