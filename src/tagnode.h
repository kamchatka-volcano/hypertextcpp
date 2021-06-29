#pragma once
#include "idocumentnode.h"
#include "nodeextension.h"
#include <vector>
#include <memory>

namespace htcpp{

class TagNode : public IDocumentNode
{
    enum class ReadResult{
        Ok,
        ParsingCompleted
    };

public:
    void load(StreamReader& stream) override;
    std::string docTemplate() override;
    std::string docRenderingCode() override;

private:
    ReadResult readName(StreamReader& stream);
    ReadResult readAttributes(StreamReader& stream);
    void consumeReadedText(std::vector<std::unique_ptr<IDocumentNode>>& nodes);

private:
    std::string readedText_;
    std::string name_;
    bool attributesReaded_ = false;
    std::vector<std::unique_ptr<IDocumentNode>> attributeNodes_;
    std::vector<std::unique_ptr<IDocumentNode>> contentNodes_;
    NodeExtension extension_;
    bool extensionIsOnClosingTag_ = false;
};

}
