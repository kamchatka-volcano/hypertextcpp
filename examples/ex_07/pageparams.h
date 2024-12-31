#pragma once
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