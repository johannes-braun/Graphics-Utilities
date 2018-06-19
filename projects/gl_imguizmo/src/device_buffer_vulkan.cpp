#include "context_vulkan.hpp"
#include "device_buffer_vulkan.hpp"
#include "host_buffer_vulkan.hpp"

#include "vulkan_alloc.hpp"

namespace gfx::vulkan
{
void device_buffer_implementation::update_flags(const buffer_usage_flags usage)
{
    _usage_vk |= vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst;
    if(usage & buffer_usage::index)
        _usage_vk |= vk::BufferUsageFlagBits::eIndexBuffer;
    if(usage & buffer_usage::indirect)
        _usage_vk |= vk::BufferUsageFlagBits::eIndirectBuffer;
    if(usage & buffer_usage::storage)
        _usage_vk |= vk::BufferUsageFlagBits::eStorageBuffer;
    if(usage & buffer_usage::uniform)
        _usage_vk |= vk::BufferUsageFlagBits::eUniformBuffer;
    if(usage & buffer_usage::vertex)
        _usage_vk |= vk::BufferUsageFlagBits::eVertexBuffer;
}

void device_buffer_implementation::allocate(const size_type size)
{
    vk::BufferCreateInfo create_info;

    auto& ctx  = context::current();
    auto  impl = static_cast<context_implementation*>(
            std::any_cast<detail::context_implementation*>(ctx->implementation()));

    if(!_transfer_command)
    {
        vk::CommandBufferAllocateInfo cmd_alloc;
        cmd_alloc.commandBufferCount = 1;
        cmd_alloc.commandPool        = impl->command_pools()[fam::transfer].get();
        cmd_alloc.level              = vk::CommandBufferLevel::ePrimary;
        _transfer_command = std::move(impl->device().allocateCommandBuffersUnique(cmd_alloc)[0]);

        _transfer_queue = impl->queues()[fam::transfer];
        _transfer_fence = impl->device().createFence({vk::FenceCreateFlagBits::eSignaled});
        _device         = impl->device();
        _allocator      = impl->allocator();
    }

    std::vector<uint32_t> family_indices =
            impl->families({fam::transfer, fam::graphics, fam::compute});
    create_info.pQueueFamilyIndices   = std::data(family_indices);
    create_info.queueFamilyIndexCount = static_cast<uint32_t>(std::size(family_indices));
    create_info.sharingMode =
            family_indices.size() == 1 ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent;
    create_info.size  = size;
    create_info.usage = _usage_vk;

    if(_buffer)
        vmaDestroyBuffer(_allocator, _buffer, _allocation);

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage                   = VMA_MEMORY_USAGE_GPU_ONLY;
    vmaCreateBuffer(_allocator,
                    reinterpret_cast<VkBufferCreateInfo*>(&create_info),
                    &allocInfo,
                    &_buffer,
                    &_allocation,
                    nullptr);
}

void device_buffer_implementation::copy(const std::any& source, const std::any& target,
                                        const difference_type src_offset,
                                        const difference_type dst_offset, const size_type size)
{
    const auto get_buffer = [](const std::any& any, vk::AccessFlags& base_access) -> vk::Buffer {
        if(any.type() == typeid(detail::device_buffer_implementation*))
        {
            base_access = vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite;
            return static_cast<device_buffer_implementation*>(
                           std::any_cast<detail::device_buffer_implementation*>(any))
                    ->_buffer;
        }
        else if(any.type() == typeid(detail::host_buffer_implementation*))
        {
            base_access = vk::AccessFlagBits::eHostRead | vk::AccessFlagBits::eHostWrite;
            return static_cast<host_buffer_implementation*>(
                           std::any_cast<detail::host_buffer_implementation*>(any))
                    ->buffer();
        }
        else
            throw std::invalid_argument("Invalid buffer implementation type.");
    };
    vk::AccessFlags src_access;
    vk::AccessFlags dst_access;
    vk::Buffer      src_buffer = get_buffer(source, src_access);
    vk::Buffer      dst_buffer = get_buffer(target, dst_access);

    _device.waitForFences(_transfer_fence, true, std::numeric_limits<uint64_t>::max());
    _device.resetFences(_transfer_fence);
    vk::CommandBufferBeginInfo begin;
    begin.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    _transfer_command->begin(begin);

    vk::BufferMemoryBarrier membarr_dst;
    membarr_dst.buffer                  = dst_buffer;
    membarr_dst.srcAccessMask           = dst_access;
    membarr_dst.dstAccessMask           = vk::AccessFlagBits::eTransferWrite;
    membarr_dst.srcQueueFamilyIndex     = VK_QUEUE_FAMILY_IGNORED;
    membarr_dst.dstQueueFamilyIndex     = VK_QUEUE_FAMILY_IGNORED;
    membarr_dst.offset                  = dst_offset;
    membarr_dst.size                    = size;
    vk::BufferMemoryBarrier membarr_src = membarr_dst;
    membarr_src.srcAccessMask           = src_access;
    membarr_src.buffer                  = src_buffer;
    membarr_src.offset                  = src_offset;
    membarr_src.dstAccessMask           = vk::AccessFlagBits::eTransferRead;

    _transfer_command->pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands,
                                       vk::PipelineStageFlagBits::eTransfer,
                                       vk::DependencyFlagBits::eByRegion,
                                       nullptr,
                                       {membarr_src, membarr_dst},
                                       nullptr);

