#pragma once

#include <vector>
#include <unordered_map>
#include <utils/utils.h>
#include <racingGame/car.h>
#include <racingGame/gameConfig.h>

class b2World;
class Track;
class Scenario;

class GameManager
{
public:
    GameManager(const GameConfig& config, Scenario* scenario);
    ~GameManager();

    void SetNumberOfPlayers(unsigned int numberOfPlayers) {m_numberOfPlayers = numberOfPlayers;}
    void SpawnVehicle(unsigned int trackIndex = 0, bool reverse = false, float offset = 0.0f);
    void UnspawnVehicle(unsigned int id);

    int Run();
    void Reset();

    const Track* GetTrack() const { return m_track; }
    const std::vector<const Car*>& GetRanking() const { return m_raceRanking; }
    const Car::LapInfo* GetLapInfoFromId(unsigned int id) const;
    void GetCarsIndexOnTrack(std::vector<unsigned int>& outVector) const;
    void UpdateCarsRanking();

    float GetElapsedTime() const;

private:
    int Initialize();

    void Step(float dt);

    void ClearCars();
    void UpdateCamera();

    void CreateSingletons();
    void DestroySingletons();

    b2World* m_world = nullptr;
    Track* m_track;
    std::unordered_map<unsigned int, Car*> m_cars;
    std::vector<const Car*> m_raceRanking;
    unsigned int m_numberOfPlayers = 0;
    Utils::RingBuffer<float, 5> m_smoothCameraRotation;

    unsigned int m_nbFrames = 0;
    GameConfig m_initialGameConfig;
    Scenario* m_scenario = nullptr;
};