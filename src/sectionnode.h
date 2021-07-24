#pragma once
#include "idocumentnode.h"
#include "nodeextension.h"
#include <vector>
#include <memory>

namespace htcpp{
class NodeReader;

class SectionNode : public IDocumentNode
{
public:
    explicit SectionNode(StreamReader& stream);
    std::string docTemplate() override;
    std::string docRenderingCode() override;
    const NodeExtension& extension() const;

private:
    void load(StreamReader& stream);

private:
    std::vector<std::unique_ptr<IDocumentNode>> contentNodes_;
    NodeExtension extension_;
    bool extensionIsOnClosingBraces_ = false;
};

}
