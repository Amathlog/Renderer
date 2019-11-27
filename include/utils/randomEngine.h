#pragma once

#include <random>
#include <chrono>
#include <utils/singleton.h>

class RandomEngine : public Singleton<RandomEngine>
{
public:
    std::default_random_engine& GetGenerator()
    {
        return m_generator;
    }

    void SetSeed(unsigned int seed)
    {
        m_generator.seed(seed);
    }

    RandomEngine(Token) : Singleton()
    {
        unsigned int seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
        m_generator.seed(seed);
    }

private:

    static inline RandomEngine* ms_instance = nullptr;
    std::default_random_engine m_generator;
};