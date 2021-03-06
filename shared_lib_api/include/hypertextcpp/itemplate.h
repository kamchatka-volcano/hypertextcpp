#pragma once
#include <string>
#include <memory>

namespace htcpp{

template <typename TCfg>
class ITemplate{
public:
    virtual ~ITemplate() = default;
    virtual std::string render(const TCfg&) const = 0;
    virtual std::string render(const std::string& renderFuncName, const TCfg&) const = 0;
    virtual void print(const TCfg&) const = 0;
    virtual void print(const std::string& renderFuncName, const TCfg&) const = 0;
    virtual void print(const TCfg& cfg, std::ostream& stream) const = 0;
    virtual void print(const std::string& renderFuncName, const TCfg& cfg, std::ostream& stream) const = 0;
};

template<typename TCfg>
using TemplatePtr = std::unique_ptr<ITemplate<TCfg>, void(*)(ITemplate<TCfg>*)>;

}
