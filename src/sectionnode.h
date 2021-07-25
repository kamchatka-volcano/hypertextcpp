#pragma once
#include "idocumentnode.h"
#include "nodeextension.h"
#include <vector>
#include <memory>

namespace htcpp{

class SectionNode : public IDocumentNode
{
public:
    explicit SectionNode(StreamReader& stream);

    std::string renderingCode() override;
    const NodeExtension& extension() const;

private:
    void load(StreamReader& stream);

private:
    std::vector<std::unique_ptr<IDocumentNode>> contentNodes_;
    NodeExtension extension_;
};

}
