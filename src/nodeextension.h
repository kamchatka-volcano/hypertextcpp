#pragma once
#include <string>

namespace htcpp{
class StreamReader;

class NodeExtension{
public:
    enum class Type{
        Conditional,
        Loop,
        None
    };
    NodeExtension();
    NodeExtension(Type type, const std::string& content);
    std::string docTemplate() const;
    bool isEmpty() const;

    Type type() const;
    const std::string& content() const;

private:
    Type type_ = Type::None;
    std::string content_;
};

NodeExtension readNodeExtension(StreamReader& stream);

}
