#pragma once

#include <glm/glm.hpp>
#include <Box2D/Box2D.h>
#include <array>

namespace Utils
{
    template<typename T, size_t N>
    struct RingBuffer
    {
        RingBuffer()
        {
            buffer.fill(0.0f);
        }

        void push_back(const T& item)
        {
            buffer[index] = item;
            index = (index + 1) % N;
            currentSize = currentSize == N ? N : currentSize + 1;
        }

        void clear()
        {
            buffer.fill(0.0f);
            index = 0;
        }

        size_t size()
        {
            return currentSize;
        }

        size_t index = 0;
        size_t currentSize = 0;
        std::array<T, N> buffer;
    };


    void NormalizeWithEpsilonOnPlace(glm::vec2& v);
    glm::vec2 NormalizeWithEpsilon(const glm::vec2& v);

    void NormalizeWithEpsilonOnPlace(glm::vec3& v);
    glm::vec3 NormalizeWithEpsilon(const glm::vec3& v);

    glm::vec2 GetSide(const glm::vec2& v);
    float GetAngle(const glm::vec2& v1, const glm::vec2& v2);
    float GetAngle(const glm::vec3& v1, const glm::vec3& v2);

    glm::vec2 Convertb2Toglm(const b2Vec2& v);

    glm::mat4 GetRotationMatrixFromP1toP2(const glm::vec3& p1, glm::vec3& p2);
}
