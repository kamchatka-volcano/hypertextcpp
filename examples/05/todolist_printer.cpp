#include <hypertextcpp/templateloader.h>
#include <string>
#include <vector>

struct PageParams{
    struct Task{
        std::string name;
        bool isCompleted = false;
    };
    std::string name = "Bob";
    std::vector<Task> tasks = {{"laundry", true}, {"cooking", false}};
};
HTCPP_CONFIG(PageParams);

int main()
{	
    auto pageParams = PageParams{};	
    auto page = htcpp::loadTemplate<PageParams>("libtodolist.so");
    page->print(pageParams);
    return 0;
}
