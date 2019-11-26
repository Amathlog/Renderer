#pragma once

#include <array>
#include <string>
#include <racingGame/track.h>
#include <racingGame/constants.h>


/// Description of a car state
/// 1       - Distance between the car and it's closest projection on the road.
///           Signed to tell if the car if left side of the road or right side.
/// 2-3     - Velocity of the car, given in the road reference.
/// 4       - Angle with road. 0 means aligned (in rad)
/// 5-6     - Angles of the two front wheels (rear ones don't rotate) 
///           0 means aligned with the hull(in rad)
/// 7-11    - Angular velocity of each wheel
/// 12      - Angular veocity of the car
/// 13      - Angle between car forward vector and car velocity vector
/// 14-...  - Points further down the road, given in the car reference
///           Number of points and distances can be changed with the 
///           constexpr just below.
/// ...     - Same thing but in the road ref. Gives a more stable vision of the path.

class Car;

// Precompute indexes and distances, for better performances
struct SamplingIndexes
{
    constexpr static inline size_t SAMPLING_INDEXES_SIZE = 5;
    constexpr static inline std::array<float, SAMPLING_INDEXES_SIZE> SAMPLING_DISTANCES = { 3.0f, 5.0f, 10.0f, 25.0f, 50.0f };

    constexpr SamplingIndexes() : precomputedDistances(), precomputedIndexes()
    {
        // We do not use math functions because they are not constexpr
        for (auto i = 0; i < SAMPLING_INDEXES_SIZE; ++i)
        {
            const float distanceAhead = SAMPLING_DISTANCES[i];
            const float division = distanceAhead / Constants::TRACK_DETAIL_STEP;
            // Floor of the division
            const size_t index = static_cast<size_t>(division);
            // Float mod of the division
            const float remainingDistance = division - static_cast<float>(index);

            precomputedDistances[i] = remainingDistance;
            precomputedIndexes[i] = index;
        }
    }

    float precomputedDistances[SAMPLING_INDEXES_SIZE];
    size_t precomputedIndexes[SAMPLING_INDEXES_SIZE];
};

struct CarState
{
    constexpr static inline float MAX_SPEED = 40.0f;
    constexpr static inline float MAX_OMEGA = 40.0f;
    constexpr static inline unsigned int STATE_SIZE = 4 + 2 + 2 + 4 + 2 * SamplingIndexes::SAMPLING_INDEXES_SIZE;

    CarState() = default;

    float distanceFromRoad = 0.0f;
    std::array<float, 2> carVelocityRoadRef;
    float angleWithRoad = 0.0f;
    std::array<float, 2> wheelAngles;
    std::array<float, 4> wheelOmegas;
    float carOmega = 0.0f;
    float driftAngle = 0.0f;
    std::array<float, SamplingIndexes::SAMPLING_INDEXES_SIZE*2> pointsFurther;
    std::array<float, SamplingIndexes::SAMPLING_INDEXES_SIZE> debugPointsFurtherDistances;

    static CarState GenerateState(const Car& car, const Track::Path& path, unsigned int currentIndex, bool addDebugInfo = false, unsigned int carId = 0);

    void ExportToArray(std::array<float, STATE_SIZE>& outArray) const;

    std::string ToString();
};