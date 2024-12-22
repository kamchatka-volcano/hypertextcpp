#pragma once
#include "generated_file_type.h"
#include "idocumentnoderenderer.h"
#include "procedurenode.h"
#include <sfun/interface.h>
#include <gsl/pointers>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace htcpp {
class IDocumentNodeRenderer;

class ITranspilerRenderer : private sfun::interface<ITranspilerRenderer> {
public:
    virtual std::unordered_map<GeneratedFileType, std::string> generateCode(
            const std::vector<gsl::not_null<IDocumentNodeRenderer*>>& globalStatements,
            const std::vector<std::unique_ptr<ProcedureNode>>& procedures,
            const std::vector<gsl::not_null<IDocumentNodeRenderer*>>& nodes) const = 0;
};

} //namespace htcpp