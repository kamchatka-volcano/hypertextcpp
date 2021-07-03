#include "nodereader.h"
#include "tagnode.h"
#include "sectionnode.h"
#include "streamreader.h"
#include "codenode.h"

namespace htcpp{

namespace{
std::unique_ptr<IDocumentNode> readCommonNodes(StreamReader& stream)
{
    if (stream.atEnd())
        return nullptr;
    if (stream.peek(2) == "[[")
        return std::make_unique<SectionNode>();
    if (stream.peek(2) == "$(")
        return std::make_unique<ExpressionNode>();
    if (stream.peek(2) == "${")
        return std::make_unique<RenderStatementNode>();
    return nullptr;

}
}

std::unique_ptr<IDocumentNode> readTopLevelNode(StreamReader& stream)
{
    if (stream.atEnd())
        return nullptr;
    if (stream.peek() == "<")
        return std::make_unique<TagNode>();
    if (stream.peek(2) == "#{")
        return std::make_unique<GlobalStatementNode>();
    return readCommonNodes(stream);
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
        return std::make_unique<TagNode>();

    return readCommonNodes(stream);
}

std::unique_ptr<IDocumentNode> readSectionNode(StreamReader& stream)
{
    if (stream.atEnd())
        return nullptr;
    if (stream.peek() == "<")
        return std::make_unique<TagNode>();

    return readCommonNodes(stream);
}

}

