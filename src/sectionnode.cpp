#include "sectionnode.h"
#include "streamreader.h"
#include "errors.h"
#include "nodereader.h"
#include "textnode.h"
#include "utils.h"
#include <gsl/gsl>

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
    const auto nodePos = stream.position();
    auto openSeq = stream.read(2);
    Expects(openSeq == "[[");

    extension_ = readNodeExtension(stream);

    auto readText = std::string{};
    while (!stream.atEnd()){
        if (stream.peek(2) == "]]"){
            stream.skip(2);
            utils::consumeReadText(readText, contentNodes_);
            const auto extensionPos = stream.position();
            const auto closingBracesExtension = readNodeExtension(stream);
            if (!closingBracesExtension.isEmpty()){
                if (!extension_.isEmpty())
                    throw TemplateError{extensionPos, "Section can't have multiple extensions"};
                extension_ = closingBracesExtension;
            }
            if (contentNodes_.empty())
                throw TemplateError{nodePos, "Section can't be empty"};
            return;
        }
        auto node = readNonTagNode(stream);
        if (node){
            utils::consumeReadText(readText, contentNodes_, node.get());
            contentNodes_.emplace_back(std::move(node));
        }
        else
            readText += stream.read();
    }
    throw TemplateError{nodePos, "Section isn't closed with ']]'"};
}

std::string SectionNode::renderingCode()
{
    auto result = std::string{};
    if (!extension_.isEmpty()){
        if (extension_.type() == NodeExtension::Type::Conditional)
            result += "if (" + extension_.content() + "){ ";
        else if (extension_.type() == NodeExtension::Type::Loop)
            result += "for (" + extension_.content() + "){ ";
    }
    for (auto& node : contentNodes_)
        result += node->renderingCode();
    if (!extension_.isEmpty())
        result += " } ";
    return result;
}

}
