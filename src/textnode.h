#pragma once
#include "document_node_interface_access.h"
#include "idocumentnode.h"
#include "idocumentnoderenderer.h"
#include "irendered_as_string_part.h"

namespace htcpp {

class TextNode : public IDocumentNode,
                 public IDocumentNodeRenderer,
                 public IRenderedAsStringPart {
    DOCUMENT_NODE_INTERFACE_ACCESS(IDocumentNodeRenderer)
    DOCUMENT_NODE_INTERFACE_ACCESS(IRenderedAsStringPart)

public:
    explicit TextNode(std::string value);
    std::string renderingCode() const override;
    std::string content() const override;

private:
    std::string content_;
};

} //namespace htcpp
