#include "graphics/texture.h"

#include <stb_image.h>

#include "graphics/shader.h"

Texture CreateTexture(
    const char *const fileName,
    const GLenum texType,
    const GLenum slot,
    const GLenum format,
    const GLenum pixelType
)
{
    Texture result = {};

    result.m_Type = texType;

    i32 imgWidth, imgHeight, numOfColorChannels;
    u8* const imgBytes = stbi_load(
        fileName, &imgWidth, &imgHeight, &numOfColorChannels, 0
    );

    glGenTextures(1, &result.m_TextureId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, result.m_TextureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgBytes);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(imgBytes);

    glBindTexture(GL_TEXTURE_2D, 0);

    return result;
}

void SetTextureUnit(const u32 shaderId, const char *const uniform, const u32 unit)
{
    const u32 texUniform = glGetUniformLocation(shaderId, uniform);
    ActivateShader(shaderId);
    glUniform1i(texUniform, unit);
}

void BindTexture(const Texture texture)
{
    glBindTexture(GL_TEXTURE_2D, texture.m_TextureId);
}

void UnbindTexture(const Texture texture)
{
    glBindTexture(texture.m_Type, 0);
}

void DeleteTexture(const Texture texture)
{
    glDeleteTextures(1, &texture.m_Type);
}
