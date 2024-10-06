#include "assert_exception.h"
#include <gtest/gtest.h>
#include <errors.h>
#include <procedurenode.h>
#include <streamreader.h>
#include <nodereader.h>

namespace{

void test(const std::string& input, const std::string& expectedName, const std::string& expectedCode)
{
    auto stream = std::istringstream{input};
    auto streamReader = htcpp::StreamReader{stream};
    auto procedure = readProcedure(streamReader);
    auto result = procedure->renderingCode();
    EXPECT_EQ(result, expectedCode);
    EXPECT_EQ(expectedName, procedure->name());
}

void testError(const std::string& input, const std::string& expectedErrorMsg)
{
    assert_exception<htcpp::TemplateError>(
        [input]{
            auto stream = std::istringstream{input};
            auto streamReader = htcpp::StreamReader{stream};
            auto procedure = readProcedure(streamReader);
        },
        [expectedErrorMsg](const htcpp::TemplateError& e){
            EXPECT_EQ(e.what(), expectedErrorMsg);
        });
}

}

TEST(ProcedureNode, Basic)
{
    test("#hello_world(){ <p>Hello World! </p> }",
         "hello_world",
         "out << R\"_htcpp_str_( <p>Hello World! </p> )_htcpp_str_\";");
}


TEST(InvalidProcedureNode, Unclosed)
{
    testError("#hello_world(){<p>Hello World! </p>",
              "[line:1, column:1] Procedure isn't closed with '}'");
}
