#pragma once
#include "itranspiler_renderer.h"
#include "procedurenode.h"

namespace htcpp {

class HeaderAndSourceTranspilerRenderer : public ITranspilerRenderer {
public:
    HeaderAndSourceTranspilerRenderer(std::string className, std::string headerFileName, std::string configTypeName);

    std::unordered_map<GeneratedFileType, std::string> generateCode(
            const std::vector<gsl::not_null<IDocumentNodeRenderer*>>& globalStatements,
            const std::vector<std::unique_ptr<ProcedureNode>>& procedures,
            const std::vector<gsl::not_null<IDocumentNodeRenderer*>>& nodes) const override;

private:
    std::string className_;
    std::string headerFileName_;
    std::string configTypeName_;
};

} //namespace htcpp
