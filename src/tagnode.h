#pragma once
#include "idocumentnode.h"
#include "nodeextension.h"
#include <vector>
#include <memory>

namespace htcpp{
class NodeReader;

class TagNode : public IDocumentNode
{
    enum class ReadResult{
        Ok,
        ParsingCompleted
    };

public:
    explicit TagNode(StreamReader& stream);
    std::string docTemplate() override;
    std::string docRenderingCode() override;
    const NodeExtension& extension() const;

private:
    void load(StreamReader& stream);
    ReadResult readName(StreamReader& stream);
    ReadResult readAttributes(StreamReader& stream);

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
