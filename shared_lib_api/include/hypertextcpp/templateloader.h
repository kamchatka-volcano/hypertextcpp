#pragma once
#include "itemplate.h"
#include <memory>
#include <dlfcn.h>

namespace htcpp{

template <typename TCfg>
std::unique_ptr<ITemplate<TCfg>, void(*)(ITemplate<TCfg>*)> loadTemplate(const std::string&)
{
    return nullptr;
}

#define HTCPP_CONFIG(TCfg)\
template <>\
std::unique_ptr<htcpp::ITemplate<TCfg>, void(*)(htcpp::ITemplate<TCfg>*)> htcpp::loadTemplate<TCfg>(const std::string& libraryName)\
{\
    using make_t = htcpp::ITemplate<TCfg>*();\
    using delete_t = void(htcpp::ITemplate<TCfg>*);\
\
    void* templateLib = dlopen(libraryName.c_str(), RTLD_LAZY);\
    if (!templateLib) {\
        std::cerr << "Cannot load library: " << dlerror() << '\n';\
        throw std::runtime_error("");\
    }\
    dlerror();\
\
    make_t* make_template = reinterpret_cast<make_t*>(dlsym(templateLib, "make"#TCfg"Template"));\
    const char* dlsym_error = dlerror();\
    if (dlsym_error) {\
        std::cerr << "Cannot load symbol make"#TCfg"Template: " << dlsym_error << '\n';\
        throw std::runtime_error("");\
    }\
\
    delete_t* destroy_template = reinterpret_cast<delete_t*>(dlsym(templateLib, "delete"#TCfg"Template"));\
    dlsym_error = dlerror();\
    if (dlsym_error) {\
        std::cerr << "Cannot load symbol delete"#TCfg"Template: " << dlsym_error << '\n';\
        throw std::runtime_error("");\
    }\
\
    return std::unique_ptr<htcpp::ITemplate<TCfg>, void(*)(htcpp::ITemplate<TCfg>*)>{make_template(), destroy_template};\
}


}

