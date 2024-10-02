#include "codenode.h"
#include "streamreader.h"
#include "errors.h"
#include "utils.h"
#include <utility>

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
                    if (closingTokenExtension.has_value()){
                        if (extension_.has_value())
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

const std::string& CodeNode::content() const
{
    return content_;
}

const std::optional<NodeExtension>& CodeNode::extension() const
{
    return extension_;
}

std::string ExpressionNode::renderingCode() const
{
    auto result = std::string{};
    const auto& extension = impl_.extension();
    if (extension.has_value()){
        if (extension.value().type() == NodeExtension::Type::Conditional)
            result += "if (" + extension.value().content() + "){ ";
        else if (extension.value().type() == NodeExtension::Type::Loop)
            result += "for (" + extension.value().content() + "){ ";
    }
    result += "out << (" + impl_.content() + ");";
    if (extension.has_value())
        result += " } ";
    return result;
}

ExpressionNode::ExpressionNode(StreamReader& stream)
    : impl_("Expression", '$', '(', ')', stream)
{}

StatementNode::StatementNode(StreamReader& stream)
    :impl_("Statement", '$', '{', '}', stream)
{}

std::string StatementNode::renderingCode() const
{
    return impl_.content();
}

GlobalStatementNode::GlobalStatementNode(StreamReader& stream)
    : impl_("Global statement", '#', '{', '}', stream)
{}

std::string GlobalStatementNode::renderingCode() const
{
    return impl_.content();
}

}
