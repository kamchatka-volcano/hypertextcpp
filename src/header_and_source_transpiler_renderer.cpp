#include "header_and_source_transpiler_renderer.h"
#include "idocumentnoderenderer.h"
#include "procedurenode.h"
#include <fmt/format.h>

namespace htcpp {

HeaderAndSourceTranspilerRenderer::HeaderAndSourceTranspilerRenderer(
        std::string className,
        std::string headerFileName,
        std::string configTypeName)
    : className_(std::move(className))
    , headerFileName_(std::move(headerFileName))
    , configTypeName_(std::move(configTypeName))
{
}

namespace {

std::string generateHeader(
        const std::string& className,
        const std::string& configTypeName,
        const std::vector<gsl::not_null<IDocumentNodeRenderer*>>& globalStatements,
        const std::vector<std::unique_ptr<ProcedureNode>>& procedures)
{
    auto globalStatementsCode = std::string{};
    for (const auto& globalStatement : globalStatements)
        globalStatementsCode += globalStatement->renderingCode() + "\n";

    auto proceduresDeclarations = std::string{};
    for (const auto& procedure : procedures)
        proceduresDeclarations += "    std::string " + procedure->name() + "() const;\n";

    return fmt::format(
            R"(
#pragma once
#include <string>
#include <iostream>
#include <sstream>

{globalStatementsCode}

class {className} {{
private:
class Renderer{{
    const {configTypeName}& cfg;
    std::ostream& out;
    {proceduresDeclarations}

public:
    Renderer(const {configTypeName}& cfg, std::ostream& out);
    void renderHTML() const;
    void renderHTMLPart(const std::string& name) const;
}};
public:
    std::string render(const {configTypeName}& cfg) const;
    std::string render(const std::string& renderFuncName, const {configTypeName}& cfg) const;
    void print(const {configTypeName}& cfg) const;
    void print(const std::string& renderFuncName, const {configTypeName}& cfg) const;
    void print(const {configTypeName}& cfg, std::ostream& stream) const;
    void print(const std::string& renderFuncName, const {configTypeName}& cfg, std::ostream& stream) const;
}};)",
            fmt::arg("className", className),
            fmt::arg("configTypeName", configTypeName),
            fmt::arg("globalStatementsCode", globalStatementsCode),
            fmt::arg("proceduresDeclarations", proceduresDeclarations));
}

std::string generateSource(
        const std::string& headerFileName,
        const std::string& className,
        const std::string& configTypeName,
        const std::vector<std::unique_ptr<ProcedureNode>>& procedures,
        const std::vector<gsl::not_null<IDocumentNodeRenderer*>>& nodes)
{
    auto proceduresImplementations = std::string{};
    for (const auto& procedure : procedures) {
        proceduresImplementations += "std::string " + className + "::Renderer::" + procedure->name() + "() const{\n";
        proceduresImplementations += procedure->renderingCode() + "\n return {};\n}\n";
    }

    auto templateHtmlRenderingCode = std::string{};
    for (const auto& node : nodes)
        templateHtmlRenderingCode += node->renderingCode();

    auto procedureInvocations = std::string{};
    for (const auto& procedure : procedures) {
        procedureInvocations += "if (name == \"" + procedure->name() + "\")\n";
        procedureInvocations += "    " + procedure->name() + "();\n";
    }

    return fmt::format(
            R"(
#include "{headerFileName}"

{procedureImplementations}

{className}::Renderer::Renderer(const {configTypeName}& cfg, std::ostream& out)
        : cfg(cfg), out(out)
{{}}

void {className}::Renderer::renderHTML() const
{{
{templateHtmlRenderingCode}
}}

void {className}::Renderer::renderHTMLPart(const std::string& name) const
{{
    static_cast<void>(name);
{procedureInvocations}
}}

std::string {className}::render(const {configTypeName}& cfg) const
{{
        auto stream  = std::stringstream{{}};
        auto renderer = Renderer{{cfg, stream}};
        renderer.renderHTML();
        return stream.str();
}}

std::string {className}::render(const std::string& renderFuncName, const {configTypeName}& cfg) const
{{
    auto stream  = std::stringstream{{}};
    auto renderer = Renderer{{cfg, stream}};
    renderer.renderHTMLPart(renderFuncName);
    return stream.str();
}}

void {className}::print(const {configTypeName}& cfg) const
{{
    auto renderer = Renderer{{cfg, std::cout}};
    renderer.renderHTML();
}}

void {className}::print(const std::string& renderFuncName, const {configTypeName}& cfg) const
{{
    auto renderer = Renderer{{cfg, std::cout}};
    renderer.renderHTMLPart(renderFuncName);
}}

void {className}::print(const {configTypeName}& cfg, std::ostream& stream) const
{{
    auto renderer = Renderer{{cfg, stream}};
    renderer.renderHTML();
}}

void {className}::print(const std::string& renderFuncName, const {configTypeName}& cfg, std::ostream& stream) const
{{
    auto renderer = Renderer{{cfg, stream}};
    renderer.renderHTMLPart(renderFuncName);
}}
)",
            fmt::arg("headerFileName", headerFileName),
            fmt::arg("procedureImplementations", proceduresImplementations),
            fmt::arg("className", className),
            fmt::arg("configTypeName", configTypeName),
            fmt::arg("templateHtmlRenderingCode", templateHtmlRenderingCode),
            fmt::arg("procedureInvocations", procedureInvocations));
}

} //namespace

std::unordered_map<GeneratedFileType, std::string> HeaderAndSourceTranspilerRenderer::generateCode(
        const std::vector<gsl::not_null<IDocumentNodeRenderer*>>& globalStatements,
        const std::vector<std::unique_ptr<ProcedureNode>>& procedures,
        const std::vector<gsl::not_null<IDocumentNodeRenderer*>>& nodes) const
{
    return {{GeneratedFileType::Header, generateHeader(className_, configTypeName_, globalStatements, procedures)},
            {GeneratedFileType::Source,
             generateSource(headerFileName_, className_, configTypeName_, procedures, nodes)}};
}

} //namespace htcpp
