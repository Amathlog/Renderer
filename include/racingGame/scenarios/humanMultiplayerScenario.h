#pragma once

#include <racingGame/scenarios/scenario.h>
#include <unordered_map>
#include <vector>

class HumanCarController;

class HumanMultiplayerScenario : public Scenario
{
public:
    HumanMultiplayerScenario(unsigned int nbPlayers);

    virtual void Update(GameManager& manager) override;
    virtual void OnVehicleSpawned(Car* car) override;
    virtual void OnVehicleUnspawned(Car* car) override;
private:
    using MapIdControllers = std::unordered_map<unsigned int, HumanCarController*>;
    MapIdControllers m_controllers;

    unsigned int m_nbPlayers = 0;
    std::vector<unsigned int> m_availablePlayers;
};
