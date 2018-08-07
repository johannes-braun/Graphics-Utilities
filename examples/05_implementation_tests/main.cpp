#define GFX_EXPOSE_APIS
#include <gfx/gfx.hpp>
#include <iostream>

#include <vulkan/vulkan.hpp>

#include "gfx/graphics/opengl/formats.hpp"
#include "gfx/graphics/vulkan/context_vulkan.hpp"
#include "gfx/graphics/vulkan/graphics_pipeline_vulkan.hpp"
#include <gfx/graphics/graphics_pipeline.hpp>
#include <gfx/graphics/opengl/binding_set_opengl.hpp>
#include <gfx/graphics/vulkan/framebuffer_vulkan.hpp>

int main()
{
    const auto           then = std::chrono::steady_clock::now();
    gfx::context_options opt;
    opt.graphics_api = gfx::gapi::vulkan;
    opt.debug        = true;
    opt.use_window   = true;

    auto context = gfx::context::create(opt);
    context->make_current();

    //// Buffer
    gfx::hbuffer<float> x{1.f, 23.f, 29.f};
    gfx::hbuffer<float> src = std::move(x);
    gfx::buffer<float>  dst(gfx::buffer_usage::uniform, {2.f, 1.f, 0.5f, 10.f, 1.f, 9.f});
    dst << src;

    gfx::buf_copy(src, dst, dst.capacity());

    for (auto f : src) gfx::ilog << f;

    // Images
    const auto cubemap_format = gfx::rgba8unorm;
    const auto info           = gfx::image_file::info("hdri/hdr/posx.hdr");
    gfx::image file_texture(gfx::img_type::image2d, cubemap_format, gfx::extent(info.width, info.height, 6), 11);
    file_texture.layer(0) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/posx.hdr", gfx::bits::b32, 3));
    file_texture.layer(1) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/negx.hdr", gfx::bits::b32, 3));
    file_texture.layer(2) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/posy.hdr", gfx::bits::b32, 3));
    file_texture.layer(3) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/negy.hdr", gfx::bits::b32, 3));
    file_texture.layer(4) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/posz.hdr", gfx::bits::b32, 3));
    file_texture.layer(5) << gfx::host_image(cubemap_format, gfx::image_file("hdri/hdr/negz.hdr", gfx::bits::b32, 3));
    file_texture.generate_mipmaps();

    gfx::image another_image(gfx::himage(gfx::rgba8unorm, gfx::image_file("Lena.png", gfx::bits::b8, 4)));

    auto cubemap = file_texture.view(gfx::imgv_type::image_cube);
    auto texture = another_image.view(gfx::imgv_type::image2d);

    // Samplers
    gfx::sampler sampler;
    sampler.set_anisotropy(true, 16.f);
    sampler.set_filter(gfx::filter_mode::min, gfx::filter::linear);
    sampler.set_filter(gfx::filter_mode::mag, gfx::filter::linear);
    sampler.set_filter(gfx::filter_mode::mipmap, gfx::filter::linear);
    sampler.set_wrap(gfx::wrap::u, gfx::wrap_mode::mirror_repeat);
    sampler.set_wrap(gfx::wrap::v, gfx::wrap_mode::mirror_repeat);
    sampler.set_wrap(gfx::wrap::w, gfx::wrap_mode::mirror_repeat);

    // TODO: for building/creating a sampler it must be casted once.
    sampler.api_handle();

    gfx::binding_layout layout1;
    layout1
        .push(gfx::binding_type::uniform_buffer)      // VK: { set: 0, binding: 0 }; GL: { uniform buffer binding: 0 }
        .push(gfx::binding_type::sampled_image, 2)    // VK: { set: 0, binding: 1 }; GL: { texture binding: 0 }
        .push(gfx::binding_type::sampled_image)       // VK: { set: 0, binding: 2 }; GL: { texture binding: 1 }
        .push(gfx::binding_type::storage_image);      // VK: { set: 0, binding: 3 }; GL: { image binding: 0 }
    gfx::binding_layout layout2;
    layout2
        .push(gfx::binding_type::uniform_buffer)     // VK: { set: 1, binding: 0, arr:0, 1 }; GL: { uniform buffer binding: 1, 2 }
        .push(gfx::binding_type::uniform_buffer)     // VK: { set: 1, binding: 1 }; GL: { uniform buffer binding: 3 }
        .push(gfx::binding_type::storage_buffer);    // VK: { set: 1, binding: 0 }; GL: { uniform buffer binding: 0

    gfx::binding_set set1(layout1);
    set1.bind(0, dst);
    set1.bind(1, 0, cubemap, sampler);
    set1.bind(1, 1, cubemap, sampler);
    set1.bind(2, texture, sampler);
    set1.bind(3, texture);

    gfx::binding_set set2(layout2);
    set2.bind(0, dst);
    set2.bind(1, src);
    set2.bind(2, src);

    gfx::v2::renderpass_layout rpl;
    rpl.add_color_attachment(gfx::rgba8unorm);
    rpl.set_depth_stencil_attachment(gfx::d32f);

    gfx::v2::pipeline_state::layout layout;
    layout.binding_layouts.push_back(&layout1);
    gfx::v2::pipeline_state g_state;
    g_state.state_bindings = &layout;

    gfx::v2::graphics_pipeline pipeline(g_state, rpl,
                                        {gfx::shader{gfx::shader_type::vert, "05_implementation_tests/vert.vert"},
                                         gfx::shader{gfx::shader_type::frag, "05_implementation_tests/frag.frag"}});

    gfx::v2::compute_pipeline cp({}, gfx::shader{gfx::shader_type::comp, "05_implementation_tests/comp.comp"});

    gfx::image       col(gfx::img_type::attachment, gfx::rgba8unorm, {1280, 720}, 1);
    gfx::image       dep(gfx::img_type::attachment, gfx::d32f, {1280, 720}, 1);
    gfx::image_view  att_col = col.view(gfx::imgv_type::image2d);
    gfx::image_view  att_dep = dep.view(gfx::imgv_type::image2d);
    gfx::framebuffer fbo(1280, 720, 1, rpl);
    fbo.attach(gfx::attachment::color, 0, att_col, glm::vec4(0.5f, 0.2f, 0.1f, 1.f));
    fbo.attach(gfx::attachment::depth_stencil, 0, att_dep, gfx::depth_stencil(0.f, 0));

    gfx::file::message("Initialization", "Finished in " + std::to_string((std::chrono::steady_clock::now() - then).count()) + "ns",
                       gfx::msg_type::ok, gfx::msg_icon::info);

    // CMDbuf
    /*
    - primary cmd buffer
        -> enqueue secondary cmd buffers. Different types:
            -> render commands
                -> "enqueued" into primary cmd buf (aka push_back into vector on execute()ion time)
                -> runs graphics and compute commands
    Or: all cmd bufs have semaphores. Every time a cmd is executed, the submit will sync on the last semaphore and append the current as
    signal sem. Then: non-synched, collect semaphores and wait for all? cross-synching like below?
            -> non-render commands (e.g. async compute pipeline)
                -> enqueued directly into compute queue.
                -> only runs compute commands (also synched on semaphores, but different ones. maybe cross-sync ala cmd.begin_after(cmd1,
    fbo) vs unsynched cmd.begin(fbo)?)

    abstract:
        imgui.render(cmd);
        ...
        mesh.render(cmd);
        etc...
     */
    /*
        static std::vector<VkSemaphore>          final_wait_semaphores;
        static std::vector<VkPipelineStageFlags> final_wait_stages;*/

    enum class commands_type
    {
        graphics,
        compute
    };

    class compute_commands
    {
    public:
        compute_commands(gfx::u32 family = gfx::vulkan::fam::compute) : _family(family)
        {
            auto& ctx = gfx::context::current();
            _ctx_impl = static_cast<gfx::vulkan::context_implementation*>(
                std::any_cast<gfx::detail::context_implementation*>(ctx->implementation()));
            _device = _ctx_impl->device();
            _pool   = _ctx_impl->command_pools()[family];
            _queue  = _ctx_impl->queues()[family];

            gfx::v1::vulkan::init<VkCommandBufferAllocateInfo> alloc{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
            alloc.commandPool        = _pool;
            alloc.commandBufferCount = 1;
            alloc.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            vkAllocateCommandBuffers(_device, &alloc, &_cmd);

            gfx::v1::vulkan::init<VkSemaphoreCreateInfo> sem{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
            vkCreateSemaphore(_device, &sem, nullptr, &_signal);
        }

        void reset() { vkResetCommandBuffer(_cmd, 0); }

        void begin()
        {
            gfx::v1::vulkan::init<VkCommandBufferBeginInfo> beg{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
            beg.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            _sets.clear();
            vkBeginCommandBuffer(_cmd, &beg);
        }

        void end()
        {
			vkEndCommandBuffer(_cmd);
        }

        void execute_sync_after(const compute_commands& cmd, gfx::fence* f = nullptr)
        {
            if (const auto it = std::find(_ctx_impl->final_wait_semaphores[_family].begin(),
                                          _ctx_impl->final_wait_semaphores[_family].end(), cmd._signal);
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

            gfx::v1::vulkan::init<VkSubmitInfo> submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};
            submit.commandBufferCount       = 1;
            submit.pCommandBuffers          = &_cmd;
            submit.signalSemaphoreCount     = 1;
            submit.pSignalSemaphores        = &_signal;
            submit.waitSemaphoreCount       = 1;
            submit.pWaitSemaphores          = &cmd._signal;
            VkPipelineStageFlags stageFlags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            submit.pWaitDstStageMask        = &stageFlags;
            vkQueueSubmit(_queue, 1, &submit, f ? gfx::handle_cast<VkFence>(*f) : nullptr);
        }

        void execute(gfx::fence* f = nullptr)
        {
            _ctx_impl->final_wait_semaphores[_family].push_back(_signal);
            _ctx_impl->final_wait_stages[_family].push_back(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

            gfx::v1::vulkan::init<VkSubmitInfo> submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};
            submit.commandBufferCount   = 1;
            submit.pCommandBuffers      = &_cmd;
            submit.signalSemaphoreCount = 1;
            submit.pSignalSemaphores    = &_signal;
            vkQueueSubmit(_queue, 1, &submit, f ? gfx::handle_cast<VkFence>(*f) : nullptr);
        }

        void bind_compute_pipeline(const gfx::v2::compute_pipeline& p, std::initializer_list<gfx::binding_set*> bindings)
        {
            vkCmdBindPipeline(_cmd, VK_PIPELINE_BIND_POINT_COMPUTE, gfx::handle_cast<VkPipeline>(p));
            if (bindings.size() > 0) {
                const gfx::u32 offset = gfx::u32(_sets.size());
                for (auto& b : bindings) _sets.push_back(gfx::handle_cast<VkDescriptorSet>(*b));
                vkCmdBindDescriptorSets(_cmd, VK_PIPELINE_BIND_POINT_COMPUTE,
                                        static_cast<gfx::v2::vulkan::compute_pipeline_implementation*>(&*p.implementation())->layout(), 0,
                                        std::size(bindings), _sets.data() + offset, 0, nullptr);
            }
        }

        void dispatch(gfx::u32 x, gfx::u32 y = 1, gfx::u32 z = 1) { vkCmdDispatch(_cmd, x, y, z); }

        gfx::vulkan::context_implementation* _ctx_impl;
        gfx::u32                             _family;
        std::vector<VkDescriptorSet>         _sets;
        VkDevice                             _device = nullptr;
        VkCommandPool                        _pool   = nullptr;
        VkQueue                              _queue  = nullptr;
        gfx::movable_handle<VkCommandBuffer> _cmd    = nullptr;
        gfx::movable_handle<VkSemaphore>     _signal = nullptr;
    };

    class graphics_commands : public compute_commands
    {
    public:
        graphics_commands() : compute_commands(gfx::vulkan::fam::graphics) {}

        void begin_pass(const gfx::framebuffer& fbo, std::optional<gfx::rect2f> render_area = {})
        {
            VkRenderPassBeginInfo begin = static_cast<gfx::vulkan::framebuffer_implementation*>(&*fbo.implementation())->begin_info();
            if (!render_area) render_area = gfx::rect2f(glm::vec2(0, 0), glm::vec2(fbo.width(), fbo.height()));
            const auto ras            = render_area->size();
            begin.renderArea.offset.x = render_area->min.x;
            begin.renderArea.offset.y = render_area->min.y;

            begin.renderArea.extent.width  = ras.x;
            begin.renderArea.extent.height = ras.y;

            vkCmdBeginRenderPass(_cmd, &begin, VK_SUBPASS_CONTENTS_INLINE);
        }

        void end_pass() { vkCmdEndRenderPass(_cmd); }

        void bind_graphics_pipeline(const gfx::v2::graphics_pipeline& p, std::initializer_list<gfx::binding_set*> bindings)
        {
            vkCmdBindPipeline(_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, gfx::handle_cast<VkPipeline>(p));
            if (bindings.size() > 0) {
                const gfx::u32 offset = gfx::u32(_sets.size());
                for (auto& b : bindings) _sets.push_back(gfx::handle_cast<VkDescriptorSet>(*b));
                vkCmdBindDescriptorSets(_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        static_cast<gfx::v2::vulkan::graphics_pipeline_implementation*>(&*p.implementation())->layout(), 0,
                                        std::size(bindings), _sets.data() + offset, 0, nullptr);
            }
        }

        void draw(gfx::u32 vertex_count, gfx::u32 instance_count, gfx::u32 base_vertex, gfx::u32 base_instance)
        {
            vkCmdDraw(_cmd, vertex_count, instance_count, base_vertex, base_instance);
        }
    };


    graphics_commands cmd1;
    cmd1.begin();
    cmd1.bind_compute_pipeline(cp, {});
    cmd1.dispatch(1, 1);
    cmd1.end();

    graphics_commands cmd2;
    cmd2.begin();
    cmd2.bind_compute_pipeline(cp, {});
    cmd2.dispatch(1, 1);
    cmd2.end();

    graphics_commands cmd3;
    cmd3.begin();
    cmd3.begin_pass(fbo);
    cmd3.bind_graphics_pipeline(pipeline, {&set1});

    VkViewport vps{0};
    vps.width    = 1280;
    vps.height   = 720;
    vps.maxDepth = 1.f;
    vps.minDepth = 0.f;
    vkCmdSetViewport(cmd3._cmd, 0, 1, &vps);

    VkRect2D scs{0};
    scs.extent = {1280u, 720u};
    vkCmdSetScissor(cmd3._cmd, 0, 1, &scs);

    cmd3.draw(3, 1, 0, 0);
    cmd3.end_pass();
    cmd3.end();

    gfx::fence fence;

    int i = 0;
    while (context->run()) {
        cmd1.execute();
        cmd2.execute_sync_after(cmd1);
        cmd3.execute();
    }

    std::cin.ignore();
}
