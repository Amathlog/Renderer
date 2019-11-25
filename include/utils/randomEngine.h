#pragma once

#include <random>
#include <chrono>

class RandomEngine
{
public:
    static std::default_random_engine& GetGenerator()
    {
        if (ms_instance == nullptr)
        {
            // Create instance and set seed
            ms_instance = new RandomEngine();
            unsigned int seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
            ms_instance->m_generator.seed(seed);
        }
        return ms_instance->m_generator;
    }

    static void SetSeed(unsigned int seed)
    {
        std::default_random_engine& generator = GetGenerator();
        generator.seed(seed);
    }

    ~RandomEngine()
    {
        delete ms_instance;
        ms_instance = nullptr;
    }

private:
    RandomEngine() = default;

    static inline RandomEngine* ms_instance = nullptr;
    std::default_random_engine m_generator;
};