#include "racingGame/carState.h"
#include "racingGame/car.h"
#include "racingGame/track.h"
#include "utils/utils.h"

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265359f
#endif


CarState CarState::GenerateState(const Car& car, const Track::Path& path, unsigned int currentIndex)
{
    CarState state;

    const b2Body* hull = car.GetHull();
    if (hull == nullptr)
        return state;

    const std::vector<Car::Wheel>& wheels = car.GetWheels();
    if (wheels.size() != 4)
        return state;

    glm::vec2 carPosition = Utils::Convertb2Toglm(hull->GetPosition());
    glm::vec2 carVelocity = Utils::Convertb2Toglm(hull->GetLinearVelocity());
    glm::vec2 carForward = Utils::Convertb2Toglm(hull->GetWorldVector(b2Vec2(0.0f, 1.0f)));
    glm::vec2 carSide = Utils::Convertb2Toglm(hull->GetWorldVector(b2Vec2(1.0f, 0.0f)));

    // Compute relevant information first
    const glm::vec2& currentPathPoint = path[currentIndex];
    // Find the closest other point
    size_t previousIndex = currentIndex == 0 ? path.size() - 1 : currentIndex - 1;
    size_t nextIndex = (currentIndex + 1) % path.size();
    const glm::vec2& previousPathPoint = path[previousIndex];
    const glm::vec2& nextPathPoint = path[nextIndex];
    
    glm::vec2 firstPoint, secondPoint;
    size_t startingIndex;
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
    float projectionDistance = glm::dot(carPosition - firstPoint, roadDirection);
    glm::vec2 projectionOnRoad = firstPoint + projectionDistance * roadDirection;

    glm::vec2 positionToProjection = carPosition - projectionOnRoad;
    
    state.distanceFromRoad = glm::dot(positionToProjection, roadSide) > 0 ?
                             glm::length(positionToProjection) :
                             -glm::length(positionToProjection);

    state.carVelocityRoadRef = {
        glm::dot(carVelocity, roadDirection) / MAX_SPEED,
        glm::dot(carVelocity, roadSide) / MAX_SPEED
    };

    state.angleWithRoad = Utils::GetAngle(carForward, roadDirection) / M_PI;

    // Wheel state
    for (size_t i = 0; i < 4; ++i)
    {
        // Only store angles for front wheels
        if (i < 2)
        {
            state.wheelAngles[i] = wheels[i].body->GetAngle() / M_PI;
        }
        state.wheelOmegas[i] = wheels[i].omega / MAX_OMEGA;
    }

    // Drifting state
    state.carOmega = hull->GetAngularVelocity() / MAX_OMEGA;
    state.driftAngle = Utils::GetAngle(carForward, Utils::NormalizeWithEpsilon(carVelocity)) / M_PI;

    // Project further
    SamplingIndexes samplingIndexes;
    
    for (unsigned int i = 0; i < SamplingIndexes::SAMPLING_INDEXES_SIZE; ++i)
    {
        float distance = samplingIndexes.precomputedDistances[i] + projectionDistance;
        size_t index = (samplingIndexes.precomputedIndexes[i] + startingIndex) % path.size();
        while (distance >= Constants::TRACK_DETAIL_STEP)
        {
            index = (index + 1) % path.size();
            distance -= Constants::TRACK_DETAIL_STEP;
        }
        firstPoint = path[index];
        secondPoint = path[(index + 1) % path.size()];
        glm::vec2 dir = Utils::NormalizeWithEpsilon(secondPoint - firstPoint);
        glm::vec2 wantedPoint = firstPoint + distance * dir;

        // Compute in car reference
        glm::vec2 wantedDir = Utils::NormalizeWithEpsilon(wantedPoint - carPosition);

        state.pointsFurtherCarRef[2 * i] = glm::dot(wantedDir, carForward);
        state.pointsFurtherCarRef[2 * i + 1] = glm::dot(wantedDir, carSide);

        // Compute in road reference
        wantedDir = Utils::NormalizeWithEpsilon(wantedPoint - projectionOnRoad);

        state.pointsFurtherRoadRef[2 * i] = glm::dot(wantedDir, roadDirection);
        state.pointsFurtherRoadRef[2 * i + 1] = glm::dot(wantedDir, roadSide);
    }

    return state;
}