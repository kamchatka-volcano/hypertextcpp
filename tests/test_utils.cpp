#include "assert_exception.h"
#include <gtest/gtest.h>
#include <errors.h>
#include <utils.h>

TEST(PreprocessRawString, Basic)
{
    auto code = "auto x = `Hello World`;";
    EXPECT_EQ(htcpp::preprocessRawStrings(code),
              "auto x = R\"(Hello World)\";");
}
