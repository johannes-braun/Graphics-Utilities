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

    struct queue_filter
    {
        using filter_props = std::function<bool(const vk::QueueFamilyProperties&)>;
        using filter_family = std::function<bool(int32_t)>;
        using filter_both = std::function<bool(int32_t, const vk::QueueFamilyProperties&)>;

        queue_filter(float priority, vk::QueueFlags flags);
        queue_filter(float priority, int32_t family);
        queue_filter(float priority, filter_props filter);
        queue_filter(float priority, filter_family filter);
        queue_filter(float priority, filter_both filter);

        float priority = 1.f;
        filter_both filter;
    };

    class queue_creator
    {
    public:
        queue_creator(vk::PhysicalDevice physical_device, vk::ArrayProxy<const queue_filter> filters);

        const std::vector<uint32_t>& families() const;
        const std::vector<uint32_t>& unique_families() const;
        const std::vector<vk::DeviceQueueCreateInfo>& create_infos() const;

    private:
        std::map<uint32_t, std::tuple<uint32_t, std::vector<float>>> m_family_filter;
        std::vector<uint32_t> m_families;
        std::vector<uint32_t> m_unique_families;
        std::vector<vk::DeviceQueueCreateInfo> m_queue_infos;
    };
}
