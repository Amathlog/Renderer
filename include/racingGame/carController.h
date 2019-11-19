#pragma once

class Car;
class CarState;

class CarController
{
public:
    CarController(unsigned int stateInterval)
        : m_stateInterval(stateInterval)
    {

    }

    unsigned int GetStateInterval() { return m_stateInterval; }

    // Will be called only each m_stateInterval frames
    virtual void Update(const CarState& state, Car& car) = 0;
protected:
    unsigned int m_stateInterval;
};