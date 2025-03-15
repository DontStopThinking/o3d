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
#include "camera.h"
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
static u32 g_LightVAO = -1;
static u32 g_LightVBO = -1;
static u32 g_LightEBO = -1;
static u32 g_DefaultShader = -1;
static u32 g_LightShader = -1;
static Texture g_Texture = {};
static Texture g_TextureSpecular = {};
static Camera g_Camera = {};

static RenderMethod g_RenderMethod = RenderMethod::Fill;

constexpr float VERTICES[] =
{ //     COORDINATES     /        COLORS           /   TexCoord   /       Normals
    -1.0f, 0.0f,  1.0f,		0.0f, 0.0f, 0.0f,		0.0f, 0.0f,		0.0f, 1.0f, 0.0f,
    -1.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		0.0f, 1.0f,		0.0f, 1.0f, 0.0f,
     1.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
     1.0f, 0.0f,  1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 0.0f,		0.0f, 1.0f, 0.0f,
};

constexpr u32 INDICES[] =
{
    0, 1, 2,
    0, 2, 3,
};

constexpr float LIGHT_VERTICES[] =
{ //     COORDINATES     //
    -0.1f, -0.1f,  0.1f,
    -0.1f, -0.1f, -0.1f,
     0.1f, -0.1f, -0.1f,
     0.1f, -0.1f,  0.1f,
    -0.1f,  0.1f,  0.1f,
    -0.1f,  0.1f, -0.1f,
     0.1f,  0.1f, -0.1f,
     0.1f,  0.1f,  0.1f
};

constexpr u32 LIGHT_INDICES[] =
{
    0, 1, 2,
    0, 2, 3,
    0, 4, 7,
    0, 7, 3,
    3, 7, 6,
    3, 6, 2,
    2, 6, 5,
    2, 5, 1,
    1, 5, 4,
    1, 4, 0,
    4, 5, 6,
    4, 6, 7
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
    // SECTION: Initialize GLFW.
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

    // SECTION: Initialize GLAD.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_ERROR("Failed to initialize GLAD.");
        return false;
    }

    glViewport(0, 0, g_WindowWidth, g_WindowHeight); // Set viewport.

    glfwSetFramebufferSizeCallback(g_Window, FrameBufferSizeCallback); // Set window resize callback.

    glEnable(GL_DEPTH_TEST);

    // SECTION: Create default shader.
    g_DefaultShader = CreateShader("shaders/default.vert", "shaders/default.frag");

    g_VAO = CreateVAO();
    BindVAO(g_VAO);

    g_VBO = CreateVBO(VERTICES, sizeof(VERTICES));
    g_EBO = CreateEBO(INDICES, sizeof(INDICES));

    LinkAttrib(g_VBO, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0); // Coordinates
    LinkAttrib(g_VBO, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float))); // Colors
    LinkAttrib(g_VBO, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float))); // Texture Coordinates.
    LinkAttrib(g_VBO, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float))); // Normals

    UnbindVAO();
    UnbindVBO();
    UnbindEBO();

    // SECTION: Create light shader.
    g_LightShader = CreateShader("shaders/light.vert", "shaders/light.frag");

    g_LightVAO = CreateVAO();
    BindVAO(g_LightVAO);

    g_LightVBO = CreateVBO(LIGHT_VERTICES, sizeof(LIGHT_VERTICES));

    g_LightEBO = CreateEBO(LIGHT_INDICES, sizeof(LIGHT_INDICES));

    LinkAttrib(g_LightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

    UnbindVAO();
    UnbindEBO();
    UnbindVBO();

    // SECTION: Pass values to shader uniforms.
    const glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    const glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);

    const glm::vec3 pyramidPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 pyramidModel = glm::mat4(1.0f);
    pyramidModel = glm::translate(pyramidModel, pyramidPos);

    ActivateShader(g_LightShader);
    glUniformMatrix4fv(
        glGetUniformLocation(g_LightShader, "model"),
        1,
        GL_FALSE,
        glm::value_ptr(lightModel)
    );
    glUniform4f(
        glGetUniformLocation(g_LightShader, "lightColor"),
        lightColor.x,
        lightColor.y,
        lightColor.z,
        lightColor.w
    );

    ActivateShader(g_DefaultShader);
    glUniformMatrix4fv(
        glGetUniformLocation(g_DefaultShader, "model"),
        1,
        GL_FALSE,
        glm::value_ptr(pyramidModel)
    );
    glUniform4f(
        glGetUniformLocation(g_DefaultShader, "lightColor"),
        lightColor.x,
        lightColor.y,
        lightColor.z,
        lightColor.w
    );
    glUniform3f(
        glGetUniformLocation(g_DefaultShader, "lightPos"),
        lightPos.x,
        lightPos.y,
        lightPos.z
    );

    // SECTION: Texture
    stbi_set_flip_vertically_on_load(true); // OpenGL reads images from bottom-left corder to top-right corner.
                                            // Whereas STB_Image by default reads them from the top-left corner
                                            // to the bottom-right corner. So we use this to make STB_Image's
                                            // behaviour more similar to OpenGL.

    g_Texture = CreateTexture(
        "textures/planks.png",
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE
    );
    SetTextureUnit(g_DefaultShader, "tex0", 0);

    g_TextureSpecular = CreateTexture(
        "textures/planksSpec.png",
        GL_TEXTURE_2D,
        1,
        GL_RED,
        GL_UNSIGNED_BYTE
    );
    SetTextureUnit(g_DefaultShader, "tex1", 1);

    // SECTION: Camera
    g_Camera = CreateCamera(g_WindowWidth, g_WindowHeight, glm::vec3(0.0f, 0.0f, 2.0f));

    return true;
}

static void ProcessInput(const float dt)
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

    UpdateCameraInput(g_Camera, g_Window, dt);
}

static void Update(const float dt)
{
}

static void Render()
{
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    UpdateCameraMatrix(
        g_Camera,
        45.0f,
        0.1f,
        100.0f
    );

    ActivateShader(g_DefaultShader);

    ExportCameraMatrixToShader(g_Camera, g_DefaultShader, "camMatrix");

    // Set texture.
    BindTexture(g_Texture);
    BindTexture(g_TextureSpecular);

    BindVAO(g_VAO);

    if (g_RenderMethod == RenderMethod::Wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glDrawElements(GL_TRIANGLES, sizeof(INDICES) / sizeof(u32), GL_UNSIGNED_INT, 0);

    ActivateShader(g_LightShader);

    ExportCameraMatrixToShader(g_Camera, g_LightShader, "camMatrix");

    BindVAO(g_LightVAO);

    glDrawElements(GL_TRIANGLES, sizeof(LIGHT_INDICES) / sizeof(u32), GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(g_Window);

    UnbindVAO();
}

static void FreeResources()
{
    DeleteVAO(g_VAO);
    DeleteEBO(g_EBO);
    DeleteVBO(g_VBO);
    DeleteShader(g_DefaultShader);
    DeleteVAO(g_LightVAO);
    DeleteEBO(g_LightVBO);
    DeleteVBO(g_LightEBO);
    DeleteShader(g_LightShader);
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

            ProcessInput(scast<float>(deltaTime));

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
