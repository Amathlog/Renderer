#pragma once

#include <unordered_map>
#include <string>

#include <shaders/shaders.h>
#include <utils/singleton.h>

class ShaderManager : public Singleton<ShaderManager>
{
public:
    Shader* LoadShader(const char* vertexPath, const char* fragmentPath);

    ShaderManager(Token) : Singleton() {}
protected:
    using MapToShaders = std::unordered_map<std::string, Shader>;
    MapToShaders m_mapToShaders;
};