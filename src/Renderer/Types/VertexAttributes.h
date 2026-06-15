#pragma once


#include "vulkan/vulkan.hpp"

#include "BasicTypeAliases.h"
#include <map>
#include <bitset>
#include <string>




namespace KE::R
{
struct VertexAttributeInfo {
    u8    index;
    vk::Format  format;
};

//Position separately for z-prepass and RT performance (cache coherence)
static std::map<std::string, VertexAttributeInfo> vertexAttributeMap = {
    {"NORMAL", {1, vk::Format::eR16G16B16Sfloat}},
    {"TANGENT", {2, vk::Format::eR16G16B16Sfloat}},
    {"TEXCOORD", {3, vk::Format::eR32G32Sfloat}},
    // Color for vertex painting
    // blend indices for bones
    // blend weights for bones
};

}