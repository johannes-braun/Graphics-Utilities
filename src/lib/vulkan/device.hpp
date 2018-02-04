#pragma once
#include <vulkan/vulkan.hpp>
#include <map>
#include <functional>
#include "memory.hpp"
#include <jpu/memory>

namespace vkn
{
    struct queue_info
    {
        vk::Queue queue{ nullptr };
        uint32_t family{ 0 };
    };

    class device : public vk::Device, public jpu::ref_count
    {
    public:
        explicit device(vk::PhysicalDevice gpu,
            vk::SurfaceKHR surface,
            vk::QueueFlags enabled_queues,
            uint32_t memory_block_size);
        ~device();

        const queue_info& queue(vk::QueueFlagBits type) const;
        const vk::CommandPool& command_pool(vk::QueueFlagBits type) const;
        const std::vector<uint32_t>& families() const;
        void unique_command(vk::QueueFlagBits queue_type, std::function<void(vk::CommandBuffer)> runnable, vk::Fence fence = nullptr) const;

        vk::PhysicalDevice gpu() const;
        memory_pool* memory() const;

    private:
        std::map<vk::QueueFlagBits, queue_info> _queues;
        std::vector<uint32_t> _unique_families;
        std::map<vk::QueueFlagBits, vk::CommandPool> _command_pools;
        jpu::ref_ptr<memory_pool> _memory_pool;
        vk::PhysicalDevice _gpu;
        vk::SurfaceKHR _surface;
    };
}
