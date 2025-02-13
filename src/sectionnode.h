#pragma once
#include "document_node_interface_access.h"
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
    DOCUMENT_NODE_INTERFACE_ACCESS(INodeCollection)

public:
    explicit SectionNode(StreamReader& stream);
    std::vector<std::unique_ptr<IDocumentNode>>& content() override;
    std::vector<std::unique_ptr<IDocumentNode>> flatten() override;
    std::string_view procedureName() const override;

    sfun::optional_ref<const IConvertibleToProcedure> getIConvertibleToProcedure() const override;
    sfun::optional_ref<IConvertibleToProcedure> getIConvertibleToProcedure() override;

private:
    void load(StreamReader& stream);
    template<typename T>
    friend sfun::optional_ref<T> getIConvertibleToProcedure(auto selfPtr);

private:
    std::vector<std::unique_ptr<IDocumentNode>> contentNodes_;
    std::optional<NodeExtension> extension_;
};

}
