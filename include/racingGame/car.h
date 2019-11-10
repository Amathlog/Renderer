#pragma once

#include "renderable/polygon.h"
#include "Box2D/Box2D.h"

class Car
{
    Car(b2World* world, const glm::vec4& color);
    ~Car();

    void InitializePhysics();
    void InitializeRendering();
    void Step(float dt);

    void Gas(float gas);
    void Brake(float brake);
    void Steer(float steer);
private:
    void CreateParticles(const glm::vec3& p1, const glm::vec3& p2, bool inGrass);
    // Physics part
    b2World* m_world;
    b2Body* m_hull;
    b2Body* m_wheels[4];
    // Rendering part
    glm::vec4 m_hullColor;
    Polygon* m_hullPolygon;
    Polygon* m_wheelsPolygon[4];
    Polygon* m_particlesPolygon[10];
};