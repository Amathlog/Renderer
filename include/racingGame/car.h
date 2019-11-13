#pragma once

#include "glm.hpp"
#include <vector>
#include <list>

class b2Body;
class b2World;
class b2RevoluteJoint;
class Polygon;

class Car
{
public:

    struct Wheel
    {
        Wheel() = default;

        b2Body* body = nullptr;
        b2RevoluteJoint* joint = nullptr;
        float gas = 0.0f;
        float brake = 0.0f;
        float steer = 0.0f;
        float phase = 0.0f;
        float omega = 0.0f;
    };

    Car(b2World* world, const glm::vec4& color);
    ~Car();

    void InitializePhysics();
    void InitializeRendering();
    void Step(float dt);

    void UpdateRendering();

    void Gas(float gas);
    void Brake(float brake);
    void Steer(float steer);

    void SetIntialState(const glm::vec2& pos, float angle);
    glm::vec3 GetPosition();
    float GetAngle();
private:
    void CreateParticles(const glm::vec3& p1, const glm::vec3& p2, bool inGrass);
    // Physics part
    b2World* m_world = nullptr;
    b2Body* m_hull = nullptr;
    std::vector<Wheel> m_wheels;
    // Rendering part
    glm::vec4 m_hullColor;
    Polygon* m_hullPolygon = nullptr;
    std::vector<Polygon*> m_wheelsPolygon;
    std::list<Polygon*> m_particlesPolygon;
};