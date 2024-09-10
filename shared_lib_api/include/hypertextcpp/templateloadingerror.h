#ifndef HYPERTEXTCPP_TEMPLATELOADINGERROR_H
#define HYPERTEXTCPP_TEMPLATELOADINGERROR_H

#include <stdexcept>

namespace htcpp{

class TemplateLoadingError: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

}

#endif // HYPERTEXTCPP_TEMPLATELOADINGERROR_H