#include "racingGame/gameManager.h"

#include "glm/glm.hpp"
#include "Box2D/Box2D.h"
#include "racingGame/car.h"

#include "renderer/renderer.h"
#include "renderer/camera.h"
#include "racingGame/constants.h"
#include "racingGame/carState.h"
#include "racingGame/controllers/humanCarController.h"

#include <iostream>
#include <chrono> 
#include <thread>
#include <GLFW/glfw3.h>

#define PROFILING

GameManager::GameManager(const GameConfig& config)
    : m_config(config)
{

}

GameManager::~GameManager()
{
    ClearCars();
    if (m_world != nullptr)
    {
        delete m_world;
        m_world = nullptr;
    }
    if (m_track != nullptr)
    {
        delete m_track;
        m_track = nullptr;
    }
}

void GameManager::Initialize()
{
    if (m_world != nullptr)
        return;

    m_world = new b2World(b2Vec2(0.0f, 0.0f));
    m_track = new Track();
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
    if (m_world == nullptr || m_track == nullptr)
        return;

    Renderer* renderer = Renderer::GetInstance();
    renderer->ClearInputCallbacks();

    ClearCars();
    m_track->ClearTrack();
    while(!m_track->GenerateTrack());
    for (unsigned int i = 0; i < m_numberOfPlayers; ++i)
    {
        Car* car = new Car(m_world, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        car->SetIntialState(m_track->GetPath()[0], m_track->GetIntialAngle());
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
            controller->Update(CarState::GenerateState(*car, m_track->GetPath(), car->GetCurrentTrackIndex()), *car);
        }

        car->Step(dt);
        car->UpdateRendering();
    }

    m_world->Step(dt, 6*30, 2*30);

    if (m_config.attachCamera)
        UpdateCamera();

    if (shouldReset)
        Reset();

    m_elapsedTime += dt;
    m_nbFrames++;
};

int GameManager::Run()
{
    // Initialize the renderer
    Renderer* renderer = Renderer::GetInstance();
    renderer->Enable(m_config.enableRendering);
    int errorCode = renderer->Initialize(m_config.windowWidth, m_config.windowHeight);
    if (errorCode != 0)
        return errorCode;

    // Setup the camera
    Camera& camera = renderer->GetCamera();
    camera.SetIsOrthographic();
    camera.SetPosition(glm::vec3(0.0f, 0.0f, -1.0f));
    camera.SetDirection(glm::vec3(0.0f, 0.0f, 1.0f));
    camera.SetUp(glm::vec3(0.0f, 1.0f, 0.0f));
    Camera::OrthographicParams& params = camera.GetOrthographicParams();
    // Setup differently if we are attached or not to a car
    if (m_config.attachCamera)
        params = { -30.0f, 30.0f, -30.0f, 30.0f, 0.1f, 10.0f };
    else
        params = { -200.0f, 200.0f, -200.0f, 200.0f, 0.1f, 10.0f };

    int64_t deltaTimeUS = static_cast<int64_t>(floorf(1000000.0f / m_config.fps));
    float dt = 1.0f / m_config.fps;

    // Then initialize the game
    Initialize();
    // CHANGE WITH AI CONTROLLERS
    SetNumberOfPlayers(m_config.humanPlay ? 1 : 1);
    Reset();

#ifdef PROFILING
    int64_t count = 0;
    int64_t sumTimeRendering = 0;
    int64_t sumTimePhysics = 0;
#endif // PROFILING

    while (!renderer->IsEnabled() || !renderer->RequestedClose())
    {
        auto lastTickTime = std::chrono::high_resolution_clock::now();

#ifdef PROFILING
        auto tickTimePhysics = std::chrono::high_resolution_clock::now();
#endif // PROFILING

        Step(dt);

#ifdef PROFILING
        auto differencePhysics = std::chrono::high_resolution_clock::now() - tickTimePhysics;
        sumTimePhysics += std::chrono::duration_cast<std::chrono::microseconds>(differencePhysics).count();
#endif // PROFILING

        if (m_config.enableRendering)
        {
            renderer->ProcessInput();
            renderer->Render();
        }

        auto currentTime = std::chrono::high_resolution_clock::now();
        auto difference = currentTime - lastTickTime;
        auto renderTime = std::chrono::duration_cast<std::chrono::microseconds>(difference).count();

#ifdef PROFILING
        sumTimeRendering += renderTime;
        if (++count == 60)
        {
            if (m_config.enableRendering)
                std::cout << "Mean render time: " << sumTimeRendering / 60 << "us" << std::endl;
            std::cout << "Mean physics time: " << sumTimePhysics / 60 << "us" << std::endl;
            count = 0;
            sumTimePhysics = 0;
            sumTimeRendering = 0;
        }
#endif // PROFILING
        // Do not wait if we don't render
        if (m_config.enableRendering)
        {
            if (renderTime < deltaTimeUS)
                std::this_thread::sleep_for(std::chrono::microseconds(deltaTimeUS - renderTime));
            else
                std::cout << "This frame took too much time... " << renderTime << "us" << std::endl;
        }
    }
    renderer->Close();
    return 0;
}
