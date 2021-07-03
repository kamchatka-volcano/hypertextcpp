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

NodeExtension::NodeExtension() = default;
NodeExtension::NodeExtension(NodeExtension::Type type,
                             const std::string& content)
    : type_(type)
    , content_(content)
{    
}

bool NodeExtension::isEmpty() const
{
    return type_ == Type::None;
}

NodeExtension::Type NodeExtension::type() const
{
    return type_;
}

const std::string& NodeExtension::content() const
{
    return content_;
}

std::string NodeExtension::docTemplate() const
{
    switch(type_){
    case NodeExtension::Type::Conditional : return "?(" + content_ + ")";
    case NodeExtension::Type::Loop: return "@(" + content_ + ")";
    default: return {};
    }
}

NodeExtension readNodeExtension(StreamReader& stream)
{
    const auto nodePos =  stream.positionInfo();
    const auto nextChars = stream.peek(2);
    auto extensionType = NodeExtension::Type::None;
    if (nextChars.empty())
        return {};
    if (nextChars == "?(")
        extensionType = NodeExtension::Type::Conditional;
    else if (nextChars == "@(")
        extensionType = NodeExtension::Type::Loop;

    if (extensionType == NodeExtension::Type::None)
        return{};

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
                if (!utils::stringHasContent(extensionContent))
                    throw TemplateError{nodePos + " " + nodeExtensionName(extensionType) + " can't be empty"};
                return {extensionType, extensionContent};
            }
        }
        extensionContent += res;
    }
    throw TemplateError{nodePos + " " + nodeExtensionName(extensionType) + " isn't closed with ')'"};
}

}
