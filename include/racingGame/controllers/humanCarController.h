#pragma once

#include "racingGame/controllers/carController.h"

class HumanCarController : public CarController
{
public:
    HumanCarController(unsigned int stateInterval);
    ~HumanCarController();

    // No need for human
    virtual void Update(const CarState& state, Car& car) override;

protected:
    bool m_upPressed = false;
    bool m_downPressed = false;
    bool m_leftPressed = false;
    bool m_rightPressed = false;
};