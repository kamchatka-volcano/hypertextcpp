#pragma once
#include "document_node_interface_access.h"
#include <sfun/interface.h>
#include <sfun/optional_ref.h>
#include <gsl/assert>
#include <string>
#include <typeinfo>
#include <memory>
#include <vector>

namespace htcpp {
class INodeCollection;
class IDocumentNodeRenderer;
class IRenderedAsStringPart;
class IConvertibleToProcedure;
class IAttribute;

template<typename TInterface>
struct InterfaceGetterMapping;

class IDocumentNode : private sfun::interface<IDocumentNode>
{
public:
    template<typename TInterface>
    auto getInterface() const
    {
        return (this->*InterfaceGetterMapping<TInterface>::getterPtr())();
    }

    template<typename TInterface>
    auto getInterface()
    {
        return (this->*InterfaceGetterMapping<TInterface>::getterPtr())();
    }

    template<typename T>
    bool is()
    {
        return typeid(*this) == typeid(T);
    }

private:
    DOCUMENT_NODE_ADD_INTERFACE_GETTER(INodeCollection);
    DOCUMENT_NODE_ADD_INTERFACE_GETTER(IDocumentNodeRenderer);
    DOCUMENT_NODE_ADD_INTERFACE_GETTER(IRenderedAsStringPart);
    DOCUMENT_NODE_ADD_INTERFACE_GETTER(IConvertibleToProcedure);
    DOCUMENT_NODE_ADD_INTERFACE_GETTER(IAttribute);

    template<typename TInterface>
    friend struct InterfaceGetterMapping;
};

DOCUMENT_NODE_REGISTER_INTERFACE(INodeCollection);
DOCUMENT_NODE_REGISTER_INTERFACE(IDocumentNodeRenderer);
DOCUMENT_NODE_REGISTER_INTERFACE(IRenderedAsStringPart);
DOCUMENT_NODE_REGISTER_INTERFACE(IConvertibleToProcedure);
DOCUMENT_NODE_REGISTER_INTERFACE(IAttribute);


} // namespace htcpp
