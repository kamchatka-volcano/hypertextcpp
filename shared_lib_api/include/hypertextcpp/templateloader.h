#pragma once
#include "itemplate.h"
#include "templateloadingerror.h"
#include <string>
#include <memory>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif


namespace htcpp{

template <typename TCfg>
inline TemplatePtr<TCfg> loadTemplate(const std::string&)
{
    return nullptr;
}

#ifdef _WIN32
#define HTCPP_CONFIG(TCfg)\
template <>\
inline htcpp::TemplatePtr<TCfg> htcpp::loadTemplate<TCfg>(const std::string& libraryName)\
{\
    using make_t = htcpp::ITemplate<TCfg>*();\
    using delete_t = void(htcpp::ITemplate<TCfg>*);\
\
    HINSTANCE templateLib =  LoadLibrary(libraryName.c_str());\
    if (!templateLib)\
        throw TemplateLoadingError{std::string{} + "Cannot load library " + libraryName};\
    \
\
    make_t* make_template = reinterpret_cast<make_t*>(GetProcAddress(templateLib, "makeTemplate"));\
    if (!make_template) {\
        throw TemplateLoadingError{"Cannot load symbol makeTemplate"};\
    }\
\
    delete_t* destroy_template = reinterpret_cast<delete_t*>(GetProcAddress(templateLib, "deleteTemplate"));\
    if (!destroy_template)\
        throw TemplateLoadingError{std::string{} + "Cannot load symbol deleteTemplate"};\
    \
    return htcpp::TemplatePtr<TCfg>{make_template(), destroy_template};\
}
#else
#define HTCPP_CONFIG(TCfg)\
template <>\
inline htcpp::TemplatePtr<TCfg> htcpp::loadTemplate<TCfg>(const std::string& libraryName)\
{\
  using make_t = htcpp::ITemplate<TCfg>*();\
  using delete_t = void(htcpp::ITemplate<TCfg>*);\
\
  void* templateLib = dlopen(libraryName.c_str(), RTLD_LAZY);\
  if (!templateLib)\
    throw TemplateLoadingError{std::string{} + "Cannot load library " + libraryName + ": " + dlerror()};\
    \
  dlerror();\
\
  make_t* make_template = reinterpret_cast<make_t*>(dlsym(templateLib, "makeTemplate"));\
  const char* dlsym_error = dlerror();\
  if (dlsym_error) {\
    throw TemplateLoadingError{std::string{} + "Cannot load symbol makeTemplate: " + dlsym_error};\
  }\
\
  delete_t* destroy_template = reinterpret_cast<delete_t*>(dlsym(templateLib, "deleteTemplate"));\
  dlsym_error = dlerror();\
  if (dlsym_error)\
    throw TemplateLoadingError{std::string{} + "Cannot load symbol deleteTemplate: " + dlsym_error};\
    \
  return htcpp::TemplatePtr<TCfg>{make_template(), destroy_template};\
}
#endif


}

