#include <dstr/distributor.h>
#include <windows.h>
#include <iostream>

using namespace dstr;

// Some magic.
int Sum(int x, int y)
{
    return x + y;
}

int main()
{
    // Create Distributor with 3 task queues inside.
    distributor d(3);
    d.start();

    // Declare some pretty stupid tasks.
    task t1 = []() { std::cout << "Time consuming\n"; Sleep(2000); };
    task t2 = []() { std::cout << "Easy and fast\n"; };
    task t3 = []() { std::cout << "Easy and fast\n"; };
    task t4 = []() { std::cout << "Easy and fast\n"; };
    std::vector<task*> easyTasks = { &t2, &t3, &t4 };

    // Lock the queue since a long operation
    // can reduce its throughput. This way we can
    // increase efficiency of our program.
    d.push_task(t1)->lock();
    // In this particular case we prevent pushing
    // two "time consuming" task into the same
    // task queue.

    // Spread (distribute to different task queues) a
    // vector of tasks in order to execute all of them
    // simultaneously.
    d.spread_tasks(easyTasks);
    // Second pushing of the "time consuming" task.
    d.push_task(t1);
    d.wait(); // Wait for all queues to finish their work.

    int x = 75, y = 25;
    int result;
    task getSum = [&]() { result = Sum(x, y); };
    std::cout << "\nCalculating...\n";
    d.push_task(getSum)->wait(); // Wait for the task to complete.
    // The line below will be executed after the task is completed.
    std::cout << "x + y = " << result << "\n";

    d.push_task([]() { std::cout << "\nSeems like time would fail me.\n"; });
    d.stop(); // Stop (freeze) all task queues.
    std::cout << "Feeding unicorns...\n";
    Sleep(3000);
    d.start(); // Resume their work.

    system("pause"); // That's all folks!
    return 0;
}