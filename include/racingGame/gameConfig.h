#pragma once

#include <utils/singleton.h>

struct GameConfig
{
    bool enableRendering = true;
    unsigned int windowWidth = 800;
    unsigned int windowHeight = 600;
    unsigned int fps = 60;
    float speed = 1.0f;
    bool humanPlay = true;
    bool attachCamera = true;
    bool debugInfo = false;
    bool computeRankings = true;

    float GetDt() const { return 1.0f / fps; }
};

class GameConfigSingleton : public Singleton<GameConfigSingleton>
{
public:
    GameConfigSingleton(Token) : Singleton()
    {
    }

    void Reset(const GameConfig& gameConfig) { m_gameConfig = gameConfig; }

    GameConfig m_gameConfig;
};