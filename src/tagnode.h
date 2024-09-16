#pragma once
#include "document_node_interface_access.h"
#include "idocumentnode.h"
#include "inodecollection.h"
#include "nodeextension.h"
#include "streamreaderposition.h"
#include <memory>
#include <optional>
#include <vector>

namespace htcpp {

class TagNode : public IDocumentNode,
                public INodeCollection {
    DOCUMENT_NODE_INTERFACE_ACCESS(INodeCollection)

    enum class ReadResult {
        Ok,
        ParsingCompleted
    };

public:
    explicit TagNode(StreamReader& stream);
    std::vector<std::unique_ptr<IDocumentNode>> flatten() override;

private:
    void load(StreamReader& stream);
    TagNode::ReadResult readName(StreamReader& stream, const htcpp::StreamReaderPosition& nodePos);
    ReadResult readAttributes(StreamReader& stream);

private:
    std::string readText_;
    std::string name_;
    bool attributesRead_ = false;
    std::vector<std::unique_ptr<IDocumentNode>> attributeNodes_;
    std::vector<std::unique_ptr<IDocumentNode>> contentNodes_;
    std::optional<NodeExtension> extension_;
};

} //namespace htcpp
