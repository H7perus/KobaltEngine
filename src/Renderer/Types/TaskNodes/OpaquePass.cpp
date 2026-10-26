#include "OpaquePass.h"

#include "Renderer/Types/RenderContext.h"

KE::VK::OpaquePass::OpaquePass()
{
    Writes("HDRFramebuffer");
}
void KE::VK::OpaquePass::Execute(vk::CommandBuffer cmd)
{
    auto& renderContext = R::RenderContext::GetInstance();

}
