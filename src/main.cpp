#include "errors.h"
#include "header_and_source_transpiler_renderer.h"
#include "nameutils.h"
#include "shared_lib_transpiler_renderer.h"
#include "single_header_transpiler_renderer.h"
#include "transpiler.h"
#include <cmdlime/commandlinereader.h>
#include <cmdlime/config.h>
#include <cmdlime/errors.h>
#include <sfun/contract.h>
#include <sfun/functional.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

namespace fs = std::filesystem;

namespace {
struct HeaderOnlyResultModeCfg : public cmdlime::Config {
    CMDLIME_ARG(input, fs::path) << ".htcpp file to transpile";
    CMDLIME_PARAM(outputDir, fs::path)() << "output dir\n(if empty, current working directory is used)";
    CMDLIME_PARAM(className, std::string)() << "generated class name\n(if empty, input file name is used)";
};

struct SharedLibrarySourceResultModeCfg : public cmdlime::Config {
    CMDLIME_ARG(input, fs::path) << ".htcpp file to transpile";
    CMDLIME_PARAM(outputDir, fs::path)() << "output dir\n(if empty, current working directory is used)";
};

struct HeaderAndSourceResultModeCfg : public cmdlime::Config {
    CMDLIME_ARG(input, fs::path) << ".htcpp file to transpile";
    CMDLIME_PARAM(outputDir, fs::path)() << "output dir\n(if empty, current working directory is used)";
    CMDLIME_PARAM(className, std::string)() << "generated class name\n(if empty, input file name is used)";
    CMDLIME_PARAM(configClassName, std::string) << "config class name";
};

struct Cfg : public cmdlime::Config {
    CMDLIME_COMMAND(generateHeaderOnly, HeaderOnlyResultModeCfg) << "generate header only file";
    CMDLIME_COMMAND(generateSharedLibrarySource, SharedLibrarySourceResultModeCfg) << "generate shared library source file";
    CMDLIME_COMMAND(generateHeaderAndSource, HeaderAndSourceResultModeCfg) << "generate header and source files";
};

using CommandCfg =
        std::variant<HeaderOnlyResultModeCfg, SharedLibrarySourceResultModeCfg, HeaderAndSourceResultModeCfg>;

std::unique_ptr<htcpp::ITranspilerRenderer> makeTranspilerRenderer(const CommandCfg& cfg);
std::variant<HeaderOnlyResultModeCfg, SharedLibrarySourceResultModeCfg, HeaderAndSourceResultModeCfg> getCommand(
        const Cfg& cfg);

std::string getClassName(const auto& commandCfg)
{
    const auto& className = commandCfg.className;
    if (className.empty())
        return commandCfg.input.stem().string();

    return className;
}

fs::path getOutputFilePath(const CommandCfg& command, htcpp::GeneratedFileType fileType);

} //namespace

namespace cmdlime {
template<>
struct PostProcessor<Cfg> {
    void operator()(Cfg& cfg)
    {
        if (!cfg.generateHeaderOnly.has_value() && !cfg.generateHeaderAndSource.has_value() &&
            !cfg.generateSharedLibrarySource.has_value())
            throw cmdlime::ValidationError{"At least one command must be specified"};
    }
};
} //namespace cmdlime

int run(const Cfg& cfg)
{
    const auto command = getCommand(cfg);
    auto transpilerRenderer = makeTranspilerRenderer(command);
    auto transpiler = htcpp::Transpiler{*transpilerRenderer};
    try {
        const auto input = std::visit(
                [](const auto& cfg)
                {
                    return cfg.input;
                },
                command);
        const auto result = transpiler.process(input);
        for (const auto& [fileType, code] : result) {
            auto stream = std::ofstream{getOutputFilePath(command, fileType)};
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
    return cmdlime::CommandLineReader<cmdlime::Format::Simple>{"hypertextcpp"}.exec<Cfg>(argc, argv, run);
}

namespace {

fs::path getOutputFilePath(const CommandCfg& command, htcpp::GeneratedFileType fileType)
{
    const auto inputFile = std::visit(
            [](const auto& cfg)
            {
                return cfg.input;
            },
            command);
    const auto outputDir = std::visit(
            [](const auto& cfg)
            {
                return cfg.outputDir;
            },
            command);

    auto path = fs::current_path();
    if (!outputDir.empty()) {
        if (outputDir.is_absolute())
            path = outputDir;
        else
            path /= outputDir;
    }

    if (fileType == htcpp::GeneratedFileType::Source)
        path /= (inputFile.stem().string() + ".cpp");
    else if (fileType == htcpp::GeneratedFileType::Header)
        path /= (inputFile.stem().string() + ".h");

    return path;
}

std::unique_ptr<htcpp::ITranspilerRenderer> makeTranspilerRenderer(const CommandCfg& cfg)
{
    return std::visit(
            sfun::overloaded{
                    [](const HeaderOnlyResultModeCfg& commandCfg) -> std::unique_ptr<htcpp::ITranspilerRenderer>
                    {
                        return std::make_unique<htcpp::SingleHeaderTranspilerRenderer>(getClassName(commandCfg));
                    },
                    [](const SharedLibrarySourceResultModeCfg&) -> std::unique_ptr<htcpp::ITranspilerRenderer>
                    {
                        return std::make_unique<htcpp::SharedLibTranspilerRenderer>();
                    },
                    [](const HeaderAndSourceResultModeCfg& commandCfg) -> std::unique_ptr<htcpp::ITranspilerRenderer>
                    {
                        return std::make_unique<htcpp::HeaderAndSourceTranspilerRenderer>(
                                getClassName(commandCfg),
                                getOutputFilePath(commandCfg, htcpp::GeneratedFileType::Header).filename().string(),
                                commandCfg.configClassName);
                    }},
            cfg);
}

std::variant<HeaderOnlyResultModeCfg, SharedLibrarySourceResultModeCfg, HeaderAndSourceResultModeCfg> getCommand(
        const Cfg& cfg)
{
    if (cfg.generateHeaderOnly.has_value())
        return cfg.generateHeaderOnly.value();
    if (cfg.generateSharedLibrarySource.has_value())
        return cfg.generateSharedLibrarySource.value();
    if (cfg.generateHeaderAndSource.has_value())
        return cfg.generateHeaderAndSource.value();

    sfun::unreachable();
}

} //namespace
