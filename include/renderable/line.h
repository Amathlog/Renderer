#pragma once

#include "renderable/renderable.h"
#include "glm/glm.hpp"

class Line : public Renderable
{
public:
    Line(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& color);
    virtual ~Line();

    virtual void CreateShader() override;

    glm::vec4& GetColor() {return m_color;}
    const glm::vec4& GetColor() const {return m_color;}

    void UpdatePoints(const glm::vec3& p1, const glm::vec3& p2);

protected:
    virtual void InternalDraw(const glm::mat4& mvp) const override;
    
    unsigned int m_VBO;
    unsigned int m_VAO;
    unsigned int m_EBO;
    glm::vec4 m_color;
};