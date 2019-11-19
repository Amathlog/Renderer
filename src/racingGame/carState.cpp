#include "racingGame/carState.h"
#include "racingGame/car.h"
#include "racingGame/track.h"
#include "utils/utils.h"

#include <cmath>

CarState CarState::GenerateState(const Car& car, const Track::Path& path, unsigned int currentIndex)
{
    CarState state;

    glm::vec2 carPosition = car.GetPosition();
    glm::vec2 carVelocity = car.GetVelocity();
    glm::vec2 carForward = car.GetForward();
    glm::vec2 carSide = car.GetSide();

    // Compute relevant information first
    const glm::vec2& currentPathPoint = path[currentIndex];
    // Find the closest other point
    unsigned int previousIndex = currentIndex == 0 ? path.size() - 1 : currentIndex - 1;
    unsigned int nextIndex = (currentIndex + 1) % path.size();
    const glm::vec2& previousPathPoint = path[previousIndex];
    const glm::vec2& nextPathPoint = path[nextIndex];
    
    glm::vec2 firstPoint, secondPoint;
    unsigned int startingIndex;
    if (glm::length(carPosition - previousPathPoint) < glm::length(carPosition - nextPathPoint))
    {
        firstPoint = previousPathPoint;
        secondPoint = currentPathPoint;
        startingIndex = previousIndex;
    }
    else
    {
        firstPoint = currentPathPoint;
        secondPoint = nextPathPoint;
        startingIndex = currentIndex;
    }

    glm::vec2 roadDirection = Utils::NormalizeWithEpsilon(secondPoint - firstPoint);
    glm::vec2 roadSide = Utils::GetSide(roadDirection);
    glm::vec2 projectionOnRoad = firstPoint + glm::dot(carPosition - firstPoint, roadDirection) * roadDirection;

    glm::vec2 positionToProjection = carPosition - projectionOnRoad;
    
    state.distanceFromRoad = glm::dot(positionToProjection, roadSide) > 0 ?
                             glm::length(positionToProjection) :
                             -glm::length(positionToProjection);

    state.carVelocityRoadRef = {
        glm::dot(carVelocity, roadDirection) / MAX_SPEED,
        glm::dot(carVelocity, roadSide) / MAX_SPEED
    };

    state.angleWithRoad = Utils::GetAngle(carForward, roadDirection) / M_PI;

    return state;
}