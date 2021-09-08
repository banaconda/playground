#pragma once

#include <list>

template <typename T> //
class PriorityQueue
{
    std::list<std::pair<T, int>> queue;

  public:
    void
    enqueue(T e, int priority)
    {
        queue.push_back(std::make_pair(e, priority));
        queue.sort([](const std::pair<T, int> a, const std::pair<T, int> b) {
            return a.second < b.second;
        });
    }

    T *
    dequeue()
    {
        if (queue.empty()) {
            return nullptr;
        }

        std::pair<T, int> *e = &queue.front();
        queue.pop_front();
        return &(e->first);
    }
};