#pragma once
#include "generated_file_type.h"
#include <sfun/member.h>
#include <filesystem>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace htcpp{
namespace fs = std::filesystem;
class IDocumentNode;
class ITranspilerRenderer;
class GlobalStatementNode;
class ProcedureNode;
class StreamReader;

class Transpiler final{
public:
    explicit Transpiler(ITranspilerRenderer&);
    std::unordered_map<GeneratedFileType, std::string> process(const fs::path& filePath);

private:
    bool readNode(StreamReader& stream);
    void parseTemplateFile(const fs::path& filePath);
    void reset();

private:
    sfun::member<const ITranspilerRenderer&> renderer_;
    std::string readText_;
    std::vector<std::unique_ptr<IDocumentNode>> nodeList_;
    std::vector<std::unique_ptr<IDocumentNode>> globalStatementList_;
    std::vector<std::unique_ptr<ProcedureNode>> procedureList_;
};

}
