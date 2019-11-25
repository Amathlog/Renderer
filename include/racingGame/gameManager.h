#pragma once

#include <unordered_map>

class b2World;
class Car;
class Track;
class Scenario;

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
    GameManager(const GameConfig& config, Scenario* scenario);
    ~GameManager();

    void SetNumberOfPlayers(unsigned int numberOfPlayers) {m_numberOfPlayers = numberOfPlayers;}
    void SpawnVehicle();
    void UnspawnVehicle(unsigned int id);

    int Run();
    void Reset();

private:
    int Initialize();

    void Step(float dt);

    void ClearCars();
    void UpdateCamera();

    b2World* m_world = nullptr;
    Track* m_track;
    std::unordered_map<unsigned int, Car*> m_cars;
    unsigned int m_numberOfPlayers = 0;

    float m_elapsedTime = 0.0f;
    unsigned int m_nbFrames = 0;
    GameConfig m_config;
    Scenario* m_scenario = nullptr;
};