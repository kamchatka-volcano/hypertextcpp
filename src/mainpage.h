#include <iostream>
#include <sstream>
class MainPage{
    struct Impl{
        template<typename TCfg>
        void renderHTML(TCfg& cfg, std::ostream& out)
        {
out << "<!DOCTYPE";out << R"( html)";out << ">";out << R"(
)";out << "<html";out << ">";out << R"(
)";out << "<body";out << ">";out << R"(

)";out << "<h1";out << ">";out << (cfg.header);out << "</h1>";
out << R"(
)";out << "<p";out << R"( )";if (cfg.isBold){
out << R"(style="font-weight: bold")";}
out << ">";out << R"(Hello world!)";out << "</p>";
out << R"(

)";out << "</body>";
out << R"(
)";out << "</html>";
        }
    } impl_;
public:
    template<typename TCfg>
    std::string render(const TCfg& cfg)
    {
        auto stream  = std::stringstream{};
        impl_.renderHTML(cfg, stream);
        return stream.str();
    }
    template<typename TCfg>
    void print(const TCfg& cfg)
    {
        impl_.renderHTML(cfg, std::cout);
    }
};
