#include "renderable/polygon.h"
#include "renderer/camera.h"
#include "shaders/shaders.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <gtc/type_ptr.hpp>
#include <gtx/string_cast.hpp> 

#include <iostream>

Polygon::Polygon(const std::vector<float>& vertrices, const std::vector<unsigned int>& indexes, const glm::vec4& color)
    : Renderable()
{
    CreateShader();

    m_nbVertrices = indexes.size();
    m_color = color;

    glGenVertexArrays(1, &m_VAO); 
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);
 
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertrices.size() * sizeof(float), vertrices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(unsigned int), indexes.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); 

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 

    glBindVertexArray(0); 
}

Polygon::~Polygon()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

void Polygon::CreateShader()
{
    m_shader = new Shader("/home/adrien/Programming/Renderer/shaders/polygon_shader.vs",
                         "/home/adrien/Programming/Renderer/shaders/polygon_shader.fs");
    // m_shader = new Shader("C:\\Users\\adrie\\Documents\\Programming\\Renderer\\shaders\\polygon_shader.vs",
    //     "C:\\Users\\adrie\\Documents\\Programming\\Renderer\\shaders\\polygon_shader.fs");
}

void Polygon::Draw(const glm::mat4& mvp) const
{
    if (m_shader == nullptr)
        return;

    m_shader->Use();

    glm::mat4 new_MVP = mvp * m_transform;

    m_shader->SetMat4("MVP", new_MVP);
    m_shader->SetVec4("ourColor", m_color);

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_nbVertrices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}