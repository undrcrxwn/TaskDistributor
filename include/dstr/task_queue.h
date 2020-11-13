#pragma once
#include <queue>
#include <functional>

namespace dstr
{
    typedef std::function<void()> task;

    class task_queue : public std::queue<task>
    {
    private:
        bool is_working_ = false;
        bool is_locked_ = false;

    public:
        void lock();
        void unlock();
        bool is_locked();
        void start();
        void stop();
        void wait();
    };
}