    vk::BufferCopy cpy;
    cpy.srcOffset = src_offset;
    cpy.dstOffset = dst_offset;
    cpy.size      = size;
    _transfer_command->copyBuffer(src_buffer, dst_buffer, cpy);

    membarr_src.srcAccessMask = vk::AccessFlagBits::eTransferRead;
    membarr_src.dstAccessMask = src_access;

    membarr_dst.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    membarr_dst.dstAccessMask = dst_access;

    _transfer_command->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                       vk::PipelineStageFlagBits::eAllCommands,
                                       vk::DependencyFlagBits::eByRegion,
                                       nullptr,
                                       {membarr_src, membarr_dst},
                                       nullptr);

    _transfer_command->end();

    vk::SubmitInfo submit;
    submit.commandBufferCount = 1;
    submit.pCommandBuffers    = &_transfer_command.get();
    _transfer_queue.submit(submit, _transfer_fence);
}

void device_buffer_implementation::update(difference_type offset, size_type size,
                                          const std::byte* data)
{
    _device.waitForFences(_transfer_fence, true, std::numeric_limits<uint64_t>::max());
    _device.resetFences(_transfer_fence);
    vk::CommandBufferBeginInfo begin;
    begin.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    _transfer_command->begin(begin);

    vk::BufferMemoryBarrier membarr_dst;
    membarr_dst.buffer        = _buffer;
    membarr_dst.srcAccessMask = vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite;
    membarr_dst.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
    membarr_dst.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    membarr_dst.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    membarr_dst.offset              = offset;
    membarr_dst.size                = size;

    _transfer_command->pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands,
                                       vk::PipelineStageFlagBits::eTransfer,
                                       vk::DependencyFlagBits::eByRegion,
                                       nullptr,
                                       membarr_dst,
                                       nullptr);

    _transfer_command->updateBuffer(_buffer, offset, size, data);

    membarr_dst.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    membarr_dst.dstAccessMask = vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite;
    _transfer_command->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                       vk::PipelineStageFlagBits::eAllCommands,
                                       vk::DependencyFlagBits::eByRegion,
                                       nullptr,
                                       membarr_dst,
                                       nullptr);

    _transfer_command->end();

    vk::SubmitInfo submit;
    submit.commandBufferCount = 1;
    submit.pCommandBuffers    = &_transfer_command.get();
    _transfer_queue.submit(submit, _transfer_fence);
}

std::any device_buffer_implementation::api_handle() { return vk::Buffer(_buffer); }

device_buffer_implementation::~device_buffer_implementation()
{
    if(_buffer)
        vmaDestroyBuffer(_allocator, _buffer, _allocation);
}
} // namespace gfx::opengl
