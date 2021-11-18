#include "procedurenode.h"
#include "streamreader.h"
#include "errors.h"
#include "utils.h"
#include "nodereader.h"
#include <gsl/gsl>


namespace htcpp{

ProcedureNode::ProcedureNode(std::string procedureName,
                             StreamReader& stream)
    : procedureName_(std::move(procedureName))
{
    Expects(!procedureName_.empty());
    load(stream);
}

const std::string& ProcedureNode::name() const
{
    return procedureName_;
}

void ProcedureNode::load(StreamReader& stream)
{
    auto nodePos = stream.position();
    auto openSeq = stream.read(static_cast<int>(procedureName_.size()) + 4);
    Expects(openSeq == "#" + procedureName_ + "(){");

    auto readText = std::string{};
    while (!stream.atEnd()){
        if (stream.peek() == "}"){
            stream.skip(1);
            utils::consumeReadText(readText, contentNodes_);
            return;
        }
        auto node = readNonTagNode(stream);
        if (node){
            utils::consumeReadText(readText, contentNodes_, node.get());
            contentNodes_.emplace_back(std::move(node));
        }
        else
            readText += stream.read();
    }
    throw TemplateError{nodePos, "Procedure isn't closed with '}'"};
}

std::string ProcedureNode::renderingCode()
{
    auto result = std::string{};
    for (auto& node : contentNodes_)
        result += node->renderingCode();
    return result;
}

}
