#version 440 core

// Input a color from the vertex shader.
in vec3 color;

// Input a texture coordinate from the vertex shader.
in vec2 texCoord;

// This fragment shader outputs a vec4 color.
out vec4 FragColor;

// This uniform tells OpenGL which texture unit it should use.
uniform sampler2D tex0;

void main()
{
    FragColor = texture(tex0, texCoord);
}
