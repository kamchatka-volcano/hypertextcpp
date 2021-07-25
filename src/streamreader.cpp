#include "streamreader.h"
#include <tuple>

namespace htcpp{

StreamReader::StreamReader(std::istream& stream, const StreamReaderPosition& startPosition)
    : stream_(stream)
    , startPosition_(startPosition)
{
}

std::string StreamReader::read(int size)
{
    auto result = std::string{};
    auto ch = char{};
    for (auto i = 0; i < size; ++i){
        if (!stream_.get(ch))
            return {};
        if (ch == '\n'){
            position_.line++;
            position_.column = 0;
        }
        else if (ch == '\t')
            position_.column += 4;
        else
            position_.column++;
        result.push_back(ch);
    }
    return result;
}

std::string StreamReader::peek(int size)
{ 
    auto result = std::string{};
    auto ch = char{};
    auto pos = stream_.tellg();
    for (auto i = 0; i < size; ++i){        
        if (!stream_.get(ch)){
            stream_.clear();
            result.clear();
            break;
        }
        result.push_back(ch);
    }    
    stream_.seekg(pos);
    return result;
}

void StreamReader::skip(int size)
{
    read(size);
}

bool StreamReader::atEnd()
{
    return peek().empty();
}

StreamReaderPosition StreamReader::position() const
{
    return {startPosition_.line + position_.line,
            startPosition_.column + position_.column};
}

}

