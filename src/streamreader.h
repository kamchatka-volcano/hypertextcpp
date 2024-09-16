#pragma once
#include "streamreaderposition.h"
#include <sfun/member.h>
#include <string>
#include <istream>

namespace htcpp{

class StreamReader{
public:
    explicit StreamReader(std::istream& stream, const StreamReaderPosition& startPosition = StreamReaderPosition{1, 1});
    std::string read(int size = 1);
    std::string peek(int size = 1);
    void skip(int size);
    bool atEnd();
    StreamReaderPosition position() const;

private:
    sfun::member<std::istream&> stream_;
    StreamReaderPosition position_;
    StreamReaderPosition startPosition_;
};

}
