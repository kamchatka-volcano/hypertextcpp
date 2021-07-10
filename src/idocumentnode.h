#pragma once
#include <string>

namespace htcpp{
class StreamReader;

class IDocumentNode{
public:
    virtual ~IDocumentNode() = default;
    virtual std::string docTemplate() = 0;
    virtual std::string docRenderingCode() = 0;
    virtual bool isGlobal() {return false;}

    template<typename T>
    bool hasType()
    {
        return typeid(*this) == typeid (T);
    }
};

}




