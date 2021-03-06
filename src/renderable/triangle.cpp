#include <renderable/triangle.h>
#include <renderer/camera.h>
#include <shaders/shaders.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtx/string_cast.hpp> 

#include <iostream>

Triangle::Triangle(const std::vector<float>& data)
    : Renderable()
{
    if (data.size() != 18)
    {
        std::cerr << "ERROR WHILE BUILDING TRIANGLE, FORMAT NOT RESPECTED" << std::endl;
    }

    CreateShader();

    glGenVertexArrays(1, &m_VAO); 
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
 
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); 

    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    glBindVertexArray(0); 
}

Triangle::~Triangle()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void Triangle::CreateShader()
{
    m_shader = new Shader("/home/adrien/Programming/Renderer/shaders/shader.vs",
                         "/home/adrien/Programming/Renderer/shaders/shader.fs");
    // m_shader = new Shader("C:\\Users\\adrie\\Documents\\Programming\\Renderer\\shaders\\shader.vs",
    //     "C:\\Users\\adrie\\Documents\\Programming\\Renderer\\shaders\\shader.fs");
}

void Triangle::InternalDraw(const glm::mat4& mvp) const
{
    if (m_shader == nullptr)
        return;

    m_shader->Use();

    m_shader->SetMat4("MVP", mvp);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}