#include "nodereader.h"
#include "tagnode.h"
#include "sectionnode.h"
#include "streamreader.h"
#include "codenode.h"
#include "prototypenode.h"

namespace htcpp{

NodeReader::NodeReader(std::map<std::string, std::string>& prototypeFuncMap)
    : prototypeFuncMap_(prototypeFuncMap)
{}

template<typename TNode, typename... TNodeArgs>
std::unique_ptr<IDocumentNode> createNodeOrPrototype(std::map<std::string, std::string>& prototypeFuncMap, TNodeArgs&&... args)
{
    auto node = std::make_unique<TNode>(std::forward<TNodeArgs>(args)...);
    if (node->extension().type() == NodeExtension::Type::Prototype){
        prototypeFuncMap[node->extension().content()] = node->docRenderingCode();
        return nullptr;
    }
    return node;
}


std::unique_ptr<IDocumentNode> NodeReader::readCommonNodes(StreamReader& stream)
{
    if (stream.atEnd())
        return nullptr;
    if (stream.peek(2) == "[[")
        return createNodeOrPrototype<SectionNode>(prototypeFuncMap_, stream, *this);
    if (stream.peek(2) == "##")
        return std::make_unique<PrototypeNode>(stream, prototypeFuncMap_);
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
        return createNodeOrPrototype<TagNode>(prototypeFuncMap_, stream, *this);
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
        return createNodeOrPrototype<TagNode>(prototypeFuncMap_, stream, *this);

    return readCommonNodes(stream);
}

std::unique_ptr<IDocumentNode> NodeReader::readSectionNode(StreamReader& stream)
{
    if (stream.atEnd())
        return nullptr;
    if (stream.peek() == "<")
        return createNodeOrPrototype<TagNode>(prototypeFuncMap_, stream, *this);

    return readCommonNodes(stream);
}

}

