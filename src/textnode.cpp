#include "textnode.h"

namespace htcpp {

TextNode::TextNode(std::string value)
    : content_{std::move(value)}
{
}

std::string TextNode::renderingCode() const
{
    return "out << R\"(" + content_ + ")\";";
}

std::string TextNode::content() const
{
    return content_;
}

} //namespace htcpp
