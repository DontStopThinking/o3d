#include "graphics/shader.h"

#include <string>
#include <fstream>

#include <glad/glad.h>

static std::string GetFileContents(const char* const fileName)
{
    std::ifstream in(fileName, std::ios::binary);
    if (!in)
    {
        LOG_ERROR("Failed to open file: %s.", fileName);
        return {};
    }

    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return contents;
}

u32 CreateShader(const char* const vertexFile, const char* const fragmentFile)
{
    const std::string vertexCode = GetFileContents(vertexFile);
    const std::string fragmentCode = GetFileContents(fragmentFile);

    const char* const vertexSource = vertexCode.c_str();
    const char* const fragmentSource = fragmentCode.c_str();

    u32 result = -1;

    // Create vertex shader.
    const u32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr); // Specify shader source.
    LOG_INFO("Compiling vertex shader...");
    glCompileShader(vertexShader); // Compile the shader.

    // Check shader compile error.
    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        constexpr u32 size = 512;
        char infoLog[size] = {};
        glGetShaderInfoLog(vertexShader, size, nullptr, infoLog);
        LOG_ERROR("Vertex shader compilation failed\n%s", infoLog);

        return result;
    }
    else
    {
        LOG_INFO("Successfully compiled vertex shader.");
    }

    // Create fragment shader.
    const u32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr); // Specify shader source.
    LOG_INFO("Compiling fragment shader...");
    glCompileShader(fragmentShader); // Compile the shader.

    // Check shader compile error.
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        constexpr u32 size = 512;
        char infoLog[size] = {};
        glGetShaderInfoLog(fragmentShader, size, nullptr, infoLog);
        LOG_ERROR("Fragment shader compilation failed\n%s", infoLog);

        return result;
    }
    else
    {
        LOG_INFO("Successfully compiled fragment shader.");
    }

    // Create shader program (final linked version with multiple shaders combined).
    result = glCreateProgram();
    LOG_INFO("Linked vertex and fragment shaders...");
    glAttachShader(result, vertexShader); // Attach vertex shader to the program.
    glAttachShader(result, fragmentShader); // Attach fragment shader to the program.
    glLinkProgram(result); // Link program.

    // Check shader program linking error.
    glGetProgramiv(result, GL_LINK_STATUS, &success);
    if (!success)
    {
        constexpr u32 size = 512;
        char infoLog[size] = {};
        glGetProgramInfoLog(result, size, nullptr, infoLog);
        LOG_ERROR("Shader program linking error:\n%s", infoLog);

        return false;
    }
    else
    {
        LOG_INFO("Successfully linked vertex and fragment shaders.");
    }

    // Delete shader objects; Don't need them anymore after linking succeeds.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return result;
}

void ActivateShader(const u32 id)
{
    glUseProgram(id);
}

void DeleteShader(const u32 id)
{
    glDeleteProgram(id);
}
