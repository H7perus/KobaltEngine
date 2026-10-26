#pragma once

#include "BasicTypeAliases.h"

#include "Engine/Load/RenderMesh.h"
#include "Renderer/Types/Submesh.h"
#include <vector>



namespace KE::R
{
class Mesh
{


  public:
    std::string          meshName = "";
    std::vector<Submesh> submeshes_;
    Mesh() = default;
    Mesh(std::string meshName) : meshName(meshName)
    {
    }

    Mesh(Mesh&& other)      = default;
    Mesh(const Mesh& other) = delete;

    Mesh& operator=(const Mesh& other) = delete;
};
} // namespace KE::R