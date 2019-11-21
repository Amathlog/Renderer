#include "renderable/renderable.h"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/string_cast.hpp"

void Renderable::AddChild(Renderable* child)
{
    if (child == nullptr)
        return;
    m_children.emplace(child->GetId(), child);
}

Renderable* Renderable::GetChild(unsigned int id)
{
    auto it = m_children.find(id);
    return it != m_children.end() ? it->second : nullptr;
}

void Renderable::RemoveChild(unsigned int id)
{
    m_children.erase(id);
}

void Renderable::Draw(const glm::mat4& mvp) const{
    if (m_shader == nullptr)
        return;

    glm::mat4 transform = GetTransform();
    glm::mat4 new_mvp = mvp * transform;
    InternalDraw(new_mvp);

    for (auto child : m_children)
    {
        child.second->Draw(new_mvp);
    }
}

glm::mat4 Renderable::GetTransform() const
{
    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, m_position);
    transform = glm::rotate(transform, m_rotation[2], glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::rotate(transform, m_rotation[1], glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, m_rotation[0], glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::scale(transform, m_scale);
    return transform;
}

void Renderable::SetRoationFromMatrix(const glm::mat4& mat)
{
    // https://www.learnopencv.com/rotation-matrix-to-euler-angles/

    float sy = std::sqrt(mat[0][0] * mat[0][0] + mat[1][0] * mat[1][0]);

    bool singular = sy < 1e-6; // If

    float x, y, z;
    if (!singular)
    {
        x = std::atan2(mat[2][1], mat[2][2]);
        y = std::atan2(-mat[2][0], sy);
        z = std::atan2(mat[1][0], mat[0][0]);
    }
    else
    {
        x = std::atan2(-mat[1][2], mat[1][1]);
        y = std::atan2(-mat[2][0], sy);
        z = 0;
    }
    m_rotation = glm::vec3(x, y, z);
}
