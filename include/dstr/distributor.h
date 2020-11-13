#pragma once
#include "task_queue.h"
#include <vector>
#include <thread>

namespace dstr
{
    class distributor : private std::vector<task_queue*>
    {
    private:
        std::vector<task_queue*> get_available_queues();

    public:
        distributor(size_t size = 0);
        ~distributor();

        void add_queue(size_t amount = 1);
        task_queue* push_task(task& t);
        task_queue* push_task(task&& t);
        task_queue* push_task(task& t, size_t i);
        std::vector<task_queue*> spread_tasks(std::vector<task*> pts);
        void start();
        void stop();
        void wait();
    };
}