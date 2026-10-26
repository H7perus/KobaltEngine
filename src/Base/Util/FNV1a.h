#pragma once

#include "BasicTypeAliases.h"
#include <string>

constexpr u64 FNV_OFFSET = 0xcbf29ce484222325ULL;
constexpr u64 FNV_PRIME  = 0x100000001b3ULL;


constexpr inline u64 FNV1aTo64(const char* input)
{
    u64 hash = FNV_OFFSET;
    //char_traits::length instead of strlen: it's actually constexpr
    for (size_t i = 0; i < std::char_traits<char>::length(input); i++)
    {
        hash ^= input[i];
        hash *= FNV_PRIME;
    }
    return hash;
}