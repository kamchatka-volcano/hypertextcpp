#include "macronode.h"
#include "streamreader.h"
#include "errors.h"
#include <gsl/assert>

namespace htcpp{

MacroNode::MacroNode(StreamReader& stream, const std::map<std::string, std::string>& macrosMap)
{
    load(stream, macrosMap);
}

void MacroNode::load(StreamReader& stream, const std::map<std::string, std::string>& macrosMap)
{
    auto nodePosInfo = stream.positionInfo();
    Expects(stream.read(2) == "##");

    while (!stream.atEnd()){
        if (stream.peek(2) == "##"){
            stream.skip(2);
            auto macroIt = macrosMap.find(name_);
            if (macroIt == macrosMap.end())
                throw TemplateError{nodePosInfo + " macro " + name_ + " isn't defined"};
            content_ = macroIt->second;
            return;
        }
        name_ += stream.read();
    }
}

std::string MacroNode::docTemplate()
{
    return "##" + name_ + "##";
}

std::string MacroNode::docRenderingCode()
{
    return content_;
}

}
