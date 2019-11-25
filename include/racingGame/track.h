// Work initially done by Oleg Klimov. Adapted by Adrien Logut

#pragma once

#include <vector>
#include <glm/glm.hpp>

class Polygon;

class Track
{
public:
    using Path = std::vector<glm::vec2>;
    
    Track();

    ~Track()
    {
        ClearTrack();
        ClearBackground();
    }
    
    bool GenerateTrack();
    void ClearTrack();
    void ClearBackground();
    const Path& GetPath() {return m_path;}
    float GetIntialAngle() {return m_initialAngle;}

private:
    std::vector<Polygon*> m_backgroundSquares;
    Polygon* m_tilesPolygon = nullptr;
    std::vector<Polygon*> m_bordersPolygon;
    Path m_path;
    float m_initialAngle = 0.0f;
};