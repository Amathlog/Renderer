#pragma once

#include "renderable/renderable.h"
#include <vector>

class Triangle : public Renderable
{
public:
    // Data is 3 vertrices and 3 colors
    // v1 c1 v2 c2 v3 c3 => 18 floats
    Triangle(const std::vector<float>& data);
    virtual ~Triangle();

    virtual void CreateShader() override;
    virtual void Draw(const glm::mat4& camera) const override;

protected:
    unsigned int m_VBO;
    unsigned int m_VAO;
};