#version 440 core

// Input a color from the vertex shader.
in vec3 color;
// Input a texture coordinate from the vertex shader.
in vec2 texCoord;
// Input a normal from the vertex shader.
in vec3 normal;
// Input the current position from the vertex shader.
in vec3 currPos;

// This fragment shader outputs a vec4 color.
out vec4 FragColor;

// Which texture units to use, specified from C++.
uniform sampler2D tex0;
uniform sampler2D tex1;
// Color of the light, specified from C++.
uniform vec4 lightColor;
// Position of the light, specified from C++.
uniform vec3 lightPos;
// Position of the camera, specified from C++.
uniform vec3 camPos;

void main()
{
    // Ambient lighting.
    float ambient = 0.20f;

    // Diffuse lighting.
    vec3 myNormal = normalize(normal);
    vec3 lightDir = normalize(lightPos - currPos);
    float diffuse = max(dot(normal, lightDir), 0.0f);

    // Specular lighting
    float specularLight = 0.50f;
    vec3 viewDir = normalize(camPos - currPos);
    vec3 reflectionDir = reflect(-lightDir, myNormal);
    float specAmount = pow(max(dot(viewDir, reflectionDir), 0.0f), 16);
    float specular = specAmount * specularLight;

    FragColor = (texture(tex0, texCoord) * (diffuse + ambient) + texture(tex1, texCoord).r * specular) * lightColor;
}
