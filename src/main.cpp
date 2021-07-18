#include "transpiler.h"
#include "errors.h"
#include "nameutils.h"
#include <cmdlime/config.h>
#include <cmdlime/configreader.h>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <fstream>

namespace fs = std::filesystem;

namespace {
struct Cfg : public cmdlime::Config{
    ARG(input, fs::path)                                 << ".htcpp file to transpile";
    PARAM(output, fs::path)()                            << "output c++ file path";
    PARAM(className, std::string)()                      << "generated class name";
    FLAG(sharedLib)                                      << "generate result as shared library source files";
    FLAG(classPascalcase) << cmdlime::WithoutShortName{} << "generate class name by using .htcpp filename in PascalCase";
    FLAG(classSnakecase)  << cmdlime::WithoutShortName{} << "generate class name by using .htcpp filename in snake_case";
    FLAG(classLowercase)  << cmdlime::WithoutShortName{} << "generate class name by using .htcpp filename in lowercase";
};
fs::path getOutputFilePath(const Cfg& cfg);
std::string getClassName(const Cfg& cfg);
}

int main(int argc, char**argv)
{
    auto cfg = Cfg{};
    auto cfgReader = cmdlime::ConfigReader{cfg, "hypertextcpp"};
    if (!cfgReader.readCommandLine(argc, argv))
        return cfgReader.exitCode();

    auto result = std::string{};
    try{
        if (cfg.sharedLib)
            result = htcpp::transpileToSharedLibRendererClass(cfg.input);
        else
            result = htcpp::transpileToSingleHeaderRendererClass(cfg.input, getClassName(cfg));
    }
    catch(const htcpp::Error& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    auto stream = std::ofstream{getOutputFilePath(cfg)};
    stream << result;

    return 0;
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
        else if (cfg.classSnakecase)
            result = htcpp::utils::toSnakeCase(result);
        else if (cfg.classLowercase)
            result = htcpp::utils::toLowerCase(result);
    }
    return result;
}

}
