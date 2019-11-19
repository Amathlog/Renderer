#pragma once

#include "racingGame/track.h"
#include <vector>

class b2World;
class Car;

class GameManager
{
public:
    GameManager() = default;
    ~GameManager();

    void Initialize();

    void Step(float dt);

    void Reset();
    void ClearCars();

    void SetNumberOfPlayers(unsigned int numberOfPlayers) {m_numberOfPlayers = numberOfPlayers;}

private:
    void UpdateCamera();

    b2World* m_world = nullptr;
    Track m_track;
    std::vector<Car*> m_cars;
    unsigned int m_numberOfPlayers = 0;

    float m_elapsedTime = 0.0f;
    unsigned int m_nbFrames = 0;
};