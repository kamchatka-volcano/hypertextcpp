#include "textnode.h"

namespace htcpp{

TextNode::TextNode(std::string value)
    : value_(std::move(value))
{
}


std::string TextNode::renderingCode()
{
    return "out << R\"(" + value_ + ")\";";
}

}

