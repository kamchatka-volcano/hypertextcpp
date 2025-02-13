#include "assert_exception.h"
#include "idocumentnoderenderer.h"
#include "node_utils.h"
#include "procedurenode.h"
#include "utils.h"
#include <errors.h>
#include <nodereader.h>
#include <streamreader.h>
#include <tagnode.h>
#include <sfun/optional_ref.h>
#include <gtest/gtest.h>

namespace {

void test(
        const std::string& input,
        const std::string& expected,
        sfun::optional_ref<std::vector<std::unique_ptr<htcpp::ProcedureNode>>> procedureNodes = std::nullopt)
{
    auto stream = std::istringstream{input};
    auto streamReader = htcpp::StreamReader{stream};
    auto funcMap = std::map<std::string, std::string>{};
    std::unique_ptr<htcpp::IDocumentNode> tagNode = std::make_unique<htcpp::TagNode>(streamReader);
    if (procedureNodes.has_value())
        htcpp::utils::replaceElementsWithIdsToProcedures(tagNode, procedureNodes.value());
    else {
        auto procedures = std::vector<std::unique_ptr<htcpp::ProcedureNode>>{};
        htcpp::utils::replaceElementsWithIdsToProcedures(tagNode, procedures);
    }

    auto result = std::string{};
    if (tagNode->as<htcpp::INodeCollection>()) {
        auto nodes = htcpp::optimizeNodes(tagNode->as<htcpp::INodeCollection>()->flatten());
        for (auto& node : nodes)
            result += node->as<htcpp::IDocumentNodeRenderer>()->renderingCode();
    }
    else {
        result += tagNode->as<htcpp::IDocumentNodeRenderer>()->renderingCode();
    }
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
         "out << R\"_htcpp_str_(<p> Hello world! </p>)_htcpp_str_\";");
}

TEST(TagNode, Basic)
{
    test("<div id=\"9\"> Hello world! </div>",
         "out << R\"_htcpp_str_(<div id=\"9\"> Hello world! </div>)_htcpp_str_\";");
}

TEST(TagNode, BasicWithConditionalExtension)
{
    test("<div id=\"9\">?(isVisible) Hello world! </div>",
         "if (isVisible){ out << R\"_htcpp_str_(<div id=\"9\"> Hello world! </div>)_htcpp_str_\"; } ");
}

TEST(TagNode, BasicWithConditionalExtensionOnClosingTag)
{
    test("<div id=\"9\">Hello world! </div>?(isVisible)",
         "if (isVisible){ out << R\"_htcpp_str_(<div id=\"9\">Hello world! </div>)_htcpp_str_\"; } ");
}

TEST(TagNode, BasicWithLoopExtension)
{
    test("<div id=\"9\">@(auto i = 0; i < 5; ++i) Hello world! </div>",
         "for (auto i = 0; i < 5; ++i){ out << R\"_htcpp_str_(<div id=\"9\"> Hello world! </div>)_htcpp_str_\"; } ");
}

TEST(TagNode, BasicWithLoopExtensionOnClosingTag)
{
    test("<div id=\"9\">Hello world! </div>@(auto i = 0; i < 5; ++i)",
         "for (auto i = 0; i < 5; ++i){ out << R\"_htcpp_str_(<div id=\"9\">Hello world! </div>)_htcpp_str_\"; } ");
}

TEST(TagNode, WithOtherNodes)
{
    test("<a href=\"$(cfg.url)\" [[class=\"visible\"]]?(isVisible)></a>",
         "out << R\"_htcpp_str_(<a href=\")_htcpp_str_\";out << (cfg.url);out << R\"_htcpp_str_(\" )_htcpp_str_\";if (isVisible){ out << R\"_htcpp_str_(class=\"visible\")_htcpp_str_\"; "
         "} out << R\"_htcpp_str_(></a>)_htcpp_str_\";");
}


TEST(TagNode, NestedWithAttributes)
{
    test("<div id=\"9\"> <p>Hello world!</p> </div>",
         "out << R\"_htcpp_str_(<div id=\"9\"> <p>Hello world!</p> </div>)_htcpp_str_\";");
}

