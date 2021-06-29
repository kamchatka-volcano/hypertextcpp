#include "assert_exception.h"
#include <gtest/gtest.h>
#include <errors.h>
#include <sectionnode.h>
#include <tagnode.h>
#include <streamreader.h>

namespace{

void test(const std::string& input, const std::string& expected)
{
    auto stream = std::istringstream{input};
    auto tag = htcpp::SectionNode{};
    auto streamReader = htcpp::StreamReader{stream};
    tag.load(streamReader);
    auto result = tag.docTemplate();
    EXPECT_EQ(result, expected);
}

void testError(const std::string& input, const std::string& expectedErrorMsg)
{
    assert_exception<htcpp::ParsingError>(
        [input]{
            auto stream = std::istringstream{input};
            auto token = htcpp::SectionNode{};
            auto streamReader = htcpp::StreamReader{stream};
            token.load(streamReader);
        },
        [expectedErrorMsg](const htcpp::ParsingError& e){
            EXPECT_EQ(e.what(), expectedErrorMsg);
        });
}

}

TEST(SectionNode, Basic)
{
    test("[[ Hello world! ]]",
         "[[ Hello world! ]]");
}

TEST(SectionNode, BasicWithConditionalExtension)
{
    test("[[?(isVisible) Hello world! ]]",
         "[[?(isVisible) Hello world! ]]");
}

TEST(SectionNode, BasicWithLoopExtension)
{
    test("[[@(auto i = 0; i < 5; ++i) Hello world! ]]",
         "[[@(auto i = 0; i < 5; ++i) Hello world! ]]");
}

TEST(SectionNode, BasicWithConditionalExtensionOnClosingBraces)
{
    test("[[ Hello world! ]]?(isVisible)",
         "[[ Hello world! ]]?(isVisible)");
}

TEST(SectionNode, BasicWithLoopExtensionOnClosingBraces)
{
    test("[[ Hello world! ]]@(auto i = 0; i < 5; ++i)",
         "[[ Hello world! ]]@(auto i = 0; i < 5; ++i)");
}


TEST(SectionNode, Nested)
{
    test("[[ Hello <p>world</p> [[!]] ]]",
         "[[ Hello <p>world</p> [[!]] ]]");
}

TEST(SectionNode, NestedWithConditionalExtension)
{
    test("[[ Hello <p>?(isVisible)world</p> [[!]]?(isVisible) ]]?(isVisible)",
         "[[ Hello <p>?(isVisible)world</p> [[!]]?(isVisible) ]]?(isVisible)");
}

TEST(SectionNode, NestedWithLoopExtension)
{
    test("[[ Hello <p>@(auto i = 0; i < 5; ++i)world</p> [[!]]@(auto i = 0; i < 3; ++i) ]]@(auto i = 0; i < 5; ++i)",
         "[[ Hello <p>@(auto i = 0; i < 5; ++i)world</p> [[!]]@(auto i = 0; i < 3; ++i) ]]@(auto i = 0; i < 5; ++i)");
}



