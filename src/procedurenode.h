#pragma once
#include "idocumentnode.h"
#include <string>
#include <vector>
#include <memory>

namespace htcpp{
class StreamReader;

class ProcedureNode : public IDocumentNode
{
public:
    ProcedureNode(std::string procedureName, StreamReader& stream);
    const std::string& name() const;

    std::string renderingCode() override;


private:
    void load(StreamReader& stream);

private:
    std::string procedureName_;
    std::vector<std::unique_ptr<IDocumentNode>> contentNodes_;
};

}

