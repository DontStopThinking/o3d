#include <cstdio>
#include <cstdlib>

#include <glad/glad.h> // glad.h must be included *before* any OpenGL stuff.
#include <GLFW/glfw3.h>

#include "log.h"

static constinit int g_WindowWidth = 1024;
static constinit int g_WindowHeight = 720;
constexpr const char* WINDOW_TITLE = "O3D";
static constinit GLFWwindow* g_Window = nullptr;

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

int main()
{
    const bool init = Initialize();

    int exitCode = EXIT_SUCCESS;

    if (init)
    {
        while (!glfwWindowShouldClose(g_Window))
        {
            ProcessInput();

            glfwSwapBuffers(g_Window);
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
