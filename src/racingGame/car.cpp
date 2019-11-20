#include "racingGame/car.h"
#include "racingGame/constants.h"
#include "racingGame/controllers/carController.h"
#include "renderer/renderer.h"
#include "renderable/polygon.h"
#include "Box2D/Box2D.h"


Car::Car(b2World* world, const glm::vec4& color)
    : m_world(world)
    , m_hullColor(color)
{
    InitializePhysics();
    InitializeRendering();
}

Car::~Car()
{
    // Rendering
    if (m_hullPolygon != nullptr)
    {
        // Delete the wheels
        for (unsigned int i = 0; i < m_wheelsPolygon.size(); ++i)
        {
            m_hullPolygon->RemoveChild(m_wheelsPolygon[i]->GetId());
            delete m_wheelsPolygon[i];
        }
        m_wheelsPolygon.clear();

        Renderer* renderer = Renderer::GetInstance();
        renderer->RemoveRenderable(m_hullPolygon->GetId());
        delete m_hullPolygon;
        m_hullPolygon = nullptr;
    }

    // Physics
    if (m_world != nullptr)
    {
        for(auto& wheel : m_wheels)
        {
            // m_world->DestroyJoint(wheel.joint);
            m_world->DestroyBody(wheel.body);
        }
        m_world->DestroyBody(m_hull);
    }

    // Controller
    if (m_controller != nullptr)
    {
        delete m_controller;
        m_controller = nullptr;
    }
}

void Car::InitializePhysics()
{
    if (m_world == nullptr)
        return;
    
    // First the hull
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    m_hull = m_world->CreateBody(&bodyDef);
    std::vector<float> vertrices(Constants::HULL_VERTRICES);
    std::vector<unsigned int> sizes = {4, 4, 9, 4};
    unsigned int i = 0;
    for(unsigned int size : sizes)
    {
        b2PolygonShape shape;
        std::vector<b2Vec2> polygon;
        for (unsigned int j = i; j < i + size; ++j)
        {
            // Remove center point used for rendering
            if (size == 9 && j == i + size - 1)
                continue;
            polygon.push_back(b2Vec2(vertrices[3*j] * Constants::SCALE_CAR, vertrices[3*j + 1] * Constants::SCALE_CAR));
        }
        shape.Set(polygon.data(), (int32)polygon.size());
        i += size;
        m_hull->CreateFixture(&shape, 1.0f);
    }

    // Then the wheels (and attached them to the hull)
    m_wheels.clear();
    std::vector<float> vertricesWheel(Constants::WHEEL_VERTRICES);
    b2PolygonShape wheelShape;
    wheelShape.SetAsBox(Constants::WHEEL_W * Constants::SCALE_CAR, Constants::WHEEL_R * Constants::SCALE_CAR);
    b2FixtureDef wheelFixture;
    wheelFixture.shape = &wheelShape;
    wheelFixture.density = 0.1f;
    wheelFixture.restitution = 0.0f;
    for (unsigned int j = 0; j < 4; ++j)
    {
        // First body
        b2BodyDef wheelBodyDef;
        wheelBodyDef.type = b2_dynamicBody;
        wheelBodyDef.position.Set(Constants::WHEELPOS[2 * j] * Constants::SCALE_CAR, Constants::WHEELPOS[2 * j + 1] * Constants::SCALE_CAR);
        b2Body* wheel = m_world->CreateBody(&wheelBodyDef);
        wheel->CreateFixture(&wheelFixture);

        Wheel wheelStruct;
        wheelStruct.body = wheel;

        // Then joint
        b2RevoluteJointDef jointDef;
        jointDef.bodyA = m_hull;
        jointDef.bodyB = wheel;
        jointDef.localAnchorA = wheelBodyDef.position;
        jointDef.localAnchorB = b2Vec2_zero;
        jointDef.enableMotor = true;
        jointDef.enableLimit = true;
        jointDef.maxMotorTorque = 180.0f * 900.0f * Constants::SCALE_CAR * Constants::SCALE_CAR;
        jointDef.motorSpeed = 0.0f;
        jointDef.lowerAngle = -0.4f;
        jointDef.upperAngle = 0.4f;
        wheelStruct.joint = reinterpret_cast<b2RevoluteJoint*>(m_world->CreateJoint(&jointDef));

        // Finally add the wheel
        m_wheels.push_back(wheelStruct);
    }
}

