#include "assert_exception.h"
#include <gtest/gtest.h>
#include <errors.h>
#include <codenode.h>
#include <streamreader.h>

namespace{

template <typename TNode>
void test(const std::string& input, const std::string& expected)
{
    auto stream = std::istringstream{input};    
    auto streamReader = htcpp::StreamReader{stream};
    auto tag = TNode{streamReader};
    auto result = tag.docTemplate();
    EXPECT_EQ(result, expected);
}

template <typename TNode>
void testError(const std::string& input, const std::string& expectedErrorMsg)
{
    assert_exception<htcpp::TemplateError>(
        [input]{
            auto stream = std::istringstream{input};
            auto token = TNode{};
            auto streamReader = htcpp::StreamReader{stream};
            token.load(streamReader);
        },
        [expectedErrorMsg](const htcpp::TemplateError& e){
            EXPECT_EQ(e.what(), expectedErrorMsg);
        });
}

}

TEST(RenderedExpressionNode, Basic)
{
    test<htcpp::ExpressionNode>
            ("$( isVisible ? 100 : defaultValue())",
             "$( isVisible ? 100 : defaultValue())");
}

TEST(RenderedExpressionNode, WithStringOutput)
{
    test<htcpp::ExpressionNode>
            ("$( isVisible ? `Hello:)` : defaultValue())",
             "$( isVisible ? `Hello:)` : defaultValue())");
}

TEST(RenderedExpressionNode, WithConditionalExtension)
{
    test<htcpp::ExpressionNode>
            ("$(?(isVisible) isVisible ? 100 : defaultValue())",
             "$(?(isVisible) isVisible ? 100 : defaultValue())");
}

TEST(RenderedExpressionNode, WithLoopExtension)
{
    test<htcpp::ExpressionNode>
            ("$(@(auto i = 0; i < 5; ++i) isVisible ? 100 : defaultValue())",
             "$(@(auto i = 0; i < 5; ++i) isVisible ? 100 : defaultValue())");
}

TEST(RenderedExpressionNode, WithMacroExtension)
{
    test<htcpp::ExpressionNode>
            ("$(#(test_macro) isVisible ? 100 : defaultValue())",
             "$(#(test_macro) isVisible ? 100 : defaultValue())");
}

TEST(RenderedExpressionNode, WithConditionalExtensionOnClosingBraces)
{
    test<htcpp::ExpressionNode>
            ("$( isVisible ? 100 : defaultValue())?(isVisible)",
             "$( isVisible ? 100 : defaultValue())?(isVisible)");
}

TEST(RenderedExpressionNode, WithLoopExtensionOnClosingBraces)
{
    test<htcpp::ExpressionNode>
            ("$( isVisible ? 100 : defaultValue())@(auto i = 0; i < 5; ++i)",
             "$( isVisible ? 100 : defaultValue())@(auto i = 0; i < 5; ++i)");
}


TEST(RenderedExpressionNode, WithMacroExtensionOnClosingBraces)
{
    test<htcpp::ExpressionNode>
            ("$( isVisible ? 100 : defaultValue())#(test_macro)",
             "$( isVisible ? 100 : defaultValue())#(test_macro)");
}


TEST(RenderingCodeNode, Basic)
{
    test<htcpp::RenderStatementNode>
            ("${ auto a = 0; {/*local scope*/} }",
             "${ auto a = 0; {/*local scope*/} }");
}

TEST(RenderingCodeNode, BasicWithString)
{
    test<htcpp::RenderStatementNode>
            ("${ auto a = 0; {auto str = `Hello {}`;} }",
             "${ auto a = 0; {auto str = `Hello {}`;} }");
}

TEST(GlobalCodeNode, Basic)
{
    test<htcpp::GlobalStatementNode>
            ("#{ auto a = 0; {/*local scope*/} }",
             "#{ auto a = 0; {/*local scope*/} }");
}

TEST(GlobalCodeNode, BasicWithString)
{
    test<htcpp::GlobalStatementNode>
            ("#{ auto a = 0; {auto str = `Hello {}`;} }",
             "#{ auto a = 0; {auto str = `Hello {}`;} }");
}






