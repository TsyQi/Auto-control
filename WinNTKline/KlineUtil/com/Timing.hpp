#ifndef TIMER_H_
#define TIMER_H_

#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable> 
#include <iostream>

class Timing {
public:
    Timing() :expired_(true), try_to_expire_(false) {}

    Timing(const Timing& t) : expired_(t.expired_.load()), try_to_expire_(t.try_to_expire_.load()) {}
    ~Timing() {
        Expire();
#ifdef _DEBUG
        std::cout << "timer destructed!" << std::endl;
#endif
    }

    void StartTimer(int interval, std::function<void()> task) {
        if (expired_ == false) {
#ifdef _DEBUG
            std::cout << "timer is currently running, please expire it first..." << std::endl;
#endif
            return;
        }
        expired_ = false;
        std::thread([this, interval, task]() {
            while (!try_to_expire_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(interval));
                task();
            }
#ifdef _DEBUG
            std::cout << "stop task..." << std::endl;
#endif
            {
                std::lock_guard<std::mutex> locker(mutex_);
                expired_ = true;
                expired_cond_.notify_one();
            }
        }).detach();
    }

    void Expire() {
        if (expired_) {
            return;
        }

        if (try_to_expire_) {
#ifdef _DEBUG
            std::cout << "timer is trying to expire, please wait..." << std::endl;
#endif
            return;
        }
        try_to_expire_ = true;
        {
            std::unique_lock<std::mutex> locker(mutex_);
            expired_cond_.wait(locker, [this] {return expired_ == true; });
            if (expired_ == true) {
#ifdef _DEBUG
                std::cout << "timer expired!" << std::endl;
#endif
                try_to_expire_ = false;
            }
        }
    }

    template<typename callable, class... arguments>
    void SyncWait(int after, callable&& f, arguments&&... args) {

        std::function<typename std::result_of<callable(arguments...)>::type()> task
        (std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
        std::this_thread::sleep_for(std::chrono::milliseconds(after));
        task();
    }
    template<typename callable, class... arguments>
    void AsyncWait(int after, callable&& f, arguments&&... args) {
        std::function<typename std::result_of<callable(arguments...)>::type()> task
        (std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

        std::thread([after, task]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(after));
            task();
        }).detach();
    }

private:
    std::atomic<bool> expired_;
    std::atomic<bool> try_to_expire_;
    std::mutex mutex_;
    std::condition_variable expired_cond_;
};
#endif