void Car::InitializeRendering()
{
    if (!Renderer::GetInstance()->IsEnabled())
        return;

    // First the hull
    std::vector<float> vertrices(Constants::HULL_VERTRICES);
    std::vector<unsigned int> indexes(Constants::HULL_INDEXES);
    m_hullPolygon = new Polygon(vertrices, indexes, m_hullColor);
    m_hullPolygon->GetScale() = glm::vec3(Constants::SCALE_CAR, Constants::SCALE_CAR, 1.0f);
    Renderer::GetInstance()->AddRenderable(m_hullPolygon);

    // Then the wheels
    std::vector<unsigned int> indexesWheel = {0, 1, 2, 0, 2, 3};
    glm::vec4 wheelColor(Constants::WHEEL_COLOR[0], Constants::WHEEL_COLOR[1], Constants::WHEEL_COLOR[2], 1.0f);
    std::vector<float> vertricesWheel(Constants::WHEEL_VERTRICES);
    for(unsigned int i = 0; i < 4; ++i){
        Polygon* wheel = new Polygon(vertricesWheel, indexesWheel, wheelColor);
        wheel->GetPosition() = glm::vec3(Constants::WHEELPOS[2 * i], Constants::WHEELPOS[2 * i + 1], 0.0f);
        m_hullPolygon->AddChild(wheel);
        m_wheelsPolygon.push_back(wheel);
    }
}

void Car::SetIntialState(const glm::vec2& pos, float angle)
{
    if (m_hull == nullptr || m_wheels.empty())
        return;
    
    b2Vec2 box2DPos(pos[0], pos[1]);
    m_hull->SetTransform(box2DPos, angle);
    for (auto& wheel : m_wheels)
    {
        wheel.body->SetTransform(box2DPos + wheel.body->GetPosition(), angle);
    }
    UpdateRendering();
    m_isDrifting = false;
    m_currentTrackIndex = 0;
}

glm::vec2 Car::GetPosition() const
{
    if (m_hull == nullptr)
        return glm::vec2(0.0f);
    b2Vec2 pos = m_hull->GetPosition();
    return glm::vec2(pos.x, pos.y);
}

float Car::GetAngle() const
{
    if (m_hull == nullptr)
        return 0.0f;
    return m_hull->GetAngle();
}

void Car::UpdateRendering()
{
    if (m_hullPolygon == nullptr || m_hull == nullptr)
        return;
    
    glm::vec2 pos = GetPosition();
    pos[0] /= Constants::SCALE_CAR;
    pos[1] /= Constants::SCALE_CAR;
    m_hullPolygon->GetPosition() = glm::vec3(pos.x, pos.y, 0.0f);
    m_hullPolygon->GetRotation()[2] = GetAngle();

    for (unsigned int i = 0; i < m_wheels.size(); ++i)
    {
        m_wheelsPolygon[i]->GetRotation()[2] = m_wheels[i].body->GetAngle() - m_hullPolygon->GetRotation()[2];
    }
}

void Car::Gas(float gas)
{
    gas = std::clamp(gas, 0.0f, 1.0f);
    // Only rear wheels
    for (unsigned int i = 2; i < 4; ++i)
    {
        float diff = gas - m_wheels[i].gas;
        if (diff > 0.1f)
            diff = 0.1f;  // gradually increase, but stop immediately
        m_wheels[i].gas += diff;
    }
}

void Car::Brake(float brake)
{
    // control: brake b=0..1, more than 0.9 blocks wheels to zero rotation
    for (auto& wheel : m_wheels)
        wheel.brake = brake;
}

void Car::Steer(float steer)
{
    // Only front wheels
    for (unsigned int i = 0; i < 2; ++i)
    {
        m_wheels[i].steer = steer;
    }
}

