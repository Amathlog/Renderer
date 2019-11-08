#pragma once
#include "renderable/polygon.h"

class Car : public Polygon
{
public:
    Car(const glm::vec4& color);
};