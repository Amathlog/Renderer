#include "shaders/shaderManager.h"

Shader* ShaderManager::LoadShader(const char* vertexPath, const char* fragmentPath)
{
    std::string key(vertexPath);
    key += fragmentPath;
    MapToShaders::iterator it = m_mapToShaders.find(key);
    if (it == m_mapToShaders.end())
        it = m_mapToShaders.emplace(key, Shader(vertexPath, fragmentPath)).first;
    
    return &it->second;
}