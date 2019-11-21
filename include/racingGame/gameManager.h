#pragma once

#include "racingGame/track.h"
#include <vector>

class b2World;
class Car;

struct GameConfig
{
    bool enableRendering        = true;
    unsigned int windowWidth    = 800;
    unsigned int windowHeight   = 600;
    unsigned int fps            = 60;
    bool humanPlay              = true;
    bool attachCamera           = true;
    bool debugInfo              = false;
    // TODO: Add AI config
};

class GameManager
{
public:
    GameManager(const GameConfig& config);
    ~GameManager();

    void SetNumberOfPlayers(unsigned int numberOfPlayers) {m_numberOfPlayers = numberOfPlayers;}

    int Run();

private:
    void Initialize();

    void Step(float dt);

    void Reset();
    void ClearCars();
    void UpdateCamera();

    b2World* m_world = nullptr;
    Track* m_track;
    std::vector<Car*> m_cars;
    unsigned int m_numberOfPlayers = 0;

    float m_elapsedTime = 0.0f;
    unsigned int m_nbFrames = 0;
    GameConfig m_config;
};