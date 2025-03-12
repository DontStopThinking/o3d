#pragma once

#include "vbo.h"

// Create a vertex array object and return its ID.
u32 CreateVAO();

// Link a vertex buffer object to the vertex array object using the given layout.
// TODO: Maybe this should take in an ID of the VAO to link the VBO to?
void LinkVBO(const u32 vboID, const u32 layout);

// Bind the VAO.
void BindVAO(const u32 vaoID);

// Unbind the VAO.
void UnbindVAO();

// Delete the VAO.
void DeleteVAO(const u32 vaoID);
