#pragma once
#include <string>

namespace htcpp{
class StreamReader;

class IDocumentNode{
public:
    virtual ~IDocumentNode() = default;
    virtual void load(StreamReader& stream) = 0;
    virtual std::string docTemplate() = 0;
    virtual std::string docRenderingCode() = 0;
    virtual bool isGlobalScoped() {return false;}
};

}




