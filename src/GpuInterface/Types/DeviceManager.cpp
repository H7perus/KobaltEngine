#include "DeviceManager.h"
using namespace KE::VK;

void KE::VK::ContextManager::Init()
{
    GetInstance();
}
KE::VK::ContextManager &KE::VK::ContextManager::GetInstance()
{
    static ContextManager instance;
    return instance;
};

Device& ContextManager::GetDevice(u32 index)
{
    return GetInstance().devices_.at(index);
}

Swapchain& ContextManager::GetSwapchain(u32 index)
{
    if(GetInstance().swapchains_.find(index) == GetInstance().swapchains_.end())
    {
        throw std::runtime_error("Swapchain index not found");
    }
    return GetInstance().swapchains_.at(index);
}


