#include "transpiler.h"
#include "textnode.h"
#include "streamreader.h"
#include "nodereader.h"
#include "errors.h"
#include "utils.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <memory>
#include <map>
#include <tuple>

namespace htcpp{
namespace {

std::tuple<std::vector<std::unique_ptr<IDocumentNode>>,
           std::map<std::string, std::string>> parseTemplateFile(const fs::path& filePath)
{
    auto nodeList = std::vector<std::unique_ptr<IDocumentNode>>{};
    auto fileStream = std::ifstream{filePath};
    if (!fileStream.is_open())
        throw ParsingError("Can't open file " + filePath.string());

    auto stream = StreamReader{fileStream};
    auto prototypeFuncMap = std::map<std::string, std::string>{};
    auto nodeReader = NodeReader{prototypeFuncMap};
    auto readedText = std::string{};

    while(!stream.atEnd()){
        auto node = nodeReader.readTopLevelNode(stream);
        if (node){
            utils::consumeReadedText(readedText, nodeList, node.get());
            nodeList.push_back(std::move(node));
        }
        else{
            readedText += stream.read();
        }
    }
    utils::consumeReadedText(readedText, nodeList);
    return {std::move(nodeList), prototypeFuncMap};
}
}

std::string transpileToSingleHeaderRendererClass(const fs::path& filePath, const std::string& className)
{
    const auto [nodeList, funcMap] = parseTemplateFile(filePath);
    auto result = std::string{};
    result +=
    "#include <iostream>\n"
    "#include <sstream>\n";
    for(auto& node : nodeList)
        if (node->isGlobal())
            result += node->docRenderingCode() + "\n";

    for (const auto& nameFuncPair : funcMap){
        result += "template <typename TCfg>\n";
        result += nameFuncPair.first + "(const TCfg& cfg, std::ostream& out){\n";
        result += nameFuncPair.second + "}\n";
    }

    result +=
    "class " + className + "{\n"
    "    struct Impl{\n"
    "        template<typename TCfg>\n"
    "        void renderHTML(TCfg& cfg, std::ostream& out)\n"
    "        {\n";

    for(auto& node : nodeList)
        if (!node->isGlobal())
            result += node->docRenderingCode();

    result +=
    "        }\n"
    "    } impl_;\n"
    "public:\n"
    "    template<typename TCfg>\n"
    "    std::string render(const TCfg& cfg)\n"
    "    {\n"
    "        auto stream  = std::stringstream{};\n"
    "        impl_.renderHTML(cfg, stream);\n"
    "        return stream.str();\n"
    "    }\n"
    "    template<typename TCfg>\n"
    "    void print(const TCfg& cfg)\n"
    "    {\n"
    "        impl_.renderHTML(cfg, std::cout);\n"
    "    }\n"
    "    template<typename TCfg>\n"
    "    void print(const TCfg& cfg, std::ostream& stream)\n"
    "    {\n"
    "        impl_.renderHTML(cfg, stream);\n"
    "    }\n"
    "};\n";

    return result;
}

std::string transpileToSharedLibRendererClass(const fs::path& filePath)
{
    const auto [nodeList, funcMap] = parseTemplateFile(filePath);
    auto result = std::string{R"(
    #include <string>
    #include <iostream>
    #include <sstream>

    namespace htcpp{
    template <typename TCfg>
    class ITemplate{
    public:
        virtual ~ITemplate() = default;
        virtual std::string render(const TCfg&) = 0;
        virtual std::string render(const std::string& renderFuncName, const TCfg&) = 0;
        virtual void print(const TCfg&) = 0;
        virtual void print(const std::string& renderFuncName, const TCfg&) = 0;
        virtual void print(const TCfg& cfg, std::ostream& stream) = 0;
        virtual void print(const std::string& renderFuncName, const TCfg& cfg, std::ostream& stream) = 0;
    };
    }

    #define HTCPP_CONFIG(TCfg) using Cfg = TCfg;\
    struct Template : public htcpp::ITemplate<Cfg>{\
        struct Impl{\
            void renderHTML(const Cfg& cfg, std::ostream& out);\
            void renderHTMLPart(const std::string& name, const Cfg& cfg, std::ostream& out);\
        } impl_;\
    \
        std::string render(const Cfg& cfg) override\
        {\
           auto stream  = std::stringstream{};\
           impl_.renderHTML(cfg, stream);\
          return stream.str();\
        }\
        std::string render(const std::string& renderFuncName, const Cfg& cfg) override\
        {\
           auto stream  = std::stringstream{};\
           impl_.renderHTMLPart(renderFuncName, cfg, stream);\
          return stream.str();\
        }\
    \
        void print(const Cfg& cfg) override\
        {\
            impl_.renderHTML(cfg, std::cout);\
        }\
        void print(const std::string& renderFuncName, const Cfg& cfg) override\
        {\
            impl_.renderHTMLPart(renderFuncName, cfg, std::cout);\
        }\
    \
        void print(const Cfg& cfg, std::ostream& stream) override\
        {\
            impl_.renderHTML(cfg, stream);\
        }\
        void print(const std::string& renderFuncName, const Cfg& cfg, std::ostream& stream) override\
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
    for(auto& node : nodeList)
        if (node->isGlobal())
            result += node->docRenderingCode() + "\n";

    for (const auto& nameFuncPair : funcMap){
        result += "void " + nameFuncPair.first + "(const Cfg& cfg, std::ostream& out){\n";
        result += nameFuncPair.second + "}\n";
    }

    result +=
    "void Template::Impl::renderHTML(const Cfg& cfg, std::ostream& out)\n"
    "{\n";

    for(auto& node : nodeList)
        if (!node->isGlobal())
            result += node->docRenderingCode();
    result += "\n}\n";

    result +=
    "void Template::Impl::renderHTMLPart(const std::string& name, const Cfg& cfg, std::ostream& out)\n"
    "{\n";

    for (const auto& nameFuncPair : funcMap){
        result += "if (name == \"" + nameFuncPair.first + "\")\n";
        result += nameFuncPair.first + "(cfg, out);";
    }
    result += "}\n";

    return result;
}

}
