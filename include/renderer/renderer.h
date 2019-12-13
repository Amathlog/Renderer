#pragma once

#include <unordered_map>
#include <renderer/camera.h>
#include <functional>
#include <unordered_map>
#include <utils/singleton.h>

struct GLFWwindow;
class Shader;
class Renderable;

class Renderer : public Singleton<Renderer>
{
public:
    int Initialize(unsigned int width, unsigned int height);
    void Close();

    void Render();
    void UpdateCamera();
    void ProcessInput();

    GLFWwindow* GetWindow() {return m_window;}
    bool RequestedClose();

    Renderer(Token) : Singleton() {}
    ~Renderer();

    void AddRenderable(const Renderable* renderable);
    void RemoveRenderable(unsigned int id);

    const Camera& GetCamera() const { return m_camera; }
    Camera& GetCamera() { return m_camera; }

    bool upPressed = false;
    bool downPressed = false;
    bool leftPressed = false;
    bool rightPressed = false;

    void Enable(bool enable) {m_enable = enable;}
    bool IsEnabled() {return m_enable;}

    unsigned int RegisterInputCallback(int inputKey, std::function<void(int)> callback);
    void RemoveInputCallback(unsigned int id);
    void ClearInputCallbacks() { m_inputCallbacks.clear(); }

private:

    GLFWwindow* m_window = nullptr;

    using MapToRenderable = std::unordered_map<unsigned int, const Renderable*>;
    MapToRenderable m_mapToRenderable;
    Camera m_camera;

    bool m_enable = true;
    bool m_initialize = false;

    using MapIdCallback = std::unordered_map<unsigned int, std::pair<int, std::function<void(int)>>>;
    MapIdCallback m_inputCallbacks;
    unsigned int m_latestCallbackId = 0;
};