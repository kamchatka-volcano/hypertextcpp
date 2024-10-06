#include "assert_exception.h"
#include "node_utils.h"
#include "idocumentnoderenderer.h"
#include <gtest/gtest.h>
#include <errors.h>
#include <sectionnode.h>
#include <streamreader.h>

namespace{

void test(const std::string& input, const std::string& expected)
{
    auto stream = std::istringstream{input};
    auto streamReader = htcpp::StreamReader{stream};
    auto sectionNode = htcpp::SectionNode{streamReader};
    auto nodes = htcpp::optimizeNodes(sectionNode.flatten());
    auto result = std::string{};
    for (auto& node : nodes)
        result += node->interface<htcpp::IDocumentNodeRenderer>()->renderingCode();
    EXPECT_EQ(result, expected);
}

void testError(const std::string& input, const std::string& expectedErrorMsg)
{
    assert_exception<htcpp::TemplateError>(
        [input]{
            auto stream = std::istringstream{input};
            auto streamReader = htcpp::StreamReader{stream};
            auto node = htcpp::SectionNode{streamReader};
        },
        [expectedErrorMsg](const htcpp::TemplateError& e){
            EXPECT_EQ(e.what(), expectedErrorMsg);
        });
}

}

TEST(SectionNode, Basic)
{
    test("[[ Hello world! ]]",
         "out << R\"_htcpp_str_( Hello world! )_htcpp_str_\";");
}

TEST(SectionNode, BasicWithConditionalExtension)
{
    test("[[?(isVisible) Hello world! ]]",
         "if (isVisible){ out << R\"_htcpp_str_( Hello world! )_htcpp_str_\"; } ");
}

TEST(SectionNode, BasicWithLoopExtension)
{
    test("[[@(auto i = 0; i < 5; ++i) Hello world! ]]",
         "for (auto i = 0; i < 5; ++i){ out << R\"_htcpp_str_( Hello world! )_htcpp_str_\"; } ");
}

TEST(SectionNode, BasicWithConditionalExtensionOnClosingBraces)
{
    test("[[ Hello world! ]]?(isVisible)",
         "if (isVisible){ out << R\"_htcpp_str_( Hello world! )_htcpp_str_\"; } ");
}

TEST(SectionNode, BasicWithLoopExtensionOnClosingBraces)
{
    test("[[ Hello world! ]]@(auto i = 0; i < 5; ++i)",
         "for (auto i = 0; i < 5; ++i){ out << R\"_htcpp_str_( Hello world! )_htcpp_str_\"; } ");
}


TEST(SectionNode, Nested)
{
    test("[[ Hello <p>world</p> [[!]] ]]",
         "out << R\"_htcpp_str_( Hello <p>world</p> ! )_htcpp_str_\";");
}

TEST(SectionNode, NestedWithConditionalExtension)
{
    test("[[ Hello <p>?(isVisible)world</p> [[!]]?(isVisible) ]]?(isVisible)",
         "if (isVisible){ out << R\"_htcpp_str_( Hello )_htcpp_str_\";if (isVisible){ out << R\"_htcpp_str_(<p>world</p>)_htcpp_str_\"; } out << R\"_htcpp_str_( )_htcpp_str_\";if "
         "(isVisible){ out << R\"_htcpp_str_(!)_htcpp_str_\"; } out << R\"_htcpp_str_( )_htcpp_str_\"; } ");
}

TEST(SectionNode, NestedWithLoopExtension)
{
    test("[[ Hello <p>@(auto i = 0; i < 5; ++i)world</p> [[!]]@(auto i = 0; i < 3; ++i) ]]@(auto i = 0; i < 5; ++i)",
         "for (auto i = 0; i < 5; ++i){ out << R\"_htcpp_str_( Hello )_htcpp_str_\";for (auto i = 0; i < 5; ++i){ out << "
         "R\"_htcpp_str_(<p>world</p>)_htcpp_str_\"; } out << R\"_htcpp_str_( )_htcpp_str_\";for (auto i = 0; i < 3; ++i){ out << R\"_htcpp_str_(!)_htcpp_str_\"; } out << R\"_htcpp_str_( )_htcpp_str_\"; } ");
}


TEST(InvalidSectionNode, Unclosed)
{
    testError("[[ Hello world! ", "[line:1, column:1] Section isn't closed with ']]'");
}

TEST(InvalidSectionNode, Empty)
{
    testError("[[]]", "[line:1, column:1] Section can't be empty");
}

TEST(InvalidSectionNode, MultipleExtensionsTwoConditionals)
{
    testError("[[?(isVisible) Hello world! ]]?(isVisible)",
            "[line:1, column:31] Section can't have multiple extensions");
}

TEST(InvalidSectionNode, MultipleExtensionsLoopAndConditional)
{
    testError("[[@(auto i; i < 5; ++i) Hello world! ]]?(isVisible)",
            "[line:1, column:40] Section can't have multiple extensions");
}

TEST(InvalidSectionNode, MultipleExtensionsConditionalAndLoop)
{
    testError("[[?(isVisible) Hello world! ]]@(auto i; i < 5; ++i)",
            "[line:1, column:31] Section can't have multiple extensions");
}

TEST(InvalidSectionNode, MultipleExtensionsTwoLoops)
{
    testError("[[@(auto i; i < 5; ++i) Hello world! ]]@(auto i; i < 5; ++i)",
            "[line:1, column:40] Section can't have multiple extensions");
}
