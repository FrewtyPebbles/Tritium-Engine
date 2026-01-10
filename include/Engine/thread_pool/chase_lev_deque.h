#pragma once

#include <atomic>
#include <memory>
#include <optional>
#include <vector>

namespace ThreadPool {

    template<typename T>
    class ChaseLevDeque {
        struct Array {
            size_t capacity;
            std::unique_ptr<T[]> storage;

            explicit Array(size_t cap)
                : capacity(cap), storage(new T[cap]) {
            }

            T& operator[](size_t i) noexcept { return storage[i % capacity]; }
            const T& operator[](size_t i) const noexcept { return storage[i % capacity]; }

            std::unique_ptr<Array> grow(size_t b, size_t t) const {
                auto new_array = std::make_unique<Array>(capacity * 2);
                for (size_t i = t; i < b; ++i) {
                    new_array->operator[](i) = std::move((*this)[i]);
                }
                return new_array;
            }
        };

        alignas(64) std::atomic<size_t> top{ 0 };
        alignas(64) std::atomic<size_t> bottom{ 0 };
        alignas(64) std::atomic<Array*> array;

        std::vector<Array*> retired;

        static constexpr size_t RETIRE_THRESHOLD = 32;

        void reclaim_retired() {
            Array* current = array.load(std::memory_order_acquire);
            for (auto it = retired.begin(); it != retired.end(); ) {
                if (*it != current) {
                    delete* it;
                    it = retired.erase(it);
                }
                else {
                    ++it;
                }
            }
        }

    public:
        explicit ChaseLevDeque(size_t initial_capacity = 256)
            : array(new Array(initial_capacity))
        {
        }

        ChaseLevDeque(const ChaseLevDeque&) = delete;
        ChaseLevDeque& operator=(const ChaseLevDeque&) = delete;

        ~ChaseLevDeque() {
            Array* curr = array.load(std::memory_order_relaxed);
            delete curr;
            for (Array* old : retired) {
                delete old;
            }
        }

        void push(T task) {
            size_t b = bottom.load(std::memory_order_relaxed);
            size_t t = top.load(std::memory_order_acquire);

            Array* a = array.load(std::memory_order_relaxed);

            if (b - t >= a->capacity - 1) {
                auto new_array_uptr = a->grow(b, t);
                Array* new_array = new_array_uptr.get();

                retired.push_back(a);

                array.store(new_array, std::memory_order_release);
                new_array_uptr.release();

                a = new_array;
            }

            (*a)[b] = std::move(task);

            std::atomic_thread_fence(std::memory_order_release);
            bottom.store(b + 1, std::memory_order_relaxed);
        }

        std::optional<T> pop() {
            size_t old_b = bottom.load(std::memory_order_relaxed);
            if (old_b == 0) {
                return std::nullopt;
            }
            size_t b = old_b - 1;
            bottom.store(b, std::memory_order_relaxed);

            std::atomic_thread_fence(std::memory_order_seq_cst);

            size_t t = top.load(std::memory_order_relaxed);

            Array* a = array.load(std::memory_order_relaxed);

            std::optional<T> result;

            if (t <= b) {
                result.emplace(std::move((*a)[b]));

                if (t == b) {
                    if (!top.compare_exchange_strong(t, t + 1,
                        std::memory_order_seq_cst,
                        std::memory_order_relaxed)) {
                        result.reset();
                    }
                    bottom.store(b + 1, std::memory_order_relaxed);
                }
            }
            else {
                bottom.store(b + 1, std::memory_order_relaxed);
            }

            if (retired.size() >= RETIRE_THRESHOLD) {
                reclaim_retired();
            }

            return result;
        }

        std::optional<T> steal() {
            size_t t = top.load(std::memory_order_acquire);

            std::atomic_thread_fence(std::memory_order_seq_cst);

            size_t b = bottom.load(std::memory_order_acquire);

            if (b <= t) {
                return std::nullopt;
            }

            Array* a = array.load(std::memory_order_consume);

            if (!top.compare_exchange_strong(t, t + 1,
                std::memory_order_seq_cst,
                std::memory_order_relaxed)) {
                return std::nullopt;
            }

            return std::move((*a)[t]);
        }

        bool empty_approx() const noexcept {
            size_t b = bottom.load(std::memory_order_relaxed);
            size_t t = top.load(std::memory_order_relaxed);
            return b <= t;
        }

        size_t size_approx() const noexcept {
            size_t b = bottom.load(std::memory_order_relaxed);
            size_t t = top.load(std::memory_order_relaxed);
            return (b > t) ? b - t : 0;
        }
    };

} // namespace ThreadPool