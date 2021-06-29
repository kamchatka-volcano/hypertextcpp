#pragma once
#include <string>

namespace htcpp{

template <typename TCfg>
class ITemplate{
public:
    virtual ~ITemplate() = default;
    virtual std::string render(const TCfg&) = 0;
    virtual void print(const TCfg&) = 0;
};

}
