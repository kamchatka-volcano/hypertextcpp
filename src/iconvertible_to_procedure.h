#pragma once
#include <sfun/interface.h>
#include <string_view>

namespace htcpp{

class IConvertibleToProcedure : private sfun::interface<IConvertibleToProcedure> {
public:
    virtual std::string_view procedureName() const = 0;
};

}