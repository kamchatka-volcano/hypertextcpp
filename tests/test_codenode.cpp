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
    auto node = TNode{streamReader};
    auto result = node.renderingCode();
    EXPECT_EQ(result, expected);
}

template <typename TNode>
void testError(const std::string& input, const std::string& expectedErrorMsg)
{
    assert_exception<htcpp::TemplateError>(
        [input]{
            auto stream = std::istringstream{input};
            auto streamReader = htcpp::StreamReader{stream};
            auto node = TNode{streamReader};
        },
        [expectedErrorMsg](const htcpp::TemplateError& e){
            EXPECT_EQ(e.what(), expectedErrorMsg);
        });
}

}

TEST(ExpressionNode, Basic)
{
    test<htcpp::ExpressionNode>
            ("$( isVisible ? 100 : defaultValue())",
             "out << ( isVisible ? 100 : defaultValue());");
}

TEST(ExpressionNode, WithStringOutput)
{
    test<htcpp::ExpressionNode>
            ("$( isVisible ? \"Hello:)\" : defaultValue())",
             "out << ( isVisible ? \"Hello:)\" : defaultValue());");
}

TEST(ExpressionNode, WithRawStringOutput)
{
    test<htcpp::ExpressionNode>
            ("$( isVisible ? R\"(Hello:))\" : defaultValue())",
             "out << ( isVisible ? R\"(Hello:))\" : defaultValue());");
}

TEST(ExpressionNode, WithCustomRawStringOutput)
{
    test<htcpp::ExpressionNode>
            ("$( isVisible ? `Hello:)` : defaultValue())",
             "out << ( isVisible ? R\"(Hello:))\" : defaultValue());");
}

TEST(ExpressionNode, WithCharOutput)
{
    test<htcpp::ExpressionNode>
            ("$( isVisible ? std::string{\"Hello:\"} + ')' : defaultValue())",
             "out << ( isVisible ? std::string{\"Hello:\"} + ')' : defaultValue());");
}

TEST(ExpressionNode, WithConditionalExtension)
{
    test<htcpp::ExpressionNode>
            ("$(?(isVisible) isVisible ? 100 : defaultValue())",
             "if (isVisible){ out << ( isVisible ? 100 : defaultValue()); } ");
}

TEST(ExpressionNode, WithLoopExtension)
{
    test<htcpp::ExpressionNode>
            ("$(@(auto i = 0; i < 5; ++i) isVisible ? 100 : defaultValue())",
             "for (auto i = 0; i < 5; ++i){ out << ( isVisible ? 100 : defaultValue()); } ");
}

TEST(ExpressionNode, WithConditionalExtensionOnClosingBraces)
{
    test<htcpp::ExpressionNode>
            ("$( isVisible ? 100 : defaultValue())?(isVisible)",
             "if (isVisible){ out << ( isVisible ? 100 : defaultValue()); } ");
}

TEST(ExpressionNode, WithLoopExtensionOnClosingBraces)
{
    test<htcpp::ExpressionNode>
            ("$( isVisible ? 100 : defaultValue())@(auto i = 0; i < 5; ++i)",
             "for (auto i = 0; i < 5; ++i){ out << ( isVisible ? 100 : defaultValue()); } ");
}


TEST(ExpressionNode, WithMacroExtensionOnClosingBraces)
{
    test<htcpp::ExpressionNode>
            ("$( isVisible ? 100 : defaultValue())#(test_macro)",
             "out << ( isVisible ? 100 : defaultValue());");
}


TEST(StatementNode, Basic)
{
    test<htcpp::StatementNode>
            ("${ auto a = 0; {/*local scope*/} }",
             " auto a = 0; {/*local scope*/} ");
}

TEST(StatementNode, BasicWithString)
{
    test<htcpp::StatementNode>
            ("${ auto a = 0; {auto str = `Hello world's{}`;} }",
             " auto a = 0; {auto str = R\"(Hello world's{})\";} ");
}

TEST(GlobalStatementNode, Basic)
{
    test<htcpp::GlobalStatementNode>
            ("#{ auto a = 0; {/*local scope*/} }",
             " auto a = 0; {/*local scope*/} ");
}

TEST(GlobalStatementNode, BasicWithString)
{
    test<htcpp::GlobalStatementNode>
            ("#{ auto a = 0; {auto str = `Hello {}`;} }",
             " auto a = 0; {auto str = R\"(Hello {})\";} ");
}

TEST(InvalidExpressionNode, Unclosed)
{
    testError<htcpp::ExpressionNode>("$(cfg.value",
                                     "[line:1, column:1] Expression isn't closed with ')'");
}

TEST(InvalidExpressionNode, UnclosedString)
{
    testError<htcpp::ExpressionNode>("$(\"Hello)",
                                     "[line:1, column:4] String isn't closed with '\"'");
}

TEST(InvalidExpressionNode, UnclosedRawString)
{
    testError<htcpp::ExpressionNode>("$(`Hello)",
                                     "[line:1, column:4] String isn't closed with '`'");
}

TEST(InvalidExpressionNode, UnclosedChar)
{
    testError<htcpp::ExpressionNode>("$(std::string{\"Hello\"} + '!)",
                                     "[line:1, column:27] Char literal isn't closed with \"'\"");
}


TEST(InvalidExpressionNode, Empty)
{
    testError<htcpp::ExpressionNode>("$(  )",
                                     "[line:1, column:1] Expression can't be empty");
}

TEST(InvalidExpressionNode, MultipleExtensionsTwoConditionals)
{
    testError<htcpp::ExpressionNode>("$(?(isVisible) cfg.value)?(isVisible)",
                                     "[line:1, column:26] Expression can't have multiple extensions");
}

TEST(InvalidExpressionNode, MultipleExtensionsLoopAndConditional)
{
    testError<htcpp::ExpressionNode>("$(@(auto i; i < 5; ++i) cfg.value)?(isVisible)",
                                     "[line:1, column:35] Expression can't have multiple extensions");
}

TEST(InvalidExpressionNode, MultipleExtensionsConditionalAndLoop)
{
    testError<htcpp::ExpressionNode>("$(?(isVisible) cfg.value)@(auto i; i < 5; ++i)",
                                     "[line:1, column:26] Expression can't have multiple extensions");
}

TEST(InvalidExpressionNode, MultipleExtensionsTwoLoops)
{
    testError<htcpp::ExpressionNode>("$(@(auto i; i < 5; ++i) cfg.value)@(auto i; i < 5; ++i)",
                                     "[line:1, column:35] Expression can't have multiple extensions");
}

TEST(InvalidStatementNode, Unclosed)
{
    testError<htcpp::StatementNode>("${ auto x = 0;",
                                     "[line:1, column:1] Statement isn't closed with '}'");
}

TEST(InvalidStatementNode, Empty)
{
    testError<htcpp::StatementNode>("${   }",
                                    "[line:1, column:1] Statement can't be empty");
}

TEST(InvalidGlobalStatementNode, Unclosed)
{
    testError<htcpp::GlobalStatementNode>("#{ #include <set> ",
                                    "[line:1, column:1] Global statement isn't closed with '}'");
}

TEST(InvalidGlobalStatementNode, Empty)
{
    testError<htcpp::GlobalStatementNode>("#{   }",
                                    "[line:1, column:1] Global statement can't be empty");
}

