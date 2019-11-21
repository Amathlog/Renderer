#include "racingGame/carState.h"
#include "racingGame/car.h"
#include "racingGame/track.h"
#include "utils/utils.h"

#include <cmath>
#include <sstream>
#include <debugManager/debugManager.h>

#ifndef M_PI
#define M_PI 3.14159265359f
#endif


CarState CarState::GenerateState(const Car& car, const Track::Path& path, unsigned int currentIndex, bool addDebugInfo, unsigned int carId)
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
    state.distanceFromRoad /= Constants::TRACK_WIDTH;

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
        float lengthVec = glm::length(wantedPoint - carPosition);
        glm::vec2 wantedDir = Utils::NormalizeWithEpsilon(wantedPoint - carPosition);

        state.pointsFurtherCarRef[3 * i] = glm::dot(wantedDir, carForward);
        state.pointsFurtherCarRef[3 * i + 1] = glm::dot(wantedDir, carSide);
        state.pointsFurtherCarRef[3 * i + 2] = lengthVec;

        // Compute in road reference
        wantedDir = Utils::NormalizeWithEpsilon(wantedPoint - projectionOnRoad);

        state.pointsFurtherRoadRef[3 * i] = glm::dot(wantedDir, roadDirection);
        state.pointsFurtherRoadRef[3 * i + 1] = glm::dot(wantedDir, roadSide);
        state.pointsFurtherRoadRef[3 * i + 2] = lengthVec;
    }

    if (addDebugInfo)
    {
        DebugManager* debugManager = DebugManager::GetInstance();
        constexpr unsigned int frametime = 5;
        debugManager->DrawLine("distance_" + carId, carPosition, projectionOnRoad, Colors::BLUE, frametime);

        std::array<Colors, SamplingIndexes::SAMPLING_INDEXES_SIZE> colors = {
            Colors::RED,
            Colors::YELLOW,
            Colors::CYAN,
            Colors::BLACK
        };

        for (unsigned int i = 0; i < SamplingIndexes::SAMPLING_INDEXES_SIZE; ++i)
        {
            char buffer1[50];
            char buffer2[50];
            sprintf_s(buffer1, "offsetv_%.2fm_%d", SamplingIndexes::SAMPLING_DISTANCES[i], carId);
            sprintf_s(buffer2, "offseth_%.2fm_%d", SamplingIndexes::SAMPLING_DISTANCES[i], carId);
            glm::vec2 firstPoint = carPosition + carForward * state.pointsFurtherCarRef[3 * i] * state.pointsFurtherCarRef[3 * i + 2];
            glm::vec2 secondPoint = firstPoint + carSide * state.pointsFurtherCarRef[3 * i + 1] * state.pointsFurtherCarRef[3 * i + 2];
            debugManager->DrawLine(buffer1, carPosition, firstPoint, colors[i], frametime);
            debugManager->DrawLine(buffer2, firstPoint, secondPoint, colors[i], frametime);
        }
    }

    return state;
}

std::string CarState::ToString()
{
    std::stringstream res;
    res << "Distance from road: " << distanceFromRoad << std::endl;
    res << "Velocity: x=" << carVelocityRoadRef[0] << " ; y=" << carVelocityRoadRef[1] << std::endl;
    res << "Angle with road:" << angleWithRoad << std::endl;
    res << "Wheel angles: (" << wheelAngles[0] << ", " << wheelAngles[1] << ")" << std::endl;
    res << "Wheel omegas: (" << wheelOmegas[0] << ", " << wheelOmegas[1] << ", " << wheelOmegas[2] << ", " << wheelOmegas[3] << ")" << std::endl;
    res << "Car omega: " << carOmega << std::endl;
    res << "Drift angle: " << driftAngle << std::endl;
    res << "Projections offsets: " << std::endl;
    for (size_t i = 0; i < SamplingIndexes::SAMPLING_INDEXES_SIZE; ++i)
    {
        res << "    *" << SamplingIndexes::SAMPLING_DISTANCES[i] << "m: car(" << pointsFurtherCarRef[2 * i] << ", " \
            << pointsFurtherCarRef[2 * i + 1] << ") road(" << pointsFurtherRoadRef[2 * i] << ", " << pointsFurtherRoadRef[2 * i + 1] \
            << ")" << std::endl;
    }
    return res.str();
}