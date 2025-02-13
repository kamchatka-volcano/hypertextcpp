#include "node_utils.h"
#include <algorithm>
#include <iterator>

namespace htcpp{

std::vector<std::unique_ptr<IDocumentNode>> flattenNodes(std::vector<std::unique_ptr<IDocumentNode>> nodes)
{
    auto flattenNodes = std::vector<std::unique_ptr<IDocumentNode>>{};
    for (auto& node : nodes) {
        if (const auto nodeCollection = node->template getInterface<INodeCollection>())
            std::ranges::move(nodeCollection->flatten(), std::back_inserter(flattenNodes));
        else
            flattenNodes.emplace_back(std::move(node));
    }
    return flattenNodes;
}

std::vector<std::unique_ptr<IDocumentNode>> optimizeNodes(std::vector<std::unique_ptr<IDocumentNode>> nodes)
{
    auto processedNodes = std::vector<std::unique_ptr<IDocumentNode>>{};
    for (auto& node : nodes) {
        if (auto text = node->getInterface<IRenderedAsStringPart>()) {
            if (!processedNodes.empty() && processedNodes.back()->getInterface<IRenderedAsStringPart>()) {
                auto prevText = processedNodes.back()->getInterface<IRenderedAsStringPart>()->content();
                processedNodes.back() = std::make_unique<TextNode>(prevText + text->content());
            }
            else
                processedNodes.emplace_back(std::make_unique<TextNode>(text->content()));
        }
        else {
            processedNodes.emplace_back(std::move(node));
        }
    }
    return processedNodes;
}
}