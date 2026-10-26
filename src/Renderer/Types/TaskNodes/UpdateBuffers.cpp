#include "UpdateBuffers.h"

#include "Renderer/Types/FrameData.h"
#include "Renderer/Types/RenderContext.h"
#include "Engine/General/Perspective.h"

#include "Engine/General/View.h"

#include "Base/Time/Clocks.h"

#include "GpuInterface/Types/DescriptorHandle.h"
#include "GpuInterface/Types/DeviceManager.h"


void KE::R::UpdateBuffers::Execute(vk::CommandBuffer cmd) 
{
    RenderContext& renderContext = RenderContext::GetInstance();
    renderContext.currentFrameInFlight = VK::ContextManager::GetInstance().GetSwapchain(0).GetCurrentFrameIndex();

    renderContext.FrameDataBuffers.SetCurrent(renderContext.currentFrameInFlight);
    renderContext.transformBuffers.SetCurrent(renderContext.currentFrameInFlight);

    VK::Buffer& frameDataBuffer = renderContext.FrameDataBuffers.GetResourceHandle().GetResourceRef<VK::Buffer>();
    VK::Buffer& transformBuffer = renderContext.transformBuffers.GetResourceHandle().GetResourceRef<VK::Buffer>();

    //TEMPORARY BELOW

    KE::View view;

    view.hFov = 90.f;

    float time = float(GlobalClock::QueryGlobalTime());

    view.origin.x = sin(time);
    view.origin.y = -cos(time);
    view.origin *= 6;

    view.SetAnglesEuler(ToDeg(time) + 90, 0);

    glm::mat4 identity =
        view.GetProjectionRotationMatrix(0.01);


    //UPDATE TRANSFORMS;
    for(auto& target : renderContext.staticOpaqueRenderables)
    {
        int transformIndex = target.second->gpuTransformIndex;

        renderContext.transforms[transformIndex] = target.first.get()->GetRelativeTransform(view);
    }

    //auto specTransform = glm::mat4x3(glm::mat3(1));

    //specTransform[3] = glm::vec3(0, 0, sin(time)); //glm::vec3(10);

    //renderContext.transforms[0] = glm::transpose(specTransform);


    void* transformBufferPointer = transformBuffer.map();

    memcpy(transformBufferPointer, renderContext.transforms.data(), renderContext.transforms.size() * sizeof(glm::mat3x4));

    transformBuffer.unmap();

    //TEMPORARY ABOVE

    FrameData_t* frameDataPtr = (FrameData_t*)frameDataBuffer.map();

    *frameDataPtr = {
        .ViewPerspectiveTransform = identity, 
        .currentTime = KE::GlobalClock::QueryGlobalTime(), 
        .timeDelta = 0.1,
        .transformBuffers = VK::DescriptorHandle(transformBuffer)};

    frameDataBuffer.unmap();

    vk::HostAddressRangeConstEXT range;

    VK::DescriptorHandle FrameDataHandle = VK::DescriptorHandle(frameDataBuffer);

    range.address = &FrameDataHandle;
    range.size    = sizeof(FrameDataHandle);
    vk::PushDataInfoEXT pushInfo;
    pushInfo.data = range;

    cmd.pushDataEXT(pushInfo);

    
};