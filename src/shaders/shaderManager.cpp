#include <shaders/shaderManager.h>

Shader* ShaderManager::LoadShader(const char* vertexShaderName, const char* fragmentShaderName)
{
    std::string key(vertexShaderName);
    key += fragmentShaderName;
    MapToShaders::iterator it = m_mapToShaders.find(key);
    if (it == m_mapToShaders.end())
        it = m_mapToShaders.emplace(key, Shader(vertexShaderName, fragmentShaderName)).first;
    
    return &it->second;
}