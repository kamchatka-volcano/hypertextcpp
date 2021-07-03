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
    auto tag = TNode{};
    auto streamReader = htcpp::StreamReader{stream};
    tag.load(streamReader);
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






