#pragma once
#include "idocumentnode.h"
#include "inodecollection.h"
#include "iconvertible_to_procedure.h"
#include "nodeextension.h"
#include <memory>
#include <optional>
#include <vector>

namespace htcpp{

class SectionNode : public IDocumentNode,
                    public INodeCollection,
                    public IConvertibleToProcedure {
public:
    explicit SectionNode(StreamReader& stream);
    std::vector<std::unique_ptr<IDocumentNode>>& content() override;
    std::vector<std::unique_ptr<IDocumentNode>> flatten() override;
    std::optional<std::string_view> procedureName() const override;

private:
    void load(StreamReader& stream);

private:
    std::vector<std::unique_ptr<IDocumentNode>> contentNodes_;
    std::optional<NodeExtension> extension_;
};

}
