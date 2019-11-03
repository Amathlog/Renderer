#pragma once

#include <unordered_map>

class GLFWwindow;
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
    void ProcessInput();

    GLFWwindow* GetWindow() {return m_window;}
    bool RequestedClose();

    ~Renderer();

    void AddRenderable(const Renderable* renderable);
    void RemoveRenderable(unsigned int id);

private:
    Renderer() = default;
    static inline Renderer* ms_instance = nullptr;

    GLFWwindow* m_window = nullptr;

    using MapToRenderable = std::unordered_map<unsigned int, const Renderable*>;
    MapToRenderable m_mapToRenderable;
};