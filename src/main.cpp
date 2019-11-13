#include "renderer/renderer.h"
#include "racingGame/gameManager.h"

#include <vector>
#include <chrono> 
#include <cmath>
#include <thread>
#include <iostream>

#include "glm.hpp"
#include "gtx/transform.hpp"


#include "GLFW/glfw3.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FPS 60

// #define PROFILING

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
    params = {-30.0f, 30.0f, -30.0f, 30.0f, 0.1f, 10.0f};

    int64_t deltaTimeUS = static_cast<int64_t>(floorf(1000000.0f / FPS));

    GameManager gameManager;
    gameManager.Initialize();
    gameManager.SetNumberOfPlayers(1);
    gameManager.Reset();

#ifdef PROFILING
    int64_t count = 0;
    int64_t sumTimeRendering = 0;
    int64_t sumTimePhysics = 0;
#endif // PROFILING

    while(!renderer->RequestedClose())
    {
        auto lastTickTime = std::chrono::high_resolution_clock::now();

        float time = (float)glfwGetTime();

        // static bool test = false;
        // if (time > 2.0f && !test)
        // {
        //     test = true;
        //     std::cout << "Test" << std::endl;
        //     gameManager.Reset();
        // }

        // Update the physics
#ifdef PROFILING
        auto tickTimePhysics = std::chrono::high_resolution_clock::now();
#endif // PROFILING

        gameManager.Step(1.0f / FPS);

#ifdef PROFILING
        auto differencePhysics = std::chrono::high_resolution_clock::now() - tickTimePhysics;
        sumTimePhysics += std::chrono::duration_cast<std::chrono::microseconds>(differencePhysics).count();
#endif // PROFILING

        // Do the rendering/input processing
        renderer->ProcessInput();
        renderer->Render();
        
        // Reach FPS target
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto difference = currentTime - lastTickTime;
        auto renderTime = std::chrono::duration_cast<std::chrono::microseconds>(difference).count();
#ifdef PROFILING
        sumTimeRendering += renderTime;
        if(++count == 60)
        {
            std::cout << "Mean render time: " << sumTimeRendering / 60 << "us" << std::endl;
            std::cout << "Mean physics time: " << sumTimePhysics / 60 << "us" << std::endl;
            count = 0;
            sumTimePhysics = 0;
            sumTimeRendering = 0;
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
