#include "assert_exception.h"
#include <gtest/gtest.h>
#include <errors.h>
#include <tagnode.h>
#include <streamreader.h>
#include <nodereader.h>

namespace{

void test(const std::string& input, const std::string& expected)
{
    auto stream = std::istringstream{input};
    auto streamReader = htcpp::StreamReader{stream};
    auto nodeReader = htcpp::NodeReader{};
    auto tag = htcpp::TagNode{streamReader, nodeReader};
    auto result = tag.docTemplate();
    EXPECT_EQ(result, expected);
}

void testError(const std::string& input, const std::string& expectedErrorMsg)
{
    assert_exception<htcpp::TemplateError>(
        [input]{
            auto stream = std::istringstream{input};
            auto streamReader = htcpp::StreamReader{stream};
            auto nodeReader = htcpp::NodeReader{};
            auto token = htcpp::TagNode{streamReader, nodeReader};
        },
        [expectedErrorMsg](const htcpp::TemplateError& e){
            EXPECT_EQ(e.what(), expectedErrorMsg);
        });
}

}

TEST(TagNode, BasicNoAttributes)
{
    test("<p> Hello world! </p>",
         "<p> Hello world! </p>");
}

TEST(TagNode, Basic)
{
    test("<div id=\"9\"> Hello world! </div>",
         "<div id=\"9\"> Hello world! </div>");
}

TEST(TagNode, BasicWithConditionalExtension)
{
    test("<div id=\"9\">?(isVisible) Hello world! </div>",
         "<div id=\"9\">?(isVisible) Hello world! </div>");
}

TEST(TagNode, BasicWithConditionalExtensionOnClosingTag)
{
    test("<div id=\"9\">Hello world! </div>?(isVisible)",
         "<div id=\"9\">Hello world! </div>?(isVisible)");
}

TEST(TagNode, BasicWithLoopExtension)
{
    test("<div id=\"9\">@(auto i = 0; i < 5; ++i) Hello world! </div>",
         "<div id=\"9\">@(auto i = 0; i < 5; ++i) Hello world! </div>");
}

TEST(TagNode, BasicWithLoopExtensionOnClosingTag)
{
    test("<div id=\"9\">Hello world! </div>@(auto i = 0; i < 5; ++i)",
         "<div id=\"9\">Hello world! </div>@(auto i = 0; i < 5; ++i)");
}

TEST(TagNode, WithOtherNodes)
{
    test("<a href=\"$(cfg.url)\" [[class=\"visible\"]]?(isVisible)></a>",
         "<a href=\"$(cfg.url)\" [[class=\"visible\"]]?(isVisible)></a>");
}


TEST(TagNode, NestedWithAttributes)
{
    test("<div id=\"9\"> <p>Hello world!</p> </div>",
         "<div id=\"9\"> <p>Hello world!</p> </div>");
}

TEST(TagNode, NestedWithAttributesWithConditionalExtension)
{
    test("<div id=\"9\">?(isVisible) <p>?(isVisible)Hello world!</p> </div>",
         "<div id=\"9\">?(isVisible) <p>?(isVisible)Hello world!</p> </div>");
}

TEST(TagNode, NestedWithAttributesWithLoopExtension)
{
    test("<div id=\"9\">@(auto i = 0; i < 5; ++i) <p>@(auto i = 0; i < 5; ++i)Hello world!</p> </div>",
         "<div id=\"9\">@(auto i = 0; i < 5; ++i) <p>@(auto i = 0; i < 5; ++i)Hello world!</p> </div>");
}

TEST(TagNode, EmptyElementNoAttributes)
{
    test("<br>",
         "<br>");
}

TEST(TagNode, EmptyElement)
{
    test("<img src=\"1.jpg\">",
         "<img src=\"1.jpg\">");
}

TEST(TagNode, EmptyElementWithConditionalExtension)
{
    test("<img src=\"1.jpg\">?(isVisible)",
         "<img src=\"1.jpg\">?(isVisible)");
}

TEST(TagNode, EmptyElementWithLoopExtension)
{
    test("<img src=\"1.jpg\">@(auto i = 0; i < 5; ++i)",
         "<img src=\"1.jpg\">@(auto i = 0; i < 5; ++i)");
}
