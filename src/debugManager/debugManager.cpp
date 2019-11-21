#include "debugManager/debugManager.h"
#include "renderer/renderer.h"
#include "renderable/polygon.h"
#include <utils/utils.h>

constexpr float LINE_WIDTH = 1.0f;

#ifndef M_PI
#define M_PI 3.14159265359f
#endif

DebugManager::DebugFigure::~DebugFigure()
{
    if (renderable != nullptr)
    {
        Renderer* renderer = Renderer::GetInstance();
        renderer->RemoveRenderable(renderable->GetId());
        delete renderable;
    }
}

void DebugManager::Enable(bool enable)
{
    if (m_enabled && !enable)
        Clear();

    m_enabled = enable;
}

void DebugManager::Clear()
{
    m_mapDebugFigures.clear();
}

void DebugManager::Update()
{
    auto it = m_mapDebugFigures.begin();
    // Remove items that have no more time remaining
    while (it != m_mapDebugFigures.end())
    {
        if (--(it->second.frameTimeRemaining) < 0)
            m_mapDebugFigures.erase(it);
        else
            ++it;
    }
}

void DebugManager::DrawLine(const std::string& id, const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& color, int frameTime)
{
    if (!m_enabled)
        return;

    // Recycle previous item
    Polygon* polygon = nullptr;

    auto it = m_mapDebugFigures.find(id);
    if (it != m_mapDebugFigures.end())
    {
        polygon = dynamic_cast<Polygon*>(it->second.renderable);
        if (polygon == nullptr)
        {
            // Previous Id existed, but was not a polygon...
            m_mapDebugFigures.erase(it);
            it = m_mapDebugFigures.end();
        }
    }

    constexpr float length = 1.0f;

    if (polygon == nullptr)
    {

        std::vector<float> vertrices = {
            -LINE_WIDTH * 0.5f, 0.0f, 0.0f,
            LINE_WIDTH * 0.5f, 0.0f, 0.0f,
            LINE_WIDTH * 0.5f, length, 0.0f,
            -LINE_WIDTH * 0.5f, length, 0.0f
        };

        std::vector<unsigned int> indexes = { 0, 1, 2, 0, 2, 3 };

        polygon = new Polygon(vertrices, indexes, color);
    }
    else
    {
        polygon->GetColor() = color;
    }

    polygon->GetPosition() = p1;

    glm::vec3 line = p2 - p1;
    polygon->GetScale()[1] = glm::length(line) / length;
    //polygon->GetScale()[1] = 10.0f;
    glm::vec3& rotation = polygon->GetRotation();
    Utils::NormalizeWithEpsilonOnPlace(line);

    // Compute rotation matrix
    // https://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d
    glm::vec3 startVec(0.0f, 1.0f, 0.0f);
    glm::vec3 v = glm::cross(line, startVec);
    float cosine = glm::dot(line, startVec);

    if (cosine == -1.0f)
    {
        // It means that the vector points to (0.0f, -1.0f, 0.0f), which means a roation of Pi around Z
        polygon->GetRotation()[2] = M_PI;
    }
    else
    {
        glm::mat3 skewSymmetricV = { {0.0f, v[2], -v[1]}, {-v[2], 0.0f, v[0]}, {v[1], -v[0], 0.0f} };
        glm::mat3 identity = { {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} };
        glm::mat3 rotation = identity + skewSymmetricV + skewSymmetricV * skewSymmetricV * 1.0f / (1.0f + cosine);

        polygon->SetRoationFromMatrix(rotation);
    }

    if (it != m_mapDebugFigures.end())
    {
        it->second.frameTimeRemaining = frameTime;
    }
    else
    {
        Renderer::GetInstance()->AddRenderable(polygon);

        it = m_mapDebugFigures.emplace(id, DebugFigure()).first;
        it->second.id = id;
        it->second.renderable = polygon;
        it->second.frameTimeRemaining = frameTime;
    }
}