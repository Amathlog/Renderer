#pragma once

class Car;
class GameManager;

class Scenario
{
public:
    // Function called at the beginning of each frame.
    virtual void Update(GameManager& manager) {}
    virtual void OnVehicleSpawned(Car* car) {}
    virtual void OnVehicleUnspawned(unsigned int id) {}
};