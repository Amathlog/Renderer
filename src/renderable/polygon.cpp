#include <renderable/polygon.h>
#include <renderer/camera.h>
#include <shaders/shaders.h>
#include <shaders/shaderManager.h>
#include <renderer/renderer.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> 

#include <iostream>
#include <filesystem>

Polygon::Polygon(const std::vector<float>& vertices, 
                 const std::vector<unsigned int>& indexes, 
                 const glm::vec4& color,
                 Shader* specificShader)
    : Renderable()
{
    if (specificShader != nullptr)
        m_shader = specificShader;
    else
        CreateShader();

    m_nbVertices = (unsigned int)indexes.size();
    m_color = color;

    glGenVertexArrays(1, &m_VAO); 
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);
 
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

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
    m_shader = ShaderManager::GetInstance()->LoadShader("polygon_shader.vs", "polygon_shader.fs");
}

void Polygon::InternalDraw(const glm::mat4& mvp) const
{
    if (m_shader == nullptr)
        return;
    
    m_shader->Use();

    m_shader->SetMat4("MVP", mvp);
    m_shader->SetVec4("ourColor", m_color);

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_nbVertices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    m_shader->Disable();
}