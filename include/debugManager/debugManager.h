#pragma once

#include "glm/glm.hpp"
#include "renderable/renderable.h"
#include <string>
#include <unordered_map>

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