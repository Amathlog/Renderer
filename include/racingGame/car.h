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

    struct LapInfo
    {
        int nbLaps = -1;
        float lastLapTimeS = 0.0f;
        float bestLapTimeS = 0.0f;
        float timeAtLapStartS = 0.0f;

        void InitializeLap(unsigned int initialTrackIndex, float currentTimeS);
        void UpdateLap(float currentTimeS);
    };

    Car(b2World* world, const glm::vec4& color, unsigned int initialTrackIndex, bool isReverse, float currentTimeS);
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

    void SetIntialState(const glm::vec2& pos, float angle, float offset = 0.0f);
    glm::vec2 GetPosition() const;
    glm::vec2 GetVelocity() const;
    float GetAngle() const;
    bool IsDrifting() const { return m_isDrifting; }

    const Hull& GetHull() const { return m_hull; }

    unsigned int GetId() const { return m_id; }

    void UpdateTrackIndex(const Track::Path& path, float currentTimeS);
    unsigned int GetCurrentTrackIndex() const { return m_currentTrackIndex; }

    const LapInfo& GetLapInfo() const { return m_lapInfo; }

    void EnableCollision(bool enable);

    bool GetIsReverse() const { return m_isReverse; }
private:
    // Not implemented yet
    //void CreateParticles(const glm::vec3& p1, const glm::vec3& p2, bool inGrass);
    
    b2World* m_world = nullptr;

    Hull m_hull;

    bool m_isDrifting = false;
    unsigned int m_id = 0;
    unsigned int m_currentTrackIndex = 0;
    LapInfo m_lapInfo;
    CarController* m_controller = nullptr;
    bool m_isReverse = false;
};