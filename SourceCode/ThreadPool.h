//
// Created by leoin on 2025-07-19.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>


class ThreadPool {
public:
    ThreadPool(int num_threads);
    ~ThreadPool();

    void enqueue(std::function<void()> task);
    void wait_all();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
    std::atomic<int> active_tasks;

    void worker_loop();
};



#endif //THREADPOOL_H
