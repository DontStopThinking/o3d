#version 440 core

layout (location = 0) in vec3 aPos; // Take an input position "aPos" in location 0.
layout (location = 1) in vec3 aColor; // Take an input color "aColor" in location 1.
layout (location = 2) in vec2 aTex; // Take an input texture coordinate in location 2.

out vec3 color; // Output a color for the fragment shader.
out vec2 texCoord; // Output a texture coordinate for the fragment shader.

// Input camera matrix (which is the view + projection matrix).
uniform mat4 camMatrix;

void main()
{
    // Set position of the output vertex.
    gl_Position = camMatrix * vec4(aPos, 1.0);
    color = aColor;
    texCoord = aTex;
}
