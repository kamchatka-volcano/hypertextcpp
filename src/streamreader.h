#pragma once
#include "streamreaderposition.h"
#include <string>
#include <istream>

namespace htcpp{

class StreamReader{
public:
    explicit StreamReader(std::istream& stream, const StreamReaderPosition& startPosition = StreamReaderPosition{1, 1});
    StreamReader(StreamReader&&) = default;
    StreamReader(const StreamReader&) = delete;
    StreamReader& operator=(const StreamReader&) = delete;
    std::string read(int size = 1);
    std::string peek(int size = 1);
    void skip(int size);
    bool atEnd();
    StreamReaderPosition position() const;

private:
    std::istream& stream_;
    StreamReaderPosition position_;
    StreamReaderPosition startPosition_;
};

}
