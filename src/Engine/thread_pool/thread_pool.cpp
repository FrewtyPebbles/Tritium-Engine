#include "Engine/thread_pool/thread_pool.h"
#include <iostream>


namespace ThreadPool {

    Worker::Worker(size_t priority_count) {
        deques.reserve(priority_count);
        for (size_t i = 0; i < priority_count; i++) {
            deques.push_back(std::make_unique<ChaseLevDeque<Job>>(256));
        }
    }

    Pool::Pool(
        size_t priority_count,
        size_t thread_count
    ) : priority_count(priority_count), thread_count(thread_count) {
        workers.reserve(thread_count);

        for (size_t i = 0; i < thread_count; i++) {
            workers.push_back(std::make_unique<Worker>(priority_count));
        }

        // Start worker threads
        for (size_t i = 0; i < thread_count; i++) {
            workers[i]->thread = std::thread(&Pool::worker_loop, this, i);
        }
    }

    Pool::~Pool() {
        {
            std::lock_guard<std::mutex> lock(shutdown_mutex);
            this->shutdown.store(true, std::memory_order_release);
        }

        shutdown_cv.notify_all();  // Wake all sleeping threads

        for (auto& worker : this->workers) {
            if (worker->thread.joinable()) {
                worker->thread.join();
            }
        }
    }

    void Pool::submit(std::function<void()> work, size_t priority) {
        active_jobs.fetch_add(1, std::memory_order_relaxed);

        thread_local static size_t worker_hint =
            std::hash<std::thread::id>{}(std::this_thread::get_id()) % workers.size();

        size_t priority_idx = static_cast<size_t>(priority);
        workers[worker_hint]->deques[priority_idx]->push(Job(work));

        worker_hint = (worker_hint + 1) % workers.size();
    }

    void Pool::wait() {
        while (this->active_jobs.load(std::memory_order_acquire) > 0) {
            std::this_thread::yield();
        }
    }

    void Pool::worker_loop(size_t worker_id) {
        Worker& self = *workers[worker_id];
        std::random_device rd;
        std::mt19937 gen(rd());


        while (true) {
            if (shutdown.load(std::memory_order_acquire)) {
                break;
            }

            std::optional<Job> job = get_job_by_priority(self, worker_id, gen);

            if (job.has_value()) {
                if (job.value()) {
                    try {
                        job.value()();
                        active_jobs.fetch_sub(1, std::memory_order_release);
                    }
                    catch (...) {
                        active_jobs.fetch_sub(1, std::memory_order_release);
                    }
                }
                else {
                    active_jobs.fetch_sub(1, std::memory_order_release);
                }
            }
            else {
                // Wait on condition variable with timeout
                std::unique_lock<std::mutex> lock(shutdown_mutex);
                shutdown_cv.wait_for(lock, std::chrono::milliseconds(10), [this] {
                    return shutdown.load(std::memory_order_acquire);
                    });
            }
        }


        for (size_t p = 0; p < self.deques.size(); p++) {
            std::optional<Job> job = std::nullopt;
            int jobCount = 0;
            while ((job = self.deques[p]->pop()).has_value()) {
                jobCount++;
                if (job.value()) {
                    try {
                        job.value()();
                        active_jobs.fetch_sub(1, std::memory_order_release);
                    }
                    catch (...) {
                        active_jobs.fetch_sub(1, std::memory_order_release);
                    }
                }
                else {
                    active_jobs.fetch_sub(1, std::memory_order_release);
                }
            }
        }

    }

    std::optional<Job> Pool::get_job_by_priority(Worker& self, size_t worker_id, std::mt19937& gen) {
        // First, try own deques from highest to lowest priority
        for (size_t p = 0; p < this->priority_count; p++) {
            auto job = self.deques[p]->pop();
            if (job.has_value()) return job;
        }

        // If no local work, try to steal from others (priority-aware)
        return this->try_steal_by_priority(worker_id, gen);
    }

    std::optional<Job> Pool::try_steal_by_priority(size_t worker_id, std::mt19937& gen) {
        size_t numWorkers = this->workers.size();
        std::uniform_int_distribution<size_t> dist(0, numWorkers - 1);

        // Try higher priority deques first across all workers
        for (size_t p = 0; p < this->priority_count; p++) {
            // Try multiple random victims for this priority level
            for (size_t attempt = 0; attempt < numWorkers; attempt++) {
                size_t victim = dist(gen);

                if (victim == worker_id) continue;

                auto job = this->workers[victim]->deques[p]->steal();
                if (job.has_value()) {
                    return job;
                }
            }
        }

        return std::nullopt;
    }

}