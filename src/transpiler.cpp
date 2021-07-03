#include "transpiler.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <memory>
#include "textnode.h"
#include "string_utils.h"
#include "streamreader.h"
#include "nodereader.h"
#include "errors.h"

namespace htcpp{
namespace {
std::vector<std::unique_ptr<IDocumentNode>> parseTemplateFile(const fs::path& filePath)
{
    auto nodeList = std::vector<std::unique_ptr<IDocumentNode>>{};
    auto fileStream = std::ifstream{filePath};
    if (!fileStream.is_open())
        throw ParsingError("Can't open file " + filePath.string());

    auto stream = StreamReader{fileStream};
    auto nodeReader = NodeReader{};
    auto readedText = std::string{};

    while(!stream.atEnd()){
        auto node = nodeReader.readTopLevelNode(stream);
        if (node){
            if (!readedText.empty()){
                nodeList.emplace_back(std::make_unique<TextNode>(readedText));
                readedText.clear();
            }            
            nodeList.push_back(std::move(node));
        }
        else{
            readedText += stream.read();
        }
    }
    if (!readedText.empty()){
        nodeList.emplace_back(std::make_unique<TextNode>(readedText));
        readedText.clear();
    }
    return nodeList;
}
}

std::string transpileToSingleHeaderRendererClass(const fs::path& filePath, const std::string& className)
{
    const auto nodeList = parseTemplateFile(filePath);
    auto result = std::string{};
    result +=
    "#include <iostream>\n"
    "#include <sstream>\n";
    for(auto& node : nodeList)
        if (node->isGlobal())
            result += node->docRenderingCode() + "\n";

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
    const auto nodeList = parseTemplateFile(filePath);
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
        virtual void print(const TCfg&) = 0;
    };
    }

    #define HTCPP_CONFIG(TCfg) using Cfg = TCfg;\
    struct Template : public htcpp::ITemplate<Cfg>{\
        struct Impl{\
            void renderHTML(const Cfg& cfg, std::ostream& out);\
        } impl_;\
    \
        std::string render(const Cfg& cfg)\
        {\
           auto stream  = std::stringstream{};\
           impl_.render(cfg, stream);\
          return stream.str();\
        }\
    \
        void print(const Cfg& cfg)\
        {\
            impl_.render(cfg, std::cout);\
        }\
    \
        void print(const Cfg& cfg, std::ostream& stream)\
        {\
            impl_.render(cfg, stream);\
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

    result +=
    "void Template::Impl::renderHTML(const Cfg& cfg, std::ostream& out)\n"
    "{\n";

    for(auto& node : nodeList)
        if (!node->isGlobal())
            result += node->docRenderingCode();
    result += "\n}\n";

    return result;
}

}
