#include "renderer/renderer.h"
#include "racingGame/gameManager.h"

#include <vector>
#include <chrono> 
#include <cmath>
#include <thread>
#include <iostream>
#include <cstring>

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"


#include "GLFW/glfw3.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FPS 60

#define PROFILING

int main(int argc, char** argv)
{
    bool enableRendering = true;
    if (argc > 1)
        enableRendering = strcmp(argv[1], "--no-render") != 0;
     
    Renderer* renderer = Renderer::GetInstance();
    renderer->Enable(enableRendering);
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

    auto startingTime = std::chrono::high_resolution_clock::now();

    while(!renderer->IsEnabled() || !renderer->RequestedClose())
    {
        auto lastTickTime = std::chrono::high_resolution_clock::now();

        int64_t time = std::chrono::duration_cast<std::chrono::milliseconds>(lastTickTime - startingTime).count();
        time = time + 1 - 1;

        // static bool test = false;
        // if (time > 2000 && !test)
        // {
        //     test = true;
        //     std::cout << "Test" << std::endl;
        //     gameManager.Reset();
        //     break;
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

        // Do the rendering/input processing, if necessary
        if (enableRendering)
        {
            renderer->ProcessInput();
            renderer->Render();
        }
        
        // Reach FPS target
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto difference = currentTime - lastTickTime;
        auto renderTime = std::chrono::duration_cast<std::chrono::microseconds>(difference).count();
#ifdef PROFILING
        sumTimeRendering += renderTime;
        if(++count == 60)
        {
            if (enableRendering)
                std::cout << "Mean render time: " << sumTimeRendering / 60 << "us" << std::endl;
            std::cout << "Mean physics time: " << sumTimePhysics / 60 << "us" << std::endl;
            count = 0;
            sumTimePhysics = 0;
            sumTimeRendering = 0;
        }
#endif // PROFILING
        // Do not wait if we don't render
        if (enableRendering)
        {
            if (renderTime < deltaTimeUS)
                std::this_thread::sleep_for(std::chrono::microseconds(deltaTimeUS - renderTime));
            else
                std::cout << "This frame took too much time... " << renderTime << "us" << std::endl; 
        }
    }
    renderer->Close();
    return 0;
}
