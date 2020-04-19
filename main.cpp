#include <iostream>
#include "task.h"
#include <vector>
#include <string>



int main() {
    Task task1 = Task(string("proxy1"), 0, 10, proxy(__cxx11::basic_string(), 0));
    Task task2 = Task(string("proxy2"), 10, 20, proxy(__cxx11::basic_string(), 0));
    vector<Task> task_list = {task1, task2};
    cout << task_list.at(0).speed;
}
