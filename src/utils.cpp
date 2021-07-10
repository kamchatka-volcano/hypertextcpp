#include "utils.h"
#include "streamreader.h"
#include "errors.h"
#include "textnode.h"
#include "tagnode.h"
#include <string>
#include <algorithm>
#include <vector>
#include <sstream>


namespace htcpp::utils{

//https://developer.mozilla.org/en-US/docs/Glossary/Empty_element
bool isTagEmptyElement(const std::string& tagName)
{    
    if (tagName == "area")
        return true;
    if (tagName == "base")
        return true;
    if (tagName == "br")
        return true;
    if (tagName == "col")
        return true;
    if (tagName == "embed")
        return true;
    if (tagName == "hr")
        return true;
    if (tagName == "img")
        return true;
    if (tagName == "input")
        return true;
    if (tagName == "keygen")
        return true;
    if (tagName == "link")
        return true;
    if (tagName == "meta")
        return true;
    if (tagName == "param")
        return true;
    if (tagName == "source")
        return true;
    if (tagName == "track")
        return true;
    if (tagName == "wbr")
        return true;
    if (tagName.front() == '!')
        return true;

    return false;
}

bool isBlank(const std::string& str)
{
    auto nonWhitespaceIt = std::find_if(str.begin(), str.end(), [](auto ch){return !std::isspace(ch);});
    return nonWhitespaceIt == str.end();
}

std::string preprocessRawStrings(const std::string& cppCode)
{
    auto result = std::string{};
    auto codeStream = std::stringstream{cppCode};
    auto stream = StreamReader{codeStream};
    auto insideString = false;
    while (!stream.atEnd()){
         auto res = stream.read();
         if (res == "`"){
             if (!insideString)
                 result += "R\"(";
             else
                 result += ")\"";
             insideString = !insideString;
         }
         else
             result += res;
    }
    if (insideString)
        throw TemplateError{"String is unclosed"};

    return result;
}

namespace{
void trimFrontNewLine(std::string& str)
{    
    if (str.find("\n") == 0 || str.find("\r\n") == 0)
        str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
            return !std::isspace(ch);
        }));
}

void trimLastBlankLine(std::string& str)
{
    auto trimFrom =
    [&str](auto newLinePos)
    {
        if (newLinePos == std::string::npos)
            return;
        if (newLinePos > 0 && std::isspace(str[newLinePos - 1]))
            return;
        const auto isLastLineBlank = std::find_if(str.begin() + static_cast<int>(newLinePos), str.end(),
                                                  [](auto ch){return !std::isspace(ch);}) == str.end();
        if (isLastLineBlank)
            str.resize(newLinePos);
    };
    trimFrom(str.rfind("\n"));
    trimFrom(str.rfind("\r\n"));
}

}

void trimBlankLines(std::string& str)
{    
    trimFrontNewLine(str);
    trimLastBlankLine(str);
}

void consumeReadedAttributesText(std::string& readedText, std::vector<std::unique_ptr<IDocumentNode>>& nodes)
{
    if (readedText.empty())
        return;

    utils::trimBlankLines(readedText);
    if (!readedText.empty())
        nodes.emplace_back(std::make_unique<TextNode>(readedText));
    readedText.clear();
}

void consumeReadedText(std::string& readedText, std::vector<std::unique_ptr<IDocumentNode>>& nodes, IDocumentNode* newNode)
{
    if (readedText.empty())
        return;

    if (nodes.empty() || nodes.back()->hasType<TagNode>() || (newNode && newNode->hasType<TagNode>())){
        nodes.emplace_back(std::make_unique<TextNode>(readedText));
        readedText.clear();
        return;
    }

    utils::trimBlankLines(readedText);
    if (!readedText.empty())
        nodes.emplace_back(std::make_unique<TextNode>(readedText));
    readedText.clear();
}

}
