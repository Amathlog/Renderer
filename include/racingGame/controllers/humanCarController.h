#pragma once

#include <racingGame/controllers/carController.h>
#include <array>

class HumanCarController : public CarController
{
public:
    HumanCarController(unsigned int stateInterval, unsigned int playerId);
    ~HumanCarController();

    virtual void Update(const CarState& state, Car& car) override;

    unsigned int GetPlayerId() const { return m_playerId; }

protected:
    bool m_upPressed = false;
    bool m_downPressed = false;
    bool m_leftPressed = false;
    bool m_rightPressed = false;
    std::array<unsigned int, 4> m_callbackIds;
    unsigned int m_playerId;
};