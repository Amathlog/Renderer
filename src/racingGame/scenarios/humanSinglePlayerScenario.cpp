#include <racingGame/scenarios/humanSinglePlayerScenario.h>
#include <racingGame/controllers/humanCarController.h>
#include <racingGame/car.h>
#include <racingGame/gameManager.h>
#include <racingGame/scenarios/spawningStrategy.h>

void HumanSinglePlayerScenario::OnVehicleSpawned(Car* car)
{
    m_currentController = new HumanCarController(1, 0);
    car->AttachController(m_currentController);
}

void HumanSinglePlayerScenario::OnVehicleUnspawned(Car* car)
{
    car->DetachController();
    delete m_currentController;
    m_currentController = nullptr;
}

void HumanSinglePlayerScenario::Update(GameManager& manager)
{
    if (m_currentController == nullptr)
        SpawningStrategy::SpawnVehicle(manager, SpawningStrategy::Strategy::Strategy_AllOnStart);
}

