#include "procedurenode.h"
#include "streamreader.h"
#include "errors.h"
#include "utils.h"
#include "nodereader.h"
#include <gsl/assert>

namespace htcpp{

ProcedureNode::ProcedureNode(const std::string& procedureName,
                             StreamReader& stream)
    : procedureName_(procedureName)
{
    load(stream);
}

const std::string& ProcedureNode::name() const
{
    return procedureName_;
}

void ProcedureNode::load(StreamReader& stream)
{
    auto nodePosInfo = stream.positionInfo();    
    Expects(stream.read(static_cast<int>(procedureName_.size()) + 4) == "#" + procedureName_ + "(){");

    auto readedText = std::string{};
    while (!stream.atEnd()){
        if (stream.peek() == "}"){
            stream.skip(1);
            utils::consumeReadText(readedText, contentNodes_);
            return;
        }
        auto node = readNonTagNode(stream);
        if (node){
            utils::consumeReadText(readedText, contentNodes_, node.get());
            contentNodes_.emplace_back(std::move(node));
        }
        else
            readedText += stream.read();
    }
}

std::string ProcedureNode::docTemplate()
{
    auto result = procedureName_ + "(){\n";
    for (auto& node : contentNodes_)
        result += node->docTemplate();
    result += "}\n";
    return result;
}

std::string ProcedureNode::docRenderingCode()
{
    auto result = std::string{};
    for (auto& node : contentNodes_)
        result += node->docRenderingCode();
    return result;
}

}
