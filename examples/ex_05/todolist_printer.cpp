#include <hypertextcpp/templateloader.h>
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

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
#ifdef _WIN32
    const auto libFilename = std::string{"./todolist.dll"};
#else
    const auto libFilename = std::string{"./libtodolist.so"};
#endif
    std::cout << std::filesystem::canonical(".").string();
    auto page = htcpp::loadTemplate<PageParams>(std::filesystem::canonical(libFilename).string());
    page->print(pageParams);
    return 0;
}
