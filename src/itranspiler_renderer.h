#pragma once
#include "idocumentnoderenderer.h"
#include "procedurenode.h"
#include <sfun/interface.h>
#include <gsl/pointers>
#include <memory>
#include <string>
#include <vector>

namespace htcpp {
class IDocumentNodeRenderer;

class ITranspilerRenderer : private sfun::interface<ITranspilerRenderer> {
public:
    virtual std::string generateCode(
            const std::vector<gsl::not_null<IDocumentNodeRenderer*>>& globalStatements,
            const std::vector<std::unique_ptr<ProcedureNode>>& procedures,
            const std::vector<gsl::not_null<IDocumentNodeRenderer*>>& nodes) const = 0;
};

} //namespace htcpp