#include <racingGame/scenarios/humanMultiplayerScenario.h>
#include <racingGame/controllers/humanCarController.h>
#include <racingGame/car.h>
#include <racingGame/gameManager.h>
#include <racingGame/constants.h>
#include <iostream>
#include <racingGame/scenarios/spawningStrategy.h>

HumanMultiplayerScenario::HumanMultiplayerScenario(unsigned int nbPlayers)
{
    if (nbPlayers > 2)
    {
        std::cerr << "Too many players, only support 2 players for now..." << std::endl;
        nbPlayers = 2;
    }

    m_nbPlayers = nbPlayers;
    m_availablePlayers.resize(m_nbPlayers);

    for (unsigned int i = 0; i < m_nbPlayers; ++i)
        m_availablePlayers[i] = i;
}
    

void HumanMultiplayerScenario::OnVehicleSpawned(Car* car)
{
    unsigned int playerId = m_availablePlayers.back();
    m_availablePlayers.pop_back();

    HumanCarController* controller = new HumanCarController(1, playerId);
    m_controllers.emplace(car->GetId(), controller);
    car->AttachController(controller);
}

void HumanMultiplayerScenario::OnVehicleUnspawned(Car* car)
{
    car->DetachController();
    auto it = m_controllers.find(car->GetId());
    if (it == m_controllers.end())
        return;

    m_availablePlayers.push_back(it->second->GetPlayerId());
    delete it->second;
    m_controllers.erase(it);
}

void HumanMultiplayerScenario::Update(GameManager& manager)
{
    SpawningStrategy::ResetInternalVariables();
    while (m_controllers.size() < m_nbPlayers)
        SpawningStrategy::SpawnVehicle(manager, SpawningStrategy::Strategy::Strategy_Formula1);
}

