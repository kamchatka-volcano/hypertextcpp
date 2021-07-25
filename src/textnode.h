#pragma once
#include "idocumentnode.h"

namespace htcpp{

class TextNode: public IDocumentNode{
public:
    explicit TextNode(std::string value);

    std::string renderingCode() override;

private:
    std::string value_;
};

}
