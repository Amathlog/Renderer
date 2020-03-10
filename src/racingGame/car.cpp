#include <racingGame/car.h>
#include <racingGame/constants.h>
#include <racingGame/controllers/carController.h>
#include <renderer/renderer.h>
#include <renderable/polygon.h>
#include <Box2D/Box2D.h>
#include <atomic>


// ---------------------------------------------------------
// Wheel Implementation
// ---------------------------------------------------------

void Car::Wheel::Destroy(b2World* world)
{
    delete polygon;
    world->DestroyBody(body);
}

// --------------------------------------------------------
// Hull implementation
// --------------------------------------------------------

void Car::Hull::Destroy(b2World* world)
{
    for (auto& wheel : wheels)
    {
        wheel.Destroy(world);
    }

    Renderer* renderer = Renderer::GetInstance();
    if (renderer != nullptr)
        renderer->RemoveRenderable(polygon->GetId());

    delete polygon;
    world->DestroyBody(body);
}

// --------------------------------------------------------
// LapInfo implementation
// --------------------------------------------------------
void Car::LapInfo::InitializeLap(unsigned int initialTrackIndex, float currentTimeS)
{
    // If we don't start at 0, the first lap doesn't count (as we don't do a full lap)
    nbLaps = initialTrackIndex == 0 ? 0 : -1;
    timeAtLapStartS = currentTimeS;
}

void Car::LapInfo::UpdateLap(float currentTimeS)
{
    nbLaps++;
    if (nbLaps > 0)
    {
        lastLapTimeS = currentTimeS - timeAtLapStartS;
        if (bestLapTimeS == 0.0f || lastLapTimeS < bestLapTimeS)
            bestLapTimeS = lastLapTimeS;
    }
    timeAtLapStartS = currentTimeS;
}

// --------------------------------------------------------
// Car implementation
// --------------------------------------------------------

Car::Car(b2World* world, const glm::vec4& color, unsigned int initialTrackIndex, bool isReverse, float currentTime)
    : m_world(world)
    , m_currentTrackIndex(initialTrackIndex)
    , m_isReverse(isReverse)
{
    static std::atomic<unsigned int> IDS = 1;
    m_id = IDS++;

    m_hull.color = color;

    InitializePhysics();
    InitializeRendering();
    m_lapInfo.InitializeLap(initialTrackIndex, currentTime);

    m_isDrifting = false;
}

Car::~Car()
{
    m_hull.Destroy(m_world);
}

void Car::InitializePhysics()
{
    if (m_world == nullptr)
        return;
    
    // First the hull
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    m_hull.body = m_world->CreateBody(&bodyDef);
    std::vector<float> vertices(Constants::HULL_VERTICES);
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
            polygon.push_back(b2Vec2(vertices[3*j] * Constants::SCALE_CAR, vertices[3*j + 1] * Constants::SCALE_CAR));
        }
        shape.Set(polygon.data(), (int32)polygon.size());
        i += size;
        m_hull.body->CreateFixture(&shape, 1.0f);
    }

    // Then the wheels (and attached them to the hull)
    std::vector<float> verticesWheel(Constants::WHEEL_VERTICES);
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
        jointDef.bodyA = m_hull.body;
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
        m_hull.wheels.push_back(wheelStruct);
    }
}

void Car::InitializeRendering()
{
    if (!Renderer::GetInstance()->IsEnabled())
        return;

    // First the hull
    std::vector<float> vertices(Constants::HULL_VERTICES);
    std::vector<unsigned int> indexes(Constants::HULL_INDEXES);
    m_hull.polygon = new Polygon(vertices, indexes, m_hull.color);
    m_hull.polygon->GetScale() = glm::vec3(Constants::SCALE_CAR, Constants::SCALE_CAR, 1.0f);
    Renderer::GetInstance()->AddRenderable(m_hull.polygon);

    // Then the wheels
    std::vector<unsigned int> indexesWheel = {0, 1, 2, 0, 2, 3};
    glm::vec4 wheelColor(Constants::WHEEL_COLOR[0], Constants::WHEEL_COLOR[1], Constants::WHEEL_COLOR[2], 1.0f);
    std::vector<float> verticesWheel(Constants::WHEEL_VERTICES);
    for(unsigned int i = 0; i < 4; ++i){
        Polygon* wheel = new Polygon(verticesWheel, indexesWheel, wheelColor);
        wheel->GetPosition() = glm::vec3(Constants::WHEELPOS[2 * i], Constants::WHEELPOS[2 * i + 1], 0.0f);
        m_hull.polygon->AddChild(wheel);
        m_hull.wheels[i].polygon = wheel;
    }
}

