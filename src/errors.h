#pragma once
#include "streamreaderposition.h"
#include <stdexcept>
#include <string>

namespace htcpp{

class Error : public std::runtime_error{
public:
    explicit Error(const std::string& errorMsg)
        : std::runtime_error(errorMsg)
    {}
};

class ParsingError : public Error{
    using Error::Error;
};

class TemplateError : public Error{
public:
    TemplateError(const StreamReaderPosition& errorPosition, const std::string& errorMsg)
        : Error("[line:" + std::to_string(errorPosition.line) +
                ", column:" + std::to_string(errorPosition.column) + "] " + errorMsg)
    {
    }
    using Error::Error;
};

}
