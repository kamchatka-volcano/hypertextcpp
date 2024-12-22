#pragma once
#include "itranspiler_renderer.h"
#include "procedurenode.h"

namespace htcpp {
class IDocumentNodeRenderer;

class SharedLibTranspilerRenderer : public ITranspilerRenderer {
public:
    std::unordered_map<GeneratedFileType, std::string> generateCode(
            const std::vector<gsl::not_null<IDocumentNodeRenderer*>>& globalStatements,
            const std::vector<std::unique_ptr<ProcedureNode>>& procedures,
            const std::vector<gsl::not_null<IDocumentNodeRenderer*>>& nodes) const override;
};

} //namespace htcpp
