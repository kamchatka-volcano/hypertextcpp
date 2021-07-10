#pragma once
#include <stdexcept>

namespace htcpp{

class TemplateLoadingError: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

}
