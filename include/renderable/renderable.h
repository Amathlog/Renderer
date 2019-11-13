#pragma once

#include <glm.hpp>
#include "shaders/shaders.h"
#include <iostream>
#include <unordered_map>

class Renderable
{
public:
    Renderable()
    {
        m_ID = ++CURRENT_ID;
    }

    virtual ~Renderable() = default;

    virtual void CreateShader() = 0;
    void Draw(const glm::mat4& mvp) const;

    glm::mat4 GetTransform() const;
    
    const glm::vec3& GetPosition() const {return m_position;}
    glm::vec3& GetPosition() {return m_position;}

    const glm::vec3& GetRotation() const {return m_rotation;}
    glm::vec3& GetRotation() {return m_rotation;}

    const glm::vec3& GetScale() const {return m_scale;}
    glm::vec3& GetScale() {return m_scale;}

    unsigned int GetId() const {return m_ID;}

    void AddChild(Renderable* child);
    Renderable* GetChild(unsigned int id);
    void RemoveChild(unsigned int id);
protected:
    virtual void InternalDraw(const glm::mat4& mvp) const {};

    Shader* m_shader = nullptr;

    glm::vec3 m_position = glm::vec3(0.0f);
    glm::vec3 m_rotation = glm::vec3(0.0f);
    glm::vec3 m_scale = glm::vec3(1.0f, 1.0f, 1.0f);

    using MapIdToRenderable = std::unordered_map<unsigned int, Renderable*>;
    MapIdToRenderable m_children;

    unsigned int m_ID;
private:
    static inline unsigned int CURRENT_ID = 0;
};