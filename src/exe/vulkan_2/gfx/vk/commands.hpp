#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace gfx::vk
{
    class device;
    class command_buffer;
    class command_pool : public std::enable_shared_from_this<command_pool>
    {
    public:
        friend command_buffer;
        command_pool(const std::shared_ptr<device>& device, uint32_t queue_family, VkCommandPoolCreateFlags flags);
        ~command_pool();
        command_pool(const command_pool&) = delete;
        command_pool& operator=(const command_pool&) = delete;
        command_pool(command_pool&&) = default;
        command_pool& operator=(command_pool&&) = default;

        operator VkCommandPool() const noexcept;

        std::vector<std::shared_ptr<command_buffer>> allocate_buffers(uint32_t count, VkCommandBufferLevel level) noexcept;
        void reset(VkCommandBufferResetFlags flags) const noexcept;

    private:
        std::shared_ptr<device> _device;
        VkCommandPool _pool;
    };

    class command_buffer
    {
    public:
        friend command_pool;
        command_buffer(const std::shared_ptr<command_pool>& pool, VkCommandBufferLevel level);
        ~command_buffer();

        operator VkCommandBuffer() const noexcept;
        void reset(VkCommandBufferResetFlags flags) const noexcept;

        void begin(VkCommandBufferUsageFlags flags, const VkCommandBufferInheritanceInfo* inheritance = nullptr) const noexcept;
        void end() const noexcept;
    private:
        command_buffer() = default;

        std::shared_ptr<command_pool> _pool;
        VkCommandBuffer _command_buffer;
    };
}