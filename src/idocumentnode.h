#pragma once
#include <sfun/interface.h>
#include <sfun/optional_ref.h>

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
    sfun::optional_ref<const TInterface> as() const
    {
        auto obj = dynamic_cast<const TInterface*>(this);
        return obj ? sfun::optional_ref<const TInterface>{*obj} : sfun::optional_ref<const TInterface>{};
    }

    template<typename TInterface>
    sfun::optional_ref<TInterface> as()
    {
        auto obj = dynamic_cast<TInterface*>(this);
        return obj ? sfun::optional_ref<TInterface>{*obj} : sfun::optional_ref<TInterface>{};
    }

    template<typename T>
    bool is() const
    {
        return as<T>().has_value();
    }
};

} // namespace htcpp
