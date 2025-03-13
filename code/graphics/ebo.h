#pragma once

#include "common.h"

u32 CreateEBO(const u32* const indices, const size_t size);
void BindEBO(const u32 id);
void UnbindEBO();
void DeleteEBO(const u32 id);
