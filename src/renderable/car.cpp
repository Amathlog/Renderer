#include "renderable/car.h"

#include <vector>

constexpr float LAYER_CAR_Z = 0.5f;

constexpr std::initializer_list<float> HULL_VERTRICES = {
    // HULL_1
    60.0f, 130.0f, LAYER_CAR_Z,
    -60.0f, 130.0f, LAYER_CAR_Z,
    60.0f, 110.0f, LAYER_CAR_Z,
    -60.0f, 110.0f, LAYER_CAR_Z,
    // HULL_2
    -15.0f, 120.0f, LAYER_CAR_Z,
    15.0f, 120.0f, LAYER_CAR_Z,
    20.0f, 20.0f, LAYER_CAR_Z,
    -20.0f, 20.0f, LAYER_CAR_Z,
    // HULL_3
    25.0f, 20.0f, LAYER_CAR_Z,
    50.0f, -10.0f, LAYER_CAR_Z,
    50.0f, -40.0f, LAYER_CAR_Z,
    20.0f, -90.0f, LAYER_CAR_Z,
    -20.0f, -90.0f, LAYER_CAR_Z,
    -50.0f, -40.0f, LAYER_CAR_Z,
    -50.0f, -10.0f, LAYER_CAR_Z,
    -25.0f, 20.0f, LAYER_CAR_Z,
    0.0f, -30.0f, LAYER_CAR_Z, // Center
    // HULL_4
    -50.0f,-120.0f, LAYER_CAR_Z,
    50.0f, -120.0f, LAYER_CAR_Z,
    50.0f, -90.0f, LAYER_CAR_Z,
    -50.0f, -90.0f, LAYER_CAR_Z
};

constexpr std::initializer_list<unsigned int> HULL_INDEXES = {
    // HULL_1
    0, 1, 2,
    1, 2, 3,
    // HULL_2
    4, 5, 6,
    6, 7, 4,
    // HULL_3
    8, 9, 16,
    9, 10, 16,
    10, 11, 16,
    11, 12, 16,
    12, 13, 16,
    13, 14, 16,
    14, 15, 16,
    15, 8, 16,
    // HULL_4
    17, 18, 19,
    19, 20, 17
};

constexpr float WHEEL_R = 27.0f;
constexpr float WHEEL_W = 14.0F;
constexpr float SCALE_CAR = 0.02f;

Car::Car(const glm::vec4& color)
    : Polygon(std::vector<float>(HULL_VERTRICES), std::vector<unsigned int>(HULL_INDEXES), color)
{
    
}