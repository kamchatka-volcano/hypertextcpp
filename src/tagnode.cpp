#include "tagnode.h"
#include "textnode.h"
#include "streamreader.h"
#include "nodereader.h"
#include "string_utils.h"
#include "errors.h"
#include "utils.h"
#include <gsl/assert>

namespace htcpp{

TagNode::TagNode(StreamReader& stream, NodeReader& nodeReader)
{
    load(stream, nodeReader);
}

const NodeExtension& TagNode::extension() const
{
    return extension_;
}

void TagNode::load(StreamReader& stream, NodeReader& nodeReader)
{        
    const auto nodePos = stream.positionInfo();
    Expects(stream.read() == "<");

    while(!stream.atEnd()){
        if (name_.empty()){
            if (readName(stream) == ReadResult::ParsingCompleted)
                return;
            continue;
        }
        if (!attributesReaded_){
            if (readAttributes(stream, nodeReader) == ReadResult::ParsingCompleted)
                return;
            continue;
        }
        const auto closingTag = "</" + name_ + ">";
        const auto closingTagSize = static_cast<int>(closingTag.size());
        if (stream.peek(closingTagSize) == closingTag){
            consumeReadedText(contentNodes_);
            stream.skip(closingTagSize);
            const auto extensionPos = stream.positionInfo();
            const auto closingTagExtension = readNodeExtension(stream);
            if (!closingTagExtension.isEmpty()){
                if (!extension_.isEmpty())
                    throw TemplateError{extensionPos + " tag can't have multiple extensions"};
                extension_ = closingTagExtension;
                extensionIsOnClosingTag_ = true;
            }
            return;
        }        
        auto node = nodeReader.readTagContentNode(stream);
        if (node){
            consumeReadedText(contentNodes_);            
            contentNodes_.emplace_back(std::move(node));
        }
        else
            readedText_ += stream.read();
    }
    throw TemplateError{nodePos + " tag isn't closed with '>'"};
}

TagNode::ReadResult TagNode::readName(StreamReader& stream)
{
    if (std::isspace(stream.peek().front())){
        name_ = readedText_;
        readedText_.clear();
    }
    else if (stream.peek() == ">"){
        name_ = readedText_;
        readedText_.clear();
        stream.skip(1);
        extension_ = readNodeExtension(stream);
        attributesReaded_ = true;
        if (utils::isTagEmptyElement(name_)){
            return ReadResult::ParsingCompleted;
        }
    }
    else
        readedText_+= stream.read();

    return ReadResult::Ok;
}

TagNode::ReadResult TagNode::readAttributes(StreamReader& stream, NodeReader& nodeReader)
{    
    if (stream.peek() == ">"){
        attributesReaded_ = true;
        consumeReadedText(attributeNodes_);
        stream.skip(1);
        extension_ = readNodeExtension(stream);
        if (utils::isTagEmptyElement(name_))
            return ReadResult::ParsingCompleted;
        return ReadResult::Ok;
    }

    auto node = nodeReader.readTagAttributeNode(stream);
    if (node){
        consumeReadedText(attributeNodes_);        
        attributeNodes_.emplace_back(std::move(node));
    }
    else
        readedText_ += stream.read();

    return ReadResult::Ok;
}


void TagNode::consumeReadedText( std::vector<std::unique_ptr<IDocumentNode>>& nodes)
{
    if (!readedText_.empty()){
        nodes.emplace_back(std::make_unique<TextNode>(readedText_));
        readedText_.clear();
    }
}

std::string TagNode::docTemplate()
{
    auto result  = "<" + name_;
    for (auto& node : attributeNodes_)
        result += node->docTemplate();
    result += ">";
    if (!extensionIsOnClosingTag_)
        result += extension_.docTemplate();

    for (auto& node : contentNodes_)
        result += node->docTemplate();
    if (!utils::isTagEmptyElement(name_))
        result += "</" + name_ + ">";
    if (extensionIsOnClosingTag_)
        result += extension_.docTemplate();
    return result;
}

std::string TagNode::docRenderingCode()
{
    auto result = std::string{};
    if (!extension_.isEmpty()){
        if (extension_.type() == NodeExtension::Type::Conditional)
            result += "if (" + extension_.content() + "){\n";
        else if (extension_.type() == NodeExtension::Type::Loop)
            result += "for (" + extension_.content() + "){\n";
    }
    result  += "out << \"<" + name_ + "\";";
    for (auto& node : attributeNodes_)
        result += node->docRenderingCode();
    result += "out << \">\";";

    for (auto& node : contentNodes_)
        result += node->docRenderingCode();
    if (!utils::isTagEmptyElement(name_))
        result += "out << \"</" + name_ + ">\";\n";
    if (!extension_.isEmpty())
        result += "}\n";

    return result;
}

}
