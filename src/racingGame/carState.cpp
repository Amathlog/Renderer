#include <racingGame/carState.h>
#include <racingGame/car.h>
#include <racingGame/track.h>
#include <utils/utils.h>

#include <cmath>
#include <sstream>
#include <debugManager/debugManager.h>

#ifndef M_PI
#define M_PI 3.14159265359f
#endif


CarState CarState::GenerateState(const Car& car, const Track::Path& path, unsigned int currentIndex, bool addDebugInfo, unsigned int carId)
{
    bool reverse = car.GetIsReverse();

    auto getIndex = [&path, reverse](size_t start, long long idxFurther)
    {
        long long res = reverse ? start - idxFurther : start + idxFurther;
        // Resolve negative values
        while (res < 0)
            res += path.size();
        // Resolve out of path values
        while (res >= static_cast<long long>(path.size()))
            res -= path.size();
        // We are sure to have a positive number
        return static_cast<size_t>(res);
    };

    CarState state;

    const b2Body* hull = car.GetHull().body;
    if (hull == nullptr)
        return state;

    const std::vector<Car::Wheel>& wheels = car.GetHull().wheels;
    if (wheels.size() != 4)
        return state;

    glm::vec2 carPosition = Utils::Convertb2Toglm(hull->GetPosition());
    glm::vec2 carVelocity = Utils::Convertb2Toglm(hull->GetLinearVelocity());
    glm::vec2 carForward = Utils::Convertb2Toglm(hull->GetWorldVector(b2Vec2(0.0f, 1.0f)));
    glm::vec2 carSide = Utils::Convertb2Toglm(hull->GetWorldVector(b2Vec2(1.0f, 0.0f)));

    // Compute relevant information first
    const glm::vec2& currentPathPoint = path[currentIndex];
    // Find the closest other point
    size_t previousIndex = getIndex(currentIndex, -1);
    size_t nextIndex = getIndex(currentIndex, 1);
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
            state.wheelAngles[i] = (wheels[i].body->GetAngle() - hull->GetAngle()) / M_PI;
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
        size_t index = getIndex(startingIndex, samplingIndexes.precomputedIndexes[i]);
        while (distance >= Constants::TRACK_DETAIL_STEP)
        {
            index = getIndex(index, 1);
            distance -= Constants::TRACK_DETAIL_STEP;
        }
        firstPoint = path[index];
        secondPoint = path[getIndex(index, 1)];
        glm::vec2 dir = Utils::NormalizeWithEpsilon(secondPoint - firstPoint);
        glm::vec2 wantedPoint = firstPoint + distance * dir;

        // Compute in car reference
        float lengthVec = glm::length(wantedPoint - carPosition);
        glm::vec2 wantedDir = Utils::NormalizeWithEpsilon(wantedPoint - carPosition);

        state.pointsFurther[2 * i] = glm::dot(wantedDir, carForward);
        state.pointsFurther[2 * i + 1] = glm::dot(wantedDir, carSide);
        state.debugPointsFurtherDistances[i] = lengthVec;
    }

    if (addDebugInfo)
    {
        DebugManager* debugManager = DebugManager::GetInstance();
        constexpr unsigned int frametime = 5;
        std::stringstream buffer;
        buffer << "distance_" << carId;
        debugManager->DrawLine(buffer.str().c_str(), carPosition, projectionOnRoad, Colors::BLUE, frametime);

        std::array<Colors, SamplingIndexes::SAMPLING_INDEXES_SIZE> colors = {
            Colors::RED,
            Colors::YELLOW,
            Colors::CYAN,
            Colors::BLACK,
            Colors::WHITE
        };

        for (unsigned int i = 0; i < SamplingIndexes::SAMPLING_INDEXES_SIZE; ++i)
        {
            std::stringstream buffer1;
            std::stringstream buffer2;
            buffer1.precision(2);
            buffer2.precision(2);
            buffer1 << "offsetv_" << SamplingIndexes::SAMPLING_DISTANCES[i] << "m_" << carId;
            buffer2 << "offseth_" << SamplingIndexes::SAMPLING_DISTANCES[i] << "m_" << carId;
            firstPoint = carPosition + carForward * state.pointsFurther[2 * i] * state.debugPointsFurtherDistances[i];
            secondPoint = firstPoint + carSide * state.pointsFurther[2 * i + 1] * state.debugPointsFurtherDistances[i];
            debugManager->DrawLine(buffer1.str().c_str(), carPosition, firstPoint, colors[i], frametime);
            debugManager->DrawLine(buffer2.str().c_str(), firstPoint, secondPoint, colors[i], frametime);
        }
    }

    return state;
}

void CarState::ExportToArray(std::array<float, STATE_SIZE>& outArray) const
{
    size_t size = 0;

    // Utility function
    auto copyToOutput = [&size, &outArray](auto& arrayToCopy)
    {
        std::copy(arrayToCopy.begin(), arrayToCopy.end(), outArray.begin() + size);
        size += arrayToCopy.size();
    };

    outArray[size++] = distanceFromRoad;
    copyToOutput(carVelocityRoadRef);
    outArray[size++] = angleWithRoad;
    copyToOutput(wheelAngles);
    copyToOutput(wheelOmegas);
    outArray[size++] = carOmega;
    outArray[size++] = driftAngle;
    copyToOutput(pointsFurther);

    assert(size == STATE_SIZE);
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
    res << "Projections offsets (in car ref): " << std::endl;
    for (size_t i = 0; i < SamplingIndexes::SAMPLING_INDEXES_SIZE; ++i)
    {
        res << "    *" << SamplingIndexes::SAMPLING_DISTANCES[i] << "m: " << pointsFurther[2 * i] << ", " \
            << pointsFurther[2 * i + 1] << std::endl;
    }
    return res.str();
}