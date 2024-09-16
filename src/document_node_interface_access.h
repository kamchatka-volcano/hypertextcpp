#pragma once
#include <sfun/optional_ref.h>

#define DOCUMENT_NODE_ADD_INTERFACE_GETTER(InterfaceName) \
virtual sfun::optional_ref<const InterfaceName> get##InterfaceName() const { return {}; } \
virtual sfun::optional_ref<InterfaceName> get##InterfaceName() { return {}; }


#define DOCUMENT_NODE_REGISTER_INTERFACE(InterfaceName) \
class InterfaceName;                                    \
template<> \
struct InterfaceGetterMapping<InterfaceName>{ \
    using GetterPtr = sfun::optional_ref<InterfaceName>(IDocumentNode::*)(); \
    constexpr static GetterPtr getterPtr(){return &IDocumentNode::get##InterfaceName;} \
};


#define DOCUMENT_NODE_INTERFACE_ACCESS_READ(InterfaceName) \
sfun::optional_ref<const InterfaceName> getInterfaceName() const override { return *this; }

#define DOCUMENT_NODE_INTERFACE_ACCESS_WRITE(InterfaceName) \
sfun::optional_ref<InterfaceName> getInterfaceName() override { return *this; }

#define DOCUMENT_NODE_INTERFACE_ACCESS(InterfaceName) \
sfun::optional_ref<const InterfaceName> get##InterfaceName() const override { return *this; } \
sfun::optional_ref<InterfaceName> get##InterfaceName() override { return *this; }
