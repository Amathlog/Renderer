#pragma once

#include <glm/glm.hpp>
#include <renderable/renderable.h>
#include <string>
#include <unordered_map>
#include <utils/colors.h>

class DebugManager
{
public:
    static DebugManager* GetInstance()
    {
        if (ms_instance == nullptr)
            ms_instance = new DebugManager();
        return ms_instance;
    }

    ~DebugManager()
    {
        Clear();
    }

    bool IsEnabled() { return m_enabled; }
    void Enable(bool enable);

    void Clear();
    void Update();

    void DrawLine(const std::string& id, const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& color, int frameTime = 1);

    // Duplicate helper functions
    void DrawLine(const std::string& id, const glm::vec3& p1, const glm::vec3& p2, Colors color, int frameTime = 1)
    {
        DrawLine(id, p1, p2, ColorsUtils::GetVecColor(color), frameTime);
    }
    void DrawLine(const std::string& id, const glm::vec2& p1, const glm::vec2& p2, const glm::vec4& color, int frameTime = 1)
    {
        DrawLine(id, glm::vec3(p1.x, p1.y, 0.0f), glm::vec3(p2.x, p2.y, 0.0f), color, frameTime);
    }
    void DrawLine(const std::string& id, const glm::vec2& p1, const glm::vec2& p2, Colors color, int frameTime = 1)
    {
        DrawLine(id, p1, p2, ColorsUtils::GetVecColor(color), frameTime);
    }

private:
    struct DebugFigure
    {
        DebugFigure() = default;
        ~DebugFigure();

        std::string id;
        Renderable* renderable = nullptr;
        int frameTimeRemaining;
    };

    DebugManager() = default;
    static inline DebugManager* ms_instance = nullptr;

    bool m_enabled;

    using MapDebugFigures = std::unordered_map<std::string, DebugFigure>;
    MapDebugFigures m_mapDebugFigures;
};