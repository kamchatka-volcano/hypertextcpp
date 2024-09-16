#pragma once
#include "document_node_interface_access.h"
#include "idocumentnode.h"
#include "inodecollection.h"
#include "nodeextension.h"
#include <memory>
#include <optional>
#include <vector>

namespace htcpp{

class SectionNode : public IDocumentNode,
                    public INodeCollection {
    DOCUMENT_NODE_INTERFACE_ACCESS(INodeCollection)

public:
    explicit SectionNode(StreamReader& stream);
    std::vector<std::unique_ptr<IDocumentNode>> flatten() override;

private:
    void load(StreamReader& stream);

private:
    std::vector<std::unique_ptr<IDocumentNode>> contentNodes_;
    std::optional<NodeExtension> extension_;
};

}
