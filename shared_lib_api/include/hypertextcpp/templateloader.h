#ifndef HYPERTEXTCPP_TEMPLATELOADER_H
#define HYPERTEXTCPP_TEMPLATELOADER_H

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
    using MakeTemplateFunc = htcpp::ITemplate<TCfg>*();\
    using DeleteTemplateFunc = void(htcpp::ITemplate<TCfg>*);\
\
    HMODULE templateLib =  LoadLibrary(libraryName.c_str());\
    if (!templateLib)\
        throw TemplateLoadingError{std::string{} + "Cannot load library " + libraryName};\
    \
\
    MakeTemplateFunc* makeTemplate = reinterpret_cast<MakeTemplateFunc*>(GetProcAddress(templateLib, "makeTemplate"));\
    if (!makeTemplate) {\
        throw TemplateLoadingError{"Cannot load symbol makeTemplate"};\
    }\
\
    DeleteTemplateFunc* deleteTemplate = reinterpret_cast<DeleteTemplateFunc*>(GetProcAddress(templateLib, "deleteTemplate"));\
    if (!deleteTemplate)\
        throw TemplateLoadingError{std::string{} + "Cannot load symbol deleteTemplate"};\
    \
    return htcpp::TemplatePtr<TCfg>{makeTemplate(), TemplatePtrDeleter<TCfg>{deleteTemplate, templateLib}};\
}
#else
#define HTCPP_CONFIG(TCfg)\
template <>\
inline htcpp::TemplatePtr<TCfg> htcpp::loadTemplate<TCfg>(const std::string& libraryName)\
{\
  using MakeTemplateFunc = htcpp::ITemplate<TCfg>*();\
  using DeleteTemplateFunc = void(htcpp::ITemplate<TCfg>*);\
\
  void* templateLib = dlopen(libraryName.c_str(), RTLD_LAZY);\
  if (!templateLib)\
    throw TemplateLoadingError{std::string{} + "Cannot load library " + libraryName + ": " + dlerror()};\
    \
  dlerror();\
\
  MakeTemplateFunc* makeTemplate = reinterpret_cast<MakeTemplateFunc*>(dlsym(templateLib, "makeTemplate"));\
  const char* dlsym_error = dlerror();\
  if (dlsym_error) {\
    throw TemplateLoadingError{std::string{} + "Cannot load symbol makeTemplate: " + dlsym_error};\
  }\
\
  DeleteTemplateFunc* deleteTemplate = reinterpret_cast<DeleteTemplateFunc*>(dlsym(templateLib, "deleteTemplate"));\
  dlsym_error = dlerror();\
  if (dlsym_error)\
    throw TemplateLoadingError{std::string{} + "Cannot load symbol deleteTemplate: " + dlsym_error};\
    \
  return htcpp::TemplatePtr<TCfg>{makeTemplate(), TemplatePtrDeleter<TCfg>{deleteTemplate, templateLib}};\
}
#endif


}

#endif // HYPERTEXTCPP_TEMPLATELOADER_H