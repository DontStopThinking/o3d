#include "vao.h"

#include <glad/glad.h>

u32 CreateVAO()
{
    u32 result = -1;
    glGenVertexArrays(1, &result);
    return result;
}

void LinkAttrib(
    const u32 vboID,
    const u32 layout,
    const u32 numComponents,
    const u32 type,
    const u32 stride,
    const void* const offset
)
{
    BindVBO(vboID);
    // Tell OpenGL how to interpret our array of vertices.
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(layout);
    UnbindVBO();
}

void BindVAO(const u32 vaoID)
{
    glBindVertexArray(vaoID);
}

void UnbindVAO()
{
    glBindVertexArray(0);
}

void DeleteVAO(const u32 vaoID)
{
    glDeleteVertexArrays(1, &vaoID);
}
