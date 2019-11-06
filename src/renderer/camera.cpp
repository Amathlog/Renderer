#include "renderer/camera.h"

#include "gtx/transform.hpp"

glm::mat4 Camera::PerspectiveParams::GetMatrix() const
{
    return glm::perspective(fov, ratio, near, far);
}

glm::mat4 Camera::OrthographicParams::GetMatrix() const
{
    return glm::ortho(left, right, bottom, top, near, far);
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    return m_isPerspective ? m_perspectiveParams.GetMatrix() : m_orthgraphicParams.GetMatrix();
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(m_position, m_position + m_direction, m_up);
}

void Camera::SetDirection(const glm::vec3& dir)
{
    m_direction = glm::normalize(dir);
}

void Camera::SetUp(const glm::vec3& up)
{
    m_up = glm::normalize(up);
}