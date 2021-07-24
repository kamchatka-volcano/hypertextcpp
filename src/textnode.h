#pragma once
#include "idocumentnode.h"

namespace htcpp{

class TextNode: public IDocumentNode{
public:
    explicit TextNode(std::string value);
    std::string docTemplate() override;
    std::string docRenderingCode() override;    

private:
    std::string value_;
};

}
