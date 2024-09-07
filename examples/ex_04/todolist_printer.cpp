#include "todolist.h"
#include <string>
#include <vector>

int main()
{	
    struct PageParams{
        struct Task{
            std::string name;
            bool isCompleted = false;
        };
        std::string name = "Bob";
        std::vector<Task> tasks = {{"laundry", true}, {"cooking", false}};
    } pageParams;
	auto page = TodoList{};
	page.print(pageParams);
    return 0;
}
