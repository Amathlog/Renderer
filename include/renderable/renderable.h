#pragma once

#include <glm.hpp>
#include "shaders/shaders.h"
#include <iostream>

class Camera;

class Renderable
{
public:
    Renderable()
    {
        m_ID = ++CURRENT_ID;
    }

    virtual ~Renderable()
    {
        if (m_shader != nullptr)
        {
            delete m_shader;
            m_shader = nullptr;
        }
    }

    virtual void CreateShader() = 0;
    virtual void Draw(const Camera& camera) const = 0;

    const glm::mat4& GetTransform() const {return m_transform;}
    void SetTransform(const glm::mat4& transform) {m_transform = transform;}

    unsigned int GetId() const {return m_ID;}
protected:
    Shader* m_shader = nullptr;
    glm::mat4 m_transform = glm::mat4(1.0f);
    unsigned int m_ID;
private:
    static inline unsigned int CURRENT_ID = 0;
};