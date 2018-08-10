#include "command_list_opengl.hpp"
#include "binding_set_opengl.hpp"
#include "fence_opengl.hpp"
#include "framebuffer_opengl.hpp"
#include "pipeline_opengl.hpp"

namespace gfx {
inline namespace v1 {
namespace opengl {
void commands_implementation::initialize(commands_type type) {}
void commands_implementation::reset()
{
    _has_state = false;
    _queue.clear();
    _curr_pipeline = nullptr;
}
void commands_implementation::begin() {}
void commands_implementation::end() {}
void commands_implementation::execute_sync_after(const commands& cmd, fence* f)
{
    execute(f);
}
void commands_implementation::execute(fence* f)
{
    for (auto& q : _queue) { q(); }
    _curr_pipeline = nullptr;

    glBindFramebuffer(GL_FRAMEBUFFER, mygl::framebuffer::zero);
    push_fence(f);
    glFlush();
}
void commands_implementation::bind_pipeline(const compute_pipeline& p, std::initializer_list<binding_set*> bindings)
{
    _has_state     = false;
    _curr_pipeline = nullptr;
    _queue.emplace_back([hnd = handle_cast<mygl::pipeline>(p)] { glBindProgramPipeline(hnd); });
}
void commands_implementation::dispatch(u32 x, u32 y, u32 z)
{
    _queue.emplace_back([=] { glDispatchCompute(x, y, z); });
}
void commands_implementation::begin_pass(const framebuffer& fbo, std::optional<rect2f> render_area)
{
    _curr_framebuffer = &fbo;

	if (!render_area)
		render_area ={ {0, 0}, {fbo.width(), fbo.height()} };

    _queue.emplace_back([fbo = _curr_framebuffer, render_area] {
        static_cast<framebuffer_implementation*>(&*fbo->implementation())->begin();

        for (int i = 0; i < fbo->color_clear_values().size(); ++i)
        {
            if (const auto cv = fbo->color_clear_values()[i]; cv && std::holds_alternative<glm::vec4>(*cv))
            { glClearNamedFramebufferfv(handle_cast<mygl::framebuffer>(*fbo), GL_COLOR, i, glm::value_ptr(std::get<glm::vec4>(*cv))); }
        }
        if (const auto dv = fbo->depth_clear_value(); dv && std::holds_alternative<depth_stencil>(*dv))
        {
            const auto& dvv = std::get<depth_stencil>(*dv);
            glClearNamedFramebufferfi(handle_cast<mygl::framebuffer>(*fbo), GL_DEPTH_STENCIL, 0, dvv.depth, dvv.stencil);
        }
    });
}
void commands_implementation::end_pass()
{
    _queue.emplace_back(
        [fbo = _curr_framebuffer] { static_cast<framebuffer_implementation*>(&*fbo->implementation())->end(); });
}
void commands_implementation::bind_pipeline(const graphics_pipeline& p, std::initializer_list<binding_set*> bindings)
{
	assert(_curr_framebuffer);
    _has_state     = true;
    _curr_pipeline = &p;
    _queue.emplace_back([&, s = glm::vec2(_curr_framebuffer->width(), _curr_framebuffer->height())] {
		for (int i=0; i<15; ++i)
		{
			glViewportIndexedf(i, 0, 0, s.x, s.y);
			glDepthRangeIndexed(i, 0.f, 1.f);
			glScissorIndexed(i, 0, 0, s.x, s.y);
		}

        static_cast<graphics_pipeline_implementation*>(&*p.implementation())->apply_all(); 
    });

    if (bindings.size() > 0)
    {
        u32 ssb = 0;
        u32 ub  = 0;
        u32 img = 0;
        u32 tex = 0;
        for (auto* b : bindings)
        {
            _queue.emplace_back(static_cast<binding_set_implementation*>(&*b->implementation())->bind_all(ssb, ub, img, tex));
        }
    }
}
void commands_implementation::draw(u32 vertex_count, u32 instance_count, u32 base_vertex, u32 base_instance)
{
    _queue.emplace_back([=, draw_mode = current_draw_mode()] {
        glDrawArraysInstancedBaseInstance(draw_mode, static_cast<int>(base_vertex), static_cast<int>(vertex_count),
                                          static_cast<int>(instance_count), static_cast<uint32_t>(base_instance));
    });
}

void commands_implementation::draw_indexed(u32 index_count, u32 instance_count, u32 base_index, u32 base_vertex, u32 base_instance)
{
    _queue.emplace_back([=, draw_mode = current_draw_mode(), et = _element_type.load(), eo = _element_offset.load(), es = _element_type_size.load()] {
        glDrawElementsInstancedBaseVertexBaseInstance(draw_mode, index_count, et,
                                                      reinterpret_cast<void*>(eo + base_index * es),
                                                      instance_count, base_vertex, base_instance);
    });
}

void commands_implementation::bind_vertex_buffer(const handle& buffer, u32 binding, i64 offset)
{
    const mygl::buffer buf = std::any_cast<mygl::buffer>(buffer);
    const auto strd = static_cast<graphics_pipeline_implementation*>(&*_curr_pipeline->implementation())->binding_strides().at(binding);
    _queue.emplace_back([=] { glBindVertexBuffer(binding, buf, offset, strd); });
}

void commands_implementation::bind_index_buffer(const handle& buffer, index_type index, i64 offset)
{
    _element_offset = offset;
    _element_type   = [=] {
        switch (index)
        {
        case index_type::uint16: return GL_UNSIGNED_SHORT;
        case index_type::uint32: return GL_UNSIGNED_INT;
        }
        return GL_UNSIGNED_INT;
    }();
    _element_type_size     = size_t(index);
    const mygl::buffer buf = std::any_cast<mygl::buffer>(buffer);
    _queue.emplace_back([=] { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf); });
}

GLenum commands_implementation::current_draw_mode() const
{
    if (!_curr_pipeline) return GLenum(0);
    switch (static_cast<graphics_pipeline_implementation*>(&*_curr_pipeline->implementation())->primitive_topology())
    {
    case topology::point_list: return GL_POINTS;
    case topology::line_list: return GL_LINES;
    case topology::line_strip: return GL_LINE_STRIP;
    case topology::triangle_list: return GL_TRIANGLES;
    case topology::triangle_strip: return GL_TRIANGLE_STRIP;
    case topology::triangle_fan: return GL_TRIANGLE_FAN;
    case topology::line_list_adj: return GL_LINES_ADJACENCY;
    case topology::line_strip_adj: return GL_LINE_STRIP_ADJACENCY;
    case topology::triangle_list_adj: return GL_TRIANGLES_ADJACENCY;
    case topology::triangle_strip_adj: return GL_TRIANGLE_STRIP_ADJACENCY;
    case topology::patch_list: return GL_PATCHES;
    default: return GLenum(0);
    }
}

void commands_implementation::push_binding(u32 set, u32 b, u32 arr_element, binding_type type, std::any obj)
{
    const auto& ps = static_cast<graphics_pipeline_implementation*>(&*_curr_pipeline->implementation())->proxy_sets();

    u32 offset = 0;
    for (int i = 0; i < set; ++i) { offset += static_cast<binding_set_implementation*>(&*ps[i].implementation())->count(type); }

    auto& proxy_set = ps[set];
    auto  binding   = offset + static_cast<binding_set_implementation*>(&*proxy_set.implementation())->binding(b, arr_element);

    switch (type)
    {
    case binding_type::storage_buffer:
    {
        mygl::buffer buffer;
        if (obj.type() == typeid(const std::unique_ptr<detail::host_buffer_implementation>*))
        {
            auto& buf = *std::any_cast<const std::unique_ptr<detail::host_buffer_implementation>*>(obj);
            buffer    = std::any_cast<mygl::buffer>(buf->api_handle());
        }
        if (obj.type() == typeid(const std::unique_ptr<detail::device_buffer_implementation>*))
        {
            auto& buf = *std::any_cast<const std::unique_ptr<detail::device_buffer_implementation>*>(obj);
            buffer    = std::any_cast<mygl::buffer>(buf->api_handle());
        }
        _queue.emplace_back([=] { glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buffer); });
    }
    break;
    case binding_type::uniform_buffer:
    {
        mygl::buffer buffer;
        if (obj.type() == typeid(const std::unique_ptr<detail::host_buffer_implementation>*))
        {
            auto& buf = *std::any_cast<const std::unique_ptr<detail::host_buffer_implementation>*>(obj);
            buffer    = std::any_cast<mygl::buffer>(buf->api_handle());
        }
        if (obj.type() == typeid(const std::unique_ptr<detail::device_buffer_implementation>*))
        {
            auto& buf = *std::any_cast<const std::unique_ptr<detail::device_buffer_implementation>*>(obj);
            buffer    = std::any_cast<mygl::buffer>(buf->api_handle());
        }
        _queue.emplace_back([=] { glBindBufferBase(GL_UNIFORM_BUFFER, binding, buffer); });
    }
    break;
    case binding_type::storage_image:
    {
        auto& iv       = *std::any_cast<const std::unique_ptr<detail::image_view_implementation>*>(obj);
        auto  img_view = std::any_cast<mygl::texture>(iv->api_handle());

        _queue.emplace_back([=] { glBindImageTextures(binding, 1, &img_view); });
    }
    break;
    case binding_type::sampled_image:
    {
        auto [ivp, sp] = std::any_cast<
            std::pair<const std::unique_ptr<detail::image_view_implementation>*, const std::unique_ptr<detail::sampler_implementation>*>>(
            obj);
        auto& iv       = *ivp;
        auto& s        = *sp;
        auto  img_view = std::any_cast<mygl::texture>(iv->api_handle());
        auto  smp      = std::any_cast<mygl::sampler>(s->api_handle());

        _queue.emplace_back([=] {
            glBindSampler(binding, smp);
            glBindTextureUnit(binding, img_view);
        });
    }
    break;
    default:;
    }
}

void commands_implementation::set_viewports(u32 first, span<viewport> vp, span<rect2f> scissors)
{
	assert(_curr_framebuffer);
    _queue.emplace_back(
        [first, vps = std::vector<viewport>(vp.begin(), vp.end()), scs = std::vector<rect2f>(scissors.begin(), scissors.end()), h = _curr_framebuffer->height()] {
            for (int i = 0; i < vps.size(); ++i)
            {
                glViewportIndexedf(i + first, vps[i].x, h-(vps[i].y + vps[i].height), vps[i].width, vps[i].height);
                glDepthRangeIndexed(i + first, vps[i].min_depth, vps[i].max_depth);
                if (!scs.empty())
                {
                    const auto s = scs[i].size();
                    glScissorIndexed(i + first, scs[i].min.x, h-scs[i].max.y, s.x, s.y);
                }
            }
        });
}

}    // namespace opengl
}    // namespace v1
}    // namespace gfx
