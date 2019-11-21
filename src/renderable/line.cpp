#include "renderable/line.h"

Line::Line(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& color)
    : Renderable()
    , m_color(color)
{
    CreateShader();

    float data[6] = {p1.x, p1.y, p1.z, p2.x, p2.y, p2.z};

    glGenBuffers(1, &m_VBO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Line::~Line()
{
    glDeleteBuffers(1, &m_VBO);
}

void Line::CreateShader()
{
    m_shader = ShaderManager::GetInstance()->LoadShader("polygon_shader.vs", "polygon_shader.fs");
}

void Line::UpdatePoints(const glm::vec3& p1, const glm::vec3& p2)
{
    float data[6] = {p1.x, p1.y, p1.z, p2.x, p2.y, p2.z};

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glSubBufferData(GL_ARRAY_BUFFER, (void*)0, 6 * sizeof(float), data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Line::InternalDraw(const glm::mat4& mvp) const
{
    if (m_shader == nullptr)
        return;
    
    m_shader->Use();

    m_shader->SetMat4("MVP", mvp);
    m_shader->SetVec4("ourColor", m_color);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glDrawArrays(GL_LINES,  0, 2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_shader->Disable();
}
}
