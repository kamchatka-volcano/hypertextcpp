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
    for(const auto& globalStatement : globalStatementList_)
        result += globalStatement->docRenderingCode() + "\n";

    for (const auto& procedure : procedureList_){
        result += "namespace {\n";
        result += "template <typename TCfg>\n";
        result += "void " + procedure->name() + "(const TCfg& cfg, std::ostream& out){\n";
        result += procedure->docRenderingCode() + "\n}\n";
        result += "}\n";
    }

    result +=
            "class " + className_ + "{\n"
                                   "    struct Impl{\n"
                                   "        template<typename TCfg>\n"
                                   "        void renderHTML(TCfg& cfg, std::ostream& out) const\n"
                                   "        {\n";

    for (const auto& procedure : procedureList_)
        result += "auto " + procedure->name() + " = [&cfg, &out]{ ::" + procedure->name() + "(cfg, out); return std::string{};};\n";

    for(const auto& node : nodeList_)
        result += node->docRenderingCode();

    result +=
            "        }\n"
            "    } impl_;\n"
            "public:\n"
            "    template<typename TCfg>\n"
            "    std::string render(const TCfg& cfg) const\n"
            "    {\n"
            "        auto stream  = std::stringstream{};\n"
            "        impl_.renderHTML(cfg, stream);\n"
            "        return stream.str();\n"
            "    }\n"
            "    template<typename TCfg>\n"
            "    void print(const TCfg& cfg) const\n"
            "    {\n"
            "        impl_.renderHTML(cfg, std::cout);\n"
            "    }\n"
            "    template<typename TCfg>\n"
            "    void print(const TCfg& cfg, std::ostream& stream) const\n"
            "    {\n"
            "        impl_.renderHTML(cfg, stream);\n"
            "    }\n"
            "};\n";

    return result;
}

}
