#pragma once

#include "GpuInterface/Types/Buffer.h"
#include "GpuInterface/Types/PipelineGraphics.h"
#include "ShaderCompile/ShaderReflection.h"

#include "Base/Util/FNV1a.h"

namespace KE::R
{



struct MaterialType
{
    // debug info
    std::string shaderPath;

    std::vector<VK::CompileTimeConstantValue> compileTimeConstants;
    // /debuginfo

    // Pipeline
    std::shared_ptr<VK::PipelineGraphics> pipeline_;

    // Reflection Data (Unpacked from SpecializedShader)
    std::vector<VK::VertexInput>  vertexInputs;
    std::vector<VK::StructMember> pushConstants;
    std::vector<VK::StructMember> materialParameters;
};

class Material
{
    std::shared_ptr<MaterialType> type;
    VK::Buffer                    MaterialParameters;
};

inline u64 FNV1aHashMaterialType(MaterialType type)
{
    u64 hash = FNV_OFFSET;

    hash ^= FNV_OFFSET;

    for (char c : type.shaderPath)
    {
        hash ^= c;
        hash *= FNV_PRIME;
    }

    for (int i = 0; i < type.compileTimeConstants.size(); i++)
    {
        hash ^= type.compileTimeConstants[i].value;
        hash *= FNV_PRIME;
    }
    return hash;
}

} // namespace KE::R