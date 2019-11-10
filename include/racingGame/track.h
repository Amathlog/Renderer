// Work initially done by Oleg Klimov. Adapted by Adrien Logut

#pragma once

#include <vector>
#include "glm.hpp"

class Polygon;

class Track
{
public:
    Track() = default;

    ~Track()
    {
        ClearTrack();
    }
    
    bool GenerateTrack();
    void ClearTrack();
    const std::vector<glm::vec2>& GetPath() {return m_path;}
    float GetIntialAngle() {return m_initialAngle;}
private:
    std::vector<Polygon*> m_tilesPolygon;
    std::vector<Polygon*> m_bordersPolygon;
    std::vector<glm::vec2> m_path;
    float m_initialAngle = 0.0f;
};