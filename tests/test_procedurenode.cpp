#include "assert_exception.h"
#include <gtest/gtest.h>
#include <errors.h>
#include <procedurenode.h>
#include <tagnode.h>
#include <streamreader.h>
#include <nodereader.h>

namespace{

void test(const std::string& name, const std::string& input, const std::string& expected)
{
    auto stream = std::istringstream{input};
    auto streamReader = htcpp::StreamReader{stream};
    //auto tag = htcpp::ProcedureNode{name, streamReader};
    auto procedure = readProcedure(streamReader);
    auto result = procedure->docRenderingCode();
    EXPECT_EQ(result, expected);
}

//void testError(const std::string& input, const std::string& expectedErrorMsg)
//{
//    assert_exception<htcpp::TemplateError>(
//        [input]{
//            auto stream = std::istringstream{input};
//            auto streamReader = htcpp::StreamReader{stream};
//            auto nodeReader = htcpp::NodeReader{};
//            auto node = htcpp::SectionNode{streamReader, nodeReader};
//        },
//        [expectedErrorMsg](const htcpp::TemplateError& e){
//            EXPECT_EQ(e.what(), expectedErrorMsg);
//        });
//}

}

TEST(ProcedureNode, Basic)
{
    test("hello_world",
         "#hello_world(){ <p>Hello World! </p> }",
         "out << R\"( )\";out << \"<p\";out << \">\";out << R\"(Hello World! )\";out << \"</p>\";out << R\"( )\";");
}



