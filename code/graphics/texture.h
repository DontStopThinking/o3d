#pragma once

#include <glad/glad.h>

#include "common.h"

struct Texture
{
    u32 m_TextureId;
    GLenum m_Type;
    u32 m_Unit;
};

Texture CreateTexture(
    const char* const fileName,
    const GLenum texType,
    const GLenum slot,
    const GLenum format,
    const GLenum pixelType
);

// Assigns a texture unit to a texture.
void SetTextureUnit(const u32 shaderId, const char* const uniform, const u32 unit);

// Binds a texture.
void BindTexture(const Texture texture);

// Unbinds a texture.
void UnbindTexture(const Texture texture);

// Deletes a texture.
void DeleteTexture(const Texture texture);
