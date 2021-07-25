#pragma once
#include "idocumentnode.h"
#include "nodeextension.h"
#include "streamreaderposition.h"
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
    explicit TagNode(StreamReader& stream);

    std::string renderingCode() override;
    const NodeExtension& extension() const;

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
    NodeExtension extension_;
};

}