void Car::SetIntialState(const glm::vec2& pos, float angle, float offset)
{
    if (m_hull.body == nullptr)
        return;
    
    b2Vec2 box2DPos(pos[0], pos[1]);
    m_hull.body->SetTransform(box2DPos, angle);

    if (offset != 0.f)
    {
        b2Vec2 carSide = m_hull.body->GetWorldVector(b2Vec2(1.0f, 0.0f));
        box2DPos += offset * carSide;
        m_hull.body->SetTransform(box2DPos, angle);
    }

    for (auto& wheel : m_hull.wheels)
    {
        wheel.body->SetTransform(box2DPos + wheel.body->GetPosition(), angle);
    }

    UpdateRendering();
}

glm::vec2 Car::GetPosition() const
{
    if (m_hull.body == nullptr)
        return glm::vec2(0.0f);
    b2Vec2 pos = m_hull.body->GetPosition();
    return glm::vec2(pos.x, pos.y);
}

glm::vec2 Car::GetVelocity() const
{
    if (m_hull.body == nullptr)
        return glm::vec2(0.0f);
    b2Vec2 vel = m_hull.body->GetLinearVelocity();
    return glm::vec2(vel.x, vel.y);
}

float Car::GetAngle() const
{
    if (m_hull.body == nullptr)
        return 0.0f;
    return m_hull.body->GetAngle();
}

void Car::UpdateRendering()
{
    if (m_hull.polygon == nullptr || m_hull.body == nullptr)
        return;
    
    glm::vec2 pos = GetPosition();
    m_hull.polygon->GetPosition() = glm::vec3(pos.x, pos.y, 0.0f);
    m_hull.polygon->GetRotation()[2] = GetAngle();

    for (auto& wheel : m_hull.wheels)
    {
        wheel.polygon->GetRotation()[2] = wheel.body->GetAngle() - m_hull.polygon->GetRotation()[2];
    }
}

void Car::Gas(float gas)
{
    gas = std::clamp(gas, 0.0f, 1.0f);
    // Only rear wheels
    for (unsigned int i = 2; i < 4; ++i)
    {
        float diff = gas - m_hull.wheels[i].gas;
        if (diff > 0.1f)
            diff = 0.1f;  // gradually increase, but stop immediately
        m_hull.wheels[i].gas += diff;
    }
}

void Car::Brake(float brake)
{
    // control: brake b=0..1, more than 0.9 blocks wheels to zero rotation
    for (auto& wheel : m_hull.wheels)
        wheel.brake = brake;
}

void Car::Steer(float steer)
{
    // Only front wheels
    for (unsigned int i = 0; i < 2; ++i)
    {
        m_hull.wheels[i].steer = steer;
    }
}

void Car::Step(float dt)
{
    for (auto& wheel : m_hull.wheels)
    {
        // std::cout << "Before Wheel: " << wheel.omega << std::endl;
        // Steer each wheel
        float val = wheel.steer - wheel.joint->GetJointAngle();
        float dir = std::signbit(val) ? -1.0f : 1.0f;

        // Magic number taken from Python implementation
        constexpr float maxMotorValue = 3.0f;
        float rescaleFactorMotorValue = 50.0f;
        wheel.joint->SetMotorSpeed(dir * std::min(rescaleFactorMotorValue * std::abs(val), maxMotorValue));


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
        // But dt is finite, that will lead to oscillations if difference is already near zero
        // Use of a magic number, taken from the original implementation in Python.
        constexpr float magicNumber = 205000.0f;
        fForce *= magicNumber * Constants::SCALE_CAR * Constants::SCALE_CAR; // Random coefficient to cut oscillations in few steps (have no effect on friction_limit)
        pForce *= magicNumber * Constants::SCALE_CAR * Constants::SCALE_CAR;
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

void Car::UpdateTrackIndex(const Track::Path& path, float currentTime)
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
    
    if (closestIndex == 0 && m_currentTrackIndex != 0)
        m_lapInfo.UpdateLap(currentTime);

    m_currentTrackIndex = closestIndex;
}

void Car::EnableCollision(bool enable)
{
    if (m_hull.body == nullptr)
        return;

    // Disable for hull
    b2Fixture* hullFixtures = m_hull.body->GetFixtureList();
    while (hullFixtures != nullptr)
    {
        hullFixtures->SetSensor(!enable);
        hullFixtures = hullFixtures->GetNext();
    }

    // Then do the same for all the wheels
    for (auto& wheel : m_hull.wheels)
    {
        b2Fixture* wheelFixtures = wheel.body->GetFixtureList();
        while (wheelFixtures != nullptr)
        {
            wheelFixtures->SetSensor(!enable);
            wheelFixtures = wheelFixtures->GetNext();
        }
    }
}
