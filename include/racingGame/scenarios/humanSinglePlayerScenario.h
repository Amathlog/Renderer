#pragma once

#include <racingGame/scenarios/scenario.h>

class HumanCarController;

class HumanSinglePlayerScenario : public Scenario
{
public:
    virtual void Update(GameManager& manager) override;
    virtual void OnVehicleSpawned(Car* car) override;
    virtual void OnVehicleUnspawned(Car* car) override;
private:
    HumanCarController* m_currentController = nullptr;
};
