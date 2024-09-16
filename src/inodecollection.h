#pragma once
#include <sfun/interface.h>
#include <vector>
#include <memory>

namespace htcpp{

class INodeCollection : private sfun::interface<INodeCollection> {
public:
    virtual std::vector<std::unique_ptr<IDocumentNode>> flatten() = 0;
};

}