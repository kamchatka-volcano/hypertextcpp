#include "assert_exception.h"
#include <gtest/gtest.h>
#include <errors.h>
#include <utils.h>

TEST(Utils, TransformRawString)
{
    {
    auto code = "auto x = `Hello World`;";
    EXPECT_EQ(htcpp::utils::transformRawStrings(code, {1, 1}),
              "auto x = R\"(Hello World)\";");
    }
    {
    auto code = "auto x = ``;";
    EXPECT_EQ(htcpp::utils::transformRawStrings(code, {1, 1}),
          "auto x = R\"()\";");
    }
}

TEST(Utils, TransformInvalidRawString)
{
    assert_exception<htcpp::TemplateError>(
        []{
            auto result = htcpp::utils::transformRawStrings("auto x = `Hello World;", {1, 1});
        },
        [](const auto& error)
        {
            EXPECT_EQ(error.what(), std::string{"[line:1, column:11] String is unclosed"});
        });
}

TEST(Utils, TrimLastBlankLine)
{
    {
    auto str = std::string{};
    htcpp::utils::trimBlankLines(str);
    EXPECT_EQ(str, "");
    }
    {
    auto str = std::string{"Hello\n   "};
    htcpp::utils::trimBlankLines(str);
    EXPECT_EQ(str, "Hello");
    }    
    {
    auto str = std::string{"Hello \n   "};
    htcpp::utils::trimBlankLines(str);
    EXPECT_EQ(str, "Hello \n   ");
    }
    {
    auto str = std::string{"Hello\r\n   "};
    htcpp::utils::trimBlankLines(str);
    EXPECT_EQ(str, "Hello");
    }
    {
    auto str = std::string{"Hello \r\n   "};
    htcpp::utils::trimBlankLines(str);
    EXPECT_EQ(str, "Hello \r\n   ");
    }
    {
    auto str = std::string{"\n   Hello"};
    htcpp::utils::trimBlankLines(str);
    EXPECT_EQ(str, "Hello");
    }
    {
    auto str = std::string{"\r\n   Hello"};
    htcpp::utils::trimBlankLines(str);
    EXPECT_EQ(str, "Hello");
    }
    {
    auto str = std::string{"\n   Hello\r\n   "};
    htcpp::utils::trimBlankLines(str);
    EXPECT_EQ(str, "Hello");
    }
    {
    auto str = std::string{"\r\n   Hello\n   "};
    htcpp::utils::trimBlankLines(str);
    EXPECT_EQ(str, "Hello");
    }
}
