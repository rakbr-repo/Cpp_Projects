#pragma once

#include <cstddef>
#include<queue>
#include<mutex>
#include<condition_variable>
#include<utility>
#include<stdexcept>
#include<optional>


template<typename T>
class BoundedQueue
{
    private:
    std::queue<T> storageQ;
    size_t max_limit;
    std::mutex mtx;
    std::condition_variable notFull;
    std::condition_variable notEmpty;
    bool closed{false};

    public:
    BoundedQueue<T>(std::size_t capacity) : max_limit(capacity)
    {
        if(capacity == 0)
        {
            throw std::invalid_argument("Queue capacity must be greater than zero");
        }
    }

    bool push(T message)
    {
        std::unique_lock<std::mutex> lock(mtx);
        notFull.wait(lock,[this]{
            return storageQ.size() < max_limit || closed;
        });

        if(closed)
        {
            return false;
        }

        storageQ.push(std::move(message));
        lock.unlock();
        notEmpty.notify_one();
        return true;
    }

    std::optional<T> pop()
    {
        std::unique_lock<std::mutex> lock(mtx);
        notEmpty.wait(lock,[this]{
            return !storageQ.empty() || closed;
        });

        if (storageQ.empty())
        {
            return std::nullopt;
        }

        T message = std::move(storageQ.front());
        storageQ.pop();

        lock.unlock();
        notFull.notify_one();

        return message;
    }

    void shutdown()
    {
        {
            std::lock_guard<std::mutex> lock(mtx);
            closed = true;
        }

        notFull.notify_all();
        notEmpty.notify_all();
    }

    std::size_t size()
    {
        std::lock_guard<std::mutex> lck(mtx);
        return max_limit;
    }

};