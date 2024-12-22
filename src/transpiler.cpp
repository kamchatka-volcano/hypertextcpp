#include "transpiler.h"
#include "errors.h"
#include "itranspiler_renderer.h"
#include "node_utils.h"
#include "nodereader.h"
#include "procedurenode.h"
#include "streamreader.h"
#include "utils.h"
#include <range/v3/range/conversion.hpp>
#include <range/v3/view.hpp>
#include <gsl/pointers>
#include <fstream>
#include <map>
#include <vector>

namespace htcpp {

Transpiler::Transpiler(ITranspilerRenderer& renderer)
    : renderer_{renderer}
{
}

void Transpiler::parseTemplateFile(const fs::path& filePath)
{
    auto fileStream = std::ifstream{filePath, std::ios_base::binary};
    if (!fileStream.is_open())
        throw ParsingError("Can't open file " + filePath.string());

    auto stream = StreamReader{fileStream};
    auto prototypeFuncMap = std::map<std::string, std::string>{};

    while (!stream.atEnd())
        if (!readNode(stream))
            readText_ += stream.read();

    utils::consumeReadText(readText_, nodeList_);
}

bool Transpiler::readNode(StreamReader& stream)
{
    auto node = readNonTagNode(stream);
    if (node) {
        utils::consumeReadText(readText_, nodeList_, node.get());
        nodeList_.push_back(std::move(node));
        return true;
    }
    auto globalStatement = readGlobalStatement(stream);
    if (globalStatement) {
        utils::consumeReadText(readText_, nodeList_);
        globalStatementList_.push_back(std::move(globalStatement));
        return true;
    }
    auto procedure = readProcedure(stream);
    if (procedure) {
        utils::consumeReadText(readText_, nodeList_);
        procedureList_.push_back(std::move(procedure));
        return true;
    }
    return false;
}

namespace {
std::vector<gsl::not_null<IDocumentNodeRenderer*>> nodesToNodeRenderers(
        const std::vector<std::unique_ptr<IDocumentNode>>& nodes)
{
    const auto toRenderer = [](const std::unique_ptr<IDocumentNode>& node)
    {
        Expects(node->interface<IDocumentNodeRenderer>().has_value());
        return gsl::not_null{ &node->interface<IDocumentNodeRenderer>().value()};
    };
    return nodes | ranges::views::transform(toRenderer) | ranges::to<std::vector>();
}

} //namespace

std::unordered_map<GeneratedFileType, std::string> Transpiler::process(const fs::path& filePath)
{
    reset();
    parseTemplateFile(filePath);
    return renderer_.get().generateCode(
            nodesToNodeRenderers(globalStatementList_),
            procedureList_,
            nodesToNodeRenderers(optimizeNodes(flattenNodes(std::move(nodeList_)))));
}

void Transpiler::reset()
{
    nodeList_.clear();
    globalStatementList_.clear();
    procedureList_.clear();
    readText_.clear();
}

} //namespace htcpp
