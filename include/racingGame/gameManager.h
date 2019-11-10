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

    void Reset();
    void ClearCars();

    void SetNumberOfPlayers(unsigned int numberOfPlayers) {m_numberOfPlayers = numberOfPlayers;}

private:
    b2World* m_world = nullptr;
    Track m_track;
    std::vector<Car*> m_cars;
    unsigned int m_numberOfPlayers = 0;
};