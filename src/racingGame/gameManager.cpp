#include <racingGame/gameManager.h>

#include <glm/glm.hpp>
#include <Box2D/Box2D.h>
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

namespace
{
    const GameConfig& GetGameConfig()
    {
        return GameConfigSingleton::GetInstance()->m_gameConfig;
    }
}

GameManager::GameManager(const GameConfig& config, Scenario* scenario)
    : m_initialGameConfig(config)
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

const Car::LapInfo* GameManager::GetLapInfoFromId(unsigned int id) const
{
    const auto& it = m_cars.find(id);
    if (it == m_cars.end())
        return nullptr;
    return &it->second->GetLapInfo();
}

void GameManager::GetCarsIndexOnTrack(std::vector<unsigned int>& outVector) const
{
    outVector.clear();
    outVector.reserve(m_cars.size());
    for (const auto& it : m_cars)
    {
        outVector.push_back(it.second->GetCurrentTrackIndex());
    }
}

void GameManager::UpdateCarsRanking()
{
    if (!GetGameConfig().computeRankings)
        return;

    unsigned int trackLength = m_track->GetLength();

    std::sort(m_raceRanking.begin(), m_raceRanking.end(),
        [trackLength](const Car*& a, const Car*& b)
    {
        unsigned int rankA = a->GetLapInfo().nbLaps == -1 ? 0 : a->GetLapInfo().nbLaps * trackLength + a->GetCurrentTrackIndex();
        unsigned int rankB = b->GetLapInfo().nbLaps == -1 ? 0 : b->GetLapInfo().nbLaps * trackLength + b->GetCurrentTrackIndex();
        return rankA > rankB;
    }
    );
}

float GameManager::GetElapsedTime() const
{
    return GetGameConfig().GetDt() * m_nbFrames;
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
    GameConfigSingleton::CreateInstance();
}

void GameManager::DestroySingletons()
{
    GameConfigSingleton::DestroyInstance();
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
    float elapsedTime = GetElapsedTime();

    const GameConfig& config = GetGameConfig();
    float realDt = dt;
    // Slow down if the speed is below 1
    // Speed up is not supported for now
    if (config.speed < 1.0f)
        realDt *= config.speed;

    // Don't update the physics if we are on pause
    if (!Renderer::GetInstance()->paused)
    {
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

            car->UpdateTrackIndex(m_track->GetPath(), elapsedTime);
            CarController* controller = car->GetController();
            // Slow down if the speed is below 1
            // Speed up is not supported for now
            unsigned int stateInterval = controller->GetStateInterval();
            if (config.speed < 1.0f)
                stateInterval = static_cast<unsigned int>(std::floor(stateInterval / config.speed));
            if (controller != nullptr && m_nbFrames % stateInterval == 0)
            {
                CarState state = CarState::GenerateState(*car, m_track->GetPath(), car->GetCurrentTrackIndex(), config.debugInfo, i, m_cars);
                controller->Update(state, *car);
            }

            car->Step(realDt);
            car->UpdateRendering();
        }

        m_world->Step(realDt, 6 * 30, 2 * 30);

        UpdateCarsRanking();
    }

    if (config.attachCamera)
        UpdateCamera();

    if (shouldReset)
        Reset();

    m_nbFrames++;
};

void GameManager::SpawnVehicle(unsigned int trackIndex, bool reverse, float offset)
{
    Car* car = new Car(m_world, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), trackIndex, reverse, GetElapsedTime());
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
        auto rankIt = std::find(m_raceRanking.begin(), m_raceRanking.end(), it->second);
        if (rankIt != m_raceRanking.end())
            m_raceRanking.erase(rankIt);

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

    GameConfigSingleton::GetInstance()->Reset(m_initialGameConfig);

    const GameConfig& config = GetGameConfig();

    // Initialize the renderer
    Renderer* renderer = Renderer::GetInstance();
    renderer->Enable(config.enableRendering);
    int errorCode = renderer->Initialize(config.windowWidth, config.windowHeight);
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
    if (config.attachCamera)
        params = { -30.0f, 30.0f, -30.0f, 30.0f, -0.1f, 10.0f };
    else
        params = { -200.0f, 200.0f, -200.0f, 200.0f, -0.1f, 10.0f };

    // Then initialize the game
    errorCode = Initialize();
    if (errorCode != 0)
        return errorCode;
    // CHANGE WITH AI CONTROLLERS
    SetNumberOfPlayers(config.humanPlay ? 1 : 1);
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

        Step(config.GetDt());

#ifdef PROFILING
        auto differencePhysics = std::chrono::high_resolution_clock::now() - tickTimePhysics;
        sumTimePhysics += std::chrono::duration_cast<std::chrono::microseconds>(differencePhysics).count();
#endif // PROFILING

        if (config.enableRendering)
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
            if (config.enableRendering)
                std::cout << "Mean render time: " << sumTimeRendering / 60 << "us" << std::endl;
            std::cout << "Mean physics time: " << sumTimePhysics / 60 << "us" << std::endl;
            count = 0;
            sumTimePhysics = 0;
            sumTimeRendering = 0;
        }
#endif // PROFILING
        // Do not wait if we don't render
        if (config.enableRendering)
        {
            int64_t deltaTimeUS = static_cast<int64_t>(floorf(1000000.0f / config.fps));
            if (renderTime < deltaTimeUS)
                std::this_thread::sleep_for(std::chrono::microseconds(deltaTimeUS - renderTime));
            else
                std::cout << "This frame took too much time... " << renderTime << "us" << std::endl;
        }
    }
    renderer->Close();
    return 0;
}
