#include "vao.h"

#include <glad/glad.h>

u32 CreateVAO()
{
    u32 result = -1;
    glGenVertexArrays(1, &result);
    return result;
}

void LinkVBO(const u32 vboID, const u32 layout)
{
    BindVBO(vboID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
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
