#include "buffer.hpp"

namespace vkn
{
    buffer::buffer(device* device, size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memory_flags)
        : _device(device), _usage(usage), _buffer(_device->createBuffer(vk::BufferCreateInfo({}, size, _usage))),
        _memory_flags(memory_flags), _requirements(_device->getBufferMemoryRequirements(_buffer)),
        _memory_block(_device->memory()->allocate(_requirements.size, _requirements.memoryTypeBits, _memory_flags))
    {
        _device->inc_ref();
        _device->bindBufferMemory(_buffer, _memory_block->memory, _memory_block->offset);
    }

    buffer::~buffer()
    {
        _device->memory()->free(_memory_block);
        _device->destroyBuffer(_buffer);
        _device->dec_ref();
    }

    // other, size, src_offset, dst_offset
    void buffer::copy_to(buffer* other, size_t size, size_t src_offset, size_t dst_offset, vk::CommandBuffer cbuf)
    {
        auto&& queue = _device->queue(vk::QueueFlagBits::eTransfer).queue;
        auto&& command_pool = _device->command_pool(vk::QueueFlagBits::eTransfer);

        if (size == ~0)
            size = other->size();

        bool passed_cbuf = cbuf;
        if (!passed_cbuf)
        {
            cbuf = _device->allocateCommandBuffers(vk::CommandBufferAllocateInfo(command_pool, vk::CommandBufferLevel::ePrimary, 1))[0];
            cbuf.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        }

        std::array<vk::BufferMemoryBarrier, 2> pre_barriers{
            vk::BufferMemoryBarrier({}, vk::AccessFlagBits::eTransferRead, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, _buffer, src_offset, size),
            vk::BufferMemoryBarrier({}, vk::AccessFlagBits::eTransferWrite, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, other->_buffer, src_offset, size)
        };

        std::array<vk::BufferMemoryBarrier, 2> post_barriers{
            vk::BufferMemoryBarrier(vk::AccessFlagBits::eTransferRead,{}, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, _buffer, src_offset, size),
            vk::BufferMemoryBarrier(vk::AccessFlagBits::eTransferWrite,{}, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, other->_buffer, src_offset, size)
        };

        cbuf.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eTransfer, {}, nullptr, pre_barriers, nullptr);
        cbuf.copyBuffer(_buffer, other->_buffer, vk::BufferCopy(src_offset, dst_offset, size));
        cbuf.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eAllCommands, {}, nullptr, post_barriers, nullptr);

        if (!passed_cbuf)
        {
            cbuf.end();
            vk::SubmitInfo submit_info;
            submit_info.setCommandBufferCount(1).setPCommandBuffers(&cbuf);
            queue.submit(submit_info, nullptr);
            queue.waitIdle();
            _device->freeCommandBuffers(command_pool, cbuf);
        }
    }

    void* buffer::data() { return _memory_block->data; }

    const void* buffer::data() const { return _memory_block->data; }

    buffer::operator vk::Buffer() const { return _buffer; }

    size_t buffer::size() const { return _requirements.size; }
}