#include "renderer/renderer.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shaders/shaders.h"
#include "renderable/renderable.h"

namespace {
    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    } 
}

int Renderer::Initialize(unsigned int width, unsigned int height)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    m_window = glfwCreateWindow(width, height, "Renderer", nullptr, nullptr);
    if (m_window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    } 

    framebuffer_size_callback(m_window, width, height);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    return 0;
}

void Renderer::Close()
{
    if (m_window != nullptr)
    {
        glfwTerminate();
        m_window = nullptr;
    }
}

Renderer::~Renderer()
{
    Close();
    delete ms_instance;
    ms_instance = nullptr;
}

void Renderer::Render()
{
    if (m_window == nullptr)
        return;

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (auto pair : m_mapToRenderable)
    {
        pair.second->Draw();
    }

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void Renderer::ProcessInput()
{
    if (m_window == nullptr)
        return;

    if(glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, true);
}

bool Renderer::RequestedClose()
{
    if (m_window == nullptr)
        return true;
    
    return glfwWindowShouldClose(m_window);
}

void Renderer::AddRenderable(const Renderable* renderable)
{
    if (renderable == nullptr)
        return;
    m_mapToRenderable.emplace(renderable->GetId(), renderable);
}

void Renderer::RemoveRenderable(unsigned int id)
{
    m_mapToRenderable.erase(id);
}