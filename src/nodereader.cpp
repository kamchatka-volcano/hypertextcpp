#include "nodereader.h"
#include "tagnode.h"
#include "sectionnode.h"
#include "streamreader.h"
#include "codenode.h"
#include "procedurenode.h"

namespace htcpp{

namespace{
std::unique_ptr<IDocumentNode> readCommonNodes(StreamReader& stream)
{
    if (stream.atEnd())
        return nullptr;
    if (stream.peek(2) == "[[")
        return std::make_unique<SectionNode>(stream);
    if (stream.peek(2) == "$(")
        return std::make_unique<ExpressionNode>(stream);
    if (stream.peek(2) == "${")
        return std::make_unique<StatementNode>(stream);
    return nullptr;
}
}

std::unique_ptr<IDocumentNode> readTagAttributeNode(StreamReader& stream)
{
    return readCommonNodes(stream);
}

std::unique_ptr<IDocumentNode> readTagContentNode(StreamReader& stream)
{
    if (stream.atEnd())
        return nullptr;
    if (stream.peek(2) != "</" && stream.peek() == "<")
        return std::make_unique<TagNode>(stream);

    return readCommonNodes(stream);
}

std::unique_ptr<IDocumentNode> readNonTagNode(StreamReader& stream)
{
    if (stream.atEnd())
        return nullptr;
    if (stream.peek() == "<")
        return std::make_unique<TagNode>(stream);

    return readCommonNodes(stream);
}

std::unique_ptr<IDocumentNode> readGlobalStatement(StreamReader& stream)
{
    if (stream.atEnd())
        return nullptr;
    if (stream.peek(2) == "#{")
        return std::make_unique<GlobalStatementNode>(stream);
    return nullptr;
}

std::unique_ptr<ProcedureNode> readProcedure(StreamReader& stream)
{
    if (stream.atEnd())
        return nullptr;
    if (stream.peek() == "#"){
        for(auto i = 2;; ++i){
            auto text = stream.peek(i);
            if (text.empty())
                return nullptr;
            if (std::isspace(text.back()))
                return nullptr;
            if (text.back() == '(' && text.size() > 1){
                auto name = std::string {text.begin() + 1, text.end() - 1};
                if (stream.peek(static_cast<int>(name.size()) + 4) ==  "#" + name + "(){")
                    return std::make_unique<ProcedureNode>(name, stream);
            }
        }
    }
    return nullptr;
}

}

