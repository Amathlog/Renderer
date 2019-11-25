#include <renderable/line.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shaders/shaderManager.h>

Line::Line(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& color)
    : Renderable()
    , m_color(color)
{
    CreateShader();

    float data[6] = {p1.x, p1.y, p1.z, p2.x, p2.y, p2.z};


    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Line::~Line()
{
    glDeleteBuffers(1, &m_VBO);
    glDeleteVertexArrays(1, &m_VAO);
}

void Line::CreateShader()
{
    m_shader = ShaderManager::GetInstance()->LoadShader("polygon_shader.vs", "polygon_shader.fs");
}

void Line::UpdatePoints(const glm::vec3& p1, const glm::vec3& p2)
{
    float data[6] = {p1.x, p1.y, p1.z, p2.x, p2.y, p2.z};

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Line::InternalDraw(const glm::mat4& mvp) const
{
    if (m_shader == nullptr)
        return;
    
    m_shader->Use();

    m_shader->SetMat4("MVP", mvp);
    m_shader->SetVec4("ourColor", m_color);

    glLineWidth(0.5f);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_LINES,  0, 2);
    glBindVertexArray(0);

    m_shader->Disable();
}

