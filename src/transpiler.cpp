#include "transpiler.h"
#include "textnode.h"
#include "procedurenode.h"
#include "streamreader.h"
#include "nodereader.h"
#include "errors.h"
#include "utils.h"
#include "codenode.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <memory>
#include <map>
#include <tuple>

namespace htcpp{
Transpiler::Transpiler() = default;
Transpiler::~Transpiler() = default;

void Transpiler::parseTemplateFile(const fs::path& filePath)
{    
    auto fileStream = std::ifstream{filePath};
    if (!fileStream.is_open())
        throw ParsingError("Can't open file " + filePath.string());

    auto stream = StreamReader{fileStream};
    auto prototypeFuncMap = std::map<std::string, std::string>{};        

    while(!stream.atEnd())
        if (!readNode(stream))
            readText_ += stream.read();

    utils::consumeReadText(readText_, nodeList_);
}

bool Transpiler::readNode(StreamReader& stream)
{
    auto node = readNonTagNode(stream);
    if (node){
        utils::consumeReadText(readText_, nodeList_, node.get());
        nodeList_.push_back(std::move(node));
        return true;
    }
    auto globalStatement = readGlobalStatement(stream);
    if (globalStatement){
        utils::consumeReadText(readText_, nodeList_);
        globalStatementList_.push_back(std::move(globalStatement));
        return true;
    }
    auto procedure = readProcedure(stream);
    if (procedure){
        utils::consumeReadText(readText_, nodeList_);
        procedureList_.push_back(std::move(procedure));
        return true;
    }
    return false;
}

std::string Transpiler::process(const fs::path& filePath)
{
    parseTemplateFile(filePath);
    return generateCode();
}

}
