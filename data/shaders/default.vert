#version 440 core

layout (location = 0) in vec3 aPos; // Take an input position "aPos" in location 0.
layout (location = 1) in vec3 aColor; // Take an input color "aColor" in location 1.
layout (location = 2) in vec2 aTex; // Take an input texture coordinate in location 2.
layout (location = 3) in vec3 aNormal; // Input normals

out vec3 color; // Output a color for the fragment shader.
out vec2 texCoord; // Output a texture coordinate for the fragment shader.
out vec3 normal; // Output the normal for the fragment shader.
out vec3 currPos; // Output the current position for the fragment shader.

// Input camera matrix (which is the view + projection matrix) from C++.
uniform mat4 camMatrix;
// Input model matrix from C++.
uniform mat4 model;

void main()
{
    // Calculate current position.
    currPos = vec3(model * vec4(aPos, 1.0f));

    // Output the position/coordinates of all vertices
    gl_Position = camMatrix * vec4(currPos, 1.0);

    // Set position of the output vertex.
    color = aColor;
    texCoord = aTex;
    normal = aNormal;
}
