#pragma once

class Car;
class GameManager;

class Scenario
{
public:
    // Function called at the beginning of each frame.
    virtual int Initialize() { return 0; }
    virtual void Update(GameManager&) {}
    virtual void OnVehicleSpawned(Car*) {}
    virtual void OnVehicleUnspawned(Car*) {}
};