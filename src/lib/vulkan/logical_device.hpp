#pragma once
#include "vulkan/vulkan.hpp"
#include <map>
#include <functional>
#include "vulkan/memory.hpp"
#include "jpu/memory"
#include "create_info.hpp"

namespace vkn
{
    class Window;

    struct Queue
    {
        Queue() = default;
        Queue(const vk::Queue queue, const uint32_t family)
            : queue(queue), family(family) {}

        vk::Queue queue{nullptr};
        uint32_t family{0};
    };

    struct LogicalDeviceCreateInfo
    {
        LogicalDeviceCreateInfo(vk::PhysicalDevice gpu = nullptr,
        vk::SurfaceKHR surface = nullptr, const vk::QueueFlags enabled_queues = vk::QueueFlagBits::eGraphics, const uint32_t memory_block_size = 1<<25)
            : gpu(gpu), surface(surface), enabled_queues(enabled_queues), memory_block_size(memory_block_size) {}

        LogicalDeviceCreateInfo& setGpu(vk::PhysicalDevice value) { gpu = value; return *this; }
        LogicalDeviceCreateInfo& setSurface(vk::SurfaceKHR value) { surface = value; return *this; }
        LogicalDeviceCreateInfo& setEnabledQueues(const vk::QueueFlags value) { enabled_queues = value; return *this; }
        LogicalDeviceCreateInfo& setMemoryBlockSize(const uint32_t value) { memory_block_size = value; return *this; }
        
        vk::PhysicalDevice gpu;
        vk::SurfaceKHR surface;
        vk::QueueFlags enabled_queues;
        uint32_t memory_block_size;
    };

    class LogicalDevice : ClassInfo<LogicalDeviceCreateInfo, LogicalDevice>, public vk::Device, public jpu::ref_count
    {
    public:
        explicit LogicalDevice(LogicalDeviceCreateInfo info);
        LogicalDevice(const LogicalDevice& other) = default;
        LogicalDevice& operator=(const LogicalDevice& other) = default;
        LogicalDevice(LogicalDevice&& other) noexcept = delete;
        LogicalDevice& operator=(LogicalDevice&& other) noexcept = delete;
        ~LogicalDevice();
        
        const Queue& queue(const vk::QueueFlagBits type) const;
        const vk::CommandPool& commandPool(const vk::QueueFlagBits type) const;
        const std::vector<uint32_t>& families() const;
        void oneTimeCommand(const vk::QueueFlagBits queue_type, std::function<void(vk::CommandBuffer)> runnable, vk::Fence fence = nullptr) const;
        vk::PhysicalDevice gpu() const;
        MemoryPool* memory() const;

        operator vk::Device() const;

    private:
        std::map<vk::QueueFlagBits, Queue> m_queues;
		std::vector<uint32_t> m_unique_families;
        std::map<vk::QueueFlagBits, vk::CommandPool> m_command_pools;
        jpu::ref_ptr<MemoryPool> m_memory_pool;
    };
}
