#include "singleheadertranspiler.h"
#include "codenode.h"
#include "procedurenode.h"

namespace htcpp{

SingleHeaderTranspiler::SingleHeaderTranspiler(std::string className)
        : className_(std::move(className))
{
}

std::string SingleHeaderTranspiler::generateCode() const
{
    auto result = std::string{};
    result +=
            "#include <iostream>\n"
            "#include <sstream>\n";

    result += "class " + className_ + ";\n";
    result += R"(
    namespace htcpp{
    class AllowRenderTag{
       friend class ::Simple;
    private:
       AllowRenderTag(){};
    };
    })";

    for(const auto& globalStatement : globalStatementList_)
        result += globalStatement->renderingCode() + "\n";

    for (const auto& procedure : procedureList_){
        result += "namespace htcpp{\n";
        result += "template <typename TCfg>\n";
        result += "void " + procedure->name() + "(const TCfg& cfg, std::ostream& out, AllowRenderTag){\n";
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
)";

    for (const auto& procedure : procedureList_)
        result += "auto " + procedure->name() + " = [&cfg, &out, tag]{ htcpp::" + procedure->name() + "(cfg, out, tag); return std::string{};};\n";

    for(const auto& node : nodeList_)
        result += node->renderingCode();

    result +=
    "    }\n";

    result += R"(
    template<typename TCfg>
    void renderHTMLPart([[maybe_unused]] const std::string& name, [[maybe_unused]]const TCfg& cfg, [[maybe_unused]]std::ostream& out, htcpp::AllowRenderTag tag) const
    {
)";

    for (const auto& procedure : procedureList_){
        result += "if (name == \"" + procedure->name() + "\")\n";
        result += "htcpp::" + procedure->name() + "(cfg, out, tag);";
    }
    result += "    }\n";
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
