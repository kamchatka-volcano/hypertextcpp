#include "nodereader.h"
#include "tagnode.h"
#include "sectionnode.h"
#include "streamreader.h"
#include "codenode.h"
#include "macronode.h"

namespace htcpp{

template<typename TNode, typename... TNodeArgs>
std::unique_ptr<IDocumentNode> createNodeOrMacro(std::map<std::string, std::string>& macrosMap, TNodeArgs&&... args)
{
    auto node = std::make_unique<TNode>(std::forward<TNodeArgs>(args)...);
    if (node->extension().type() == NodeExtension::Type::Macro){
        macrosMap[node->extension().content()] = node->docRenderingCode();
        return nullptr;
    }
    return node;
}


std::unique_ptr<IDocumentNode> NodeReader::readCommonNodes(StreamReader& stream)
{
    if (stream.atEnd())
        return nullptr;
    if (stream.peek(2) == "[[")
        return createNodeOrMacro<SectionNode>(macrosMap_, stream, *this);
    if (stream.peek(2) == "##")
        return std::make_unique<MacroNode>(stream, macrosMap_);
    if (stream.peek(2) == "$(")
        return std::make_unique<ExpressionNode>(stream);
    if (stream.peek(2) == "${")
        return std::make_unique<RenderStatementNode>(stream);
    return nullptr;
}

std::unique_ptr<IDocumentNode> NodeReader::readTopLevelNode(StreamReader& stream)
{
    if (stream.atEnd())
        return nullptr;
    if (stream.peek() == "<")
        return createNodeOrMacro<TagNode>(macrosMap_, stream, *this);
    if (stream.peek(2) == "#{")
        return std::make_unique<GlobalStatementNode>(stream);
    return readCommonNodes(stream);
}

std::unique_ptr<IDocumentNode> NodeReader::readTagAttributeNode(StreamReader& stream)
{
    return readCommonNodes(stream);
}

std::unique_ptr<IDocumentNode> NodeReader::readTagContentNode(StreamReader& stream)
{
    if (stream.atEnd())
        return nullptr;
    if (stream.peek(2) != "</" && stream.peek() == "<")
        return createNodeOrMacro<TagNode>(macrosMap_, stream, *this);

    return readCommonNodes(stream);
}

std::unique_ptr<IDocumentNode> NodeReader::readSectionNode(StreamReader& stream)
{
    if (stream.atEnd())
        return nullptr;
    if (stream.peek() == "<")
        return createNodeOrMacro<TagNode>(macrosMap_, stream, *this);

    return readCommonNodes(stream);
}

}

