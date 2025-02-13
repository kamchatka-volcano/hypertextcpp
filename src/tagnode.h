#pragma once
#include "idocumentnode.h"
#include "inodecollection.h"
#include "iconvertible_to_procedure.h"
#include "nodeextension.h"
#include "streamreaderposition.h"
#include <memory>
#include <optional>
#include <vector>

namespace htcpp {

class TagNode : public IDocumentNode,
                public INodeCollection,
                public IConvertibleToProcedure {
    enum class ReadResult {
        Ok,
        ParsingCompleted
    };

public:
    explicit TagNode(StreamReader& stream);
    std::vector<std::unique_ptr<IDocumentNode>>& content() override;
    std::vector<std::unique_ptr<IDocumentNode>> flatten() override;
    std::optional<std::string_view> procedureName() const override;

private:
    void load(StreamReader& stream);
    ReadResult readName(StreamReader& stream, const htcpp::StreamReaderPosition& nodePos);
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
