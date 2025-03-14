#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common.h"

struct Camera
{
    glm::vec3 m_Position;
    glm::vec3 m_Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

    int m_WindowWidth;
    int m_WindowHeight;

    // The speed and sensitivity of the camera when looking around.
    float m_Speed = 1.0f;
    float m_Sensitivity = 100.0f;
};

Camera CreateCamera(
    const int windowWidth,
    const int windowHeight,
    const glm::vec3 position
);

// Send the view and projection matrices to the shader.
void SetCameraMatrix(
    const Camera& camera,
    const float fovDegrees,
    const float nearPlane,
    const float farPlane,
    const u32 shader,
    const char* const uniform
);

void UpdateCameraInput(Camera& camera, GLFWwindow* window, const float dt);
