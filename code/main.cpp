#include <cstdio>
#include <cstdlib>

#include <glad/glad.h> // glad.h must be included *before* any OpenGL stuff.
#include <GLFW/glfw3.h>

#include "common.h"

static constinit int g_WindowWidth = 1024;
static constinit int g_WindowHeight = 720;
#if _DEBUG
constexpr const char* WINDOW_TITLE = "O3D [DEBUG]";
#else
constexpr const char* WINDOW_TITLE = "O3D [RELEASE]";
#endif
static constinit GLFWwindow* g_Window = nullptr;

static constinit u32 g_TriangleVAO = 0;
static constinit u32 g_ShaderProgram = 0;

constexpr const char* VERTEX_SHADER_SOURCE = R"(
    #version 440 core

    layout (location = 0) in vec3 aPos;

    void main()
    {
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }
)";

constexpr const char* FRAGMENT_SHADER_SOURCE = R"(
    #version 440 core

    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    }
)";

// Gets called when the window is resized.
static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    g_WindowWidth = width;
    g_WindowHeight = height;
}

static bool Initialize()
{
    // Initialize GLFW.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    g_Window = glfwCreateWindow(g_WindowWidth, g_WindowHeight, WINDOW_TITLE, nullptr, nullptr);
    if (g_Window == nullptr)
    {
        LOG_ERROR("Failed to create GLFW window.");
        return false;
    }

    glfwMakeContextCurrent(g_Window);

    // Initialize GLAD.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_ERROR("Failed to initialize GLAD.");
        return false;
    }

    glViewport(0, 0, g_WindowWidth, g_WindowHeight); // Set viewport.

    glfwSetFramebufferSizeCallback(g_Window, FrameBufferSizeCallback); // Set window resize callback.

    // Create a triangle.
    constexpr float VERTICES[] =
    {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,
    };

    // Create vertex buffer object (VBO).
    u32 vbo;
    glGenBuffers(1, &vbo); // Create vertex buffer object.
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // Specify that that buffer is an array.
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW); // Copy triangle vertices into the buffer's memory.

    // Create vertex shader.
    const u32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VERTEX_SHADER_SOURCE, nullptr); // Specify shader source.
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

        return false;
    }
    else
    {
        LOG_INFO("Successfully compiled vertex shader.");
    }

    // Create fragment shader.
    const u32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FRAGMENT_SHADER_SOURCE, nullptr); // Specify shader source.
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

        return false;
    }
    else
    {
        LOG_INFO("Successfully compiled fragment shader.");
    }

    // Create shader program (final linked version with multiple shaders combined).
    g_ShaderProgram = glCreateProgram();
    LOG_INFO("Linked vertex and fragment shaders...");
    glAttachShader(g_ShaderProgram, vertexShader); // Attach vertex shader to the program.
    glAttachShader(g_ShaderProgram, fragmentShader); // Attach fragment shader to the program.
    glLinkProgram(g_ShaderProgram); // Link program.

    // Check shader program linking error.
    glGetProgramiv(g_ShaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        constexpr u32 size = 512;
        char infoLog[size] = {};
        glGetProgramInfoLog(g_ShaderProgram, size, nullptr, infoLog);
        LOG_ERROR("Shader program linking error:\n%s", infoLog);

        return false;
    }
    else
    {
        LOG_INFO("Successfully linked vertex and fragment shaders.");
    }
    glUseProgram(g_ShaderProgram);

    // Delete shader objects; Don't need them anymore after linking succeeds.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Tell OpenGL how it should interpret the vertex data (per vertex attribute).
    glVertexAttribPointer(
        0,                  // Which vertex attr. to configure. We've specified layout 0 in our vertex shader.
        3,                  // The vertex attr. is a vec3, so size is 3 values.
        GL_FLOAT,           // The type of the data.
        GL_FALSE,           // Whether the vertex data should be normalized. Not relevant to us so false.
        3 * sizeof(float),  // The stride or the space between consecutive vertex attrs.
        (void*)0);          // Offset of where the position data begins in the buffer.

    // Create a vertex array object (VAO).
    glGenVertexArrays(1, &g_TriangleVAO);
    glBindVertexArray(g_TriangleVAO); // Bind the VAO.
    // Copy our vertices in a buffer for OpenGL to use.
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);
    // Set our vertex attributes pointers.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    return true;
}

static void ProcessInput()
{
    // Escape to close.
    if (glfwGetKey(g_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(g_Window, true);
    }
}

static void Render()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(g_ShaderProgram);
    glBindVertexArray(g_TriangleVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(g_Window);
}

int main()
{
    int exitCode = EXIT_SUCCESS;

    if (const bool init = Initialize(); init)
    {
        while (!glfwWindowShouldClose(g_Window))
        {
            ProcessInput();

            Render();

            glfwPollEvents();
        }
    }
    else
    {
        exitCode = EXIT_FAILURE;
    }

    LOG_INFO("Shutting down...");

    glfwTerminate();

    return exitCode;
}
