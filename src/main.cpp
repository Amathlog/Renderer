#include "renderer/renderer.h"
#include "renderable/triangle.h"
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

//#define PROFILING

int main()
{
    Renderer* renderer = Renderer::GetInstance();
    int errorCode = renderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);
    if(errorCode != 0)
        return errorCode;

    // Create a triangle and add it to the renderer
    std::vector<float> vertices = {
        // positions         // colors
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
    };
    Triangle triangle(vertices);
    renderer->AddRenderable(&triangle);

    int64_t deltaTimeUS = static_cast<int64_t>(floorf(1000000.0f / FPS));

#ifdef PROFILING
    int64_t count = 0;
    int64_t sumTime = 0;
#endif // PROFILING

    while(!renderer->RequestedClose())
    {
        auto lastTickTime = std::chrono::high_resolution_clock::now();

        // Update the physics
        triangle.SetTransform( glm::rotate((float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f)));

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
