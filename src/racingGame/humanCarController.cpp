#include "racingGame/humanCarController.h"
#include "renderer/renderer.h"
#include "racingGame/car.h"
#include "GLFW/glfw3.h"

HumanCarController::HumanCarController(unsigned int stateInterval)
    : CarController(stateInterval)
{
    Renderer* renderer = Renderer::GetInstance();

    renderer->RegisterInputCallback(GLFW_KEY_UP, [&](int state)
    {
        m_upPressed = state == GLFW_PRESS;
    }
    );

    renderer->RegisterInputCallback(GLFW_KEY_DOWN, [&](int state)
    {
        m_downPressed = state == GLFW_PRESS;
    }
    );

    renderer->RegisterInputCallback(GLFW_KEY_LEFT, [&](int state)
    {
        m_leftPressed = state == GLFW_PRESS;
    }
    );

    renderer->RegisterInputCallback(GLFW_KEY_RIGHT, [&](int state)
    {
        m_rightPressed = state == GLFW_PRESS;
    }
    );
}


HumanCarController::~HumanCarController()
{
    Renderer::GetInstance()->ClearInputCallbacks();
}

void HumanCarController::Update(const CarState&, Car& car)
{
    if (m_upPressed)
        car.Gas(1.0f);
    else
        car.Gas(0.0f);

    if (m_downPressed)
        car.Brake(0.8f);
    else
        car.Brake(0.0f);

    if (m_leftPressed)
        car.Steer(-1.0f);
    else if (m_rightPressed)
        car.Steer(1.0f);
    else
        car.Steer(0.0f);
}