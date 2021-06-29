#ifndef ERRORS_H
#define ERRORS_H
#include <stdexcept>
#include <string>

namespace htcpp{

class Error : public std::runtime_error{
public:
    Error(const std::string& errorMsg)
        : std::runtime_error(errorMsg)
    {}
};

class TemplateError : public Error{
    using Error::Error;
};

class ParsingError : public Error{
    using Error::Error;
};

}
#endif // ERRORS_H
