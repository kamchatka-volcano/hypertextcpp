#pragma once
#include "idocumentnode.h"
#include <string>
#include <map>

namespace htcpp{
class StreamReader;

class PrototypeNode : public IDocumentNode
{
public:
    PrototypeNode(StreamReader& stream, const std::map<std::string, std::string>& prototypeFuncMap);
    std::string docTemplate() override;
    std::string docRenderingCode() override;

private:
    void load(StreamReader& stream, const std::map<std::string, std::string>& prototypeFuncMap);

private:
    std::string funcName_;
};

}

