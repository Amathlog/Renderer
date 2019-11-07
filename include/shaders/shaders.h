#pragma once
  
#include <string>
#include "glm.hpp"

class Shader
{
public:
    // the program ID
    unsigned int m_ID;
  
    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    // use/activate the shader
    void Use() const;
    // utility uniform functions
    void SetBool(const std::string &name, bool value) const;  
    void SetInt(const std::string &name, int value) const;  
    void SetFloat(const std::string &name, float value) const;
    void SetVec4(const std::string &name, const glm::vec4 &value) const;
    void SetMat4(const std::string &name, const glm::mat4 &value) const;
private:
    void CheckCompileErrors(unsigned int shader, const std::string& type) const;
};
