#pragma once
#include <string>

namespace htcpp{
class StreamReader;

class IDocumentNode{
public:
    virtual ~IDocumentNode() = default;
    virtual std::string renderingCode() = 0;

    template<typename T>
    bool hasType()
    {
        return typeid(*this) == typeid (T);
    }
};

}




