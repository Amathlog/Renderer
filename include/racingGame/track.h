// Work initially done by Oleg Klimov. Adapted by Adrien Logut

#pragma once

#include <vector>
#include "glm/glm.hpp"

class Polygon;

class Track
{
public:
    Track();

    ~Track()
    {
        ClearTrack();
        ClearBackground();
    }
    
    bool GenerateTrack();
    void ClearTrack();
    void ClearBackground();
    const std::vector<glm::vec2>& GetPath() {return m_path;}
    float GetIntialAngle() {return m_initialAngle;}
private:
    std::vector<Polygon*> m_backgroundSquares;
    Polygon* m_tilesPolygon = nullptr;
    std::vector<Polygon*> m_bordersPolygon;
    std::vector<glm::vec2> m_path;
    float m_initialAngle = 0.0f;
};