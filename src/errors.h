#pragma once
#include <stdexcept>
#include <string>

namespace htcpp{

class Error : public std::runtime_error{
public:
    Error(const std::string& errorMsg)
        : std::runtime_error(errorMsg)
    {}
};

class ParsingError : public Error{
    using Error::Error;
};

class TemplateError : public Error{
    using Error::Error;
};

}
