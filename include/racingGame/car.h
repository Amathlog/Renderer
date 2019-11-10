#pragma once

#include "glm.hpp"
#include <vector>

class b2Body;
class b2World;
class Polygon;

class Car
{
public:
    Car(b2World* world, const glm::vec4& color);
    ~Car();

    void InitializePhysics();
    void InitializeRendering();
    void Step(float dt);

    void Gas(float gas);
    void Brake(float brake);
    void Steer(float steer);

    void SetIntialState(const glm::vec2& pos, float angle);
    glm::vec3 GetPosition();
    float GetAngle();
private:
    void CreateParticles(const glm::vec3& p1, const glm::vec3& p2, bool inGrass);
    // Physics part
    b2World* m_world;
    b2Body* m_hull;
    b2Body* m_wheels[4];
    // Rendering part
    glm::vec4 m_hullColor;
    Polygon* m_hullPolygon;
    std::vector<Polygon*> m_wheelsPolygon;
    std::vector<Polygon*> m_particlesPolygon;
};