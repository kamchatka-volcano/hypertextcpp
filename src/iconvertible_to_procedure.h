#pragma once
#include <sfun/interface.h>
#include <optional>
#include <string_view>

namespace htcpp {

class IConvertibleToProcedure : private sfun::interface<IConvertibleToProcedure> {
public:
    virtual std::optional<std::string_view> procedureName() const = 0;
};

} //namespace htcpp