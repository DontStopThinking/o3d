#include "camera.h"

#include <cmath>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

Camera CreateCamera(const int windowWidth, const int windowHeight, const glm::vec3 position)
{
    const Camera result =
    {
        .m_Position = position,
        .m_WindowWidth = windowWidth,
        .m_WindowHeight = windowHeight,
    };

    return result;
}

void SetCameraMatrix(
    const Camera& camera,
    const float fovDegrees,
    const float nearPlane,
    const float farPlane,
    const u32 shader,
    const char* const uniform
)
{
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);

    view = glm::lookAt(
        camera.m_Position,
        camera.m_Position + camera.m_Orientation,
        camera.m_Up
    );

    proj = glm::perspective(
        glm::radians(fovDegrees),
        scast<float>(camera.m_WindowWidth / camera.m_WindowHeight),
        nearPlane,
        farPlane
    );

    glUniformMatrix4fv(
        glGetUniformLocation(shader, uniform),
        1,
        GL_FALSE,
        glm::value_ptr(proj * view)
    );
}

static bool g_CameraFirstClick = true;

void UpdateCameraInput(Camera& camera, GLFWwindow* window, const float dt)
{
    const glm::vec3 right = glm::normalize(glm::cross(camera.m_Orientation, camera.m_Up));

    // Handle keyboard input.
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.m_Position += (camera.m_Speed * dt) * camera.m_Orientation;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.m_Position += (-camera.m_Speed * dt) * camera.m_Orientation;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.m_Position += (camera.m_Speed * dt) * -right;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.m_Position += (camera.m_Speed * dt) * right;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        camera.m_Position += (camera.m_Speed * dt) * camera.m_Up;
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        camera.m_Position += (-camera.m_Speed * dt) * camera.m_Up;
    }

    // Handle mouse input.
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        if (g_CameraFirstClick)
        {
            glfwSetCursorPos(window, (camera.m_WindowWidth / 2.0), (camera.m_WindowHeight / 2.0));
            g_CameraFirstClick = false;
        }

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        const float rotX = camera.m_Sensitivity
            * scast<float>(mouseY - (camera.m_WindowHeight / 2.0f)) / camera.m_WindowHeight;
        const float rotY = camera.m_Sensitivity
            * scast<float>(mouseX - (camera.m_WindowWidth / 2.0f)) / camera.m_WindowWidth;

        const glm::vec3 newOrientation = glm::rotate(
            camera.m_Orientation,
            glm::radians(-rotX),
            right
        );

        if (std::abs(glm::angle(newOrientation, camera.m_Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
        {
            camera.m_Orientation = newOrientation;
        }

        camera.m_Orientation = glm::rotate(camera.m_Orientation, glm::radians(-rotY), camera.m_Up);

        glfwSetCursorPos(window, (camera.m_WindowWidth / 2.0), (camera.m_WindowHeight / 2.0));
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        g_CameraFirstClick = true;
    }
}
