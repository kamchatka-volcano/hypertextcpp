#include "textnode.h"

namespace htcpp{

TextNode::TextNode(const std::string& value)
    : value_(value)
{
}

void TextNode::load(StreamReader&)
{
}

std::string TextNode::docTemplate()
{
    return value_;
}

std::string TextNode::docRenderingCode()
{
    return "out << R\"(" + value_ + ")\";";
}

};

