#include "shaders/shaders.h"

#include <glad/glad.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // WIN32

#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char* vertexShaderName, const char* fragmentShaderName)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        std::filesystem::path shaderDir;
	char currentPath[1024];
#ifdef WIN32
        HMODULE hModule = GetModuleHandle(NULL);
        if (hModule != NULL)
        {
            GetModuleFileName(hModule, currentPath, (sizeof(currentPath)));
            shaderDir = currentPath;
            shaderDir = shaderDir.parent_path();
        }
#else
        ssize_t count = readlink("/proc/self/exe", currentPath, 1024);
        if (count != -1) 
        {
            shaderDir = currentPath;
            shaderDir = shaderDir.parent_path();
        }
#endif // WIN32
        // First get the current directory
        shaderDir.append("shaders");
        // open files
        vShaderFile.open(shaderDir / vertexShaderName);
        fShaderFile.open(shaderDir / fragmentShaderName);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (const std::ifstream::failure&)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char * fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    CheckCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    CheckCompileErrors(fragment, "FRAGMENT");
    // shader Program
    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertex);
    glAttachShader(m_ID, fragment);
    glLinkProgram(m_ID);
    CheckCompileErrors(m_ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}
// activate the shader
// ------------------------------------------------------------------------
void Shader::Use() const
{ 
    glUseProgram(m_ID); 
}
// ------------------------------------------------------------------------
void Shader::Disable() const
{ 
    glUseProgram(0); 
}
// utility uniform functions
// ------------------------------------------------------------------------
void Shader::SetBool(const std::string &name, bool value) const
{         
    glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value); 
}
// ------------------------------------------------------------------------
void Shader::SetInt(const std::string &name, int value) const
{ 
    glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value); 
}
// ------------------------------------------------------------------------
void Shader::SetFloat(const std::string &name, float value) const
{ 
    glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value); 
}
//-------------------------------------------------------------------------
void Shader::SetVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(m_ID, name.c_str()), 1, glm::value_ptr(value));
}
//-------------------------------------------------------------------------
void Shader::SetMat4(const std::string &name, const glm::mat4 &value) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::CheckCompileErrors(unsigned int shader, const std::string& type) const
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
