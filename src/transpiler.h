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
    virtual ~Transpiler();
    std::string process(const fs::path& filePath);

private:
    bool readNode(StreamReader& stream);
    void parseTemplateFile(const fs::path& filePath);
    virtual std::string generateCode() const = 0;

protected:
    std::vector<std::unique_ptr<IDocumentNode>> nodeList_;
    std::vector<std::unique_ptr<GlobalStatementNode>> globalStatementList_;
    std::vector<std::unique_ptr<ProcedureNode>> procedureList_;

private:
    std::string readText_;

};

}
