#include "nodeextension.h"
#include "streamreader.h"
#include "errors.h"
#include "utils.h"

namespace htcpp{

namespace{
std::string nodeExtensionName(NodeExtension::Type type)
{
    switch(type){
    case NodeExtension::Type::Conditional : return "Conditional extension";
    case NodeExtension::Type::Loop: return "Loop extension";    
    default: return {};
    }
}
}

NodeExtension::NodeExtension(NodeExtension::Type type,
                             std::string content)
    : type_(type)
    , content_(std::move(content))
{    
}

NodeExtension::Type NodeExtension::type() const
{
    return type_;
}

const std::string& NodeExtension::content() const
{
    return content_;
}

std::optional<NodeExtension> readNodeExtension(StreamReader& stream)
{
    const auto nodePos = stream.position();
    const auto nextChars = stream.peek(2);
    if (nextChars.empty())
        return std::nullopt;

    auto extensionType = NodeExtension::Type{};
    if (nextChars == "?(")
        extensionType = NodeExtension::Type::Conditional;
    else if (nextChars == "@(")
        extensionType = NodeExtension::Type::Loop;
    else
        return std::nullopt;

    stream.skip(2);
    auto openParenthesisNum = 1;
    auto extensionContent = std::string{};
    while(!stream.atEnd()){
        auto res = stream.read();
        if (res == "(")
            openParenthesisNum++;
        else if (res == ")"){
            openParenthesisNum--;
            if (openParenthesisNum == 0){
                if (utils::isBlank(extensionContent))
                    throw TemplateError{nodePos, nodeExtensionName(extensionType) + " can't be empty"};
                return NodeExtension{extensionType, extensionContent};
            }
        }
        extensionContent += res;
    }
    throw TemplateError{nodePos, nodeExtensionName(extensionType) + " isn't closed with ')'"};
}

}
