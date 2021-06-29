#include "sectionnode.h"
#include "streamreader.h"
#include "errors.h"
#include "nodereader.h"
#include "textnode.h"

namespace htcpp{

SectionNode::SectionNode()
{
}

void SectionNode::load(StreamReader& stream)
{
    if (stream.read(2) != "[[") //assert?
        throw TemplateError{"Invalid section"};
    extension_ = readNodeExtension(stream);

    auto readedText = std::string{};
    while (!stream.atEnd()){
        if (stream.peek(2) == "]]"){
            stream.skip(2);
            if (!readedText.empty()){
                contentNodes_.emplace_back(std::make_unique<TextNode>(readedText));
                readedText.clear();
            }
            auto closingBracesExtension = readNodeExtension(stream);
            if (!closingBracesExtension.isEmpty()){
                if (!extension_.isEmpty())
                    throw TemplateError{"Section can't have multiple extensions"};
                extension_ = closingBracesExtension;
                extensionIsOnClosingBraces_ = true;
            }
            return;
        }
        auto node = readSectionNode(stream);
        if (node){
            if (!readedText.empty()){
                contentNodes_.emplace_back(std::make_unique<TextNode>(readedText));
                readedText.clear();
            }
            node->load(stream);
            contentNodes_.emplace_back(std::move(node));
        }
        else
            readedText += stream.read();
    }
    throw TemplateError{"Section is unclosed"};
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
            result += "if (" + extension_.content() + "){\n";
        else if (extension_.type() == NodeExtension::Type::Loop)
            result += "for (" + extension_.content() + "){\n";
    }
    for (auto& node : contentNodes_)
        result += node->docRenderingCode();
    if (!extension_.isEmpty())
        result += "}\n";
    return result;
}

}
