#pragma once
#include <optional>
#include <string>

namespace htcpp {
class StreamReader;

class NodeExtension {
public:
    enum class Type {
        Conditional,
        Loop
    };

    Type type() const;
    const std::string& content() const;

private:
    NodeExtension(Type type, std::string content);

private:
    Type type_;
    std::string content_;

    friend std::optional<NodeExtension> readNodeExtension(StreamReader& stream);
};

std::optional<NodeExtension> readNodeExtension(StreamReader& stream);

} //namespace htcpp
