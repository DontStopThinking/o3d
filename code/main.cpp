#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <glad/glad.h> // glad.h must be included *before* any OpenGL stuff.
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common.h"
#include "graphics/vao.h"
#include "graphics/vbo.h"
#include "graphics/ebo.h"
#include "graphics/shader.h"
#include "graphics/texture.h"

// Timestamp: https://youtu.be/45MIykWJ-C4

enum class RenderMethod
{
    Fill,
    Wireframe,
};

static int g_WindowWidth = 1024;
static int g_WindowHeight = 720;
static GLFWwindow* g_Window = nullptr;

static u32 g_VAO = -1;
static u32 g_VBO = -1;
static u32 g_EBO = -1;
static u32 g_DefaultShader = 0;
static Texture g_Texture = {};

static u32 g_UniformScale = -1;

static float g_Rotation = 0.0f;

static RenderMethod g_RenderMethod = RenderMethod::Fill;

constexpr float VERTICES[] =
{ //     COORDINATES     /        COLORS      /   TexCoord
    -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
    -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
    0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
    0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
    0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	2.5f, 5.0f,
};

constexpr u32 INDICES[] =
{
    0, 1, 2,
    0, 2, 3,
    0, 1, 4,
    1, 2, 4,
    2, 3, 4,
    3, 0, 4,
};

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

    glEnable(GL_DEPTH_TEST);

    // Create shader.
    g_DefaultShader = CreateShader("shaders/default.vert", "shaders/default.frag");

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

    g_Texture = CreateTexture(
        "textures/brick.png",
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

static void Update(const float dt)
{
    g_Rotation += 30.0f * dt;
}

static void Render()
{
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ActivateShader(g_DefaultShader);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);

    view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));
    proj = glm::perspective(
        glm::radians(45.0f), scast<float>(g_WindowWidth / g_WindowHeight), 0.1f, 100.0f
    );

    // Make our model rotate.
    model = glm::rotate(model, glm::radians(g_Rotation), glm::vec3(0.0f, 1.0f, 0.0f));

    i32 modelLoc = glGetUniformLocation(g_DefaultShader, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    i32 viewLoc = glGetUniformLocation(g_DefaultShader, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    i32 projLoc = glGetUniformLocation(g_DefaultShader, "proj");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

    // Set texture.
    BindTexture(g_Texture);

    // Set the scale uniform variable.
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

    glDrawElements(GL_TRIANGLES, sizeof(INDICES), GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(g_Window);

    UnbindVAO();
}

static void FreeResources()
{
    DeleteVAO(g_VAO);
    DeleteEBO(g_EBO);
    DeleteVBO(g_VBO);
    DeleteShader(g_DefaultShader);
    DeleteTexture(g_Texture);
}

int main()
{
    int exitCode = EXIT_SUCCESS;

    if (const bool init = Initialize(); init)
    {
        double prevTime = glfwGetTime();
        double currTime = prevTime;
        double deltaTime = 0.0;

        while (!glfwWindowShouldClose(g_Window))
        {
            currTime = glfwGetTime();
            deltaTime = currTime - prevTime;
            prevTime = currTime;

            ProcessInput();

            Update(scast<float>(deltaTime));

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
