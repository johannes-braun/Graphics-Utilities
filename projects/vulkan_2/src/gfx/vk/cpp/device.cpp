#include "../device.hpp"
#include "../swapchain.hpp"
#include "../semaphore.hpp"
#include "../commands.hpp"
#include "../fence.hpp"

#include <cassert>

namespace gfx::vk
{
    queue_info::queue_info(uint32_t family_index) : family_index(family_index) {}
    queue_info::queue_info(uint32_t family_index, array_view<float> priorities)
        : family_index(family_index), priorities(priorities.data(), priorities.data()+priorities.size()) { }

    queue_info::operator VkDeviceQueueCreateInfo() const noexcept
    {
        VkDeviceQueueCreateInfo info;
        info.flags = 0;
        info.pNext = nullptr;
        info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        info.pQueuePriorities = priorities.data();
        info.queueCount = uint32_t(priorities.size());
        info.queueFamilyIndex = family_index;
        return info;
    }

    device::device(const std::shared_ptr<physical_device>& physical_device, array_view<const char*> layers, array_view<const char*> extensions, array_view<queue_info> queue_infos, const VkPhysicalDeviceFeatures& features)
        : _physical_device(physical_device) {
        VkDeviceCreateInfo create_info;
        create_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.pNext                   = nullptr;
        create_info.flags                   = 0;
        create_info.enabledExtensionCount   = uint32_t(extensions.size());
        create_info.ppEnabledExtensionNames = extensions.data();
        create_info.enabledLayerCount       = uint32_t(layers.size());
        create_info.ppEnabledLayerNames     = layers.data();

        std::vector<VkDeviceQueueCreateInfo> infos(queue_infos.data(), queue_infos.data() + queue_infos.size());
        create_info.queueCreateInfoCount = uint32_t(infos.size());
        create_info.pQueueCreateInfos    = infos.data();
        create_info.pEnabledFeatures     = &features;
        vkCreateDevice(_physical_device->_physical_device, &create_info, nullptr, &_device);
    }

    device::~device()
    {
        vkDestroyDevice(_device, nullptr);
    }

    void device::wait_idle() const noexcept
    {
        vkDeviceWaitIdle(_device);
    }

    device::operator VkDevice() const noexcept { return _device; }

    queue::queue(const std::shared_ptr<device>& device, uint32_t family, uint32_t index)
        : _device(device), _family(family)
    {
        vkGetDeviceQueue(*_device, family, index, &_queue);
    }

    uint32_t queue::family() const noexcept { return _family; }
    queue::operator VkQueue() const noexcept { return _queue; }

    VkResult queue::present(array_view<std::shared_ptr<semaphore>> wait_sem, array_view<std::shared_ptr<swapchain>> swapchains, array_view<uint32_t> image_indices) const noexcept
    {
        std::vector<VkSwapchainKHR> sc(swapchains.size());
        for (int i=0; i<sc.size(); ++i) sc[i] = *(swapchains.data()[i]);
        std::vector<VkSemaphore> sems(wait_sem.size());
        for (int i=0; i<sems.size(); ++i) sems[i] = *(wait_sem.data()[i]);
        VkPresentInfoKHR info;
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.pNext = nullptr;
        info.pImageIndices = image_indices.data();
        info.pResults = nullptr;
        info.pSwapchains = sc.data();
        info.swapchainCount = uint32_t(sc.size());
        info.pWaitSemaphores = sems.data();
        info.waitSemaphoreCount = uint32_t(sems.size());
        
        return vkQueuePresentKHR(_queue, &info);
    }
    void queue::submit(array_view<std::shared_ptr<command_buffer>> command_buffers, const std::shared_ptr<fence>& fence) const noexcept
    {
        submit(command_buffers, nullptr, fence);
    }

    void queue::submit(array_view<std::shared_ptr<command_buffer>> command_buffers, array_view<std::shared_ptr<semaphore>> signal_sem, const std::shared_ptr<fence>& fence) const noexcept
    {
        submit(command_buffers, signal_sem, nullptr, nullptr, fence);
    }

    void queue::submit(array_view<std::shared_ptr<command_buffer>> command_buffers, array_view<std::shared_ptr<semaphore>> signal_sem, array_view<std::shared_ptr<semaphore>> wait_sem, array_view<VkPipelineStageFlags> wait_stages, const std::shared_ptr<fence>& fence) const noexcept
    {
        assert(wait_sem.size() == wait_stages.size());
        std::vector<VkSemaphore> ssems(signal_sem.size());
        for (int i=0; i<ssems.size(); ++i) ssems[i] = *(signal_sem.data()[i]);
        std::vector<VkSemaphore> wsems(wait_sem.size());
        for (int i=0; i<wsems.size(); ++i) wsems[i] = *(wait_sem.data()[i]);
        std::vector<VkCommandBuffer> cbs(command_buffers.size());
        for (int i=0; i<cbs.size(); ++i) cbs[i] = *(command_buffers.data()[i]);

        VkSubmitInfo info;
        info.sType                  = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.pNext                  = nullptr;
        info.commandBufferCount     = uint32_t(cbs.size());
        info.pCommandBuffers        = cbs.data();
        info.signalSemaphoreCount   = uint32_t(ssems.size());
        info.pSignalSemaphores      = ssems.data();
        info.pWaitDstStageMask      = wait_stages.data();
        info.waitSemaphoreCount     = uint32_t(wsems.size());
        info.pWaitSemaphores        = wsems.data();
        
        vkQueueSubmit(_queue, 1, &info, fence?*fence:nullptr);
    }

    void queue::wait_idle() const noexcept
    {
        vkQueueWaitIdle(_queue);
    }
}