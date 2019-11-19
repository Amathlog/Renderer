#include "racingGame/gameManager.h"

#include "glm/glm.hpp"
#include "Box2D/Box2D.h"
#include "racingGame/car.h"

#include "renderer/renderer.h"
#include "renderer/camera.h"
#include "racingGame/constants.h"
#include "racingGame/carState.h"
#include "racingGame/humanCarController.h"

#include <iostream>
#include <GLFW/glfw3.h>

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

    Renderer* renderer = Renderer::GetInstance();
    renderer->ClearInputCallbacks();

    ClearCars();
    m_track.ClearTrack();
    while(!m_track.GenerateTrack());
    for (unsigned int i = 0; i < m_numberOfPlayers; ++i)
    {
        Car* car = new Car(m_world, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        car->SetIntialState(m_track.GetPath()[0], m_track.GetIntialAngle());
        m_cars.push_back(car);

        // Add controller
        HumanCarController* controller = new HumanCarController(1);
        car->AttachController(controller);
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
    glm::vec2 pos = m_cars[0]->GetPosition();
    glm::vec3 cameraNewPos(pos[0], pos[1], camera.GetPosition()[2]);
    cameraNewPos[2] = camera.GetPosition()[2];
    cameraNewPos += up * 10.0f;
    camera.SetPosition(cameraNewPos);
}

void GameManager::Step(float dt) 
{
    Renderer* renderer = Renderer::GetInstance();
    bool shouldReset = false;
    for (Car* car : m_cars)
    {
        // Check if the car is out
        const glm::vec2& carPos = car->GetPosition();
        if (std::abs(carPos[0]) > 0.85f * Constants::PLAYFIELD ||
            std::abs(carPos[1]) > 0.85f * Constants::PLAYFIELD)
        {
            shouldReset = true;
            break;
        }
        
        CarController* controller = car->GetController();
        if (controller != nullptr && m_nbFrames % controller->GetStateInterval() == 0)
        {
            controller->Update(CarState::GenerateState(*car, m_track.GetPath(), car->GetCurrentTrackIndex()), *car);
        }

        car->Step(dt);
        car->UpdateRendering();
    }

    m_world->Step(dt, 6*30, 2*30);

    UpdateCamera();

    if (shouldReset)
        Reset();

    m_elapsedTime += dt;
    m_nbFrames++;
};