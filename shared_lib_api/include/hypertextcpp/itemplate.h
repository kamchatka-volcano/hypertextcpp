#ifndef HYPERTEXTCPP_ITEMPLATE_H
#define HYPERTEXTCPP_ITEMPLATE_H

#include <string>
#include <memory>
#ifdef _WIN32
#include <windows.h>
#define HTCPP_TEMPLATE_LIB_HANDLE HMODULE
#else
#include <dlfcn.h>
#define HTCPP_TEMPLATE_LIB_HANDLE void*
#endif

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
class TemplatePtrDeleter
{
  using DeleteTemplateFunc = void(htcpp::ITemplate<TCfg>*);

public:
  TemplatePtrDeleter(DeleteTemplateFunc deleteFunc, HTCPP_TEMPLATE_LIB_HANDLE templateHandle)
      : deleteFunc_{deleteFunc}
      , templateHandle_{templateHandle}
  {}

  void operator()(ITemplate<TCfg>* templatePtr)
  {
     deleteFunc_(templatePtr);
#ifdef _WIN32
    FreeLibrary(templateHandle_);
#else
     dlclose(templateHandle_);
#endif
  }

private:
  void(*deleteFunc_)(htcpp::ITemplate<TCfg>*);
  HTCPP_TEMPLATE_LIB_HANDLE templateHandle_;
};

template<typename TCfg>
using TemplatePtr = std::unique_ptr<ITemplate<TCfg>, TemplatePtrDeleter<TCfg>>;

}

#undef HTCPP_TEMPLATE_LIB_HANDLE
#endif // HYPERTEXTCPP_ITEMPLATE_H