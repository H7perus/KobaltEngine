#pragma once

#include "BasicTypeAliases.h"
#include <string>

constexpr u64 FNV_OFFSET = 0xcbf29ce484222325ULL;
constexpr u64 FNV_PRIME  = 0x100000001b3ULL;


constexpr inline u64 FNV1aTo64(std::string_view input)
{
    u64 hash = FNV_OFFSET;
    for (size_t i = 0; i < input.length(); i++)
    {
        hash ^= input[i];
        hash *= FNV_PRIME;
    }
    return hash;
}