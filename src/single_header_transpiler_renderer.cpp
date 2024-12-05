#include "single_header_transpiler_renderer.h"
#include "codenode.h"
#include "idocumentnoderenderer.h"
#include "procedurenode.h"

namespace htcpp{

SingleHeaderTranspilerRenderer::SingleHeaderTranspilerRenderer(std::string className)
        : className_(std::move(className))
{
}

std::string SingleHeaderTranspilerRenderer::generateCode(
        const std::vector<gsl::not_null<IDocumentNodeRenderer*>>& globalStatements,
        const std::vector<std::unique_ptr<ProcedureNode>>& procedures,
        const std::vector<gsl::not_null<IDocumentNodeRenderer*>>& nodes) const
{
    auto result = std::string{};
    result +=
            "#pragma once\n"
            "#include <iostream>\n"
            "#include <sstream>\n";

    result += "class " + className_ + ";\n";
    result += R"(
    namespace htcpp{
    class AllowRenderTag{)";
    result += "   friend class ::" + className_ +";";
    result += R"(
    private:
       AllowRenderTag(){};
    };
    })";

    for(const auto& globalStatement : globalStatements)
        result += globalStatement->renderingCode() + "\n";

    for (const auto& procedure : procedures){
        result += "namespace htcpp{\n";
        result += "template <typename TCfg>\n";
        result += "inline void " + procedure->name() + "(const TCfg& cfg, std::ostream& out, AllowRenderTag){\n";
        result += procedure->renderingCode() + "\n}\n";
        result += "}\n";
    }

    result +=
            "class " + className_ + "Renderer{\n";
    result += R"(
    public:
        template<typename TCfg>
        void renderHTML(TCfg& cfg, std::ostream& out, htcpp::AllowRenderTag tag) const
        {
            (void)cfg; (void)out; (void)tag;
        )";

    for (const auto& procedure : procedures)
        result += "auto " + procedure->name() + " = [&cfg, &out, tag]{ htcpp::" + procedure->name() + "(cfg, out, tag); return std::string{};};\n";

    for(const auto& node : nodes)
        result += node->renderingCode();

    result +=
    "    }\n";

    result += R"(
    template<typename TCfg>
    void renderHTMLPart(const std::string& name, const TCfg& cfg, std::ostream& out, htcpp::AllowRenderTag tag) const
    {
        (void)name; (void)cfg; (void)out; (void)tag;
    )";

    for (const auto& procedure : procedures){
        result += "if (name == \"" + procedure->name() + "\")\n";
        result += "    htcpp::" + procedure->name() + "(cfg, out, tag);\n";
    }
    result += "}\n";
    result += "};\n";


    result += "class " + className_ + "{\n";
    result += className_ + "Renderer renderer_;\n";
    result += R"(
    public:
        template<typename TCfg>
        std::string render(const TCfg& cfg) const
        {
            auto stream  = std::stringstream{};
            renderer_.renderHTML(cfg, stream, htcpp::AllowRenderTag{});
            return stream.str();
        }
        template<typename TCfg>
        std::string render(const std::string& renderFuncName, const TCfg& cfg) const
        {
            auto stream  = std::stringstream{};
            renderer_.renderHTMLPart(renderFuncName, cfg, stream, htcpp::AllowRenderTag{});
            return stream.str();
        }
        template<typename TCfg>
        void print(const TCfg& cfg) const
        {
            renderer_.renderHTML(cfg, std::cout, htcpp::AllowRenderTag{});
        }
        template<typename TCfg>
        void print(const std::string& renderFuncName, const TCfg& cfg) const
        {
            renderer_.renderHTMLPart(renderFuncName, cfg, std::cout, htcpp::AllowRenderTag{});
        }
        template<typename TCfg>
        void print(const TCfg& cfg, std::ostream& stream) const
        {
            renderer_.renderHTML(cfg, stream, htcpp::AllowRenderTag{});
        }
        template<typename TCfg>
        void print(const std::string& renderFuncName, const TCfg& cfg, std::ostream& stream) const
        {
            renderer_.renderHTML(renderFuncName, cfg, stream, htcpp::AllowRenderTag{});
        }
    };)";

    return result;
}

}
