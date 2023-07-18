#include "thread_pool.h"

std::mutex coutLocker;

void taskFunc(int id, int delay) 
{
    std::this_thread::sleep_for(std::chrono::seconds(delay));
    std::unique_lock<std::mutex> l(coutLocker);
    std::cout << "task " << id << " made by thread_id " << std::this_thread::get_id() << std::endl;
}

int main()
{
    RequestHandler rh;

    rh.createArr();
    rh.showArr();

    srand(0);
    for (int i = 1; i <= 7; ++i)
    {
        rh.pushRequest(taskFunc, i, 1 + rand() % 4);
    }
    rh.pushArrToTasks();
    std::this_thread::sleep_for(std::chrono::seconds(7));

    rh.showArr();
    
    return 0;
}