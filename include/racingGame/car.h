#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <list>
#include <racingGame/track.h>

class b2Body;
class b2World;
class b2RevoluteJoint;
class Polygon;
class CarController;

class Car
{
public:
    struct Wheel
    {
        Wheel() = default;

        void Destroy(b2World* world);

        // Physics
        b2Body* body = nullptr;
        b2RevoluteJoint* joint = nullptr;
        float gas = 0.0f;
        float brake = 0.0f;
        float steer = 0.0f;
        float phase = 0.0f;
        float omega = 0.0f;

        // Rendering
        Polygon* polygon = nullptr;
    };

    struct Hull
    {
        Hull() = default;

        void Destroy(b2World* world);

        // Physics
        b2Body* body = nullptr;
        glm::vec4 color;

        // Rendering
        Polygon* polygon = nullptr;

        // Wheels
        std::vector<Wheel> wheels;
    };

    Car(b2World* world, const glm::vec4& color);
    ~Car();

    void InitializePhysics();
    void InitializeRendering();
    void Step(float dt);

    void AttachController(CarController* controller) { m_controller = controller; }
    void DetachController() { m_controller = nullptr; }
    CarController* GetController() const { return m_controller; }

    void UpdateRendering();

    void Gas(float gas);
    void Brake(float brake);
    void Steer(float steer);

    void SetIntialState(const glm::vec2& pos, float angle);
    glm::vec2 GetPosition() const;
    float GetAngle() const;
    bool IsDrifting() const { return m_isDrifting; }

    const Hull& GetHull() const { return m_hull; }

    unsigned int GetId() const { return m_id; }

    void UpdateTrackIndex(const Track::Path& path);
    unsigned int GetCurrentTrackIndex() const { return m_currentTrackIndex; }

    void EnableCollision(bool enable);
private:
    // Not implemented yet
    //void CreateParticles(const glm::vec3& p1, const glm::vec3& p2, bool inGrass);
    
    b2World* m_world = nullptr;

    Hull m_hull;

    bool m_isDrifting = false;
    unsigned int m_id = 0;
    unsigned int m_currentTrackIndex = 0;
    CarController* m_controller = nullptr;
};