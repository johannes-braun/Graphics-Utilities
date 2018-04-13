#pragma once

#include "array_view.hpp"
#include "physical_device.hpp"

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace gfx::vk
{
    class semaphore;
    class swapchain;
    class command_buffer;
    class fence;

    struct queue_info {
        queue_info(uint32_t family_index);
        queue_info(uint32_t family_index, array_view<float> priorities);

        operator VkDeviceQueueCreateInfo() const noexcept;

    private:
        VkDeviceQueueCreateFlags    _flags;
    public:
        uint32_t            family_index = 0;
        std::vector<float>  priorities{};
    };

    class device
    {
    public:
        device(const std::shared_ptr<physical_device>& physical_device, array_view<const char*> layers, array_view<const char*> extensions, array_view<queue_info> queue_infos, const VkPhysicalDeviceFeatures& features);
        ~device();
        device(const device&) = delete;
        device& operator=(const device&) = delete;
        device(device&&) = default;
        device& operator=(device&&) = default;

        void wait_idle() const noexcept;
        operator VkDevice() const noexcept;

    private:
        std::shared_ptr<physical_device> _physical_device;
        VkDevice _device;
    };

    class queue
    {
    public:
        queue(const std::shared_ptr<device>& device, uint32_t family, uint32_t index);

        uint32_t family() const noexcept;
        operator VkQueue() const noexcept;

        VkResult present(array_view<std::shared_ptr<semaphore>> wait_sem, array_view<std::shared_ptr<swapchain>> swapchains, array_view<uint32_t> image_indices) const noexcept;
        void submit(array_view<std::shared_ptr<command_buffer>> command_buffers, const std::shared_ptr<fence>& fence) const noexcept;
        void submit(array_view<std::shared_ptr<command_buffer>> command_buffers, array_view<std::shared_ptr<semaphore>> signal_sem, const std::shared_ptr<fence>& fence = nullptr) const noexcept;
        void submit(array_view<std::shared_ptr<command_buffer>> command_buffers, array_view<std::shared_ptr<semaphore>> signal_sem = nullptr, array_view<std::shared_ptr<semaphore>> wait_sem = nullptr, array_view<VkPipelineStageFlags> wait_stages = nullptr, const std::shared_ptr<fence>& fence = nullptr) const noexcept;

    private:
        std::shared_ptr<device> _device;
        VkQueue _queue;
        uint32_t _family;
    };
}