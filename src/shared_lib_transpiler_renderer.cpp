#include "shared_lib_transpiler_renderer.h"
#include "codenode.h"
#include "idocumentnoderenderer.h"
#include "procedurenode.h"

namespace htcpp {
std::unordered_map<GeneratedFileType, std::string> SharedLibTranspilerRenderer::generateCode(
        const std::vector<gsl::not_null<IDocumentNodeRenderer*>>& globalStatements,
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

    namespace htcpp{
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
    struct Template : public htcpp::ITemplate<Cfg>{\
        class Renderer{\
            const Cfg& cfg;\
            std::ostream& out;\
        public:\
            Renderer(const Cfg& cfg, std::ostream& out)\
                : cfg(cfg), out(out)\
            {}\)"};
    result += "\n";
    for (const auto& procedure : procedures) {
        result += "            std::string " + procedure->name() + "() const;\\\n";
    }
    result += R"(\
            void renderHTML() const;\
            void renderHTMLPart(const std::string& name) const;\
        };\
        \
        std::string render(const Cfg& cfg) const override\
        {\
            auto stream  = std::stringstream{};\
            auto renderer = Renderer{cfg, stream};\
            renderer.renderHTML();\
            return stream.str();\
        }\
        std::string render(const std::string& renderFuncName, const Cfg& cfg) const override\
        {\
            auto stream  = std::stringstream{};\
            auto renderer = Renderer{cfg, stream};\
            renderer.renderHTMLPart(renderFuncName);\
            return stream.str();\
        }\
    \
        void print(const Cfg& cfg) const override\
        {\
            auto renderer = Renderer{cfg, std::cout};\
            renderer.renderHTML();\
        }\
        void print(const std::string& renderFuncName, const Cfg& cfg) const override\
        {\
            auto renderer = Renderer{cfg, std::cout};\
            renderer.renderHTMLPart(renderFuncName);\
        }\
    \
        void print(const Cfg& cfg, std::ostream& stream) const override\
        {\
            auto renderer = Renderer{cfg, stream};\
            renderer.renderHTML();\
        }\
        void print(const std::string& renderFuncName, const Cfg& cfg, std::ostream& stream) const override\
        {\
            auto renderer = Renderer{cfg, stream};\
            renderer.renderHTMLPart(renderFuncName);\
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

    )";
    for (const auto& globalStatement : globalStatements)
        result += globalStatement->renderingCode() + "\n";

    result += "namespace {\n";
    for (const auto& procedure : procedures) {
        result += "std::string Template::Renderer::" + procedure->name() + "() const{\n";
        result += procedure->renderingCode() + "\n return {};}\n";
    }
    result += "void Template::Renderer::renderHTML() const\n{\n";

    for (auto& node : nodes)
        result += node->renderingCode();
    result += "\n}\n";

    result += "void Template::Renderer::renderHTMLPart(const std::string& name) const\n"
              "{\n";
    result += "static_cast<void>(name);\n";
    for (const auto& procedure : procedures) {
        result += "if (name == \"" + procedure->name() + "\")\n";
        result += "    " + procedure->name() + "();\n";
    }
    result += "\n}\n";
    result += "}";

    return {{GeneratedFileType::Source, result}};
}

} //namespace htcpp