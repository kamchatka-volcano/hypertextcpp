#include "textnode.h"

namespace htcpp {

TextNode::TextNode(std::string value)
    : content_{std::move(value)}
{
}

std::string TextNode::renderingCode() const
{
    return "out << R\"_htcpp_str_(" + content_ + ")_htcpp_str_\";";
}

std::string TextNode::content() const
{
    return content_;
}

} //namespace htcpp
