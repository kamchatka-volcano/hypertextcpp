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
    auto funcMap = std::map<std::string, std::string>{};
    auto node = htcpp::TagNode{streamReader};
    auto result = node.renderingCode();
    EXPECT_EQ(result, expected);
}

void testError(const std::string& input, const std::string& expectedErrorMsg)
{
    assert_exception<htcpp::TemplateError>(
        [input]{
            auto stream = std::istringstream{input};
            auto streamReader = htcpp::StreamReader{stream};
            auto funcMap = std::map<std::string, std::string>{};
            auto token = htcpp::TagNode{streamReader};
        },
        [expectedErrorMsg](const htcpp::TemplateError& e){
            EXPECT_EQ(e.what(), expectedErrorMsg);
        });
}

}

TEST(TagNode, BasicNoAttributes)
{
    test("<p> Hello world! </p>",
         "out << \"<p\";out << \">\";out << R\"( Hello world! )\";out << \"</p>\";");
}

TEST(TagNode, Basic)
{
    test("<div id=\"9\"> Hello world! </div>",
         "out << \"<div\";out << R\"( id=\"9\")\";out << \">\";out << R\"( Hello world! )\";out << \"</div>\";");
}

TEST(TagNode, BasicWithConditionalExtension)
{
    test("<div id=\"9\">?(isVisible) Hello world! </div>",
         "if (isVisible){ out << \"<div\";out << R\"( id=\"9\")\";out << \">\";out "
         "<< R\"( Hello world! )\";out << \"</div>\"; } ");
}

TEST(TagNode, BasicWithConditionalExtensionOnClosingTag)
{
    test("<div id=\"9\">Hello world! </div>?(isVisible)",
         "if (isVisible){ out << \"<div\";out << R\"( id=\"9\")\";out << \">\";out "
         "<< R\"(Hello world! )\";out << \"</div>\"; } ");
}

TEST(TagNode, BasicWithLoopExtension)
{
    test("<div id=\"9\">@(auto i = 0; i < 5; ++i) Hello world! </div>",
         "for (auto i = 0; i < 5; ++i){ out << \"<div\";out << R\"( id=\"9\")\";out << \">\";out "
         "<< R\"( Hello world! )\";out << \"</div>\"; } ");
}

TEST(TagNode, BasicWithLoopExtensionOnClosingTag)
{
    test("<div id=\"9\">Hello world! </div>@(auto i = 0; i < 5; ++i)",
         "for (auto i = 0; i < 5; ++i){ out << \"<div\";out << R\"( id=\"9\")\";out << \">\";out "
         "<< R\"(Hello world! )\";out << \"</div>\"; } ");
}

TEST(TagNode, WithOtherNodes)
{
    test("<a href=\"$(cfg.url)\" [[class=\"visible\"]]?(isVisible)></a>",
         "out << \"<a\";out << R\"( href=\")\";out << (cfg.url);out << R\"(\" )\";if (isVisible){ out "
         "<< R\"(class=\"visible\")\"; } out << \">\";out << \"</a>\";");
}


TEST(TagNode, NestedWithAttributes)
{
    test("<div id=\"9\"> <p>Hello world!</p> </div>",
         "out << \"<div\";out << R\"( id=\"9\")\";out << \">\";out << R\"( )\";out << \"<p\";out << \">\";out"
         " << R\"(Hello world!)\";out << \"</p>\";out << R\"( )\";out << \"</div>\";");
}

TEST(TagNode, NestedWithAttributesWithConditionalExtension)
{
    test("<div id=\"9\">?(isVisible) <p>?(isVisible)Hello world!</p> </div>",
         "if (isVisible){ out << \"<div\";out << R\"( id=\"9\")\";out << \">\";out << R\"( )\";if (isVisible){ out"
         " << \"<p\";out << \">\";out << R\"(Hello world!)\";out << \"</p>\"; } out << R\"( )\";out << \"</div>\"; } ");
}

TEST(TagNode, NestedWithAttributesWithLoopExtension)
{
    test("<div id=\"9\">@(auto i = 0; i < 5; ++i) <p>@(auto i = 0; i < 5; ++i)Hello world!</p> </div>",
         "for (auto i = 0; i < 5; ++i){ out << \"<div\";out << R\"( id=\"9\")\";out << \">\";out "
         "<< R\"( )\";for (auto i = 0; i < 5; ++i){ out << \"<p\";out << \">\";out "
         "<< R\"(Hello world!)\";out << \"</p>\"; } out << R\"( )\";out << \"</div>\"; } ");
}

TEST(TagNode, EmptyElementNoAttributes)
{
    test("<br>",
         "out << \"<br\";out << \">\";");
}

TEST(TagNode, EmptyElement)
{
    test("<img src=\"1.jpg\">",
         "out << \"<img\";out << R\"( src=\"1.jpg\")\";out << \">\";");
}

TEST(TagNode, EmptyElementWithConditionalExtension)
{
    test("<img src=\"1.jpg\">?(isVisible)",
         "if (isVisible){ out << \"<img\";out << R\"( src=\"1.jpg\")\";out << \">\"; } ");
}

TEST(TagNode, EmptyElementWithLoopExtension)
{
    test("<img src=\"1.jpg\">@(auto i = 0; i < 5; ++i)",
         "for (auto i = 0; i < 5; ++i){ out << \"<img\";out << R\"( src=\"1.jpg\")\";out << \">\"; } ");
}

TEST(InvalidTagNode, UnclosedTag)
{
    testError("<p> Hello world!",
              "[line:1, column:1] Tag isn't closed with </p>");
}

TEST(InvalidTagNode, UnclosedOpeningTag)
{
    testError("<p Hello world!",
              "[line:1, column:1] Tag isn't closed with '>'");
}

TEST(InvalidTagNode, EmptyName)
{
    testError("<  >Hello world!</>", "[line:1, column:1] Tag's name can't be empty");
}

TEST(InvalidTagNode, MultipleExtensionsTwoConditionals)
{
    testError("<p>?(isVisible) Hello world! </p>?(isVisible)",
              "[line:1, column:34] Tag can't have multiple extensions");
}

TEST(InvalidTagNode, MultipleExtensionsLoopAndConditional)
{
    testError("<p>@(auto i; i < 5; ++i) Hello world! </p>?(isVisible)",
              "[line:1, column:43] Tag can't have multiple extensions");
}

TEST(InvalidTagNode, MultipleExtensionsConditionalAndLoop)
{
    testError("<p>?(isVisible) Hello world! </p>@(auto i; i < 5; ++i)",
              "[line:1, column:34] Tag can't have multiple extensions");
}

TEST(InvalidTagNode, MultipleExtensionsTwoLoops)
{
    testError("<p>@(auto i; i < 5; ++i) Hello world! </p>@(auto i; i < 5; ++i)",
              "[line:1, column:43] Tag can't have multiple extensions");
}
