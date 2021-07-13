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

struct Cfg : public cmdlime::Config{
    PARAM(input, fs::path)                               << ".htcpp file to transpile";
    PARAM(outputDir, fs::path)                           << "output directory";
    PARAM(className, std::string)()                      << "generated class name";
    FLAG(sharedLib)                                      << "generate result as shared library source files";
    FLAG(classPascalcase) << cmdlime::WithoutShortName{} << "generate class name by using .htcpp filename in PascalCase";
    FLAG(classSnakecase)  << cmdlime::WithoutShortName{} << "generate class name by using .htcpp filename in snake_case";
    FLAG(classLowercase)  << cmdlime::WithoutShortName{} << "generate class name by using .htcpp filename in lowercase";
};

int main(int argc, char**argv)
{
    auto cfg = Cfg{};
    auto cfgReader = cmdlime::ConfigReader{cfg, "hypertextcpp"};
    if (!cfgReader.readCommandLine(argc, argv))
        return cfgReader.exitCode();

    auto className = cfg.className;
    if (className.empty()){
        className = cfg.input.stem();
        if (cfg.classPascalcase)
            className = htcpp::utils::toPascalCase(className);
        else if (cfg.classSnakecase)
            className = htcpp::utils::toSnakeCase(className);
        else if (cfg.classLowercase)
            className = htcpp::utils::toLowerCase(className);
    }

    auto result = std::string{};
    auto resultFilePath = fs::path{};
    try{
        if (cfg.sharedLib){
            result = htcpp::transpileToSharedLibRendererClass(cfg.input);
            resultFilePath = cfg.outputDir / (cfg.input.stem().string() + ".cpp");
        }
        else{
            result = htcpp::transpileToSingleHeaderRendererClass(cfg.input, className);
            resultFilePath = cfg.outputDir / (cfg.input.stem().string() + ".h");
        }
    }
    catch(const htcpp::Error& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    auto stream = std::ofstream{resultFilePath};
    stream << result;

    return 0;
}
