#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <glad/glad.h> // glad.h must be included *before* any OpenGL stuff.
#include <GLFW/glfw3.h>

#include "common.h"
#include "graphics/vao.h"
#include "graphics/vbo.h"
#include "graphics/ebo.h"
#include "graphics/shader.h"

// Timestamp: https://youtu.be/45MIykWJ-C4

static int g_WindowWidth = 1024;
static int g_WindowHeight = 720;
static GLFWwindow* g_Window = nullptr;

static u32 g_DefaultShader = 0;
static u32 g_VAO = -1;
static u32 g_VBO = -1;
static u32 g_EBO = -1;

enum class RenderMethod
{
    Fill,
    Wireframe,
};

static RenderMethod g_RenderMethod = RenderMethod::Fill;

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

#if _DEBUG
    constexpr const char* WINDOW_TITLE = "O3D [DEBUG]";
#else
    constexpr const char* WINDOW_TITLE = "O3D [RELEASE]";
#endif // _DEBUG

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

    // Create shader.
    g_DefaultShader = CreateShader("shaders/default.vert", "shaders/default.frag");

    // Create a rectangle.
    const float vertices[] =
    {//                   COORDINATES                             RGB COLORS
        -0.5f,  (-0.5f  * sqrtf(3.0f) / 3.0f),        0.0f,    0.8f, 0.3f, 0.02f,    // Lower-left corner
         0.5f,  (-0.5f  * sqrtf(3.0f) / 3.0f),        0.0f,    0.8f, 0.3f, 0.02f,    // Lower-right corner
         0.0f,  ( 0.5f  * sqrtf(3.0f) * 2.0f / 3.0f), 0.0f,    1.0f, 0.6f, 0.32f,    // Upper corner
        -0.25f, ( 0.5f  * sqrtf(3.0f) / 6.0f),        0.0f,    0.9f, 0.45f, 0.17f,   // Inner left
         0.25f, ( 0.5f  * sqrtf(3.0f) / 6.0f),        0.0f,    0.9f, 0.45f, 0.17f,   // Inner right
         0.0f,  (-0.5f  * sqrtf(3.0f) / 3.0f),        0.0f,    0.8f, 0.3f, 0.02f,    // Inner down
    };

    constexpr u32 INDICES[] =
    {
        0, 3, 5, // lower left triangle
        3, 2, 4, // lower right triangle
        5, 4, 1, // Upper triangle
    };

    g_VAO = CreateVAO();
    BindVAO(g_VAO);

    g_VBO = CreateVBO(vertices, sizeof(vertices));
    g_EBO = CreateEBO(INDICES, sizeof(INDICES));

    LinkAttrib(g_VBO, 0, 3, GL_FLOAT, 6 * sizeof(float), 0); // Coordinates
    LinkAttrib(g_VBO, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Colors

    UnbindVAO();
    UnbindVBO();
    UnbindEBO();

    return true;
}

static void ProcessInput()
{
    glfwPollEvents();

    // Escape to close.
    if (glfwGetKey(g_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(g_Window, true);
    }
    // 1 to switch to wireframe render.
    else if (glfwGetKey(g_Window, GLFW_KEY_1) == GLFW_PRESS)
    {
        if (g_RenderMethod != RenderMethod::Wireframe)
        {
            g_RenderMethod = RenderMethod::Wireframe;
            LOG_INFO("Set RenderMethod to \"RenderMethod::Wireframe\"");
        }
    }
    // 2 to switch to filled render.
    else if (glfwGetKey(g_Window, GLFW_KEY_2) == GLFW_PRESS)
    {
        if (g_RenderMethod != RenderMethod::Fill)
        {
            g_RenderMethod = RenderMethod::Fill;
            LOG_INFO("Set RenderMethod to \"RenderMethod::Fill\"");
        }
    }
}

static void Render()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ActivateShader(g_DefaultShader);

    BindVAO(g_VAO);

    if (g_RenderMethod == RenderMethod::Wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(g_Window);

    UnbindVAO();
}

static void FreeResources()
{
    DeleteVAO(g_VAO);
    DeleteEBO(g_EBO);
    DeleteVBO(g_VBO);
    DeleteShader(g_DefaultShader);
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
        }
    }
    else
    {
        exitCode = EXIT_FAILURE;
    }

    LOG_INFO("Shutting down...");

    FreeResources();

    glfwTerminate();

    return exitCode;
}
