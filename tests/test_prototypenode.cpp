#include "assert_exception.h"
#include <gtest/gtest.h>
#include <errors.h>
#include <prototypenode.h>
#include <tagnode.h>
#include <streamreader.h>

namespace{

void test(const std::string& input, const std::string& expected, const std::map<std::string, std::string>& macrosMap)
{
    auto stream = std::istringstream{input};
    auto streamReader = htcpp::StreamReader{stream};
    auto tag = htcpp::PrototypeNode{streamReader, macrosMap};
    auto result = tag.docRenderingCode();
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

TEST(MacroNode, Basic)
{
    test("##hello_world##",
         "hello_world(cfg, out);",
         {{"hello_world", "Hello world!"}});
}



