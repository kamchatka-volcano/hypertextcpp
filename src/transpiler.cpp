#include "transpiler.h"
#include "textnode.h"
#include "procedurenode.h"
#include "streamreader.h"
#include "nodereader.h"
#include "errors.h"
#include "utils.h"
#include "codenode.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <memory>
#include <map>
#include <tuple>

namespace htcpp{
Transpiler::Transpiler() = default;
Transpiler::~Transpiler() = default;

void Transpiler::parseTemplateFile(const fs::path& filePath)
{    
    auto fileStream = std::ifstream{filePath};
    if (!fileStream.is_open())
        throw ParsingError("Can't open file " + filePath.string());

    auto stream = StreamReader{fileStream};
    auto prototypeFuncMap = std::map<std::string, std::string>{};        

    while(!stream.atEnd())
        if (!readNode(stream))
            readedText_ += stream.read();

    utils::consumeReadedText(readedText_, nodeList_);
}

bool Transpiler::readNode(StreamReader& stream)
{
    auto node = readNonTagNode(stream);
    if (node){
        utils::consumeReadedText(readedText_, nodeList_, node.get());
        nodeList_.push_back(std::move(node));
        return true;
    }
    auto globalStatement = readGlobalStatement(stream);
    if (globalStatement){
        utils::consumeReadedText(readedText_, nodeList_);
        globalStatementList_.push_back(std::move(globalStatement));
        return true;
    }
    auto procedure = readProcedure(stream);
    if (procedure){
        utils::consumeReadedText(readedText_, nodeList_);
        procedureList_.push_back(std::move(procedure));
        return true;
    }
    return false;
}

std::string Transpiler::transpileToSingleHeaderRendererClass(const fs::path& filePath, const std::string& className)
{
    parseTemplateFile(filePath);
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
    "class " + className + "{\n"
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

std::string Transpiler::transpileToSharedLibRendererClass(const fs::path& filePath)
{
    parseTemplateFile(filePath);
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
    for(const auto& globalStatement : globalStatementList_)
        result += globalStatement->docRenderingCode() + "\n";

    result += "namespace {\n";
    for (const auto& procedure : procedureList_){
        result += "void " + procedure->name() + "(const Cfg& cfg, std::ostream& out){\n";
        result += procedure->docRenderingCode() + "\n}\n";
    }
    result += "\n}\n";


    result +=
    "void Template::Impl::renderHTML(const Cfg& cfg, std::ostream& out) const\n"
    "{\n";
    for (const auto& procedure : procedureList_)
        result += "auto " + procedure->name() + " = [&cfg, &out]{::" + procedure->name() + "(cfg, out); return std::string{};};\n";

    for(auto& node : nodeList_)
        result += node->docRenderingCode();
    result += "\n}\n";

    result +=
    "void Template::Impl::renderHTMLPart(const std::string& name, const Cfg& cfg, std::ostream& out) const\n"
    "{\n";

    for (const auto& procedure : procedureList_){
        result += "if (name == \"" + procedure->name() + "\")\n";
        result += procedure->name() + "(cfg, out);";
    }
    result += "}\n";

    return result;
}

}
