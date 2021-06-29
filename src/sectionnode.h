#pragma once
#include "idocumentnode.h"
#include "nodeextension.h"
#include <vector>
#include <memory>

namespace htcpp{

class SectionNode : public IDocumentNode
{
public:
    SectionNode();
    void load(StreamReader& stream) override;
    std::string docTemplate() override;
    std::string docRenderingCode() override;

private:
    std::vector<std::unique_ptr<IDocumentNode>> contentNodes_;
    NodeExtension extension_;
    bool extensionIsOnClosingBraces_ = false;
};

}
