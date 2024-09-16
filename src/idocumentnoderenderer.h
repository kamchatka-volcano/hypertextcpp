#pragma once
#include <sfun/interface.h>
#include <string>

namespace htcpp{

class IDocumentNodeRenderer : private sfun::interface<IDocumentNodeRenderer> {
public:
    virtual std::string renderingCode() const = 0;
};

}