#include "distributor.h"

namespace dstr
{
    std::vector<task_queue*> distributor::get_available_queues()
    {
        std::vector<task_queue*> tqs = *this;
        std::remove_if(tqs.begin(), tqs.end(),
            [](task_queue* tq) { return tq->is_locked(); });

        return tqs;
    }

    distributor::distributor(size_t size)
    {
        add_queue(size);
    }

    distributor::~distributor()
    {
        for (task_queue* tq : *this)
            delete tq;
    }

    void distributor::add_queue(size_t amount)
    {
        for (size_t i = 0; i < amount; i++)
            push_back(new task_queue);
    }

    task_queue* distributor::push_task(task& t)
    {
        std::vector<task_queue*> tqs = get_available_queues();
        if (tqs.size() == 0)
            throw std::runtime_error("distributor needs to have at least one available task_queue to recieve any tasks.");

        task_queue* tq = *(std::min_element(tqs.begin(), tqs.end(),
            [](task_queue* x, task_queue* y) { return x->size() < y->size(); }));

        tq->push(t);
        return tq;
    }

    task_queue* distributor::push_task(task&& t)
    {
        return push_task(std::ref(t));
    }

    task_queue* distributor::push_task(task& t, size_t i)
    {
        if ((*this)[i]->is_locked())
            throw std::runtime_error("task_queue needs to be unlocked to recieve any tasks.");

        (*this)[i]->push(t);
        return (*this)[i];
    }

    std::vector<task_queue*> distributor::spread_tasks(std::vector<task*> pts)
    {
        std::vector<task_queue*> tqs = get_available_queues();
        if (tqs.size() == 0)
            throw std::runtime_error("distributor needs to have at least one available task_queue to recieve any tasks.");

        std::sort(tqs.begin(), tqs.end(),
            [](task_queue* x, task_queue* y) { return x->size() < y->size(); });

        for (size_t i = 0; i < pts.size(); i++)
            tqs[i % (tqs.size() - 1)]->push(*pts[i]);

        if (tqs.size() > pts.size())
            return tqs;
        else
            return std::vector<task_queue*>(tqs.begin(), tqs.begin() + (pts.size() - 1));
    }

    void distributor::start()
    {
        for (task_queue* queue : *this)
        {
            std::thread th(&task_queue::start, queue);
            th.detach();
        }
    }

    void distributor::stop()
    {
        for (task_queue* tq : *this)
            tq->stop();
    }

    void distributor::wait()
    {
        size_t ready_amount = 0;
        while (ready_amount != size())
        {
            ready_amount = std::count_if(begin(), end(),
                [](task_queue* tq) { return tq->size() == 0; });
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}