#pragma once
#include <sfun/interface.h>
#include <string>

namespace htcpp{

class IRenderedAsStringPart : private sfun::interface<IRenderedAsStringPart> {
public:
    virtual std::string content() const = 0;
};

}