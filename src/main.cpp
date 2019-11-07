#include "renderer/renderer.h"
#include "renderable/triangle.h"
#include "renderable/polygon.h"
#include <vector>
#include <chrono> 
#include <cmath>
#include <thread>

#include "glm.hpp"
#include "gtx/transform.hpp"

#include "GLFW/glfw3.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FPS 60

// #define PROFILING

constexpr float LAYER_CAR_Z = 0.5f;
constexpr float SCALE_CAR = 0.02f;

constexpr std::initializer_list<float> car_vertrices = {
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

constexpr std::initializer_list<unsigned int> car_indexes = {
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

int main()
{
    Renderer* renderer = Renderer::GetInstance();
    int errorCode = renderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);
    if(errorCode != 0)
        return errorCode;

    // Setup the camera
    Camera& camera = renderer->GetCamera();
    camera.SetIsOrthographic();
    camera.SetPosition(glm::vec3(0.0f, 0.0f, -1.0f));
    camera.SetDirection(glm::vec3(0.0f, 0.0f, 1.0f));
    camera.SetUp(glm::vec3(0.0f, 1.0f, 0.0f));
    Camera::OrthographicParams& params = camera.GetOrthographicParams();
    params = {-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f};

    // Create a triangle and add it to the renderer
    // std::vector<float> vertices = {
    //     // positions 
    //     0.5f, -0.5f, 0.5f,
    //     -0.5f, -0.5f, 0.5f,
    //     0.0f,  0.5f, 0.5f,
    // };
    // std::vector<unsigned int> indexes = {
    //     0, 1, 2
    // };
    // Polygon triangle(vertices, indexes, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    // renderer->AddRenderable(&triangle);

    // Create Car
    std::vector<float> vertrices = car_vertrices;
    std::vector<unsigned int> indexes = car_indexes;
    Polygon car(vertrices, indexes, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    renderer->AddRenderable(&car);

    int64_t deltaTimeUS = static_cast<int64_t>(floorf(1000000.0f / FPS));

#ifdef PROFILING
    int64_t count = 0;
    int64_t sumTime = 0;
#endif // PROFILING

    while(!renderer->RequestedClose())
    {
        auto lastTickTime = std::chrono::high_resolution_clock::now();

        // Update the physics
        // triangle.SetTransform( glm::rotate((float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f)));
        car.SetTransform(glm::scale(glm::vec3(SCALE_CAR, SCALE_CAR, 1.0f)));

        // Do the rendering/input processing
        renderer->ProcessInput();
        renderer->Render();
        
        // Reach FPS target
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto difference = currentTime - lastTickTime;
        auto renderTime = std::chrono::duration_cast<std::chrono::microseconds>(difference).count();
#ifdef PROFILING
        sumTime += renderTime;
        if(++count == 60)
        {
            std::cout << "Mean render time: " << sumTime / 60 << "us" << std::endl;
            count = 0;
            sumTime = 0;
        }
#endif // PROFILING
        if (renderTime < deltaTimeUS)
            std::this_thread::sleep_for(std::chrono::microseconds(deltaTimeUS - renderTime));
        else
            std::cout << "This frame took too much time... " << renderTime << "us" << std::endl; 
    }
    renderer->Close();
    return 0;
}
