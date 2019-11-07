#pragma once

#include <glm.hpp>

class Camera
{
public:
    struct OrthographicParams
    {
        float left;
        float right;
        float bottom;
        float top;
        float near;
        float far;

        glm::mat4 GetMatrix() const;
    };

    struct PerspectiveParams
    {
        float fov;
        float ratio;
        float near;
        float far;

        glm::mat4 GetMatrix() const;
    };

    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetViewMatrix() const;

    void SetPosition(const glm::vec3& pos) {m_position = pos;}
    void SetDirection(const glm::vec3& dir);
    void SetUp(const glm::vec3& up);

    OrthographicParams& GetOrthographicParams() {return m_orthgraphicParams;}
    const OrthographicParams& GetOrthographicParams() const {return m_orthgraphicParams;}

    PerspectiveParams& GetPerspectiveParams() {return m_perspectiveParams;}
    const PerspectiveParams& GetPerspectiveParams() const {return m_perspectiveParams;}

    void SetIsPespective() {m_isPerspective = true;}
    void SetIsOrthographic() {m_isPerspective = false;}

private:
    OrthographicParams m_orthgraphicParams;
    PerspectiveParams m_perspectiveParams;
    bool m_isPerspective;
    glm::vec3 m_position;
    glm::vec3 m_direction;
    glm::vec3 m_up;
};