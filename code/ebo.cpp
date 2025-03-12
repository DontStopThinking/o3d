#include "ebo.h"

#include <glad/glad.h>

u32 CreateEBO(const u32* const indices, const size_t size)
{
    u32 result = -1;
    glGenBuffers(1, &result);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
    return result;
}

void BindEBO(const u32 id)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void UnbindEBO()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DeleteEBO(const u32 id)
{
    glDeleteBuffers(1, &id);
}
