// Work initially done by Oleg Klimov. Adapted by Adrien Logut

#include <racingGame/track.h>
#include <racingGame/constants.h>
#include <renderable/polygon.h>
#include <renderer/renderer.h>
#include <shaders/shaderManager.h>
#include <utils/randomEngine.h>
#include <utils/utils.h>

#include <cmath>
#include <random>
#include <algorithm>
#include <chrono>
#include <filesystem>

#include <glm/gtx/string_cast.hpp> 

#ifndef M_PI
#define M_PI 3.14159265359f
#endif


bool Track::GenerateTrack()
{
    std::default_random_engine& randomEngine = RandomEngine::GetInstance()->GetGenerator();
    std::vector<glm::vec3> checkpoints;
    float startAlpha = -M_PI / Constants::CHECKPOINTS;
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    for (unsigned int i = 0; i < Constants::CHECKPOINTS; ++i)
    {
        float alpha = 2.0f * M_PI / Constants::CHECKPOINTS * (i + distribution(randomEngine));
        float rad = Constants::TRACK_RAD * (distribution(randomEngine) * 0.66f + 0.33f);
        if (i == 0)
        {
            alpha = 0.0f;
            rad = Constants::TRACK_RAD;
        }
        else if (i == Constants::CHECKPOINTS - 1)
        {
            alpha = 2.0f * M_PI * ((float)i / Constants::CHECKPOINTS);
            rad = Constants::TRACK_RAD;
        }
        checkpoints.push_back(glm::vec3(alpha, rad * std::cos(alpha), rad * std::sin(alpha)));
    }

    float x = 1.0f * Constants::TRACK_RAD, y = 0.0f, alpha = 0.0f, beta = 0.0f;
    unsigned int dest_i = 0, laps = 0, noFreeze = 2500;
    bool visitedOhterSide = false;
    std::vector<glm::vec4> track;
    while (1)
    {
        // std::cout << "First loop"<< std::endl;
        alpha = std::atan2(y, x);
        if (visitedOhterSide && alpha > 0.0f)
        {
            laps++;
            visitedOhterSide = false;
        }
        if (alpha < 0.0f)
        {
            visitedOhterSide = true;
            alpha += 2.0f * M_PI;
        }
        const glm::vec3* checkpoint;
        while (1)
        {
            // std::cout << "Second loop"<< std::endl;
            // Find destination from checkpoints
            bool failed = true;
            while (1)
            {
                // std::cout << "Third loop"<< std::endl;
                checkpoint = &checkpoints[dest_i % checkpoints.size()];
                if (alpha <= checkpoint->x)
                {
                    failed = false;
                    break;
                }
                if(++dest_i % checkpoints.size() == 0)
                    break;
            }
            if (!failed)
                break;
            alpha -= 2.0f * M_PI;
        }
        float r1x = std::cos(beta), r1y = std::sin(beta);
        float p1x = -r1y, p1y = r1x;
        float dest_dx = checkpoint->y - x;
        float dest_dy = checkpoint->z - y;
        // Destination vector projected on rad
        float proj = r1x * dest_dx + r1y * dest_dy; 
        while (beta - alpha > 1.5f * M_PI)
            beta -= 2.0f * M_PI;
        while (beta - alpha < -1.5f * M_PI)
            beta += 2.0f * M_PI;
        float previous_beta = beta;
        proj /= Constants::SCALE_TRACK;
        if (proj > 0.3f)
            beta -= std::min(Constants::TRACK_TURN_RATE, std::abs(0.001f * proj));
        if (proj < -0.3f)
            beta += std::min(Constants::TRACK_TURN_RATE, std::abs(0.001f * proj));
        x += p1x * Constants::TRACK_DETAIL_STEP;
        y += p1y * Constants::TRACK_DETAIL_STEP;
        track.push_back(glm::vec4(alpha, previous_beta * 0.5f + beta * 0.5f, x, y));
        // std::cout << glm::to_string(track[track.size() - 1]) << std::endl;
        if (laps > 4)
            break;
        if(--noFreeze == 0)
            break;
    }
    int i1 = -1, i2 = -1;
    int idx = (int)track.size();
    // std::cout << i << std::endl;
    while (1)
    {
        if (--idx == 0)
            return false; // Failed
        bool passThroughStart = track[idx][0] > startAlpha && track[idx-1][0] <= startAlpha;
        if (passThroughStart && i2 == -1)
        {
            // std::cout << "Found i2..." << std::endl;
            i2 = idx;
        }
        else if (passThroughStart && i1 == -1)
        {
            // std::cout << "Found i1..." << std::endl;
            i1 = idx;
            break;
        }
    }
    assert(i1 != -1);
    assert(i2 > 2);
    track = std::vector<glm::vec4>(track.begin() + i1, track.begin() + i2 - 2);
    float first_beta = track[0][1];
    float first_perp_x = std::cos(first_beta), first_perp_y = std::sin(first_beta);

    // Length of perpendicular jump to put together head and tail
    float well_glued_together = std::sqrt(
            std::pow( first_perp_x*(track[0][2] - track[track.size()-1][2]), 2.0f) +
            std::pow( first_perp_y*(track[0][3] - track[track.size()-1][3]), 2.0f));
    if (well_glued_together > Constants::TRACK_DETAIL_STEP)
    {
        std::cout << "Not well glued enough... " << well_glued_together << " vs " << Constants::TRACK_DETAIL_STEP << std::endl;
        return false;
    }

    // Red-white borders on hard turns
    std::vector<bool> borders;
    for (unsigned int i = 0; i < track.size(); ++i)
    {
        bool good = true;
        int oneside = 0;
        for (unsigned int neg = 0; neg < Constants::BORDER_MIN_COUNT; ++neg)
        {
            unsigned int index = neg + 1 > i ? i + (unsigned int)track.size() - neg -1 : i - neg - 1;
            float beta1 = track[(index + 1) % track.size()][1];
            float beta2 = track[index][1];
            good &= std::abs(beta1 - beta2) > Constants::TRACK_TURN_RATE*0.2f;
            oneside += std::signbit(beta1 - beta2) ? -1 : 1;
        }
        good &= std::abs(oneside) == Constants::BORDER_MIN_COUNT;
        borders.push_back(good);
    }
    for (unsigned int i = 0; i < track.size(); ++i)
    {
        for (unsigned int neg = 0; neg < Constants::BORDER_MIN_COUNT; ++neg)
        {
            unsigned int index = neg > i ? i + (unsigned int)track.size() - neg : i - neg;
            borders[index] = borders[index] || borders[i];
        }
    }

    // Store the path
    for (unsigned int i = 0; i < track.size(); ++i)
        // Add to the path
        m_path.push_back(glm::vec2(track[i][2], track[i][3]));

    // If we have no rendering, stop here
    if (!Renderer::GetInstance()->IsEnabled())
        return true;

    // Create tiles
    std::vector<unsigned int> indexes = {
        0, 1, 2, 
        1, 2, 3
    };
    glm::vec4 roadColor(Constants::ROAD_COLOR[0], Constants::ROAD_COLOR[1], Constants::ROAD_COLOR[2], 1.0f);
    Renderer* renderer = Renderer::GetInstance();
    // Road data
    std::vector<float> roadVertices;
    std::vector<unsigned int> roadIndexes;
    // Border data
    std::vector<float> whiteBorderVertices;
    std::vector<unsigned int> whiteBorderIndexes;
    std::vector<float> redBorderVertices;
    std::vector<unsigned int> redBorderIndexes;
    // Generate our polygons
    for(unsigned int i = 0; i < track.size(); ++i)
    {
        const glm::vec4 p1 = track[i];
        const glm::vec4 p2 = track[(i + 1) % track.size()];

        roadVertices.insert(roadVertices.end(), {
            p1[2] - Constants::TRACK_WIDTH * std::cos(p1[1]),
            p1[3] - Constants::TRACK_WIDTH * std::sin(p1[1]),
            Constants::LAYER_TRACK_Z,

            p1[2] + Constants::TRACK_WIDTH * std::cos(p1[1]),
            p1[3] + Constants::TRACK_WIDTH * std::sin(p1[1]),
            Constants::LAYER_TRACK_Z,

            p2[2] - Constants::TRACK_WIDTH * std::cos(p2[1]),
            p2[3] - Constants::TRACK_WIDTH * std::sin(p2[1]),
            Constants::LAYER_TRACK_Z,

            p2[2] + Constants::TRACK_WIDTH * std::cos(p2[1]),
            p2[3] + Constants::TRACK_WIDTH * std::sin(p2[1]),
            Constants::LAYER_TRACK_Z
        });

        unsigned int index = 4 * i;
        roadIndexes.insert(roadIndexes.end(), 
            {
                index, index + 1, index + 2,
                index + 1, index + 2, index + 3
            }
        );

        if (borders[i])
        {
            float side = std::signbit(p1[1] - p2[1]) ? -1.0f : 1.0f;
            std::vector<float>& verticesToFill = i % 2 == 0 ? whiteBorderVertices : redBorderVertices;
            std::vector<unsigned int>& indexesToFill = i % 2 == 0 ? whiteBorderIndexes : redBorderIndexes;

            verticesToFill.insert(verticesToFill.end(),
                {
                p1[2] + side * Constants::TRACK_WIDTH * std::cos(p1[1]),
                p1[3] + side * Constants::TRACK_WIDTH * std::sin(p1[1]),
                Constants::LAYER_TRACK_Z,

                p1[2] + side * (Constants::TRACK_WIDTH + Constants::BORDER) * std::cos(p1[1]),
                p1[3] + side * (Constants::TRACK_WIDTH + Constants::BORDER) * std::sin(p1[1]),
                Constants::LAYER_TRACK_Z,

                p2[2] + side * Constants::TRACK_WIDTH * std::cos(p2[1]),
                p2[3] + side * Constants::TRACK_WIDTH * std::sin(p2[1]),
                Constants::LAYER_TRACK_Z,
                
                p2[2] + side * (Constants::TRACK_WIDTH + Constants::BORDER) * std::cos(p2[1]),
                p2[3] + side * (Constants::TRACK_WIDTH + Constants::BORDER) * std::sin(p2[1]),
                Constants::LAYER_TRACK_Z
                }
            );

            index = (unsigned int)verticesToFill.size() / 3 - 4;
            indexesToFill.insert(indexesToFill.end(),
                {
                    index, index + 1, index + 2,
                    index + 1, index + 2, index + 3
                }
            );
        }
    }

    // Then create the polygons. For the track, use a specific shader
    Shader* trackShader = ShaderManager::GetInstance()->LoadShader("track_shader.vs", "track_shader.fs");

    m_tilesPolygon = new Polygon(roadVertices, roadIndexes, roadColor, trackShader);
    renderer->AddRenderable(m_tilesPolygon);

    m_bordersPolygon.push_back(new Polygon(whiteBorderVertices, whiteBorderIndexes, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
    m_bordersPolygon.push_back(new Polygon(redBorderVertices, redBorderIndexes, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));

    renderer->AddRenderable(m_bordersPolygon[0]);
    renderer->AddRenderable(m_bordersPolygon[1]);

    m_initialAngle = track[0][1];
    return true;
}

void Track::ClearTrack()
{
    Renderer* renderer = Renderer::GetInstance();

    if (m_tilesPolygon != nullptr)
    {
        renderer->RemoveRenderable(m_tilesPolygon->GetId());
        delete m_tilesPolygon;
        m_tilesPolygon = nullptr;
    }

    for (unsigned int i = 0; i < m_bordersPolygon.size(); ++i)
    {
        renderer->RemoveRenderable(m_bordersPolygon[i]->GetId());
        delete m_bordersPolygon[i];
    }
    m_bordersPolygon.clear();
    m_path.clear();
    m_initialAngle = 0.0f;
}

Track::Track()
{
    Renderer* renderer = Renderer::GetInstance();

    // If we have no renderer, nothing to do
    if (!renderer->IsEnabled())
        return;

    // Generate the background
    std::vector<float> backgroundVertices = {
        -Constants::PLAYFIELD, Constants::PLAYFIELD, Constants::LAYER_BACKGROUND_Z,
        Constants::PLAYFIELD, Constants::PLAYFIELD, Constants::LAYER_BACKGROUND_Z,
        Constants::PLAYFIELD, -Constants::PLAYFIELD, Constants::LAYER_BACKGROUND_Z,
        -Constants::PLAYFIELD, -Constants::PLAYFIELD, Constants::LAYER_BACKGROUND_Z
    };

    std::vector<unsigned int> backgroundIndexes = {0, 1, 2, 0, 2, 3};
    glm::vec4 color(Constants::BACKGROUND_COLOR1[0], Constants::BACKGROUND_COLOR1[1], Constants::BACKGROUND_COLOR1[2], 1.0f);
    Polygon* background = new Polygon(backgroundVertices, backgroundIndexes, color);
    renderer->AddRenderable(background);
    m_backgroundSquares.push_back(background);

    // Then add the little squares
    int nbSquares = 20;
    float k = Constants::PLAYFIELD / nbSquares;
    std::vector<float> squareVertices;
    std::vector<unsigned int> squareIndexes;
    unsigned int currentIndex = 0;
    for (int i = -nbSquares; i < nbSquares; i += 2)
    {
        for (int j = -nbSquares; j < nbSquares; j+= 2)
        {
                squareVertices.insert(squareVertices.end(), {
                    k * i, k * (j + 1), Constants::LAYER_BACKGROUND_Z - 0.01f,
                    k * (i + 1), k * (j + 1), Constants::LAYER_BACKGROUND_Z - 0.01f,
                    k * (i + 1), k * j, Constants::LAYER_BACKGROUND_Z - 0.01f,
                    k * i, k * j, Constants::LAYER_BACKGROUND_Z - 0.01f
                });

                squareIndexes.insert(squareIndexes.end(), 
                {currentIndex, currentIndex + 1, currentIndex + 2,
                 currentIndex, currentIndex + 2, currentIndex + 3});
                currentIndex += 4;
        }
    }
    glm::vec4 colorSquare(Constants::BACKGROUND_COLOR2[0], Constants::BACKGROUND_COLOR2[1], Constants::BACKGROUND_COLOR2[2], 1.0f);
    Polygon* square = new Polygon(squareVertices, squareIndexes, colorSquare);
    renderer->AddRenderable(square);
    m_backgroundSquares.push_back(square);
}

void Track::ClearBackground()
{
    Renderer* renderer = Renderer::GetInstance();
    for (Polygon* square : m_backgroundSquares)
    {
        renderer->RemoveRenderable(square->GetId());
        delete square;
    }
    m_backgroundSquares.clear();
}

float Track::GetAngle(size_t index, bool reverse) const
{
    size_t nextIndex;
    if (index == 0 && reverse)
        nextIndex = m_path.size() - 1;
    else if (index == m_path.size() - 1 && !reverse)
        nextIndex = 0;
    else
        nextIndex = reverse ? index - 1 : index + 1;

    return Utils::GetAngle(glm::vec2(0.0f, 1.0f), m_path[nextIndex] - m_path[index]);
}
