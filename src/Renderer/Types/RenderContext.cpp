#include "RenderContext.h"

KE::R::RenderContext::RenderContext()
{
    vertexPosBuffer = VK::Buffer(0, 1000000, vk::BufferUsageFlagBits::eVertexBuffer,
                         vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
}


KE::R::RenderContext &KE::R::RenderContext::GetInstance()
{
    static RenderContext instance;
    return instance;
};