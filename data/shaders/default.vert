#version 440 core

layout (location = 0) in vec3 aPos; // Take an input position "aPos" in location 0.
layout (location = 1) in vec3 aColor; // Take an input color "aColor" in location 1.

out vec3 color; // Output a color from the vertex shader.

// A uniform is a variable that can be accessed from C++ and from other shaders.
uniform float scale;

void main()
{
    // Set position of the output vertex.
    gl_Position = vec4(
        aPos.x + aPos.x * scale,
        aPos.y + aPos.y * scale,
        aPos.z + aPos.z * scale,
        1.0
    );
    color = aColor;
}
