#include "command_list_vulkan.hpp"
#include "framebuffer_vulkan.hpp"
#include "graphics_pipeline_vulkan.hpp"
#include "image_view_vulkan.hpp"
#include "init_struct.hpp"
#include "result.hpp"

namespace gfx {
inline namespace v1 {
namespace vulkan {
commands_implementation::~commands_implementation()
{
    if (_signal) vkDestroySemaphore(_device, _signal, nullptr);
    if (_cmd) vkFreeCommandBuffers(_device, _pool, 1, &_cmd);
}

void commands_implementation::initialize(commands_type type)
{
    _type   = type;
    _family = [type] {
        switch (type)
        {
        case commands_type::compute: return fam::compute;
        case commands_type::graphics: return fam::graphics;
        case commands_type::transfer: return fam::transfer;
        }
        return fam::graphics;
    }();

    auto& ctx = context::current();
    _ctx_impl = static_cast<context_implementation*>(std::any_cast<detail::context_implementation*>(ctx->implementation()));
    _device   = _ctx_impl->device();
    _pool     = _ctx_impl->command_pools()[_family];
    _queue    = _ctx_impl->queues()[_family];

    init<VkCommandBufferAllocateInfo> alloc {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    alloc.commandPool        = _pool;
    alloc.commandBufferCount = 1;
    alloc.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    check_result(vkAllocateCommandBuffers(_device, &alloc, &_cmd));

    init<VkSemaphoreCreateInfo> sem {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    check_result(vkCreateSemaphore(_device, &sem, nullptr, &_signal));
}
void commands_implementation::reset()
{
    check_result(vkResetCommandBuffer(_cmd, 0));
}
void commands_implementation::begin()
{
    init<VkCommandBufferBeginInfo> beg {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    beg.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    _sets.clear();
    check_result(vkBeginCommandBuffer(_cmd, &beg));
}
void commands_implementation::end()
{
    check_result(vkEndCommandBuffer(_cmd));
}
void commands_implementation::execute_sync_after(const commands& cmd, fence* f)
{
    if (const auto it = std::find(_ctx_impl->final_wait_semaphores[_family].begin(), _ctx_impl->final_wait_semaphores[_family].end(),
                                  static_cast<commands_implementation*>(&*cmd.implementation())->_signal);
        it != _ctx_impl->final_wait_semaphores[_family].end())
    {
        *it = _signal;
        _ctx_impl->final_wait_stages[_family][std::distance(it, _ctx_impl->final_wait_semaphores[_family].begin())] =
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    }
    else
    {
        _ctx_impl->final_wait_semaphores[_family].push_back(_signal);
        _ctx_impl->final_wait_stages[_family].push_back(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
    }

    init<VkSubmitInfo> submit {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit.commandBufferCount       = 1;
    submit.pCommandBuffers          = &_cmd;
    submit.signalSemaphoreCount     = 1;
    submit.pSignalSemaphores        = &_signal;
    submit.waitSemaphoreCount       = 1;
    submit.pWaitSemaphores          = &static_cast<commands_implementation*>(&*cmd.implementation())->_signal;
    VkPipelineStageFlags stageFlags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    submit.pWaitDstStageMask        = &stageFlags;
    check_result(vkQueueSubmit(_queue, 1, &submit, f ? handle_cast<VkFence>(*f) : nullptr));
}
void commands_implementation::execute(fence* f)
{
    _ctx_impl->final_wait_semaphores[_family].push_back(_signal);
    _ctx_impl->final_wait_stages[_family].push_back(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

    init<VkSubmitInfo> submit {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit.commandBufferCount   = 1;
    submit.pCommandBuffers      = &_cmd;
    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores    = &_signal;
    check_result(vkQueueSubmit(_queue, 1, &submit, f ? handle_cast<VkFence>(*f) : nullptr));
}
void commands_implementation::bind_pipeline(const compute_pipeline& p, std::initializer_list<binding_set*> bindings)
{
    _last_gpipeline = nullptr;
    _last_cpipeline = &p;
    vkCmdBindPipeline(_cmd, VK_PIPELINE_BIND_POINT_COMPUTE, handle_cast<VkPipeline>(p));
    if (bindings.size() > 0)
    {
        const u32 offset = u32(_sets.size());
        for (auto& b : bindings) _sets.push_back(handle_cast<VkDescriptorSet>(*b));
        vkCmdBindDescriptorSets(_cmd, VK_PIPELINE_BIND_POINT_COMPUTE,
                                static_cast<compute_pipeline_implementation*>(&*p.implementation())->layout(), 0, std::size(bindings),
                                _sets.data() + offset, 0, nullptr);
    }
}
void commands_implementation::dispatch(u32 x, u32 y, u32 z)
{
    vkCmdDispatch(_cmd, x, y, z);
}
void commands_implementation::begin_pass(const framebuffer& fbo, std::optional<rect2f> render_area)
{
    VkRenderPassBeginInfo begin = static_cast<framebuffer_implementation*>(&*fbo.implementation())->begin_info();
    if (!render_area) render_area = rect2f(glm::vec2(0, 0), glm::vec2(fbo.width(), fbo.height()));
    const auto ras            = render_area->size();
    begin.renderArea.offset.x = render_area->min.x;
    begin.renderArea.offset.y = render_area->min.y;

    begin.renderArea.extent.width  = ras.x;
    begin.renderArea.extent.height = ras.y;
    _last_render_area              = begin.renderArea;
    vkCmdBeginRenderPass(_cmd, &begin, VK_SUBPASS_CONTENTS_INLINE);
}
void commands_implementation::end_pass()
{
    vkCmdEndRenderPass(_cmd);
}
void commands_implementation::bind_pipeline(const graphics_pipeline& p, std::initializer_list<binding_set*> bindings)
{
    _last_gpipeline = &p;
    _last_cpipeline = nullptr;
    vkCmdBindPipeline(_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, handle_cast<VkPipeline>(p));
    if (bindings.size() > 0)
    {
        const u32 offset = u32(_sets.size());
        for (auto& b : bindings) _sets.push_back(handle_cast<VkDescriptorSet>(*b));
        vkCmdBindDescriptorSets(_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                static_cast<graphics_pipeline_implementation*>(&*p.implementation())->layout(), 0, std::size(bindings),
                                _sets.data() + offset, 0, nullptr);
    }

    // bind default viewports and scissors
    VkViewport vps {0};
    vps.width    = _last_render_area.extent.width;
    vps.height   = _last_render_area.extent.height;
    vps.maxDepth = 1.f;
    vps.minDepth = 0.f;
    vkCmdSetViewport(_cmd, 0, 1, &vps);

    VkRect2D scs = _last_render_area;
    vkCmdSetScissor(_cmd, 0, 1, &scs);
}
void commands_implementation::draw(u32 vertex_count, u32 instance_count, u32 base_vertex, u32 base_instance)
{
    vkCmdDraw(_cmd, vertex_count, instance_count, base_vertex, base_instance);
}

void commands_implementation::bind_vertex_buffer(const handle& buffer, u32 binding, i64 offset)
{
    const VkBuffer     buf = std::any_cast<VkBuffer>(buffer);
    const VkDeviceSize off = offset;
    vkCmdBindVertexBuffers(_cmd, binding, 1, &buf, &off);
}

void commands_implementation::bind_index_buffer(const handle& buffer, index_type index, i64 offset)
{
    const VkBuffer buf = std::any_cast<VkBuffer>(buffer);
    vkCmdBindIndexBuffer(_cmd, buf, offset, [=] {
        switch (index)
        {
        case index_type::uint16: return VK_INDEX_TYPE_UINT16;
        case index_type::uint32: return VK_INDEX_TYPE_UINT32;
        }
        return VK_INDEX_TYPE_UINT32;
    }());
}

void commands_implementation::draw_indexed(u32 index_count, u32 instance_count, u32 base_index, u32 base_vertex, u32 base_instance)
{
    vkCmdDrawIndexed(_cmd, index_count, instance_count, base_index, base_vertex, base_instance);
}

void commands_implementation::push_binding(u32 set, u32 binding, u32 arr_element, binding_type type, std::any obj)
{
    assert(_last_gpipeline || _last_cpipeline);
    const auto bind_point = _last_gpipeline ? VK_PIPELINE_BIND_POINT_GRAPHICS : VK_PIPELINE_BIND_POINT_COMPUTE;

    VkPipelineLayout layout = _last_cpipeline
                                  ? static_cast<compute_pipeline_implementation*>(&*_last_cpipeline->implementation())->layout()
                                  : static_cast<graphics_pipeline_implementation*>(&*_last_gpipeline->implementation())->layout();

    if (obj.type()
        == typeid(std::pair<const std::unique_ptr<detail::image_view_implementation>*,
                            const std::unique_ptr<detail::sampler_implementation>*>))
    {
        assert(type == binding_type::sampled_image);

        auto [ivp, sp] = std::any_cast<
            std::pair<const std::unique_ptr<detail::image_view_implementation>*, const std::unique_ptr<detail::sampler_implementation>*>>(
            obj);

        auto& iv = *ivp;
        auto& s  = *sp;

        auto img_view = std::any_cast<VkImageView>(iv->api_handle());
        auto smp      = std::any_cast<VkSampler>(s->api_handle());

        init<VkWriteDescriptorSet> write {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        write.descriptorCount = 1;
        write.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.dstArrayElement = arr_element;
        write.dstBinding      = binding;
        write.dstSet          = nullptr;

        init<VkDescriptorImageInfo> img_info;
        img_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        img_info.imageView   = img_view;
        img_info.sampler     = smp;
        write.pImageInfo     = &img_info;
        vkCmdPushDescriptorSetKHR(_cmd, bind_point, layout, set, 1, &write);
    }
    else if (obj.type() == typeid(const std::unique_ptr<detail::image_view_implementation>*))
    {
        assert(type == binding_type::storage_image);
        auto& iv = *std::any_cast<const std::unique_ptr<detail::image_view_implementation>*>(obj);

        auto img_view = std::any_cast<VkImageView>(iv->api_handle());

        init<VkWriteDescriptorSet> write {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        write.descriptorCount = 1;
        write.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        write.dstArrayElement = arr_element;
        write.dstBinding      = binding;
        write.dstSet          = nullptr;

        init<VkDescriptorImageInfo> img_info;
        img_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        img_info.imageView   = img_view;
        img_info.sampler     = nullptr;
        write.pImageInfo     = &img_info;
        vkCmdPushDescriptorSetKHR(_cmd, bind_point, layout, set, 1, &write);
    }
    else if (obj.type() == typeid(const std::unique_ptr<detail::host_buffer_implementation>*))
    {
        auto& buf    = *std::any_cast<const std::unique_ptr<detail::host_buffer_implementation>*>(obj);
        auto  buffer = std::any_cast<VkBuffer>(buf->api_handle());

        init<VkWriteDescriptorSet> write {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        write.descriptorCount = 1;
        write.descriptorType  = [=] {
            switch (type)
            {
            case binding_type::storage_buffer: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            case binding_type::uniform_buffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            default: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            }
        }();
        write.dstArrayElement = arr_element;
        write.dstBinding      = binding;
        write.dstSet          = nullptr;

        init<VkDescriptorBufferInfo> buf_info;
        buf_info.buffer   = buffer;
        buf_info.range    = VK_WHOLE_SIZE;
        write.pBufferInfo = &buf_info;

        vkCmdPushDescriptorSetKHR(_cmd, bind_point, layout, set, 1, &write);
    }
    else if (obj.type() == typeid(const std::unique_ptr<detail::device_buffer_implementation>*))
    {
        auto& buf    = *std::any_cast<const std::unique_ptr<detail::device_buffer_implementation>*>(obj);
        auto  buffer = std::any_cast<VkBuffer>(buf->api_handle());

        init<VkWriteDescriptorSet> write {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        write.descriptorCount = 1;
        write.descriptorType  = [=] {
            switch (type)
            {
            case binding_type::storage_buffer: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            case binding_type::uniform_buffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            default: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            }
        }();
        write.dstArrayElement = arr_element;
        write.dstBinding      = binding;
        write.dstSet          = nullptr;

        init<VkDescriptorBufferInfo> buf_info;
        buf_info.buffer   = buffer;
        buf_info.range    = VK_WHOLE_SIZE;
        write.pBufferInfo = &buf_info;

        vkCmdPushDescriptorSetKHR(_cmd, bind_point, layout, set, 1, &write);
    }
}

void commands_implementation::set_viewports(u32 first, span<viewport> vp, span<rect2f> scissors)
{
    std::vector<VkViewport> vps(vp.size());
    std::vector<VkRect2D>   sci(scissors.size());
    for (int i = 0; i < vp.size(); ++i)
    {
        if (!sci.empty())
        {
            auto size            = scissors[i].size();
            sci[i].extent.width  = size.x;
            sci[i].extent.height = size.y;
            sci[i].offset.x      = scissors[i].min.x;
            sci[i].offset.y      = scissors[i].min.y;
        }

        vps[i].width    = vp[i].width;
        vps[i].height   = vp[i].height;
        vps[i].x        = vp[i].x;
        vps[i].y        = vp[i].y;
        vps[i].minDepth = vp[i].min_depth;
        vps[i].maxDepth = vp[i].max_depth;
    }

    vkCmdSetViewport(_cmd, first, vps.size(), vps.data());
    if (!sci.empty()) { vkCmdSetScissor(_cmd, first, sci.size(), sci.data()); }
}

}    // namespace vulkan
}    // namespace v1
}    // namespace gfx
