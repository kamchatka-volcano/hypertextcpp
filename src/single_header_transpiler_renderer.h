#pragma once
#include "itranspiler_renderer.h"
#include "procedurenode.h"

namespace htcpp {

class SingleHeaderTranspilerRenderer : public ITranspilerRenderer {
public:
    explicit SingleHeaderTranspilerRenderer(std::string className);
    std::unordered_map<GeneratedFileType, std::string> generateCode(
            const std::vector<gsl::not_null<IDocumentNodeRenderer*>>& globalStatements,
            const std::vector<std::unique_ptr<ProcedureNode>>& procedures,
            const std::vector<gsl::not_null<IDocumentNodeRenderer*>>& nodes) const override;

private:
    std::string className_;
};

} //namespace htcpp
