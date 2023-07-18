#pragma once

#include <iostream>
#include <queue>
#include <future>
#include <condition_variable>
#include <vector>
#include <mutex>

void taskFunc(int id, int delay);
typedef std::function<void()> task_type;
typedef void (*FuncType) (int, int);

class RequestHandler
{
private:
    template<class T>
    struct BlockedQueue
    {
        std::mutex locker;
        std::queue<T> task_queue;
        std::condition_variable notifier;

        void push(T& item)
        {
            std::lock_guard<std::mutex> l(locker);
            task_queue.push(item);
            notifier.notify_one();
        }
        void pop(T& item)
        {
            std::unique_lock<std::mutex> l(locker);
            if (task_queue.empty())
            {
                notifier.wait(l, [this] {return !task_queue.empty(); });
            }
            item = task_queue.front();
            task_queue.pop();
        }
        bool fast_pop(T& item)
        {
            std::lock_guard<std::mutex> l(locker);
            if (task_queue.empty())
            {
                return false;
            }
            item = task_queue.front();
            task_queue.pop();
            return true;
        }
    };

    struct ThreadPool
    {
        int thread_count;
        std::vector<std::thread> threads;
        std::vector<BlockedQueue<task_type>> thread_queues;
        int index = 0;

        int arr_size = 100;
        int* arr;

        ThreadPool();
        ~ThreadPool();
        void start();
        void stop();
        void push_task(FuncType f, int id, int arg);
        void threadFunc(int qindex);

        void swap(int* a, int* b);
        int partition(int* arr, int low, int high);
        void QuickSort(int* arr, int low, int high);
        void pushSort();
    };

    ThreadPool tpool;

public:
    RequestHandler();
    ~RequestHandler();
    void pushRequest(FuncType f, int id, int arg);
    void createArr();
    void pushArrToTasks();
    void showArr();
};