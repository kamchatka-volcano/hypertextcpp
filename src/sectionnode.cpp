#include "sectionnode.h"
#include "attribute_node.h"
#include "control_flow_statement_node.h"
#include "errors.h"
#include "idocumentnoderenderer.h"
#include "nodereader.h"
#include "streamreader.h"
#include "textnode.h"
#include "utils.h"
#include <algorithm>

namespace htcpp {

SectionNode::SectionNode(StreamReader& stream)
{
    load(stream);
}

std::vector<std::unique_ptr<IDocumentNode>>& SectionNode::content()
{
    return contentNodes_;
}

void SectionNode::load(StreamReader& stream)
{
    const auto nodePos = stream.position();
    auto openSeq = stream.read(2);
    Expects(openSeq == "[[");

    extension_ = readNodeExtension(stream);

    auto readText = std::string{};
    while (!stream.atEnd()) {
        if (stream.peek(2) == "]]") {
            stream.skip(2);
            utils::consumeReadText(readText, contentNodes_);
            const auto extensionPos = stream.position();
            const auto closingBracesExtension = readNodeExtension(stream);
            if (closingBracesExtension.has_value()) {
                if (extension_.has_value())
                    throw TemplateError{extensionPos, "Section can't have multiple extensions"};
                extension_ = closingBracesExtension;
            }
            if (contentNodes_.empty())
                throw TemplateError{nodePos, "Section can't be empty"};
            return;
        }
        auto node = readSectionContentNode(stream);
        if (node) {
            utils::consumeReadText(readText, contentNodes_, node.get());
            contentNodes_.emplace_back(std::move(node));
        }
        else
            readText += stream.read();
    }
    throw TemplateError{nodePos, "Section isn't closed with ']]'"};
}

std::vector<std::unique_ptr<IDocumentNode>> SectionNode::flatten()
{
    auto result = std::vector<std::unique_ptr<IDocumentNode>>{};
    if (extension_.has_value())
        result.emplace_back(
                std::make_unique<ControlFlowStatementNode>(ControlFlowStatementNodeType::Open, extension_.value()));
    for (auto& node : contentNodes_) {
        if (node->getInterface<IAttribute>().has_value())
            continue;

        if (auto nodeCollection = node->getInterface<INodeCollection>())
            std::ranges::move(nodeCollection->flatten(), std::back_inserter(result));
        else
            result.emplace_back(std::move(node));
    }
    if (extension_.has_value())
        result.emplace_back(
                std::make_unique<ControlFlowStatementNode>(ControlFlowStatementNodeType::Close, extension_.value()));

    return result;
}

template<typename T>
sfun::optional_ref<T> getIConvertibleToProcedure(auto selfPtr)
{
    if (std::ranges::any_of(
                    selfPtr->contentNodes_,
                    [](const auto& node)
                    {
                        return node->template getInterface<IAttribute>().has_value();
                    }))
        return selfPtr;

    return std::nullopt;
}

sfun::optional_ref<const IConvertibleToProcedure> SectionNode::getIConvertibleToProcedure() const
{
    return getIConvertibleToProcedure<const IConvertibleToProcedure>(this);
}

sfun::optional_ref<IConvertibleToProcedure> SectionNode::getIConvertibleToProcedure()
{
    return getIConvertibleToProcedure<IConvertibleToProcedure>(this);
}

std::string_view SectionNode::procedureName() const
{
    auto idAttribute = std::ranges::find_if(
                contentNodes_,
                [](const auto& node)
                {
                    auto attribute = node->template getInterface<IAttribute>();
                    if (!attribute.has_value())
                        return false;
                    return attribute->name() == "htcpp-id";
                });

    if (idAttribute == contentNodes_.end())
        return {};

    return idAttribute->get()->template getInterface<IAttribute>()->value();
}

} //namespace htcpp
