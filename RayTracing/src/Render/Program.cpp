#include "Program.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <sstream>

Program::Program(const std::string& computePath)
{
    std::string computeShader = loadProgramFromFile(computePath);
    m_RendererID = createProgram(computeShader);
}

Program::~Program()
{
    glDeleteProgram(m_RendererID);
}

void Program::Bind() const
{
    glUseProgram(m_RendererID);
}

void Program::Unbind() const
{
    glUseProgram(0);
}

void Program::Dispatch(uint32_t sizeX, uint32_t sizeY)
{
    glDispatchCompute(sizeX, sizeY, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void Program::SetUniform1i(const std::string& name, int32_t v0)
{
    glUniform1i(GetUniformLocation(name), v0);
}

void Program::SetUniform1ui(const std::string& name, uint32_t v0)
{
    glUniform1ui(GetUniformLocation(name), v0);
}

void Program::SetUniform1f(const std::string& name, float v0)
{
    glUniform1f(GetUniformLocation(name), v0);
}

void Program::SetUniform2f(const std::string& name, float v0, float v1)
{
    glUniform2f(GetUniformLocation(name), v0, v1);
}

void Program::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
    glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

void Program::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Program::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

int32_t Program::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    {
        return m_UniformLocationCache[name];
    }

    int32_t location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1)
    {
        std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
    }

    m_UniformLocationCache[name] = location;
    return location;
}

std::string Program::loadProgramFromFile(const std::string& filePath) const
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cout << "can't open file: '" << filePath << "'" << std::endl;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

uint32_t Program::compileProgram(const std::string& source) const
{
    uint32_t id = glCreateShader(GL_COMPUTE_SHADER);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int32_t result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int32_t length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Faild to compiled compute shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

uint32_t Program::createProgram(const std::string& ComputeShader) const
{
    uint32_t program = glCreateProgram();
    uint32_t cs = compileProgram(ComputeShader);

    glAttachShader(program, cs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(cs);

    return program;
}
