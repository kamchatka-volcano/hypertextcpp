#include "textnode.h"

namespace htcpp{

TextNode::TextNode(std::string value)
    : value_(std::move(value))
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

