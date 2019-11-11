#include "racingGame/gameManager.h"

#include "glm.hpp"
#include "Box2D/Box2D.h"
#include "racingGame/car.h"

#include "renderer/renderer.h"
#include "renderer/camera.h"

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

void GameManager::UpdateCamera()
{
    if (m_cars.empty())
        return;
    Camera& camera = Renderer::GetInstance()->GetCamera();

    // Set the rotation with the up vector
    float angle = m_cars[0]->GetAngle();
    glm::vec3 up(glm::vec3(-std::sin(angle), std::cos(angle), 0.0f));
    camera.SetUp(up);

    // Snap the camera to the car, minus an offset, to see more of the road
    glm::vec3 pos = m_cars[0]->GetPosition();
    pos[2] = camera.GetPosition()[2];
    pos += up * 10.0f;
    camera.SetPosition(pos);
}

void GameManager::Step(float dt) 
{
    static float t = 0.0F;
    t += dt;
    Renderer* renderer = Renderer::GetInstance();
    for (auto car : m_cars)
    {
        car->Gas(renderer->upPressed ? 1.0f : 0.0f);
        if (renderer->leftPressed)
            car->Steer(-1.0f);
        else if (renderer->rightPressed)
            car->Steer(1.0f);
        else 
            car->Steer(0.0f);
        car->Brake(renderer->downPressed ? 0.8f : 0.0f);
        car->Step(dt);
        car->UpdateRendering();
    }

    m_world->Step(dt, 6*30, 2*30);

    UpdateCamera();
};