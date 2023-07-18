#include "thread_pool.h"

RequestHandler::ThreadPool::ThreadPool() :
    thread_count(std::thread::hardware_concurrency() != 0 ? std::thread::hardware_concurrency() : 4), 
    thread_queues(thread_count) 
{
    
}

RequestHandler::ThreadPool::~ThreadPool()
{
    threads.clear();
    thread_queues.clear();
    delete[] arr;
}

void RequestHandler::ThreadPool::start()
{
    for (int i = 0; i < thread_count; ++i) 
    {
        threads.emplace_back(&ThreadPool::threadFunc, this, i);
    }
}

void RequestHandler::ThreadPool::stop()
{
    for (int i = 0; i < thread_count; ++i) 
    {
        task_type empty_task;
        thread_queues[i].push(empty_task);
    }
    for (auto& t : threads) 
    {
        t.join();
    }
}

void RequestHandler::ThreadPool::push_task(FuncType f, int id, int arg)
{
    task_type task = [=] {f(id, arg); };
    thread_queues[index++].push(task);
}

void RequestHandler::ThreadPool::pushSort()
{
    task_type sortTask([&] {QuickSort(arr, 0, arr_size - 1); });
    thread_queues[index++].push(sortTask);
}

void RequestHandler::ThreadPool::threadFunc(int qindex)
{
    while (true) 
    {
        task_type task_to_do;
        bool res;
        int i = 0;
        for (; i < thread_count; ++i) 
        {
            if (res = thread_queues[(qindex + i) % thread_count].fast_pop(task_to_do))
            {
                break;
            }    
        }

        if (!res) 
        {
            thread_queues[qindex].pop(task_to_do);
        }
        else if (!task_to_do) 
        {
            thread_queues[(qindex + i) % thread_count].push(task_to_do);
        }
        if (!task_to_do) 
        {
            return;
        }
        task_to_do();
    }
}

void RequestHandler::ThreadPool::swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

int RequestHandler::ThreadPool::partition(int* arr, int low, int high)
{
    int pivot = arr[high]; 
    int i = (low - 1); 
    for (int j = low; j <= high - 1; j++)
    {
        if (arr[j] <= pivot)
        {
            ++i; 
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);

    return (i + 1);
}

void RequestHandler::ThreadPool::QuickSort(int* arr, int low, int high)
{
    if (low < high)
    {
        int pi = partition(arr, low, high);
        std::future<void> f = std::async(std::launch::async, [&]()
            {
                QuickSort(arr, low, pi - 1);
            });
        QuickSort(arr, pi + 1, high); 
    }
}

RequestHandler::RequestHandler() 
{
    tpool.start();
}
RequestHandler::~RequestHandler() 
{
    tpool.stop();
}
void RequestHandler::pushRequest(FuncType f, int id, int arg) 
{
    tpool.push_task(f, id, arg);
}

void RequestHandler::createArr()
{
    srand(0);
    tpool.arr = new int[tpool.arr_size];
    for (int i = 0; i < tpool.arr_size; ++i)
    {
        tpool.arr[i] = rand() % 5000;
    }
}

void RequestHandler::pushArrToTasks()
{
    tpool.pushSort();
}

void RequestHandler::showArr()
{
    for (int i = 0; i < tpool.arr_size; ++i)
    {
        std::cout << tpool.arr[i] << ' ';
    }
    std::cout << std::endl;
}