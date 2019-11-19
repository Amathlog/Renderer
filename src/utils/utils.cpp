#include "utils/utils.h"
#include <cmath>

constexpr float EPSILON = 1e-6;

void Utils::NormalizeWithEpsilon(glm::vec2& v)
{
    float length = glm::length(v);
    if (length < EPSILON)
    {
        v.x = 0.0f;
        v.y = 0.0f;
    }
    else
    {
        v /= length;
    }
}

glm::vec2 Utils::NormalizeWithEpsilon(const glm::vec2& v)
{
    return NormalizeWithEpsilon(glm::vec2(v));
}

glm::vec2 Utils::GetSide(const glm::vec2& v)
{
    return glm::vec2(v.y, -v.x);
}

float Utils::GetAngle(const glm::vec2 v1, const glm::vec2 v2)
{
    float dotProduct = glm::dot(v1, v2);
    float sign = v1.x * v2.y - v1.y * v2.x; // Cross product
    return std::signbit(sign) ? -std::acos(dotProduct) : std::acos(dotProduct);
}