#include "racingGame/car.h"
#include "racingGame/constants.h"
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
}

void Car::InitializePhysics()
{
    if (m_world == nullptr)
        return;
    
    // First the hull
    
}

void Car::InitializeRendering()
{
    // First the hull
    std::vector<float> vertrices(Constants::HULL_VERTRICES);
    std::vector<unsigned int> indexes(Constants::HULL_INDEXES);
    m_hullPolygon = new Polygon(vertrices, indexes, m_hullColor);
    m_hullPolygon->GetScale() = glm::vec3(Constants::SCALE_CAR, Constants::SCALE_CAR, 1.0f);
    Renderer::GetInstance()->AddRenderable(m_hullPolygon);

    // Then the wheels
    std::vector<unsigned int> indexesWheel = {0, 1, 2, 1, 2, 3};
    glm::vec4 wheelColor(Constants::WHEEL_COLOR[0], Constants::WHEEL_COLOR[1], Constants::WHEEL_COLOR[2], 1.0f);
    for(unsigned int i = 0; i < 4; ++i){
        std::vector<float> vertricesWheel = {
            -Constants::WHEEL_W, Constants::WHEEL_R, Constants::LAYER_WHEELS,
            Constants::WHEEL_W, Constants::WHEEL_R, Constants::LAYER_WHEELS,
            -Constants::WHEEL_W, -Constants::WHEEL_R, Constants::LAYER_WHEELS,
            Constants::WHEEL_W, -Constants::WHEEL_R, Constants::LAYER_WHEELS,
            };
        Polygon* wheel = new Polygon(vertricesWheel, indexesWheel, wheelColor);
        wheel->GetPosition() = glm::vec3(Constants::WHEELPOS[2 * i], Constants::WHEELPOS[2 * i + 1], 0.0f);
        m_hullPolygon->AddChild(wheel);
        m_wheelsPolygon.push_back(wheel);
    }
}

void Car::SetIntialState(const glm::vec2& pos, float angle)
{
    m_hullPolygon->GetPosition() = glm::vec3(pos[0] / Constants::SCALE_CAR, pos[1] / Constants::SCALE_CAR, m_hullPolygon->GetPosition()[2]);
    m_hullPolygon->GetRotation()[2] = angle;
}