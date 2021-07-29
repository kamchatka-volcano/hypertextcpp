#include "sharedlibtranspiler.h"
#include "codenode.h"
#include "procedurenode.h"

namespace htcpp{

std::string SharedLibTranspiler::generateCode() const
{
    auto result = std::string{R"(
    #include <string>
    #include <iostream>
    #include <sstream>

    struct Template;
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
    struct TemplateRenderer{\
    void renderHTML(const Cfg& cfg, std::ostream& out, htcpp::AllowRenderTag) const;\
    void renderHTMLPart([[maybe_unused]] const std::string& name, [[maybe_unused]]const Cfg& cfg, [[maybe_unused]]std::ostream& out, htcpp::AllowRenderTag) const;\
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
    \
    extern "C" \
    htcpp::ITemplate<TCfg>* makeTemplate()\
    {\
        return new Template;\
    }\
    \
    extern "C" \
    void deleteTemplate(htcpp::ITemplate<TCfg>* ptr)\
    {\
        delete ptr;\
    }\

    )"};
    for (const auto& globalStatement : globalStatementList_)
        result += globalStatement->renderingCode() + "\n";

    result += "namespace htcpp{\n";
    for (const auto& procedure : procedureList_) {
        result += "void " + procedure->name() + "(const Cfg& cfg, std::ostream& out, AllowRenderTag){\n";
        result += procedure->renderingCode() + "\n}\n";
    }
    result += "\n}\n";


    result +=
            "void TemplateRenderer::renderHTML(const Cfg& cfg, std::ostream& out, htcpp::AllowRenderTag tag) const\n"
            "{\n";
    for (const auto& procedure : procedureList_)
        result += "auto " + procedure->name() + " = [&cfg, &out, tag]{htcpp::" + procedure->name() +
                  "(cfg, out, tag); return std::string{};};\n";

    for (auto& node : nodeList_)
        result += node->renderingCode();
    result += "\n}\n";

    result +=
            "void TemplateRenderer::renderHTMLPart(const std::string& name, const Cfg& cfg, std::ostream& out, htcpp::AllowRenderTag tag) const\n"
            "{\n";

    for (const auto& procedure : procedureList_) {
        result += "if (name == \"" + procedure->name() + "\")\n";
        result += "    htcpp::" + procedure->name() + "(cfg, out, tag);";
    }
    result += "\n}\n";

    return result;
}

}