#include "tagnode.h"
#include "textnode.h"
#include "streamreader.h"
#include "nodereader.h"
#include "errors.h"
#include "utils.h"
#include <gsl/gsl>

namespace htcpp{

TagNode::TagNode(StreamReader& stream)
{
    load(stream);
}

const NodeExtension& TagNode::extension() const
{
    return extension_;
}

void TagNode::load(StreamReader& stream)
{        
    const auto nodePos = stream.position();
    Expects(stream.read() == "<");

    while(!stream.atEnd()){
        if (name_.empty()){
            if (readName(stream, nodePos) == ReadResult::ParsingCompleted)
                return;
            continue;
        }
        if (!attributesRead_){
            if (readAttributes(stream) == ReadResult::ParsingCompleted)
                return;
            continue;
        }
        const auto closingTag = "</" + name_ + ">";
        const auto closingTagSize = static_cast<int>(closingTag.size());
        if (stream.peek(closingTagSize) == closingTag){
            utils::consumeReadText(readText_, contentNodes_);
            stream.skip(closingTagSize);
            const auto extensionPos = stream.position();
            const auto closingTagExtension = readNodeExtension(stream);
            if (!closingTagExtension.isEmpty()){
                if (!extension_.isEmpty())
                    throw TemplateError{extensionPos, "Tag can't have multiple extensions"};
                extension_ = closingTagExtension;
            }
            return;
        }        
        auto node = readTagContentNode(stream);
        if (node){
            utils::consumeReadText(readText_, contentNodes_, node.get());
            contentNodes_.emplace_back(std::move(node));
        }
        else
            readText_ += stream.read();
    }
    if (name_.empty())
        throw TemplateError{nodePos, "Tag's name can't be empty"};
    if (attributesRead_)
        throw TemplateError{nodePos, "Tag isn't closed with </" + name_ + ">"};
    else
        throw TemplateError{nodePos, "Tag isn't closed with '>'"};
}

TagNode::ReadResult TagNode::readName(StreamReader& stream, const StreamReaderPosition& nodePos)
{
    if (std::isspace(stream.peek().front())){
        name_ = readText_;
        if (utils::isBlank(name_))
            throw TemplateError{nodePos, "Tag's name can't be empty"};
        readText_.clear();
    }
    else if (stream.peek() == ">"){
        name_ = readText_;
        if (utils::isBlank(name_))
            throw TemplateError{nodePos, "Tag's name can't be empty"};

        readText_.clear();
        stream.skip(1);
        extension_ = readNodeExtension(stream);
        attributesRead_ = true;
        if (utils::isTagEmptyElement(name_))
            return ReadResult::ParsingCompleted;
    }
    else
        readText_+= stream.read();

    return ReadResult::Ok;
}

TagNode::ReadResult TagNode::readAttributes(StreamReader& stream)
{    
    if (stream.peek() == ">"){
        attributesRead_ = true;
        utils::consumeReadAttributesText(readText_, attributeNodes_);
        stream.skip(1);
        extension_ = readNodeExtension(stream);
        if (utils::isTagEmptyElement(name_))
            return ReadResult::ParsingCompleted;
        return ReadResult::Ok;
    }

    auto node = readTagAttributeNode(stream);
    if (node){
        utils::consumeReadAttributesText(readText_, attributeNodes_);
        attributeNodes_.emplace_back(std::move(node));
    }
    else
        readText_ += stream.read();

    return ReadResult::Ok;
}

std::string TagNode::renderingCode()
{
    auto result = std::string{};
    if (!extension_.isEmpty()){
        if (extension_.type() == NodeExtension::Type::Conditional)
            result += "if (" + extension_.content() + "){ ";
        else if (extension_.type() == NodeExtension::Type::Loop)
            result += "for (" + extension_.content() + "){ ";
    }
    result  += "out << \"<" + name_ + "\";";
    for (auto& node : attributeNodes_)
        result += node->renderingCode();
    result += "out << \">\";";

    for (auto& node : contentNodes_)
        result += node->renderingCode();
    if (!utils::isTagEmptyElement(name_))
        result += "out << \"</" + name_ + ">\";";
    if (!extension_.isEmpty())
        result += " } ";

    return result;
}

}
