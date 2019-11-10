#pragma once

#include <vector>

namespace Constants
{
    constexpr float LAYER_BACKGROUND_Z = 1.0f;
    constexpr float LAYER_TRACK_Z = 0.9f;
    constexpr float LAYER_PARTICLES = 0.6f;
    constexpr float LAYER_CAR_Z = 0.5f;
    constexpr float LAYER_WHEELS = 0.4f;

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
    constexpr float WHEEL_W = 14.0f;
    constexpr float SCALE_CAR = 0.02f;

    constexpr float WHEEL_COLOR[] = {0.0f,0.0f,0.0f};
    constexpr float WHEEL_WHITE[] = {0.3f,0.3f,0.3f};
    constexpr float MUD_COLOR[] = {0.4f,0.4f,0.0f};

    constexpr float ENGINE_POWER = 100000000.0f * SCALE_CAR * SCALE_CAR;
    constexpr float WHEEL_MOMENT_OF_INERTIA = 4000.0f * SCALE_CAR * SCALE_CAR;
    constexpr float FRICTION_LIMIT = 1000000.0f * SCALE_CAR * SCALE_CAR;     // friction ~= mass ~= size^2 (calculated implicitly using density)

    constexpr float WHEELPOS[] = {
        -55.0f, 80.0f, 
        55.0f, 80.0f,
        -55.0f,-82.0f,
        55.0f,-82.0f
    };

    constexpr unsigned int CHECKPOINTS = 12;
    constexpr float SCALE_TRACK = 1.0f / 6.0f;
    constexpr float TRACK_RAD = 900.0f * SCALE_TRACK;
    constexpr float PLAYFIELD = 2000.0f * SCALE_TRACK;
    constexpr float ZOOM = 2.7f;
    constexpr float TRACK_DETAIL_STEP = 21.0f * SCALE_TRACK;
    constexpr float TRACK_TURN_RATE = 0.31f;
    constexpr float TRACK_WIDTH = 40.0f * SCALE_TRACK;
    constexpr float BORDER = 8.0f * SCALE_TRACK;
    constexpr unsigned int BORDER_MIN_COUNT = 6;

    constexpr float ROAD_COLOR[] = {0.4, 0.4, 0.4};
};