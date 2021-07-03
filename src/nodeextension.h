#pragma once
#include <string>

namespace htcpp{
class StreamReader;

class NodeExtension{
public:
    enum class Type{
        Conditional,
        Loop,
        Macro,
        None
    };
    NodeExtension();    
    std::string docTemplate() const;
    bool isEmpty() const;

    Type type() const;
    const std::string& content() const;

private:
    NodeExtension(Type type,
                  const std::string& content);

private:
    Type type_ = Type::None;
    std::string content_;

friend NodeExtension readNodeExtension(StreamReader& stream);
};

NodeExtension readNodeExtension(StreamReader& stream);

}
