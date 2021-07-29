#pragma once
#include "itemplate.h"
#include "templateloadingerror.h"
#include <string>
#include <memory>
#include <dlfcn.h>


namespace htcpp{

template <typename TCfg>
inline TemplatePtr<TCfg> loadTemplate(const std::string&)
{
    return nullptr;
}

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


}

