#include "task_queue.h"
#include <thread>

namespace dstr
{
    void task_queue::lock()
    {
        is_locked_ = true;
    }

    void task_queue::unlock()
    {
        is_locked_ = false;
    }

    bool task_queue::is_locked()
    {
        return is_locked_;
    }

    void task_queue::start()
    {
        is_working_ = true;
        while (is_working_)
        {
            if (size() == 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }

            std::thread th(front());
            th.join();
            if(size() > 0)
                pop();
        }
    }

    void task_queue::stop()
    {
        is_working_ = false;
    }

    void task_queue::wait()
    {
        bool ready = false;
        push([&]() { ready = true; });
        while (!ready)
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}