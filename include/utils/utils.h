#pragma once

#include <glm/glm.hpp>
#include <Box2D/Box2D.h>

namespace Utils
{
    void NormalizeWithEpsilonOnPlace(glm::vec2& v);
    glm::vec2 NormalizeWithEpsilon(const glm::vec2& v);

    void NormalizeWithEpsilonOnPlace(glm::vec3& v);
    glm::vec3 NormalizeWithEpsilon(const glm::vec3& v);

    glm::vec2 GetSide(const glm::vec2& v);
    float GetAngle(const glm::vec2 v1, const glm::vec2 v2);
    float GetAngle(const glm::vec3 v1, const glm::vec3 v2);

    glm::vec2 Convertb2Toglm(const b2Vec2& v);
}
