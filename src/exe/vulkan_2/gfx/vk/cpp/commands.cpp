#include "../commands.hpp"
#include "../device.hpp"
#include "../framebuffer.hpp"
#include "../renderpass.hpp"

namespace gfx::vk
{
    command_pool::command_pool(const std::shared_ptr<device>& device, uint32_t queue_family, VkCommandPoolCreateFlags flags)
        : _device(device)
    {
        VkCommandPoolCreateInfo cpi;
        cpi.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cpi.pNext = nullptr;
        cpi.flags = flags;
        cpi.queueFamilyIndex = queue_family;

        vkCreateCommandPool(*_device, &cpi, nullptr, &_pool);
    }

    command_pool::~command_pool()
    {
        vkDestroyCommandPool(*_device, _pool, nullptr);
    }

    command_pool::operator VkCommandPool() const noexcept
    {
        return _pool;
    }

    void command_pool::reset(VkCommandBufferResetFlags flags) const noexcept
    {
        vkResetCommandPool(*_device, _pool, flags);
    }

    std::vector<std::shared_ptr<command_buffer>> command_pool::allocate_buffers(uint32_t count, VkCommandBufferLevel level) noexcept
    {
        std::vector<std::shared_ptr<command_buffer>> buffers(count);
        std::vector<VkCommandBuffer> raw_buffers(count);

        VkCommandBufferAllocateInfo info;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.level = level;
        info.pNext = nullptr;
        info.commandBufferCount = count;
        info.commandPool = *this;

        vkAllocateCommandBuffers(*_device, &info, raw_buffers.data());

        int i = 0;
        for (auto&& buffer : buffers)
        {
            buffer = std::shared_ptr<command_buffer>(new command_buffer());
            buffer->_command_buffer = raw_buffers[i++];
            buffer->_pool = shared_from_this();
        }
        return buffers;
    }

    command_buffer::command_buffer(const std::shared_ptr<command_pool>& pool, VkCommandBufferLevel level)
        : _pool(pool)
    {
        VkCommandBufferAllocateInfo info;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.level = level;
        info.pNext = nullptr;
        info.commandBufferCount = 1;
        info.commandPool = *pool;

        vkAllocateCommandBuffers(*(_pool->_device), &info, &_command_buffer);
    }

    command_buffer::~command_buffer()
    {
        vkFreeCommandBuffers(*(_pool->_device), *_pool, 1, &_command_buffer);
    }

    command_buffer::operator VkCommandBuffer() const noexcept
    {
        return _command_buffer;
    }

    void command_buffer::reset(VkCommandBufferResetFlags flags) const noexcept
    {
        vkResetCommandBuffer(_command_buffer, flags);
    }

    void command_buffer::begin(VkCommandBufferUsageFlags flags, const VkCommandBufferInheritanceInfo* inheritance) const noexcept
    {
        VkCommandBufferBeginInfo info;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        info.pInheritanceInfo = inheritance;

        vkBeginCommandBuffer(_command_buffer, &info);
    }

    void command_buffer::end() const noexcept
    {
        vkEndCommandBuffer(_command_buffer);
    }

    void command_buffer::begin_renderpass(const renderpass& pass, const framebuffer& fb, VkRect2D area, VkSubpassContents contents, array_view<clear_value> clear_values) const noexcept
    {
        VkRenderPassBeginInfo info;
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.pNext = nullptr;
        info.clearValueCount = uint32_t(clear_values.size());
        info.pClearValues = reinterpret_cast<const VkClearValue*>(clear_values.data());
        info.framebuffer = fb;
        info.renderPass = pass;
        info.renderArea = area;
        vkCmdBeginRenderPass(_command_buffer, &info, contents);
    }

    void command_buffer::end_renderpass() const noexcept
    {
        vkCmdEndRenderPass(_command_buffer);
    }
}