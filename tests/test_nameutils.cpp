#include <gtest/gtest.h>
#include <nameutils.h>

using namespace htcpp::utils;

TEST(NameUtils, SnakeToPascal)
{
    EXPECT_EQ(toPascalCase(""), "");
    EXPECT_EQ(toPascalCase("hello_world"), "HelloWorld");
    EXPECT_EQ(toPascalCase("_hello_world_"), "HelloWorld");
    EXPECT_EQ(toPascalCase("great_2_see_u"), "Great2SeeU");
    EXPECT_EQ(toPascalCase("helloWorld"), "HelloWorld");
    EXPECT_EQ(toPascalCase("helloWorld2"), "HelloWorld2");
    EXPECT_EQ(toPascalCase("HelloWorld2"), "HelloWorld2");
}

TEST(NameUtils, KebabToSnake)
{
    EXPECT_EQ(toSnakeCase(""), "");
    EXPECT_EQ(toSnakeCase("hello-world"), "hello_world");
    EXPECT_EQ(toSnakeCase("hello1-"), "hello1");
    EXPECT_EQ(toSnakeCase("-hello"), "hello");
    EXPECT_EQ(toSnakeCase("_hello1"), "hello1");
    EXPECT_EQ(toSnakeCase("_hello_1_"), "hello_1");
}

TEST(NameUtils, CamelToSnake)
{
    EXPECT_EQ(toSnakeCase("helloWorld"), "hello_world");
    EXPECT_EQ(toSnakeCase("hello"), "hello");
    EXPECT_EQ(toSnakeCase("_helloWorld_"), "hello_world");
}

TEST(NameUtils, PascalToSnake)
{
    EXPECT_EQ(toSnakeCase("HelloWorld"), "hello_world");
    EXPECT_EQ(toSnakeCase("Hello"), "hello");
    EXPECT_EQ(toSnakeCase("_HelloWorld_"), "hello_world");
}

TEST(NameUtils, SnakeToLower)
{
    EXPECT_EQ(toLowerCase(""), "");
    EXPECT_EQ(toLowerCase("hello_world"), "helloworld");
    EXPECT_EQ(toLowerCase("_hello_world_"), "helloworld");
    EXPECT_EQ(toLowerCase("hello"), "hello");
}

TEST(NameUtils, CamelToLower)
{
    EXPECT_EQ(toLowerCase("helloWorld"), "helloworld");
    EXPECT_EQ(toLowerCase("_helloWorld_"), "helloworld");
    EXPECT_EQ(toLowerCase("hello"), "hello");
}
