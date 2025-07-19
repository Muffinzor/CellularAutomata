#include "ThreadPool.h"

ThreadPool::ThreadPool(int num_threads) : stop(false), active_tasks(0) {
    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back([this] { this->worker_loop(); });
    }
}

void ThreadPool::worker_loop() {
    while (!stop) {
        std::function<void()> task;
        {
            std::unique_lock lock(queue_mutex);
            condition.wait(lock, [this] {
                return stop || !tasks.empty();
                });

            if (stop && tasks.empty())
                return;

            task = std::move(tasks.front());
            tasks.pop();
            ++active_tasks;
        }

        task();

        --active_tasks;
    }
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock lock(queue_mutex);
        tasks.emplace(std::move(task));
    }
    condition.notify_one();
}

void ThreadPool::wait_all() {
    while (active_tasks > 0 || !tasks.empty()) {
        std::this_thread::yield();
    }
}

ThreadPool::~ThreadPool() {
    stop = true;
    condition.notify_all();
    for (std::thread& worker : workers)
        if (worker.joinable()) worker.join();
}