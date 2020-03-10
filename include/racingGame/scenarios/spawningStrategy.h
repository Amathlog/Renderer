#pragma once

class GameManager;

class SpawningStrategy
{
public:
    enum Strategy : unsigned int
    {
        Strategy_AllOnStart = 0,
        Strategy_Random = 1,
        Strategy_Spaced = 2,
        Strategy_Formula1 = 3
    };

    static void SpawnVehicle(::GameManager& gameManager, const Strategy& strategy);
    static void ResetInternalVariables();
private:
    static void AllOnStartStrategy(::GameManager& gameManager);
    static void RandomStrategy(::GameManager& gameManager);
    static void SpacedStrategy(::GameManager& gameManager);
    static void Formula1Strategy(::GameManager& gameManager);

    inline static unsigned int m_currentTrackIndex = 0;
    inline static float m_currentOffset = 1.0f;
    inline static bool m_firstCall = true;
};
