#include "assert_exception.h"
#include <gtest/gtest.h>
#include <errors.h>
#include <utils.h>

TEST(Utils, PreprocessRawString)
{
    auto code = "auto x = `Hello World`;";
    EXPECT_EQ(htcpp::utils::preprocessRawStrings(code),
              "auto x = R\"(Hello World)\";");
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
