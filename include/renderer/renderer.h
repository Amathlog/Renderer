#pragma once

#include <unordered_map>
#include <renderer/camera.h>
#include <functional>
#include <unordered_map>

struct GLFWwindow;
class Shader;
class Renderable;

class Renderer
{
public:
    static Renderer* GetInstance()
    {
        if (ms_instance == nullptr)
            ms_instance = new Renderer();
        return ms_instance;
    }

    int Initialize(unsigned int width, unsigned int height);
    void Close();

    void Render();
    void UpdateCamera();
    void ProcessInput();

    GLFWwindow* GetWindow() {return m_window;}
    bool RequestedClose();

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

    void RegisterInputCallback(int inputKey, std::function<void(int)> callback);
    void ClearInputCallbacks() { m_inputCallbacks.clear(); }

private:
    Renderer() = default;
    static inline Renderer* ms_instance = nullptr;

    GLFWwindow* m_window = nullptr;

    using MapToRenderable = std::unordered_map<unsigned int, const Renderable*>;
    MapToRenderable m_mapToRenderable;
    Camera m_camera;

    bool m_enable = true;
    bool m_initialize = false;

    using VectorInputCallbacks = std::vector<std::pair<int, std::function<void(int)>>>;
    VectorInputCallbacks m_inputCallbacks;
};