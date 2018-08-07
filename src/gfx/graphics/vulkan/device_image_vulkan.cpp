#include "context_vulkan.hpp"
#include "device_image_vulkan.hpp"
#include "init_struct.hpp"
#include <unordered_set>

namespace gfx {
inline namespace v1 {
namespace vulkan {

device_image_implementation::~device_image_implementation()
{
    assert((_image && _allocation) || (!_image && !_allocation));
    if (_image && _allocation) vmaDestroyImage(_allocator, _image, _allocation);
    if (_transfer_fence) vkDestroyFence(_device, _transfer_fence, nullptr);
}

VkFormat get_format(format f)
{
#define fmts(bits, type, vt)                                                 \
    case r##bits##type: return VK_FORMAT_R##bits##_##vt;                     \
    case rg##bits##type: return VK_FORMAT_R##bits##G##bits##_##vt;           \
    case rgb##bits##type: return VK_FORMAT_R##bits##G##bits##B##bits##_##vt; \
    case rgba##bits##type: return VK_FORMAT_R##bits##G##bits##B##bits##A##bits##_##vt;

    switch (f)
    {
        fmts(8, unorm, UNORM);
        fmts(16, unorm, UNORM);
        fmts(8, snorm, UNORM);
        fmts(16, snorm, UNORM);
        fmts(8, u, UINT);
        fmts(16, u, UINT);
        fmts(32, u, UINT);
        fmts(8, i, SINT);
        fmts(16, i, SINT);
        fmts(32, i, SINT);
        fmts(16, f, SFLOAT);
        fmts(32, f, SFLOAT);

    case rgb5a1unorm: return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
    case rgb10a2unorm: return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
    case rgb10a2snorm: return VK_FORMAT_A2R10G10B10_SNORM_PACK32;
    case r11g11b10f: return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
    case rgb9e5: return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
    case r5g6b5unorm: return VK_FORMAT_R5G6B5_UNORM_PACK16;
    case d16unorm: return VK_FORMAT_D16_UNORM;
    case d24unorm: return VK_FORMAT_X8_D24_UNORM_PACK32;
    case s8ui: return VK_FORMAT_S8_UINT;
    case d32f: return VK_FORMAT_D32_SFLOAT;
    case d24unorms8ui: return VK_FORMAT_D16_UNORM_S8_UINT;
    case d32fs8ui: return VK_FORMAT_D32_SFLOAT_S8_UINT;
    default: return VkFormat{};
    }
#undef fmts
}

void device_image_implementation::initialize(uint32_t layer_dimensions, format format, const extent& size, uint32_t levels,
                                             sample_count samples)
{
    auto& ctx  = context::current();
    auto  impl = static_cast<context_implementation*>(std::any_cast<detail::context_implementation*>(ctx->implementation()));

    this->~device_image_implementation();

    _device         = impl->device();
    _allocator      = impl->allocator();
    _transfer_pool  = impl->command_pools()[fam::transfer];
    _transfer_queue = impl->queues()[fam::transfer];
    _graphics_pool  = impl->command_pools()[fam::graphics];
    _graphics_queue = impl->queues()[fam::graphics];
    _levels         = levels;

    init<VkImageCreateInfo> img_create{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};

    const bool as_attachment = layer_dimensions == 4;
    layer_dimensions         = as_attachment ? 2 : layer_dimensions;

    if (layer_dimensions == 2) {
        img_create.flags = VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT;
        if (size.depth >= 6 && size.depth % 6 == 0) img_create.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    }

    img_create.format = get_format(format);
    switch (layer_dimensions)
    {
    case 1: img_create.arrayLayers = size.height; break;
    case 2: img_create.arrayLayers = size.depth; break;
    default: img_create.arrayLayers = 1; break;
    }
    _layers                  = img_create.arrayLayers;
    img_create.extent.width  = size.width;
    img_create.extent.height = layer_dimensions > 1 ? size.height : 1;
    img_create.extent.depth  = layer_dimensions > 2 ? size.depth : 1;
    _extent                  = img_create.extent;
    switch (layer_dimensions)
    {
    case 1: img_create.imageType = VK_IMAGE_TYPE_1D; break;
    case 2: img_create.imageType = VK_IMAGE_TYPE_2D; break;
    case 3: img_create.imageType = VK_IMAGE_TYPE_3D; break;
    default: img_create.imageType = VK_IMAGE_TYPE_3D; break;
    }
    img_create.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    img_create.mipLevels     = levels;

    const auto families = [&]() -> std::vector<u32> {
        std::unordered_set<u32> f{impl->queue_families()[fam::graphics], impl->queue_families()[fam::transfer],
                                  impl->queue_families()[fam::compute]};
        return {f.begin(), f.end()};
    }();
    img_create.queueFamilyIndexCount = static_cast<u32>(families.size());
    img_create.pQueueFamilyIndices   = families.data();
    img_create.samples               = VkSampleCountFlagBits(samples);
    img_create.sharingMode           = VK_SHARING_MODE_CONCURRENT;
    img_create.tiling                = VK_IMAGE_TILING_OPTIMAL;

    // TODO
    switch (format)
    {
    case d16unorm:
    case d24unorm:
    case s8ui:
    case d32f:
    case d24unorms8ui:
    case d32fs8ui:
		assert(as_attachment);
        img_create.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        break;
    default:
        img_create.usage = (as_attachment ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : 0) |
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
        break;
    }

    init<VmaAllocationCreateInfo> allocInfo = {};
    allocInfo.usage                         = VMA_MEMORY_USAGE_GPU_ONLY;
    vmaCreateImage(_allocator, &img_create, &allocInfo, &_image, &_allocation, nullptr);

    init<VkFenceCreateInfo> finf{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    vkCreateFence(_device, &finf, nullptr, &_transfer_fence);
}

void device_image_implementation::fill_from(const host_image& image, uint32_t level, uint32_t layer)
{
    movable_handle<VkCommandBuffer>   cmd;
    init<VkCommandBufferAllocateInfo> cmdalloc{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    cmdalloc.commandBufferCount = 1;
    cmdalloc.commandPool        = _transfer_pool;
    cmdalloc.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    vkAllocateCommandBuffers(_device, &cmdalloc, &cmd);

    init<VkCommandBufferBeginInfo> beg{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    beg.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmd, &beg);

    init<VkImageMemoryBarrier> imembarr{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    imembarr.srcAccessMask                   = 0;
    imembarr.dstAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
    imembarr.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    imembarr.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    imembarr.image                           = _image;
    imembarr.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
    imembarr.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imembarr.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    imembarr.subresourceRange.baseArrayLayer = layer;
    imembarr.subresourceRange.layerCount     = 1;
    imembarr.subresourceRange.baseMipLevel   = level;
    imembarr.subresourceRange.levelCount     = 1;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, nullptr,
                         0, nullptr, 1, &imembarr);

    init<VkBufferImageCopy> copy;
    copy.imageExtent                     = _extent;
    copy.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    copy.imageSubresource.baseArrayLayer = layer;
    copy.imageSubresource.layerCount     = 1;
    copy.imageSubresource.mipLevel       = level;
    const VkBuffer storage               = handle_cast<VkBuffer>(image.storage());
    vkCmdCopyBufferToImage(cmd, storage, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);

    imembarr.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imembarr.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    imembarr.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imembarr.newLayout     = VK_IMAGE_LAYOUT_GENERAL;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, nullptr,
                         0, nullptr, 1, &imembarr);

    vkEndCommandBuffer(cmd);
    init<VkSubmitInfo> submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit.commandBufferCount = 1;
    submit.pCommandBuffers    = &cmd;

    vkQueueSubmit(_transfer_queue, 1, &submit, _transfer_fence);
    vkWaitForFences(_device, 1, &_transfer_fence, true, std::numeric_limits<uint64_t>::max());
    vkResetFences(_device, 1, &_transfer_fence);
    vkFreeCommandBuffers(_device, _transfer_pool, 1, &cmd);
}

void device_image_implementation::fill_to(const host_image& image, uint32_t level, uint32_t layer)
{
    movable_handle<VkCommandBuffer>   cmd;
    init<VkCommandBufferAllocateInfo> cmdalloc{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    cmdalloc.commandBufferCount = 1;
    cmdalloc.commandPool        = _transfer_pool;
    cmdalloc.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    vkAllocateCommandBuffers(_device, &cmdalloc, &cmd);

    init<VkCommandBufferBeginInfo> beg{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    beg.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmd, &beg);

    init<VkImageMemoryBarrier> imembarr{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    imembarr.srcAccessMask                   = 0;
    imembarr.dstAccessMask                   = VK_ACCESS_TRANSFER_READ_BIT;
    imembarr.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    imembarr.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    imembarr.image                           = _image;
    imembarr.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
    imembarr.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    imembarr.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    imembarr.subresourceRange.baseArrayLayer = layer;
    imembarr.subresourceRange.layerCount     = 1;
    imembarr.subresourceRange.baseMipLevel   = level;
    imembarr.subresourceRange.levelCount     = 1;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, nullptr,
                         0, nullptr, 1, &imembarr);

    const auto              clm = [](uint32_t i) -> u32 { return static_cast<i32>(std::max(i, 1u)); };
    init<VkBufferImageCopy> copy;
    copy.imageExtent = VkExtent3D{clm(_extent.width >> (level)), clm(_extent.height >> (level)), clm(_extent.depth >> (level))};
    copy.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    copy.imageSubresource.baseArrayLayer = layer;
    copy.imageSubresource.layerCount     = 1;
    copy.imageSubresource.mipLevel       = level;
    const VkBuffer storage               = handle_cast<VkBuffer>(image.storage());
    vkCmdCopyImageToBuffer(cmd, _image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, storage, 1, &copy);

    imembarr.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    imembarr.dstAccessMask = 0;
    imembarr.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    imembarr.newLayout     = VK_IMAGE_LAYOUT_GENERAL;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, nullptr,
                         0, nullptr, 1, &imembarr);

    vkEndCommandBuffer(cmd);
    init<VkSubmitInfo> submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit.commandBufferCount = 1;
    submit.pCommandBuffers    = &cmd;

    vkQueueSubmit(_transfer_queue, 1, &submit, _transfer_fence);
    vkWaitForFences(_device, 1, &_transfer_fence, true, std::numeric_limits<uint64_t>::max());
    vkResetFences(_device, 1, &_transfer_fence);
    vkFreeCommandBuffers(_device, _transfer_pool, 1, &cmd);
}

std::any device_image_implementation::api_handle()
{
    return VkImage(_image);
}

void device_image_implementation::generate_mipmaps()
{
    movable_handle<VkCommandBuffer>   cmd;
    init<VkCommandBufferAllocateInfo> cmdalloc{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    cmdalloc.commandBufferCount = 1;
    cmdalloc.commandPool        = _graphics_pool;
    cmdalloc.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    vkAllocateCommandBuffers(_device, &cmdalloc, &cmd);

    init<VkCommandBufferBeginInfo> beg{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    beg.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmd, &beg);

    init<VkImageMemoryBarrier> initial{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    initial.srcAccessMask                   = 0;
    initial.dstAccessMask                   = VK_ACCESS_TRANSFER_READ_BIT;
    initial.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    initial.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    initial.image                           = _image;
    initial.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
    initial.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    initial.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    initial.subresourceRange.baseArrayLayer = 0;
    initial.subresourceRange.layerCount     = _layers;
    initial.subresourceRange.baseMipLevel   = 0;
    initial.subresourceRange.levelCount     = 1;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, nullptr,
                         0, nullptr, 1, &initial);

    init<VkImageMemoryBarrier> ending  = initial;
    ending.subresourceRange.layerCount = 1;
    ending.srcAccessMask               = VK_ACCESS_TRANSFER_WRITE_BIT;
    ending.oldLayout                   = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

    initial.subresourceRange.layerCount = 1;
    initial.srcAccessMask               = 0;
    initial.oldLayout                   = VK_IMAGE_LAYOUT_UNDEFINED;
    initial.dstAccessMask               = VK_ACCESS_TRANSFER_WRITE_BIT;
    initial.newLayout                   = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    for (u32 layer = 0; layer < _layers; ++layer) {
        for (u32 i = 1; i < _levels; ++i) {
            initial.subresourceRange.baseArrayLayer = layer;
            initial.subresourceRange.baseMipLevel   = i;
            vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0,
                                 nullptr, 0, nullptr, 1, &initial);

            const auto        clm = [](uint32_t i) -> i32 { return static_cast<i32>(std::max(i, 1u)); };
            init<VkImageBlit> blit;
            blit.srcOffsets[0] = VkOffset3D{0, 0, 0};
            blit.srcOffsets[1] = VkOffset3D{clm(_extent.width >> (i - 1)), clm(_extent.height >> (i - 1)), clm(_extent.depth >> (i - 1))};
            blit.dstOffsets[0] = VkOffset3D{0, 0, 0};
            blit.dstOffsets[1] = VkOffset3D{clm(_extent.width >> (i)), clm(_extent.height >> (i)), clm(_extent.depth >> (i))};
            blit.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.baseArrayLayer = layer;
            blit.srcSubresource.layerCount     = 1;
            blit.srcSubresource.mipLevel       = i - 1;
            blit.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.baseArrayLayer = layer;
            blit.dstSubresource.layerCount     = 1;
            blit.dstSubresource.mipLevel       = i;

            vkCmdBlitImage(cmd, _image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit,
                           VK_FILTER_LINEAR);

            ending.subresourceRange.baseArrayLayer = layer;
            ending.subresourceRange.baseMipLevel   = i;
            vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0,
                                 nullptr, 0, nullptr, 1, &ending);
        }
    }

    initial.srcAccessMask                   = VK_ACCESS_TRANSFER_READ_BIT;
    initial.dstAccessMask                   = 0;
    initial.oldLayout                       = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    initial.newLayout                       = VK_IMAGE_LAYOUT_GENERAL;
    initial.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    initial.subresourceRange.baseArrayLayer = 0;
    initial.subresourceRange.layerCount     = _layers;
    initial.subresourceRange.baseMipLevel   = 0;
    initial.subresourceRange.levelCount     = _levels;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, nullptr,
                         0, nullptr, 1, &initial);

    vkEndCommandBuffer(cmd);

    init<VkSubmitInfo> submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit.commandBufferCount = 1;
    submit.pCommandBuffers    = &cmd;
    vkQueueSubmit(_graphics_queue, 1, &submit, _transfer_fence);
    vkWaitForFences(_device, 1, &_transfer_fence, true, std::numeric_limits<uint64_t>::max());
    vkResetFences(_device, 1, &_transfer_fence);
    vkFreeCommandBuffers(_device, _graphics_pool, 1, &cmd);
}
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx