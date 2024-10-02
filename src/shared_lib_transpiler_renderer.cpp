#include "codenode.h"
#include "idocumentnoderenderer.h"
#include "procedurenode.h"
#include "shared_lib_transpiler_renderer.h"

namespace htcpp{

std::string SharedLibTranspilerRenderer::generateCode(const std::vector<gsl::not_null<IDocumentNodeRenderer*>>& globalStatements,
                                                      const std::vector<std::unique_ptr<ProcedureNode>>& procedures,
                                                      const std::vector<gsl::not_null<IDocumentNodeRenderer*>>& nodes) const
{
    auto result = std::string{R"(
    #include <string>
    #include <iostream>
    #include <sstream>

    #ifdef _WIN32
      #ifdef HYPERTEXTCPP_EXPORT
        #define HYPERTEXTCPP_API __declspec(dllexport)
      #else
        #define HYPERTEXTCPP_API __declspec(dllimport)
      #endif
    #else
      #define HYPERTEXTCPP_API
    #endif

    namespace{
    struct Template;
    }
    namespace htcpp{
    class AllowRenderTag{
        friend struct ::Template;
    private:
        AllowRenderTag(){};
    };

    template <typename TCfg>
    class ITemplate{
    public:
        virtual ~ITemplate() = default;
        virtual std::string render(const TCfg&) const = 0;
        virtual std::string render(const std::string& renderFuncName, const TCfg&) const = 0;
        virtual void print(const TCfg&) const = 0;
        virtual void print(const std::string& renderFuncName, const TCfg&) const = 0;
        virtual void print(const TCfg& cfg, std::ostream& stream) const = 0;
        virtual void print(const std::string& renderFuncName, const TCfg& cfg, std::ostream& stream) const = 0;
    };
    }

    #define HTCPP_CONFIG(TCfg) using Cfg = TCfg;\
    namespace {\
    struct TemplateRenderer{\
    void renderHTML(const Cfg& cfg, std::ostream& out, htcpp::AllowRenderTag) const;\
    void renderHTMLPart(const std::string& name, const Cfg& cfg, std::ostream& out, htcpp::AllowRenderTag) const;\
    };\
    struct Template : public htcpp::ITemplate<Cfg>{\
        TemplateRenderer renderer_;\
    \
        std::string render(const Cfg& cfg) const override\
        {\
            auto stream  = std::stringstream{};\
            renderer_.renderHTML(cfg, stream, htcpp::AllowRenderTag{});\
            return stream.str();\
        }\
        std::string render(const std::string& renderFuncName, const Cfg& cfg) const override\
        {\
            auto stream  = std::stringstream{};\
            renderer_.renderHTMLPart(renderFuncName, cfg, stream, htcpp::AllowRenderTag{});\
            return stream.str();\
        }\
    \
        void print(const Cfg& cfg) const override\
        {\
            renderer_.renderHTML(cfg, std::cout, htcpp::AllowRenderTag{});\
        }\
        void print(const std::string& renderFuncName, const Cfg& cfg) const override\
        {\
            renderer_.renderHTMLPart(renderFuncName, cfg, std::cout, htcpp::AllowRenderTag{});\
        }\
    \
        void print(const Cfg& cfg, std::ostream& stream) const override\
        {\
            renderer_.renderHTML(cfg, stream, htcpp::AllowRenderTag{});\
        }\
        void print(const std::string& renderFuncName, const Cfg& cfg, std::ostream& stream) const override\
        {\
            renderer_.renderHTMLPart(renderFuncName, cfg, stream, htcpp::AllowRenderTag{});\
        }\
    };\
    }\
    \
    extern "C" \
    HYPERTEXTCPP_API htcpp::ITemplate<TCfg>* makeTemplate()\
    {\
        return new Template;\
    }\
    \
    extern "C" \
    HYPERTEXTCPP_API void deleteTemplate(htcpp::ITemplate<TCfg>* ptr)\
    {\
        delete ptr;\
    }\

    )"};
    for (const auto& globalStatement : globalStatements)
        result += globalStatement->renderingCode() + "\n";

    result += "namespace htcpp{\n";
    for (const auto& procedure : procedures) {
        result += "void " + procedure->name() + "(const Cfg& cfg, std::ostream& out, AllowRenderTag){\n";
        result += procedure->renderingCode() + "\n}\n";
    }
    result += "\n}\n";


    result +="namespace {\n"
            "void TemplateRenderer::renderHTML(const Cfg& cfg, std::ostream& out, htcpp::AllowRenderTag tag) const\n"
            "{\n";
    for (const auto& procedure : procedures)
        result += "auto " + procedure->name() + " = [&cfg, &out, tag]{htcpp::" + procedure->name() +
                  "(cfg, out, tag); return std::string{};};\n";

    for (auto& node : nodes)
        result += node->renderingCode();
    result += "\n}\n";

    result +=
            "void TemplateRenderer::renderHTMLPart(const std::string& name, const Cfg& cfg, std::ostream& out, htcpp::AllowRenderTag tag) const\n"
            "{\n"
            "(void)name; (void)cfg; (void)out; (void)tag;\n";
    for (const auto& procedure : procedures) {
        result += "if (name == \"" + procedure->name() + "\")\n";
        result += "    htcpp::" + procedure->name() + "(cfg, out, tag);";
    }
    result += "\n}\n";
    result += "}";

    return result;
}

}