#include "context_vulkan.hpp"
#include "device_buffer_vulkan.hpp"
#include "host_buffer_vulkan.hpp"
#include "init_struct.hpp"
#include <unordered_set>

namespace gfx {
inline namespace v1 {
namespace vulkan {
void device_buffer_implementation::allocate(size_type size)
{
    init<VkBufferCreateInfo> create_info{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};

    auto& ctx     = context::current();
    auto  impl    = static_cast<context_implementation*>(std::any_cast<detail::context_implementation*>(ctx->implementation()));
    _command_pool = impl->command_pools()[fam::transfer];
    if (!_device) {
        _device = impl->device();
        init<VkCommandBufferAllocateInfo> cmd_alloc{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
        cmd_alloc.commandBufferCount = 2;
        cmd_alloc.commandPool        = impl->command_pools()[fam::transfer];
        cmd_alloc.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        _transfer_commands.resize(cmd_alloc.commandBufferCount);
        vkAllocateCommandBuffers(_device, &cmd_alloc, _transfer_commands.data());

        _transfer_queue = impl->queues()[fam::transfer];
        init<VkFenceCreateInfo> fen_info{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
        fen_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        for (int i = 0; i < _transfer_commands.size(); ++i)
            vkCreateFence(impl->device(), &fen_info, nullptr, &_transfer_fences.emplace_back());
        _allocator = impl->allocator();
    }
    std::unordered_set<u32> family_ind{
        impl->queue_families()[fam::transfer],
        impl->queue_families()[fam::graphics],
        impl->queue_families()[fam::compute],
    };

    std::vector<uint32_t> family_indices(family_ind.begin(), family_ind.end());
    create_info.pQueueFamilyIndices   = std::data(family_indices);
    create_info.queueFamilyIndexCount = static_cast<uint32_t>(std::size(family_indices));
    create_info.sharingMode           = family_indices.size() == 1 ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
    create_info.size                  = size;
    create_info.usage                 = _usage_vk;

    if (_buffer) vmaDestroyBuffer(_allocator, _buffer, _allocation);

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage                   = VMA_MEMORY_USAGE_GPU_ONLY;
    vmaCreateBuffer(_allocator, &create_info, &allocInfo, &_buffer, &_allocation, nullptr);
}

void device_buffer_implementation::copy(const std::any& source, const std::any& target, difference_type src_offset,
                                        difference_type dst_offset, size_type size, fence* f)
{
    const auto get_buffer = [](const std::any& any, VkAccessFlags& base_access) -> VkBuffer {
        if (any.type() == typeid(detail::device_buffer_implementation*)) {
            base_access = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
            return static_cast<device_buffer_implementation*>(std::any_cast<detail::device_buffer_implementation*>(any))->_buffer;
        }
        else if (any.type() == typeid(detail::host_buffer_implementation*))
        {
            base_access = VK_ACCESS_HOST_READ_BIT | VK_ACCESS_HOST_WRITE_BIT;
            return std::any_cast<VkBuffer>(
                static_cast<host_buffer_implementation*>(std::any_cast<detail::host_buffer_implementation*>(any))->api_handle());
        }
        else
            throw std::invalid_argument("Invalid buffer implementation type.");
    };
    VkAccessFlags src_access;
    VkAccessFlags dst_access;
    VkBuffer      src_buffer = get_buffer(source, src_access);
    VkBuffer      dst_buffer = get_buffer(target, dst_access);

    std::vector<VkFence> fences{_transfer_fences[_current_cmd]};
    if (f) fences.push_back(handle_cast<VkFence>(*f));
    vkWaitForFences(_device, fences.size(), fences.data(), true, std::numeric_limits<uint64_t>::max());
    vkResetFences(_device, 1, &fences.back());
    init<VkCommandBufferBeginInfo> begin{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(_transfer_commands[_current_cmd], &begin);

    // vk::BufferMemoryBarrier membarr_dst;
    // membarr_dst.buffer                  = dst_buffer;
    // membarr_dst.srcAccessMask           = dst_access;
    // membarr_dst.dstAccessMask           = vk::AccessFlagBits::eTransferWrite;
    // membarr_dst.srcQueueFamilyIndex     = VK_QUEUE_FAMILY_IGNORED;
    // membarr_dst.dstQueueFamilyIndex     = VK_QUEUE_FAMILY_IGNORED;
    // membarr_dst.offset                  = dst_offset;
    // membarr_dst.size                    = size;
    // vk::BufferMemoryBarrier membarr_src = membarr_dst;
    // membarr_src.srcAccessMask           = src_access;
    // membarr_src.buffer                  = src_buffer;
    // membarr_src.offset                  = src_offset;
    // membarr_src.dstAccessMask           = vk::AccessFlagBits::eTransferRead;

    //_transfer_command->pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands,
    //                                   vk::PipelineStageFlagBits::eTransfer,
    //                                   vk::DependencyFlagBits::eByRegion,
    //                                   nullptr,
    //                                   {membarr_src, membarr_dst},
    //                                   nullptr);

    init<VkBufferCopy> cpy;
    cpy.srcOffset = src_offset;
    cpy.dstOffset = dst_offset;
    cpy.size      = size;
    vkCmdCopyBuffer(_transfer_commands[_current_cmd], src_buffer, dst_buffer, 1, &cpy);

    // membarr_src.srcAccessMask = vk::AccessFlagBits::eTransferRead;
    // membarr_src.dstAccessMask = src_access;

    // membarr_dst.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    // membarr_dst.dstAccessMask = dst_access;

    //_transfer_command->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
    //                                   vk::PipelineStageFlagBits::eAllCommands,
    //                                   vk::DependencyFlagBits::eByRegion,
    //                                   nullptr,
    //                                   {membarr_src, membarr_dst},
    //                                   nullptr);

    vkEndCommandBuffer(_transfer_commands[_current_cmd]);

    init<VkSubmitInfo> submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit.commandBufferCount = 1;
    submit.pCommandBuffers    = &_transfer_commands[_current_cmd];
    vkQueueSubmit(_transfer_queue, 1, &submit, fences.back());
    _current_cmd = (_current_cmd + 1) % _transfer_commands.size();

    vkWaitForFences(_device, 1, &fences.back(), true, std::numeric_limits<uint64_t>::max());
    // vkResetFences(_device, 1, &fences.back());
}

void device_buffer_implementation::update(difference_type offset, size_type size, const std::byte* data)
{
    vkWaitForFences(_device, 1, &_transfer_fences[_current_cmd], true, std::numeric_limits<uint64_t>::max());
    vkResetFences(_device, 1, &_transfer_fences[_current_cmd]);
    init<VkCommandBufferBeginInfo> begin{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(_transfer_commands[_current_cmd], &begin);

    /* vk::BufferMemoryBarrier membarr_dst;
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
    nullptr);*/

    vkCmdUpdateBuffer(_transfer_commands[_current_cmd], _buffer, offset, size, data);

    // membarr_dst.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    // membarr_dst.dstAccessMask = vk::AccessFlagBits::eShaderRead |
    // vk::AccessFlagBits::eShaderWrite;
    //_transfer_command->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
    //                                   vk::PipelineStageFlagBits::eAllCommands,
    //                                   vk::DependencyFlagBits::eByRegion,
    //                                   nullptr,
    //                                   membarr_dst,
    //                                   nullptr);

    vkEndCommandBuffer(_transfer_commands[_current_cmd]);

    init<VkSubmitInfo> submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit.commandBufferCount = 1;
    submit.pCommandBuffers    = &_transfer_commands[_current_cmd];
    vkQueueSubmit(_transfer_queue, 1, &submit, _transfer_fences[_current_cmd]);
    _current_cmd = (_current_cmd + 1) % _transfer_commands.size();
}

std::any device_buffer_implementation::api_handle()
{
    return VkBuffer(_buffer);
}

void device_buffer_implementation::update_flags(buffer_usage_flags usage)
{
    _usage_vk |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    if (usage & buffer_usage::index) _usage_vk |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    if (usage & buffer_usage::indirect) _usage_vk |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    if (usage & buffer_usage::storage) _usage_vk |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    if (usage & buffer_usage::uniform) _usage_vk |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    if (usage & buffer_usage::vertex) _usage_vk |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
}

device_buffer_implementation::~device_buffer_implementation()
{
    assert((_buffer && _allocation) || (!_buffer && !_allocation));
    if (_buffer && _allocation) vmaDestroyBuffer(_allocator, _buffer, _allocation);

    if (_device && !_transfer_commands.empty())
        vkFreeCommandBuffers(_device, _command_pool, _transfer_commands.size(), _transfer_commands.data());
    for (auto& f : _transfer_fences) vkDestroyFence(_device, f, nullptr);
}
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx