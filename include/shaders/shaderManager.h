#pragma once

#include <unordered_map>
#include <string>

#include "shaders/shaders.h"

class ShaderManager
{
public:
    static ShaderManager* GetInstance()
    {
        if (ms_instance == nullptr)
            ms_instance = new ShaderManager();
        return ms_instance;
    }

    ~ShaderManager()
    {
        if (ms_instance != nullptr)
            delete ms_instance;
    }

    Shader* LoadShader(const char* vertexPath, const char* fragmentPath);
private:
    ShaderManager() = default;
    static inline ShaderManager* ms_instance = nullptr;
    using MapToShaders = std::unordered_map<std::string, Shader>;
    MapToShaders m_mapToShaders;
};