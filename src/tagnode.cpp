#include "tagnode.h"
#include "control_flow_statement_node.h"
#include "errors.h"
#include "nodereader.h"
#include "streamreader.h"
#include "textnode.h"
#include "utils.h"
#include <gsl/gsl>
#include <algorithm>

namespace htcpp {

TagNode::TagNode(StreamReader& stream)
{
    load(stream);
}

void TagNode::load(StreamReader& stream)
{
    const auto nodePos = stream.position();
    auto openSeq = stream.read();
    Expects(openSeq == "<");

    while (!stream.atEnd()) {
        if (name_.empty()) {
            if (readName(stream, nodePos) == ReadResult::ParsingCompleted)
                return;
            continue;
        }
        if (!attributesRead_) {
            if (readAttributes(stream) == ReadResult::ParsingCompleted)
                return;
            continue;
        }
        const auto closingTag = "</" + name_ + ">";
        const auto closingTagSize = static_cast<int>(closingTag.size());
        if (stream.peek(closingTagSize) == closingTag) {
            utils::consumeReadText(readText_, contentNodes_);
            stream.skip(closingTagSize);
            const auto extensionPos = stream.position();
            const auto closingTagExtension = readNodeExtension(stream);
            if (closingTagExtension.has_value()) {
                if (extension_.has_value())
                    throw TemplateError{extensionPos, "Tag can't have multiple extensions"};
                extension_ = closingTagExtension;
            }
            return;
        }
        auto node = readTagContentNode(stream);
        if (node) {
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
    if (std::isspace(stream.peek().front())) {
        name_ = readText_;
        if (utils::isBlank(name_))
            throw TemplateError{nodePos, "Tag's name can't be empty"};
        readText_.clear();
    }
    else if (stream.peek() == ">") {
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
        readText_ += stream.read();

    return ReadResult::Ok;
}

TagNode::ReadResult TagNode::readAttributes(StreamReader& stream)
{
    if (stream.peek() == ">") {
        attributesRead_ = true;
        utils::consumeReadAttributesText(readText_, attributeNodes_);
        stream.skip(1);
        extension_ = readNodeExtension(stream);
        if (utils::isTagEmptyElement(name_))
            return ReadResult::ParsingCompleted;
        return ReadResult::Ok;
    }

    auto node = readTagAttributeNode(stream);
    if (node) {
        utils::consumeReadAttributesText(readText_, attributeNodes_);
        attributeNodes_.emplace_back(std::move(node));
    }
    else
        readText_ += stream.read();

    return ReadResult::Ok;
}

std::vector<std::unique_ptr<IDocumentNode>> TagNode::flatten()
{
    auto result = std::vector<std::unique_ptr<IDocumentNode>>{};
    if (extension_.has_value())
        result.emplace_back(
                std::make_unique<ControlFlowStatementNode>(ControlFlowStatementNodeType::Open, extension_.value()));
    result.emplace_back(std::make_unique<TextNode>("<" + name_));
    for (auto& node : attributeNodes_){
        if (auto nodeCollection = node->interface<INodeCollection>())
            std::ranges::move(nodeCollection->flatten(), std::back_inserter(result));
        else
            result.emplace_back(std::move(node));
    }
    result.emplace_back(std::make_unique<TextNode>(">"));
    for (auto& node : contentNodes_){
        if (auto nodeCollection = node->interface<INodeCollection>())
            std::ranges::move(nodeCollection->flatten(), std::back_inserter(result));
        else
            result.emplace_back(std::move(node));
    }
    if (!utils::isTagEmptyElement(name_))
        result.emplace_back(std::make_unique<TextNode>("</" + name_ + ">"));

    if (extension_.has_value())
        result.emplace_back(
                std::make_unique<ControlFlowStatementNode>(ControlFlowStatementNodeType::Close, extension_.value()));

    return result;
}

} // namespace htcpp
