#pragma once

#include "Renderer/Types/Material.h"
#include "GpuInterface/Types/Buffer.h"

namespace KE::R
{

    // sub meshes are the smallest unit of primitive collections.

    struct Submesh
    {
        //Holds position too for now. Will later only hold normals, tangents, UVs....
        //Splitting positions
        

        VK::Buffer vertexAttributes_;
        VK::Buffer indices_;
        
        std::vector<vk::VertexInputAttributeDescription2EXT> attributeDescs_;
        vk::VertexInputBindingDescription2EXT binding_;
        std::shared_ptr<Material> material;
        
        u32 count_;
        u32 stride_;

        Submesh() = default;

        Submesh(Submesh&&) noexcept = default;
        
        Submesh(const Submesh& other) = delete; 
        
        Submesh& operator=(const Submesh& other) = delete; 
    };
}