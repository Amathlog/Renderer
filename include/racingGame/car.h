#pragma once

#include "glm/glm.hpp"
#include <vector>
#include <list>
#include "racingGame/track.h"

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
    glm::vec2 GetPosition() const;
    float GetAngle() const;
    bool IsDrifting() const { return m_isDrifting; }

    const b2Body* GetHull() const { return m_hull; }
    const std::vector<Wheel>& GetWheels() const { return m_wheels; }

    void UpdateTrackIndex(const Track::Path& path);
    unsigned int GetCurrentTrackIndex() { return m_currentTrackIndex; }
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
    bool m_isDrifting = false;
    // The rest part
    unsigned int m_currentTrackIndex = 0;
};