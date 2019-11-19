#pragma once

#include <array>
#include "racingGame/track.h"


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

struct CarState
{
    constexpr static inline size_t SAMPLING_INDEXES_SIZE = 4;
    constexpr static inline std::array<float, SAMPLING_INDEXES_SIZE> SAMPLING_DISTANCES = {3.0f, 5.0f, 10.0f, 25.0f};
    constexpr static inline float MAX_SPEED = 40.0f;

    float distanceFromRoad;
    std::array<float, 2> carVelocityRoadRef;
    float angleWithRoad;
    std::array<float, 2> wheelAngles;
    std::array<float, 4> wheelOmegas;
    float carOmega;
    bool isDrifting;
    float driftAngle;
    std::array<float, SAMPLING_INDEXES_SIZE*2> pointsFurtherCarRef;
    std::array<float, SAMPLING_INDEXES_SIZE*2> pointsFurtherRoadRef;

    static CarState GenerateState(const Car& car, const Track::Path& path, unsigned int currentIndex);
};