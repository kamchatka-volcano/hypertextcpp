#include "utils.h"
#include "streamreader.h"
#include "errors.h"
#include <sstream>
#include <cassert>

namespace htcpp{

//https://developer.mozilla.org/en-US/docs/Glossary/Empty_element
bool isTagEmptyElement(const std::string& tagName)
{
    assert(!tagName.empty());
    if (tagName.front() == '!')
        return true;
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

    return false;
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

}