TEST(TagNode, NestedWithAttributesWithConditionalExtension)
{
    test("<div id=\"9\">?(isVisible) <p>?(isVisible)Hello world!</p> </div>",
         "if (isVisible){ out << R\"_htcpp_str_(<div id=\"9\"> )_htcpp_str_\";if (isVisible){ out << R\"_htcpp_str_(<p>Hello world!</p>)_htcpp_str_\"; } out << "
         "R\"_htcpp_str_( </div>)_htcpp_str_\"; } ");
}

TEST(TagNode, NestedWithAttributesWithLoopExtension)
{
    test("<div id=\"9\">@(auto i = 0; i < 5; ++i) <p>@(auto i = 0; i < 5; ++i)Hello world!</p> </div>",
         "for (auto i = 0; i < 5; ++i){ out << R\"_htcpp_str_(<div id=\"9\"> )_htcpp_str_\";for (auto i = 0; i < 5; ++i){ out << "
         "R\"_htcpp_str_(<p>Hello world!</p>)_htcpp_str_\"; } out << R\"_htcpp_str_( </div>)_htcpp_str_\"; } ");
}

TEST(TagNode, EmptyElementNoAttributes)
{
    test("<br>",
         "out << R\"_htcpp_str_(<br>)_htcpp_str_\";");
}

TEST(TagNode, EmptyElement)
{
    test("<img src=\"1.jpg\">",
         "out << R\"_htcpp_str_(<img src=\"1.jpg\">)_htcpp_str_\";");
}

TEST(TagNode, EmptyElementWithConditionalExtension)
{
    test("<img src=\"1.jpg\">?(isVisible)",
         "if (isVisible){ out << R\"_htcpp_str_(<img src=\"1.jpg\">)_htcpp_str_\"; } ");
}

TEST(TagNode, EmptyElementWithLoopExtension)
{
    test("<img src=\"1.jpg\">@(auto i = 0; i < 5; ++i)",
         "for (auto i = 0; i < 5; ++i){ out << R\"_htcpp_str_(<img src=\"1.jpg\">)_htcpp_str_\"; } ");
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

TEST(TagNode, WithIdAttribute)
{
    auto procedures = std::vector<std::unique_ptr<htcpp::ProcedureNode>>{};
    test("<div htcpp-id=\"test\" id=\"9\"> Hello world! </div>", "out << (test());", procedures);
    ASSERT_EQ(procedures.size(), 1);
    EXPECT_EQ(procedures.front()->name(), "test");
    EXPECT_EQ(
            procedures.front()->renderingCode(),
            "out << R\"_htcpp_str_(<div  id=\"9\"> Hello world! </div>)_htcpp_str_\";");
}

TEST(TagNode, NestedWithIdAttribute)
{
    auto procedures = std::vector<std::unique_ptr<htcpp::ProcedureNode>>{};
    test("<div htcpp-id=\"test\" id=\"9\"> <div htcpp-id=\"test2\">Hello world!</div> <div htcpp-id=\"test3\">Hello "
         "world2!</div> </div>",
         "out << (test());",
         procedures);
    ASSERT_EQ(procedures.size(), 3);
    EXPECT_EQ(procedures.at(0)->name(), "test2");
    EXPECT_EQ(procedures.at(0)->renderingCode(), "out << R\"_htcpp_str_(<div >Hello world!</div>)_htcpp_str_\";");
    EXPECT_EQ(procedures.at(1)->name(), "test3");
    EXPECT_EQ(procedures.at(1)->renderingCode(), "out << R\"_htcpp_str_(<div >Hello world2!</div>)_htcpp_str_\";");
    EXPECT_EQ(procedures.at(2)->name(), "test");
    EXPECT_EQ(
            procedures.at(2)->renderingCode(),
            "out << R\"_htcpp_str_(<div  id=\"9\"> )_htcpp_str_\";out << (test2());out << R\"_htcpp_str_( "
            ")_htcpp_str_\";out << (test3());out << R\"_htcpp_str_( </div>)_htcpp_str_\";");
}