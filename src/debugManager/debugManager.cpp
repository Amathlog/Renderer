#include "debugManager/debugManager.h"
#include "renderer/renderer.h"
#include "renderable/polygon.h"
#include <utils/utils.h>
#include <renderable/line.h>

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

    m_enabled = enable && Renderer::GetInstance()->IsEnabled();
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

    Line* line = nullptr;

    auto it = m_mapDebugFigures.find(id);
    if (it != m_mapDebugFigures.end())
    {
        line = dynamic_cast<Line*>(it->second.renderable);
        if (line == nullptr)
        {
            // Previous Id existed, but was not a polygon...
            m_mapDebugFigures.erase(it);
            it = m_mapDebugFigures.end();
        }
    }

    if (line == nullptr)
    {
        line = new Line(p1, p2, color);
    }
    else
    {
        line->UpdatePoints(p1, p2);
    }

    if (it != m_mapDebugFigures.end())
    {
        it->second.frameTimeRemaining = frameTime;
    }
    else
    {
        Renderer::GetInstance()->AddRenderable(line);

        it = m_mapDebugFigures.emplace(id, DebugFigure()).first;
        it->second.id = id;
        it->second.renderable = line;
        it->second.frameTimeRemaining = frameTime;
    }
}