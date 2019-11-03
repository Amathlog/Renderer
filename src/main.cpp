#include "renderer/renderer.h"
#include "renderable/triangle.h"
#include <vector>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

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
    
    while(!renderer->RequestedClose())
    {
        renderer->ProcessInput();
        renderer->Render();
    }
    renderer->Close();
    return 0;
}
