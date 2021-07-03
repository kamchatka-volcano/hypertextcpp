#include "streamreader.h"
#include <tuple>
#include <sstream>
#include <istream>

namespace htcpp{

StreamReader::StreamReader(std::istream& stream, int firstLine, int firstColumn)
    : stream_(stream)
    , line_(1)
    , column_(0)
    , firstLine_(firstLine)
    , firstColumn_(firstColumn)
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
            line_++;
            column_ = 0;
            firstColumn_ = 0;
        }
        else if (ch == '\t')
            column_+= 4;
        else
            column_++;
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

int StreamReader::line() const
{
    return line_;
}

int StreamReader::column() const
{
    return column_;
}

std::string StreamReader::positionInfo() const
{
    auto stream = std::stringstream{};
    stream << "[line:"<< firstLine_ + line_ << ", column:" << firstColumn_ + column_<< "]";
    return stream.str();
}

}

