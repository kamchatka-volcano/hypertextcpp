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
    struct Template : public htcpp::ITemplate<Cfg>{\
        struct Impl{\
            void renderHTML(const Cfg& cfg, std::ostream& out) const;\
            void renderHTMLPart(const std::string& name, const Cfg& cfg, std::ostream& out) const;\
        } impl_;\
    \
        std::string render(const Cfg& cfg) const override\
        {\
            auto stream  = std::stringstream{};\
            impl_.renderHTML(cfg, stream);\
            return stream.str();\
        }\
        std::string render(const std::string& renderFuncName, const Cfg& cfg) const override\
        {\
            auto stream  = std::stringstream{};\
            impl_.renderHTMLPart(renderFuncName, cfg, stream);\
            return stream.str();\
        }\
    \
        void print(const Cfg& cfg) const override\
        {\
            impl_.renderHTML(cfg, std::cout);\
        }\
        void print(const std::string& renderFuncName, const Cfg& cfg) const override\
        {\
            impl_.renderHTMLPart(renderFuncName, cfg, std::cout);\
        }\
    \
        void print(const Cfg& cfg, std::ostream& stream) const override\
        {\
            impl_.renderHTML(cfg, stream);\
        }\
        void print(const std::string& renderFuncName, const Cfg& cfg, std::ostream& stream) const override\
        {\
            impl_.renderHTMLPart(renderFuncName, cfg, stream);\
        }\
    };\
    \
    extern "C" \
    htcpp::ITemplate<TCfg>* make##TCfg##Template()\
    {\
        return new Template;\
    }\
    \
    extern "C" \
    void delete##TCfg##Template(htcpp::ITemplate<TCfg>* ptr)\
    {\
        delete ptr;\
    }\

    )"};
    for (const auto& globalStatement : globalStatementList_)
        result += globalStatement->docRenderingCode() + "\n";

    result += "namespace {\n";
    for (const auto& procedure : procedureList_) {
        result += "void " + procedure->name() + "(const Cfg& cfg, std::ostream& out){\n";
        result += procedure->docRenderingCode() + "\n}\n";
    }
    result += "\n}\n";


    result +=
            "void Template::Impl::renderHTML(const Cfg& cfg, std::ostream& out) const\n"
            "{\n";
    for (const auto& procedure : procedureList_)
        result += "auto " + procedure->name() + " = [&cfg, &out]{::" + procedure->name() +
                  "(cfg, out); return std::string{};};\n";

    for (auto& node : nodeList_)
        result += node->docRenderingCode();
    result += "\n}\n";

    result +=
            "void Template::Impl::renderHTMLPart(const std::string& name, const Cfg& cfg, std::ostream& out) const\n"
            "{\n";

    for (const auto& procedure : procedureList_) {
        result += "if (name == \"" + procedure->name() + "\")\n";
        result += procedure->name() + "(cfg, out);";
    }
    result += "}\n";

    return result;
}

}