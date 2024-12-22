#include "single_header_transpiler_renderer.h"
#include "idocumentnoderenderer.h"
#include "procedurenode.h"

namespace htcpp {

SingleHeaderTranspilerRenderer::SingleHeaderTranspilerRenderer(std::string className)
    : className_(std::move(className))
{
}

std::unordered_map<GeneratedFileType, std::string> SingleHeaderTranspilerRenderer::generateCode(
        const std::vector<gsl::not_null<IDocumentNodeRenderer*>>& globalStatements,
        const std::vector<std::unique_ptr<ProcedureNode>>& procedures,
        const std::vector<gsl::not_null<IDocumentNodeRenderer*>>& nodes) const
{
    auto result = std::string{};
    result += "#pragma once\n"
              "#include <string>\n"
              "#include <iostream>\n"
              "#include <sstream>\n";
    for (const auto& globalStatement : globalStatements)
        result += globalStatement->renderingCode() + "\n";
    result += "\n";

    result += "class " + className_ + "{\n";
    result += "private:\n";
    result += "template<typename TCfg>\n";
    result += "class Renderer{\n";
    result += "const TCfg& cfg;\n";
    result += "std::ostream& out;\n";
    for (const auto& procedure : procedures) {
        result += "    std::string " + procedure->name() + "() const{\n";
        result += procedure->renderingCode() + "\nreturn {};\n}\n";
    }

    result += R"(
public:
    Renderer(const TCfg& cfg, std::ostream& out)
        : cfg(cfg), out(out)
    {}
    void renderHTML() const
    {
    )";
    for (const auto& node : nodes)
        result += node->renderingCode();
    result += "    }\n";

    result += R"(
    void renderHTMLPart(const std::string& name) const
    {
        static_cast<void>(name);
    )";

    for (const auto& procedure : procedures) {
        result += "if (name == \"" + procedure->name() + "\")\n";
        result += "    " + procedure->name() + "();\n";
    }
    result += "    }\n";

    result += "};\n";
    result += R"(
public:
    template<typename TCfg>
    std::string render(const TCfg& cfg) const
    {
        auto stream  = std::stringstream{};
        auto renderer = Renderer<TCfg>{cfg, stream};
        renderer.renderHTML();
        return stream.str();
    }
    template<typename TCfg>
    std::string render(const std::string& renderFuncName, const TCfg& cfg) const
    {
        auto stream  = std::stringstream{};
        auto renderer = Renderer<TCfg>{cfg, stream};
        renderer.renderHTMLPart(renderFuncName);
        return stream.str();
    }
    template<typename TCfg>
    void print(const TCfg& cfg) const
    {
        auto renderer = Renderer<TCfg>{cfg, std::cout};
        renderer.renderHTML();
    }
    template<typename TCfg>
    void print(const std::string& renderFuncName, const TCfg& cfg) const
    {
        auto renderer = Renderer<TCfg>{cfg, std::cout};
        renderer.renderHTMLPart(renderFuncName);
    }
    template<typename TCfg>
    void print(const TCfg& cfg, std::ostream& stream) const
    {
        auto renderer = Renderer<TCfg>{cfg, stream};
        renderer.renderHTML(cfg, stream);
    }
    template<typename TCfg>
    void print(const std::string& renderFuncName, const TCfg& cfg, std::ostream& stream) const
    {
        auto renderer = Renderer<TCfg>{cfg, stream};
        renderer.renderHTML(renderFuncName, cfg, stream);
    }
};)";

    return {{GeneratedFileType::Header, std::move(result)}};
}

} //namespace htcpp
