#include <racingGame/scenarios/spawningStrategy.h>
#include <racingGame/gameManager.h>
#include <racingGame/track.h>
#include <utils/randomEngine.h>
#include <random>
#include <racingGame/constants.h>


void SpawningStrategy::SpawnVehicle(::GameManager& gameManager, const Strategy& strategy)
{
    if (m_firstCall)
    {
        ResetInternalVariables();
        m_firstCall = false;
    }

    switch (strategy)
    {
    case Strategy::Strategy_AllOnStart:
        AllOnStartStrategy(gameManager);
        break;

    case Strategy::Strategy_Random:
        RandomStrategy(gameManager);
        break;

    case Strategy::Strategy_Spaced:
        SpacedStrategy(gameManager);
        break;

    default:
        // Strategy_Formula1
        Formula1Strategy(gameManager);
        break;
    }
}

void SpawningStrategy::ResetInternalVariables()
{
    m_currentTrackIndex = 0;
    m_currentOffset = 1.0f;
}

void SpawningStrategy::AllOnStartStrategy(::GameManager& gameManager)
{
    gameManager.SpawnVehicle();
}

void SpawningStrategy::RandomStrategy(::GameManager& gameManager)
{
    unsigned int trackSize = static_cast<unsigned int>(gameManager.GetTrack()->GetLength());
    std::uniform_int_distribution<unsigned int> dist(0, trackSize - 1);
    gameManager.SpawnVehicle(dist(RandomEngine::GetInstance()->GetGenerator()));
}

void SpawningStrategy::SpacedStrategy(::GameManager& gameManager)
{
    // Spawning algorithm:
    // - If there is no vehicle, spawn it at index 0 on the map
    // - Elsewise, find the biggest index difference between 2 cars, 
    //   and spawn it in between.

    std::vector<unsigned int> trackIndexes;
    gameManager.GetCarsIndexOnTrack(trackIndexes);

    unsigned int finalIndex = 0;

    if (!trackIndexes.empty())
    {
        unsigned int trackSize = static_cast<unsigned int>(gameManager.GetTrack()->GetLength());
        std::sort(trackIndexes.begin(), trackIndexes.end());

        unsigned int biggestDiff = 0;

        for (size_t i = 0; i < trackIndexes.size(); ++i)
        {
            unsigned int nextIndex = (i + 1) == trackIndexes.size() ? trackIndexes[0] + trackSize : trackIndexes[i + 1];
            unsigned int diff = nextIndex - trackIndexes[i];
            if (diff > biggestDiff)
            {
                biggestDiff = diff;
                finalIndex = trackIndexes[i];
            }
        }
        finalIndex = (finalIndex + biggestDiff / 2) % trackSize;
    }

    gameManager.SpawnVehicle(finalIndex);
}

void SpawningStrategy::Formula1Strategy(::GameManager& gameManager)
{
    constexpr float offset = Constants::TRACK_WIDTH / 2.0f;
    static unsigned int step = 2;
    if (m_currentTrackIndex < step)
        m_currentTrackIndex += static_cast<unsigned int>(gameManager.GetTrack()->GetLength());
    m_currentTrackIndex -= step;
    gameManager.SpawnVehicle(m_currentTrackIndex, false, m_currentOffset * offset);
    m_currentOffset *= -1.0f;
}
