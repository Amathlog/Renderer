#include "racingGame/car.h"
#include "racingGame/constants.h"

Car::Car(b2World* world, const glm::vec4& color)
    : m_world(world)
    , m_hullColor(color)
{
    InitializePhysics();
    InitializeRendering();
}

void Car::InitializePhysics()
{
    if (m_world == nullptr)
        return;
    
    // First the hull
    
}

void Car::InitializeRendering()
{

}