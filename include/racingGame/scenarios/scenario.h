#pragma once

class Car;
class GameManager;

class Scenario
{
public:
    // Function called at the beginning of each frame.
    virtual void Update(GameManager&) {}
    virtual void OnVehicleSpawned(Car*) {}
    virtual void OnVehicleUnspawned(Car*) {}
};