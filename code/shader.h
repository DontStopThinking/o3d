#pragma once

#include "common.h"

u32 CreateShader(const char* const vertexFile, const char* const fragmentFile);
void ActivateShader(const u32 id);
void DeleteShader(const u32 id);
