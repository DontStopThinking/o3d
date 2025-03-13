#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <glad/glad.h> // glad.h must be included *before* any OpenGL stuff.
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "common.h"
#include "graphics/vao.h"
#include "graphics/vbo.h"
#include "graphics/ebo.h"
#include "graphics/shader.h"
#include "graphics/texture.h"

// Timestamp: https://youtu.be/45MIykWJ-C4

static int g_WindowWidth = 1024;
static int g_WindowHeight = 720;
static GLFWwindow* g_Window = nullptr;

static u32 g_VAO = -1;
static u32 g_VBO = -1;
static u32 g_EBO = -1;
static u32 g_DefaultShader = 0;
static Texture g_PopCatTexture = {};

static u32 g_UniformScale = -1;

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
    constexpr float VERTICES[] =
    { //     COORDINATES     /        COLORS      /   TexCoord
        -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,	0.0f, 0.0f, // Lower left corner
        -0.5f,  0.5f, 0.0f,     0.0f, 1.0f, 0.0f,	0.0f, 1.0f, // Upper left corner
         0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // Upper right corner
         0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,	1.0f, 0.0f  // Lower right corner
    };

    constexpr u32 INDICES[] =
    {
        0, 2, 1, // Upper triangle.
        0, 3, 2, // Lower triangle.
    };

    g_VAO = CreateVAO();
    BindVAO(g_VAO);

    g_VBO = CreateVBO(VERTICES, sizeof(VERTICES));
    g_EBO = CreateEBO(INDICES, sizeof(INDICES));

    LinkAttrib(g_VBO, 0, 3, GL_FLOAT, 8 * sizeof(float), 0); // Coordinates
    LinkAttrib(g_VBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Colors
    LinkAttrib(g_VBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture Coordinates.

    UnbindVAO();
    UnbindVBO();
    UnbindEBO();

    // Get ID of uniform variable called "scale".
    g_UniformScale = glGetUniformLocation(g_DefaultShader, "scale");

    // Texture
    stbi_set_flip_vertically_on_load(true); // OpenGL reads images from bottom-left corder to top-right corner.
                                            // Whereas STB_Image by default reads them from the top-left corner
                                            // to the bottom-right corner. So we use this to make STB_Image's
                                            // behaviour more similar to OpenGL.

    g_PopCatTexture = CreateTexture(
        "textures/pop_cat.png",
        GL_TEXTURE_2D,
        GL_TEXTURE0,
        GL_RGBA,
        GL_UNSIGNED_BYTE
    );

    SetTextureUnit(g_DefaultShader, "tex0", 0);

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
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ActivateShader(g_DefaultShader);

    BindTexture(g_PopCatTexture);

    glUniform1f(g_UniformScale, 0.5f);

    BindVAO(g_VAO);

    if (g_RenderMethod == RenderMethod::Wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(g_Window);

    UnbindVAO();
}

static void FreeResources()
{
    DeleteVAO(g_VAO);
    DeleteEBO(g_EBO);
    DeleteVBO(g_VBO);
    DeleteShader(g_DefaultShader);
    DeleteTexture(g_PopCatTexture);
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
