#include <racingGame/gameManager.h>

#include <glm/glm.hpp>
#include <Box2D/Box2D.h>
#include <racingGame/car.h>
#include <racingGame/track.h>

#include <renderer/renderer.h>
#include <renderer/camera.h>
#include <racingGame/constants.h>
#include <racingGame/carState.h>
#include <racingGame/controllers/humanCarController.h>
#include <racingGame/scenarios/scenario.h>

#include <iostream>
#include <chrono> 
#include <thread>
#include <numeric>
#include <GLFW/glfw3.h>
#include <debugManager/debugManager.h>
#include <utils/utils.h>
#include <shaders/shaderManager.h>

#define PROFILING

GameManager::GameManager(const GameConfig& config, Scenario* scenario)
    : m_config(config)
    , m_scenario(scenario)
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
    DestroySingletons();
}

int GameManager::Initialize()
{
    // Nothing to do
    if (m_world != nullptr)
        return 0;

    m_world = new b2World(b2Vec2(0.0f, 0.0f));
    m_track = new Track();

    DebugManager::GetInstance()->Enable(true);

    if (m_scenario != nullptr)
        return m_scenario->Initialize();
    return 0;
}

void GameManager::ClearCars()
{
    for (auto it : m_cars)
    {
        if (m_scenario != nullptr)
            m_scenario->OnVehicleUnspawned(it.second);
        delete it.second;
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
}

void GameManager::UpdateCamera()
{
    if (m_cars.empty())
        return;
    Camera& camera = Renderer::GetInstance()->GetCamera();

    Car* firstCar = m_cars.begin()->second;

    // Get the current angle and store it in our buffer
    m_smoothCameraRotation.push_back(firstCar->GetAngle());
    float angle = std::accumulate(m_smoothCameraRotation.buffer.begin(), m_smoothCameraRotation.buffer.end(), 0.0f) / m_smoothCameraRotation.size();
    // Set the rotation with the up vector
    glm::vec3 up(glm::vec3(-std::sin(angle), std::cos(angle), 0.0f));
    camera.SetUp(up);

    // Snap the camera to the car, minus an offset, to see more of the road
    glm::vec2 pos = firstCar->GetPosition();
    glm::vec3 cameraNewPos(pos[0], pos[1], camera.GetPosition()[2]);
    cameraNewPos[2] = camera.GetPosition()[2];
    cameraNewPos += up * 10.0f;
    camera.SetPosition(cameraNewPos);
}

void GameManager::CreateSingletons()
{
    Renderer::CreateInstance();
    DebugManager::CreateInstance();
    RandomEngine::CreateInstance();
    ShaderManager::CreateInstance();
}

void GameManager::DestroySingletons()
{
    ShaderManager::DestroyInstance();
    RandomEngine::DestroyInstance();
    DebugManager::DestroyInstance();
    Renderer::DestroyInstance();
}

void GameManager::Step(float dt) 
{
    // No scenario, nothing to do...
    if (m_scenario == nullptr)
        return;

    m_scenario->Update(*this);

    bool shouldReset = false;
    unsigned int i = 0;
    for (auto it : m_cars)
    {
        Car* car = it.second;
        // Check if the car is out
        const glm::vec2& carPos = car->GetPosition();
        if (std::abs(carPos[0]) > 0.85f * Constants::PLAYFIELD ||
            std::abs(carPos[1]) > 0.85f * Constants::PLAYFIELD)
        {
            shouldReset = true;
            break;
        }
        
        car->UpdateTrackIndex(m_track->GetPath());
        CarController* controller = car->GetController();
        if (controller != nullptr && m_nbFrames % controller->GetStateInterval() == 0)
        {
            CarState state = CarState::GenerateState(*car, m_track->GetPath(), car->GetCurrentTrackIndex(), m_config.debugInfo, i, m_cars);
            controller->Update(state, *car);
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

void GameManager::SpawnVehicle(unsigned int trackIndex, bool reverse, float offset)
{
    Car* car = new Car(m_world, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), trackIndex, reverse);
    car->SetIntialState(m_track->GetPath()[trackIndex], m_track->GetAngle(trackIndex, reverse), offset);
    m_cars.emplace(car->GetId(), car);

    if (m_scenario != nullptr)
        m_scenario->OnVehicleSpawned(car);
}

void GameManager::UnspawnVehicle(unsigned int id)
{
    auto it = m_cars.find(id);
    if (it != m_cars.end())
    {
        // If it is the first car, we want to clear our smoothing camera
        m_smoothCameraRotation.clear();
        if (m_scenario != nullptr)
            m_scenario->OnVehicleUnspawned(it->second);
        delete it->second;
    }
    m_cars.erase(it);
}

int GameManager::Run()
{
    CreateSingletons();

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
        params = { -30.0f, 30.0f, -30.0f, 30.0f, -0.1f, 10.0f };
    else
        params = { -200.0f, 200.0f, -200.0f, 200.0f, -0.1f, 10.0f };

    int64_t deltaTimeUS = static_cast<int64_t>(floorf(1000000.0f / m_config.fps));
    float dt = 1.0f / m_config.fps;

    // Then initialize the game
    errorCode = Initialize();
    if (errorCode != 0)
        return errorCode;
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
            DebugManager::GetInstance()->Update();
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
