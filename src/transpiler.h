#pragma once
#include <filesystem>
#include <string>
#include <vector>
#include <memory>

namespace htcpp{
namespace fs = std::filesystem;
class IDocumentNode;
class GlobalStatementNode;
class ProcedureNode;
class StreamReader;

class Transpiler{
public:
    Transpiler();
    ~Transpiler();
    std::string transpileToSingleHeaderRendererClass(const fs::path& filePath,
                                                    const std::string& className);
    std::string transpileToSharedLibRendererClass(const fs::path& filePath);

private:
    bool readNode(StreamReader& stream);
    void parseTemplateFile(const fs::path& filePath);

private:
    std::string readedText_;
    std::vector<std::unique_ptr<IDocumentNode>> nodeList_;
    std::vector<std::unique_ptr<GlobalStatementNode>> globalStatementList_;
    std::vector<std::unique_ptr<ProcedureNode>> procedureList_;
};
}
