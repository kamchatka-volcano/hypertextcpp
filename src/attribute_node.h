#pragma once
#include "iattribute.h"
#include "idocumentnode.h"

namespace htcpp {

class StreamReader;

class AttributeNode : public IDocumentNode,
                      public IAttribute {
    DOCUMENT_NODE_INTERFACE_ACCESS(IAttribute)

public:
    explicit AttributeNode(std::string name, StreamReader& stream);
    std::string_view name() const override;
    std::string_view value() const override;

private:
    void load(StreamReader& stream);

private:
    std::string name_;
    std::string value_;
};

} //namespace htcpp
