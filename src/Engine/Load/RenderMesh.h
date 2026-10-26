#pragma once

#include "glm/glm.hpp"


#include "BasicTypeAliases.h"
#include <string>
#include <vector>


//namespace KE
//{
struct VertexAttribute
{
    std::string name;
    u32         offset;
    u32         size;
};

struct RenderPrimitive
{
    std::vector<glm::vec3>                         pos;
    std::vector<float>                             attrib;
    std::vector<std::tuple<std::string, u32, u32>> attributes;
    std::vector<u32>                               indices;
};

struct RenderMesh
{
    std::vector<RenderPrimitive> primitives;
};
//} // namespace KE
