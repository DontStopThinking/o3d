#version 440 core

// Input a color from the vertex shader.
in vec3 color;

// This fragment shader outputs a vec4 color.
out vec4 FragColor;

void main()
{
    FragColor = vec4(color, 1.0f);
}
