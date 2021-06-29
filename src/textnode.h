#pragma once
#include "idocumentnode.h"

namespace htcpp{

class TextNode: public IDocumentNode{
public:
    TextNode(const std::string& value);
    void load(StreamReader&) override;
    std::string docTemplate() override;
    std::string docRenderingCode() override;

private:
    std::string value_;
};

}