void Car::Step(float dt)
{
    for (auto& wheel : m_wheels)
    {
        // std::cout << "Before Wheel: " << wheel.omega << std::endl;
        // Steer each wheel
        float val = wheel.steer - wheel.joint->GetJointAngle();
        float dir = std::signbit(val) ? -1.0f : 1.0f;
        wheel.joint->SetMotorSpeed(dir * std::min(50.0f * std::abs(val), 3.0f));


        // # Position => friction_limit
        // grass = True
        // friction_limit = FRICTION_LIMIT*0.6  # Grass friction if no tile
        // for tile in w.tiles:
        //     friction_limit = max(friction_limit, FRICTION_LIMIT*tile.road_friction)
        //     grass = False

        // Force
        b2Vec2 forward = wheel.body->GetWorldVector(b2Vec2(0.0f, 1.0f));
        b2Vec2 side = wheel.body->GetWorldVector(b2Vec2(1.0f, 0.0f));
        b2Vec2 velocity = wheel.body->GetLinearVelocity();

        float velocityForward = b2Dot(forward, velocity);
        float velocitySide = b2Dot(side, velocity);

        // WHEEL_MOMENT_OF_INERTIA*np.square(w.omega)/2 = E -- energy
        // WHEEL_MOMENT_OF_INERTIA*w.omega * domega/dt = dE/dt = W -- power
        // domega = dt*W/WHEEL_MOMENT_OF_INERTIA/w.omega
        // 5.0f is a small coeff to avoid to divide by 0
        wheel.omega += dt * Constants::ENGINE_POWER * wheel.gas / (Constants::WHEEL_MOMENT_OF_INERTIA * (std::abs(wheel.omega) + 5.0f));

        if (wheel.brake >= 0.9f)
            wheel.omega = 0.0f;
        else if (wheel.brake > 0.0f)
        {
            // Inverted
            float brakeDir = std::signbit(wheel.omega) ? 1.0f : -1.0f;
            float brakeValue = Constants::BRAKE_FORCE * wheel.brake;
            if (std::abs(brakeValue) > std::abs(wheel.omega))
                brakeValue = std::abs(wheel.omega); // Low speed => same as = 0
            wheel.omega += brakeDir * brakeValue;
        }
        wheel.phase += wheel.omega * dt;

        float rotatingWheelSpeed = wheel.omega * Constants::WHEEL_R * Constants::SCALE_CAR;
        float fForce = -velocityForward + rotatingWheelSpeed; // force direction is direction of speed difference
        float pForce = -velocitySide;

        // Physically correct is to always apply friction_limit until speed is equal.
        // But dt is finite, that will lead to oscillations if difference is already near zero.
        fForce *= 205000.0f * Constants::SCALE_CAR * Constants::SCALE_CAR; // Random coefficient to cut oscillations in few steps (have no effect on friction_limit)
        pForce *= 205000.0f * Constants::SCALE_CAR * Constants::SCALE_CAR;
        float force = std::sqrt(std::pow(fForce, 2) + std::pow(pForce, 2));

        // Skid trace
        // if abs(force) > 2.0*friction_limit:
        //     if w.skid_particle and w.skid_particle.grass==grass and len(w.skid_particle.poly) < 30:
        //         w.skid_particle.poly.append( (w.position[0], w.position[1]) )
        //     elif w.skid_start is None:
        //         w.skid_start = w.position
        //     else:
        //         w.skid_particle = self._create_particle( w.skid_start, w.position, grass )
        //         w.skid_start = None
        // else:
        //     w.skid_start = None
        //     w.skid_particle = None

        if (std::abs(force) > Constants::FRICTION_LIMIT)
        {
            fForce /= force;
            pForce /= force;
            force = Constants::FRICTION_LIMIT;
            fForce *= force;
            pForce *= force;
        }

        wheel.omega -= dt * fForce * Constants::WHEEL_R * Constants::SCALE_CAR / Constants::WHEEL_MOMENT_OF_INERTIA;
        b2Vec2 forceToApply = pForce * side + fForce * forward;

        wheel.body->ApplyForceToCenter(forceToApply, true);
        // std::cout << "After Wheel: " << wheel.omega << std::endl;
        // std::cout << "Force to apply: " << forceToApply.x << " " << forceToApply.y << std::endl;
    }
}

void Car::UpdateTrackIndex(const Track::Path& path)
{
    glm::vec2 position = GetPosition();
    float minDistance = -1.0f;
    unsigned int closestIndex = 0;
    for (int i = -2; i <= 2; ++i)
    {
        int index = (m_currentTrackIndex + i) % static_cast<int>(path.size());
        if (index < 0)
            index += static_cast<int>(path.size());
        unsigned int finalIndex = static_cast<unsigned int>(index);
        float distance = glm::length(path[finalIndex] - position);
        if (minDistance < 0.0f || distance < minDistance)
        {
            minDistance = distance;
            closestIndex = finalIndex;
        }
    }
    m_currentTrackIndex = closestIndex;
}