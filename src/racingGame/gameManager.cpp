#include "racingGame/gameManager.h"

#include "glm.hpp"
#include "Box2D/Box2D.h"
#include "racingGame/car.h"

GameManager::~GameManager()
{
    ClearCars();
    if (m_world != nullptr)
    {
        delete m_world;
        m_world = nullptr;
    }
}

void GameManager::Initialize()
{
    if (m_world != nullptr)
        return;

    m_world = new b2World(b2Vec2(0.0f, 0.0f));
}

void GameManager::ClearCars()
{
    for(unsigned int i = 0; i < m_cars.size(); ++i)
    {
        delete m_cars[i];
    }
    m_cars.clear();
}

void GameManager::Reset()
{
    if (m_world == nullptr)
        return;

    ClearCars();
    m_track.ClearTrack();
    while(!m_track.GenerateTrack());
    for (unsigned int i = 0; i < m_numberOfPlayers; ++i)
    {
        Car* car = new Car(m_world, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        car->SetIntialState(m_track.GetPath()[0], m_track.GetIntialAngle());
        m_cars.push_back(car);
    }
}