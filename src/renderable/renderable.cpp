#include "renderable/renderable.h"
#include "gtx/transform.hpp"
#include "gtx/string_cast.hpp"

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
    transform = glm::scale(transform, m_scale);
    transform = glm::rotate(transform, m_rotation[0], glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, m_rotation[1], glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, m_rotation[2], glm::vec3(0.0f, 0.0f, 1.0f));
    return glm::translate(transform, m_position);
}
