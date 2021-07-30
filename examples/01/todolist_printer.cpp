#include "todolist.h"
#include <string>
#include <vector>

int main()
{	
	struct PageParams{
		struct Task{
			std::string name;
		};
		std::string name = "Bob";
		std::vector<Task> tasks = {{"laundry"}, {"cooking"}};
	} pageParams;
	auto page = todolist{};
	page.print(pageParams); 
    return 0;
}
