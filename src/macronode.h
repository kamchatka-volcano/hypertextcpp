#pragma once
#include "idocumentnode.h"
#include <string>
#include <map>

namespace htcpp{
class StreamReader;

class MacroNode : public IDocumentNode
{
public:
    MacroNode(StreamReader& stream, const std::map<std::string, std::string>& macrosMap);
    std::string docTemplate() override;
    std::string docRenderingCode() override;

private:
    void load(StreamReader& stream, const std::map<std::string, std::string>& macrosMap);

private:
    std::string name_;
    std::string content_;
};

}

