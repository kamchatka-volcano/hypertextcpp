#pragma once
#include <sfun/interface.h>
#include <string_view>

namespace htcpp{

class IAttribute : private sfun::interface<IAttribute> {
public:
    virtual std::string_view name() const = 0;
    virtual std::string_view value() const = 0;
};

}