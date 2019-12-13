#include <racingGame/controllers/humanCarController.h>
#include <renderer/renderer.h>
#include <racingGame/car.h>
#include <GLFW/glfw3.h>

HumanCarController::HumanCarController(unsigned int stateInterval, unsigned int playerId)
    : CarController(stateInterval)
    , m_playerId(playerId)
{
    Renderer* renderer = Renderer::GetInstance();

    std::array<int, 4> keys;
    if (playerId == 0)
        keys = { GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT };
    else
        keys = { GLFW_KEY_I, GLFW_KEY_K, GLFW_KEY_J, GLFW_KEY_L };

    m_callbackIds[0] = renderer->RegisterInputCallback(keys[0], [&](int state) { m_upPressed = state == GLFW_PRESS; });
    m_callbackIds[1] = renderer->RegisterInputCallback(keys[1], [&](int state) { m_downPressed = state == GLFW_PRESS; });
    m_callbackIds[2] = renderer->RegisterInputCallback(keys[2], [&](int state) { m_leftPressed = state == GLFW_PRESS; });
    m_callbackIds[3] = renderer->RegisterInputCallback(keys[3], [&](int state) { m_rightPressed = state == GLFW_PRESS; });
}


HumanCarController::~HumanCarController()
{
    Renderer* renderer = Renderer::GetInstance();
    for (auto id : m_callbackIds)
        renderer->RemoveInputCallback(id);
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