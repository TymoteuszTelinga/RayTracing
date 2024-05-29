#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

// Class for hadeling compute shader only
class Program
{
public:
    Program(const std::string& computePath);
    ~Program();

    void Bind() const;
    void Unbind() const;

    void Dispatch(uint32_t sizeX, uint32_t sizeY);

    //Set uniforms
    void SetUniform1i(const std::string& name, int32_t v0);
    void SetUniform1ui(const std::string& name, uint32_t v0);
    void SetUniform1f(const std::string& name, float v0);
    void SetUniform2f(const std::string& name, float v0, float v1);
    void SetUniform3f(const std::string& name, float v0, float v1, float v2);
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

private:
    int32_t GetUniformLocation(const std::string& name);
    std::string loadProgramFromFile(const std::string& filePath) const;
    uint32_t compileProgram(const std::string& source) const;
    uint32_t createProgram(const std::string& ComputeShader) const;

private:
    uint32_t m_RendererID;
    //cashin for uniforms
    std::unordered_map<std::string, int32_t> m_UniformLocationCache;
};

