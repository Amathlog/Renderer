#include <utils/utils.h>
#include <cmath>
#include <glm/gtx/transform.hpp>

constexpr float EPSILON = 1e-6f;

#ifndef M_PI
#define M_PI 3.14159265359f
#endif 

void Utils::NormalizeWithEpsilonOnPlace(glm::vec2& v)
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
    glm::vec2 copy(v);
    NormalizeWithEpsilonOnPlace(copy);
    return copy;
}

void Utils::NormalizeWithEpsilonOnPlace(glm::vec3& v)
{
    float length = glm::length(v);
    if (length < EPSILON)
    {
        v.x = 0.0f;
        v.y = 0.0f;
        v.z = 0.0f;
    }
    else
    {
        v /= length;
    }
}

glm::vec3 Utils::NormalizeWithEpsilon(const glm::vec3& v)
{
    glm::vec3 copy(v);
    NormalizeWithEpsilonOnPlace(copy);
    return copy;
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

float Utils::GetAngle(const glm::vec3 v1, const glm::vec3 v2)
{
    float dotProduct = glm::dot(v1, v2);
    glm::vec3 normal = glm::cross(v1, v2);
    float sign = v1.x * v2.y - v1.y * v2.x; // Cross product
    return std::signbit(sign) ? -std::acos(dotProduct) : std::acos(dotProduct);
}

glm::vec2 Utils::Convertb2Toglm(const b2Vec2& v)
{
    return glm::vec2(v.x, v.y);
}

glm::mat4 Utils::GetRotationMatrixFromP1toP2(const glm::vec3& p1, glm::vec3& p2)
{
    // https://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d
    glm::vec3 p1_n = Utils::NormalizeWithEpsilon(p1);
    glm::vec3 p2_n = Utils::NormalizeWithEpsilon(p2);
    glm::vec3 v = glm::cross(p1_n, p2_n);
    float cosine = glm::dot(p1_n, p2_n);

    if (cosine == -1.0f)
    {
        // It means that the vector points to (0.0f, -1.0f, 0.0f), which means a rotation of Pi around Z
        return glm::rotate(M_PI, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    else
    {
        glm::mat4 skewSymmetricV = { {0.0f, v[2], -v[1], 0.0f}, {-v[2], 0.0f, v[0], 0.0f}, {v[1], -v[0], 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f } };
        glm::mat4 identity(1.0f);
        glm::mat4 rotation = identity + skewSymmetricV + skewSymmetricV * skewSymmetricV * 1.0f / (1.0f + cosine);

        return rotation;
    }
}
