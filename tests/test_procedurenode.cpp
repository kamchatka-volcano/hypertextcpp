#include "assert_exception.h"
#include <gtest/gtest.h>
#include <errors.h>
#include <procedurenode.h>
#include <streamreader.h>
#include <nodereader.h>

namespace{

auto test(const std::string& input, const std::string& expectedName, const std::string& expectedCode)
{
    auto stream = std::istringstream{input};
    auto streamReader = htcpp::StreamReader{stream};
    auto procedure = readProcedure(streamReader);
    auto result = procedure->renderingCode();
    EXPECT_EQ(result, expectedCode);
    EXPECT_EQ(expectedName, procedure->name());
    return procedure->takeContentProcedures();
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

TEST(ProcedureNode, WithIdAttribute)
{
    auto contentProcedures =
            test("#hello_world(){ <p htcpp-id=\"test\">Hello World! </p> }",
                 "hello_world",
                 "out << R\"_htcpp_str_( )_htcpp_str_\";out << (test());out << R\"_htcpp_str_( )_htcpp_str_\";");

    ASSERT_EQ(contentProcedures.size(), 1);
    EXPECT_EQ(contentProcedures.at(0)->name(), "test");
    EXPECT_EQ(contentProcedures.at(0)->renderingCode(), "out << R\"_htcpp_str_(<p >Hello World! </p>)_htcpp_str_\";");
}

TEST(ProcedureNode, WithNestedIdAttribute)
{
    auto contentProcedures =
            test("#hello_world(){ <div htcpp-id=\"test2\"><p htcpp-id=\"test\">Hello World! </p></div> }",
                 "hello_world",
                 "out << R\"_htcpp_str_( )_htcpp_str_\";out << (test2());out << R\"_htcpp_str_( )_htcpp_str_\";");

    ASSERT_EQ(contentProcedures.size(), 2);
    EXPECT_EQ(contentProcedures.at(0)->name(), "test");
    EXPECT_EQ(contentProcedures.at(0)->renderingCode(), "out << R\"_htcpp_str_(<p >Hello World! </p>)_htcpp_str_\";");
    EXPECT_EQ(contentProcedures.at(1)->name(), "test2");
    EXPECT_EQ(
            contentProcedures.at(1)->renderingCode(),
            "out << R\"_htcpp_str_(<div >)_htcpp_str_\";out << (test());out << R\"_htcpp_str_(</div>)_htcpp_str_\";");
}

TEST(InvalidProcedureNode, Unclosed)
{
    testError("#hello_world(){<p>Hello World! </p>",
              "[line:1, column:1] Procedure isn't closed with '}'");
}
