#include "prototypenode.h"
#include "streamreader.h"
#include "errors.h"
#include <gsl/assert>

namespace htcpp{

PrototypeNode::PrototypeNode(StreamReader& stream, const std::map<std::string, std::string>& prototypeFuncMap)
{
    load(stream, prototypeFuncMap);
}

void PrototypeNode::load(StreamReader& stream, const std::map<std::string, std::string>& prototypeFuncMap)
{
    auto nodePosInfo = stream.positionInfo();
    Expects(stream.read(2) == "##");

    while (!stream.atEnd()){
        if (stream.peek(2) == "##"){
            stream.skip(2);
            auto funcIt = prototypeFuncMap.find(funcName_);
            if (funcIt == prototypeFuncMap.end())
                throw TemplateError{nodePosInfo + " prototype " + funcName_ + " isn't defined"};
            return;
        }
        funcName_ += stream.read();
    }
}

std::string PrototypeNode::docTemplate()
{
    return "##" + funcName_ + "##";
}

std::string PrototypeNode::docRenderingCode()
{
    return funcName_ + "(cfg, out);";
}

}
