#include "sectionnode.h"
#include "control_flow_statement_node.h"
#include "errors.h"
#include "idocumentnoderenderer.h"
#include "nodereader.h"
#include "streamreader.h"
#include "textnode.h"
#include "utils.h"
#include <gsl/gsl>

namespace htcpp{

SectionNode::SectionNode(StreamReader& stream)
{
    load(stream);
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
            if (closingBracesExtension.has_value()){
                if (extension_.has_value())
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

std::vector<std::unique_ptr<IDocumentNode>> SectionNode::flatten()
{
    auto result = std::vector<std::unique_ptr<IDocumentNode>>{};
    if (extension_.has_value())
        result.emplace_back(std::make_unique<ControlFlowStatementNode>(ControlFlowStatementNodeType::Open, extension_.value()));
    for (auto& node : contentNodes_) {
        if (auto nodeCollection = node->interface<INodeCollection>())
            std::ranges::move(nodeCollection->flatten(), std::back_inserter(result));
        else
            result.emplace_back(std::move(node));
    }
    if (extension_.has_value())
        result.emplace_back(std::make_unique<ControlFlowStatementNode>(ControlFlowStatementNodeType::Close, extension_.value()));

    return result;
}

}
