#include "errors.h"
#include "nameutils.h"
#include "shared_lib_transpiler_renderer.h"
#include "single_header_transpiler_renderer.h"
#include "transpiler.h"
#include <cmdlime/commandlinereader.h>
#include <cmdlime/config.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

namespace fs = std::filesystem;

namespace {
struct Cfg : public cmdlime::Config {
    CMDLIME_ARG(input, fs::path) << ".htcpp file to transpile";
    CMDLIME_PARAM(output, fs::path)() << "output c++ file path\n(if empty, current working directory is used)";
    CMDLIME_PARAM(className, std::string)() << "generated class name";
    CMDLIME_FLAG(sharedLib) << "generate result as shared library source files";
    CMDLIME_FLAG(classPascalcase) << cmdlime::WithoutShortName{}
                                  << "generate class name by using .htcpp filename in PascalCase";
    CMDLIME_FLAG(classSnakecase) << cmdlime::WithoutShortName{}
                                 << "generate class name by using .htcpp filename in snake_case";
    CMDLIME_FLAG(classLowercase) << cmdlime::WithoutShortName{}
                                 << "generate class name by using .htcpp filename in lowercase";
};
fs::path getOutputFilePath(const Cfg& cfg, htcpp::GeneratedFileType fileType);
std::string getClassName(const Cfg& cfg);
std::unique_ptr<htcpp::ITranspilerRenderer> makeTranspilerRenderer(const Cfg& cfg);
} //namespace

int run(const Cfg& cfg)
{
    auto transpilerRenderer = makeTranspilerRenderer(cfg);
    auto transpiler = htcpp::Transpiler{*transpilerRenderer};
    try {
        const auto result = transpiler.process(cfg.input);
        for (const auto& [fileType, code] : result) {
            auto stream = std::ofstream{getOutputFilePath(cfg, fileType)};
            stream << code;
        }
    }
    catch (const htcpp::Error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Unknown critical error:" << e.what() << std::endl;
        return 1;
    }
    return 0;
}

int main(int argc, char** argv)
{
    return cmdlime::CommandLineReader{"hypertextcpp"}.exec<Cfg>(argc, argv, run);
}

namespace {
fs::path getOutputFilePath(const Cfg& cfg, htcpp::GeneratedFileType fileType)
{
    auto path = fs::current_path();
    if (!cfg.output.empty()) {
        if (cfg.output.is_absolute())
            path = cfg.output;
        else
            path /= cfg.output;
    }
    else {
        if (fileType == htcpp::GeneratedFileType::Source)
            path /= (cfg.input.stem().string() + ".cpp");
        else if (fileType == htcpp::GeneratedFileType::Header)
            path /= (cfg.input.stem().string() + ".h");
    }
    return path;
}

std::string getClassName(const Cfg& cfg)
{
    auto result = cfg.className;
    if (result.empty()) {
        result = cfg.input.stem().string();
        if (cfg.classPascalcase)
            result = htcpp::utils::toPascalCase(result);
        if (cfg.classSnakecase)
            result = htcpp::utils::toSnakeCase(result);
        if (cfg.classLowercase)
            result = htcpp::utils::toLowerCase(result);
    }
    return result;
}

std::unique_ptr<htcpp::ITranspilerRenderer> makeTranspilerRenderer(const Cfg& cfg)
{
    if (cfg.sharedLib)
        return std::make_unique<htcpp::SharedLibTranspilerRenderer>();
    else
        return std::make_unique<htcpp::SingleHeaderTranspilerRenderer>(getClassName(cfg));
}

} //namespace
