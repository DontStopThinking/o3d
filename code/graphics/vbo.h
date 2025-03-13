#pragma once
#include "common.h"

u32 CreateVBO(const float* const vertices, const size_t size);
void BindVBO(const u32 id);
void UnbindVBO();
void DeleteVBO(const u32 id);
