#pragma once

#include <thread>
#include <mutex>

template<typename T>
class Singleton
{
public:
    static void CreateInstance()
    {
        std::lock_guard<std::mutex> lock(ms_lock);
        if (ms_instance == nullptr)
            ms_instance = new T(Token{});
    }

    static void DestroyInstance()
    {
        std::lock_guard<std::mutex> lock(ms_lock);
        delete ms_instance;
        ms_instance = nullptr;
    }

    static void HasInstance()
    {
        return ms_instance != nullptr;
    }

    static T* GetInstance()
    {
        return ms_instance;
    }

    Singleton(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;

    Singleton& operator= (const Singleton&) = delete;
    Singleton& operator= (Singleton&&) = delete;

protected:
    Singleton() = default;

    // To protect our child constructors
    struct Token {};

    static inline T* ms_instance = nullptr;
    static inline std::mutex ms_lock;
};