#include "attribute_node.h"
#include "errors.h"
#include "streamreader.h"
#include <gsl/assert>

namespace htcpp {

AttributeNode::AttributeNode(std::string name, StreamReader& stream)
    : name_(std::move(name))
{
    load(stream);
}

std::string_view AttributeNode::name() const
{
    return name_;
}

std::string_view AttributeNode::value() const
{
    return value_;
}

void AttributeNode::load(StreamReader& stream)
{
    Expects(stream.read(name_.size()) == name_);
    stream.skipWhitespace();
    if (stream.peek() != "=")
        throw TemplateError{stream.position(), "Attribute '" + name_ + "'s value is missing"};

    stream.skip(1);
    stream.skipWhitespace();
    if (stream.peek() != "\"")
        throw TemplateError{stream.position(), "Attribute '" + name_ + "'s value is missing"};

    stream.skip(1);
    while (!stream.atEnd()) {
        auto res = stream.read();
        if (res == "\"")
            return;
        value_ += res;
    }
}

} //namespace htcpp