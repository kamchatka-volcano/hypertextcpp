#pragma once
#include <string>
#include <istream>

namespace htcpp{

//class StreamPart{
//public:
//    StreamPart();
//    StreamPart(const std::string& value);

//    operator bool();
//    bool operator==(const std::string& str);
//    bool operator!=(const std::string& str);
//    const std::string& value() const;

//private:
//    std::string value_;
//};

class StreamReader{
public:
    StreamReader(std::istream& stream, int firstLine = 0, int firstColumn = 0);
    StreamReader(StreamReader&&) = default;
    StreamReader(const StreamReader&) = delete;
    StreamReader& operator=(const StreamReader&) = delete;
    std::string read(int size = 1);
    std::string peek(int size = 1);
    void skip(int size);
    bool atEnd();
    int line() const;
    int column() const;
    std::string positionInfo() const;

private:
    std::istream& stream_;
    int line_;
    int column_;
    int firstLine_;
    int firstColumn_;
};

}
