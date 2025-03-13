#include "graphics/vbo.h"

#include <glad/glad.h>

u32 CreateVBO(const float* const vertices, const size_t size)
{
    u32 result = -1;
    glGenBuffers(1, &result);
    glBindBuffer(GL_ARRAY_BUFFER, result);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    return result;
}

void BindVBO(const u32 id)
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

void UnbindVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DeleteVBO(const u32 id)
{
    glDeleteBuffers(1, &id);
}
