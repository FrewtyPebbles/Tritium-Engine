#pragma once

#include "Engine/thread_pool/chase_lev_deque.h"
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <random>
#include <memory>

namespace ThreadPool {
    
    typedef std::function<void()> Job;

    struct Worker {
        std::thread thread;
        std::vector<std::unique_ptr<ChaseLevDeque<Job>>> deques;

        Worker(size_t priority_count = 5);
    };

    class Pool {
    public:
        Pool(size_t priority_count = 5, size_t thread_count = std::thread::hardware_concurrency());
        ~Pool();

        // Submit tasks

        void submit(std::function<void()> work, size_t priority = 0);
        
        // wait for all tasks to finish
        void wait();

        // Attributes

        size_t priority_count;
        size_t thread_count;

    private:

        // functions

        void worker_loop(size_t worker_id);

        std::optional<Job> get_job_by_priority(Worker& self, size_t worker_id, std::mt19937& gen);

        std::optional<Job> try_steal_by_priority(size_t worker_id, std::mt19937& gen);

        // Attributes

        std::vector<std::unique_ptr<Worker>> workers;
        std::atomic<bool> shutdown{ false };
        std::atomic<size_t> active_jobs{ 0 };

        std::mutex shutdown_mutex;
        std::condition_variable shutdown_cv;
    };

};

