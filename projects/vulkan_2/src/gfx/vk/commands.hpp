#pragma once

#include "array_view.hpp"

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <variant>
#include <glm/glm.hpp>

namespace gfx::vk
{
    class device;
    class command_buffer;
    class renderpass;
    class framebuffer;
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

    union clear_color {
        clear_color(const glm::vec4& f)  : f32(f) {}
        clear_color(const glm::ivec4& i) : i32(i) {}
        clear_color(const glm::uvec4& u) : u32(u) {}

        glm::vec4 f32;
        glm::ivec4 i32;
        glm::uvec4 u32;
    };
    struct clear_depth_stencil
    {
        constexpr clear_depth_stencil(float depth = 0, uint32_t stencil = 0)
            : depth(depth), stencil(stencil) {}
        float       depth;
        uint32_t    stencil;
    };

    union clear_value
    {
        clear_value(const clear_color& color) : color(color) {}
        clear_value(const clear_depth_stencil& depth_stencil) : depth_stencil(depth_stencil) {}

        clear_color color;
        clear_depth_stencil depth_stencil;
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

        void begin_renderpass(const renderpass& pass, const framebuffer& fb, VkRect2D area, VkSubpassContents contents, array_view<clear_value> clear_values) const noexcept;
        void end_renderpass() const noexcept;
    private:
        command_buffer() = default;

        std::shared_ptr<command_pool> _pool;
        VkCommandBuffer _command_buffer;
    };
}