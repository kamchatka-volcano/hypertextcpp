#include "singleheadertranspiler.h"
#include "sharedlibtranspiler.h"
#include "errors.h"
#include "nameutils.h"
#include <cmdlime/config.h>
#include <cmdlime/commandlinereader.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <memory>

namespace fs = std::filesystem;

namespace {
struct Cfg : public cmdlime::Config{
    CMDLIME_ARG(input, fs::path)                                 << ".htcpp file to transpile";
    CMDLIME_PARAM(output, fs::path)()                            << "output c++ file path\n(if empty, current working directory is used)";
    CMDLIME_PARAM(className, std::string)()                      << "generated class name";
    CMDLIME_FLAG(sharedLib)                                      << "generate result as shared library source files";
    CMDLIME_FLAG(classPascalcase) << cmdlime::WithoutShortName{} << "generate class name by using .htcpp filename in PascalCase";
    CMDLIME_FLAG(classSnakecase)  << cmdlime::WithoutShortName{} << "generate class name by using .htcpp filename in snake_case";
    CMDLIME_FLAG(classLowercase)  << cmdlime::WithoutShortName{} << "generate class name by using .htcpp filename in lowercase";
};
fs::path getOutputFilePath(const Cfg& cfg);
std::string getClassName(const Cfg& cfg);
std::unique_ptr<htcpp::Transpiler> makeTranspiler(const Cfg& cfg);
}

int run(const Cfg& cfg)
{
    auto result = std::string{};
    auto transpiler = makeTranspiler(cfg);
    try{
      result = transpiler->process(cfg.input);
      auto stream = std::ofstream{getOutputFilePath(cfg)};
      stream << result;
    }
    catch(const htcpp::Error& e)
    {
      std::cerr << e.what() << std::endl;
      return 1;
    }
    catch(const std::exception& e)
    {
      std::cerr << "Unknown critical error:" << e.what() << std::endl;
      return 1;
    }
    return 0;
}

int main(int argc, char**argv)
{
    return cmdlime::CommandLineReader{"hypertextcpp"}.exec<Cfg>(argc, argv, run);
}

namespace {
fs::path getOutputFilePath(const Cfg& cfg)
{
    auto path = fs::current_path();
    if (!cfg.output.empty()){
        if (cfg.output.is_absolute())
            path = cfg.output;
        else
            path /= cfg.output;
    }
    else{
        if (cfg.sharedLib)
            path /= (cfg.input.stem().string() + ".cpp");
        else
            path /= (cfg.input.stem().string() + ".h");
    }
    return path;
}

std::string getClassName(const Cfg& cfg)
{
    auto result = cfg.className;
    if (result.empty()){
        result = cfg.input.stem();
        if (cfg.classPascalcase)
            result = htcpp::utils::toPascalCase(result);
        if (cfg.classSnakecase)
            result = htcpp::utils::toSnakeCase(result);
        if (cfg.classLowercase)
            result = htcpp::utils::toLowerCase(result);
    }
    return result;
}

std::unique_ptr<htcpp::Transpiler> makeTranspiler(const Cfg& cfg)
{
    if (cfg.sharedLib)
        return std::make_unique<htcpp::SharedLibTranspiler>();
    else
        return std::make_unique<htcpp::SingleHeaderTranspiler>(getClassName(cfg));
}

}
