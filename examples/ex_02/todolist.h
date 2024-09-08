#pragma once
#include <iostream>
#include <sstream>
class todolist;

    namespace htcpp{
    class AllowRenderTag{   friend class ::todolist;
    private:
       AllowRenderTag(){};
    };
    }class todolistRenderer{

    public:
        template<typename TCfg>
        void renderHTML(TCfg& cfg, std::ostream& out, htcpp::AllowRenderTag tag) const
        {
            (void)cfg; (void)out; (void)tag;
        out << "<html";out << ">";out << R"(
    )";out << "<body";out << ">";out << R"(
        )";out << "<h1";out << ">";out << (cfg.name);out << R"('s todo list:)";out << "</h1>";out << R"(
        )";if (cfg.tasks.empty()){ out << "<p";out << ">";out << R"( No tasks found )";out << "</p>"; } out << R"(
        )";out << "<ul";out << ">";out << R"(
            )";for (auto task : cfg.tasks){ out << "<li";out << R"( )";if (task.isCompleted){ out << R"(style="text-decoration: line-through;")"; } out << ">";out << (task.name);out << "</li>"; } out << R"(
        )";out << "</ul>";out << R"(
    )";out << "</body>";out << R"(
)";out << "</html>";out << R"(
)";    }

    template<typename TCfg>
    void renderHTMLPart(const std::string& name, const TCfg& cfg, std::ostream& out, htcpp::AllowRenderTag tag) const
    {
        (void)name; (void)cfg; (void)out; (void)tag;
        }
};
class todolist{
todolistRenderer renderer_;

    public:
        template<typename TCfg>
        std::string render(const TCfg& cfg) const
        {
            auto stream  = std::stringstream{};
            renderer_.renderHTML(cfg, stream, htcpp::AllowRenderTag{});
            return stream.str();
        }
        template<typename TCfg>
        std::string render(const std::string& renderFuncName, const TCfg& cfg) const
        {
            auto stream  = std::stringstream{};
            renderer_.renderHTMLPart(renderFuncName, cfg, stream, htcpp::AllowRenderTag{});
            return stream.str();
        }
        template<typename TCfg>
        void print(const TCfg& cfg) const
        {
            renderer_.renderHTML(cfg, std::cout, htcpp::AllowRenderTag{});
        }
        template<typename TCfg>
        void print(const std::string& renderFuncName, const TCfg& cfg) const
        {
            renderer_.renderHTMLPart(renderFuncName, cfg, std::cout, htcpp::AllowRenderTag{});
        }
        template<typename TCfg>
        void print(const TCfg& cfg, std::ostream& stream) const
        {
            renderer_.renderHTML(cfg, stream, htcpp::AllowRenderTag{});
        }
        template<typename TCfg>
        void print(const std::string& renderFuncName, const TCfg& cfg, std::ostream& stream) const
        {
            renderer_.renderHTML(renderFuncName, cfg, stream, htcpp::AllowRenderTag{});
        }
    };