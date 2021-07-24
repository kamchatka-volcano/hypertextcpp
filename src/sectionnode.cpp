#include "sectionnode.h"
#include "streamreader.h"
#include "errors.h"
#include "nodereader.h"
#include "textnode.h"
#include "utils.h"
#include <gsl/assert>

namespace htcpp{

SectionNode::SectionNode(StreamReader& stream)
{
    load(stream);
}

const NodeExtension& SectionNode::extension() const
{
    return extension_;
}

void SectionNode::load(StreamReader& stream)
{
    const auto nodePos = stream.positionInfo();
    Expects(stream.read(2) == "[[");

    extension_ = readNodeExtension(stream);

    auto readedText = std::string{};
    while (!stream.atEnd()){
        if (stream.peek(2) == "]]"){
            stream.skip(2);
            utils::consumeReadText(readedText, contentNodes_);
            const auto extensionPos = stream.positionInfo();
            const auto closingBracesExtension = readNodeExtension(stream);
            if (!closingBracesExtension.isEmpty()){
                if (!extension_.isEmpty())
                    throw TemplateError{extensionPos + " section can't have multiple extensions"};
                extension_ = closingBracesExtension;
                extensionIsOnClosingBraces_ = true;
            }
            return;
        }
        auto node = readNonTagNode(stream);
        if (node){
            utils::consumeReadText(readedText, contentNodes_, node.get());
            contentNodes_.emplace_back(std::move(node));
        }
        else
            readedText += stream.read();
    }
    throw TemplateError{nodePos + " section isn't closed with ']]'"};
}

std::string SectionNode::docTemplate()
{
    auto result = std::string{"[["};
    if (!extensionIsOnClosingBraces_)
        result += extension_.docTemplate();
    for (auto& node : contentNodes_)
        result += node->docTemplate();
    result += "]]";
    if (extensionIsOnClosingBraces_)
        result += extension_.docTemplate();
    return result;
}

std::string SectionNode::docRenderingCode()
{
    auto result = std::string{};
    if (!extension_.isEmpty()){
        if (extension_.type() == NodeExtension::Type::Conditional)
            result += "if (" + extension_.content() + "){ ";
        else if (extension_.type() == NodeExtension::Type::Loop)
            result += "for (" + extension_.content() + "){ ";
    }
    for (auto& node : contentNodes_)
        result += node->docRenderingCode();
    if (!extension_.isEmpty())
        result += " } ";
    return result;
}

}
