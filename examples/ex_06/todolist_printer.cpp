#include "todolist.h"
#include <string>
#include <vector>

int main()
{	
        PageParams pageParams;
	auto page = TodoList{};
	page.print(pageParams);
    return 0;
}
