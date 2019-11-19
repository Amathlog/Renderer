#pragma once

#include <glm/glm.hpp>

namespace Utils
{
    void NormalizeWithEpsilon(glm::vec2& v);
    glm::vec2 NormalizeWithEpsilon(const glm::vec2& v);

    glm::vec2 GetSide(const glm::vec2& v);
    float GetAngle(const glm::vec2 v1, const glm::vec2 v2);
}
