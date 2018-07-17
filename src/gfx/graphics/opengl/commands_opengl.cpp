#include "state_info.hpp"
#include "commands_opengl.hpp"

namespace gfx
{
void opengl::commands_implementation::bind_graphics_pipeline(graphics_pipeline& pipeline)
{
    _curr_pipeline = &pipeline;
    _queue.emplace_back([&] { pipeline.bind(); });
}

void opengl::commands_implementation::bind_vertex_buffer(uint32_t binding, std::any buffer_handle, ptrdiff_t offset, uint32_t stride)
{
    _queue.emplace_back([=] { glBindVertexBuffer(binding, std::any_cast<mygl::buffer>(buffer_handle), offset, stride); });
}

void opengl::commands_implementation::bind_index_buffer(std::any buffer_handle, index_type type, ptrdiff_t offset)
{
    _element_offset = offset;
    _element_type   = [&] {
        switch (type)
        {
        case index_type::uint16: return GL_UNSIGNED_SHORT;
        case index_type::uint32: return GL_UNSIGNED_INT;
        }
        return GLenum(0);
    }();
    _queue.emplace_back([=] { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, std::any_cast<mygl::buffer>(buffer_handle)); });
}

void opengl::commands_implementation::draw(size_t vertex_count, size_t instance_count, ptrdiff_t first_vertex, ptrdiff_t first_instance)
{
    _queue.emplace_back([ =, draw_mode = current_draw_mode() ] {
        glDrawArraysInstancedBaseInstance(draw_mode, static_cast<int>(first_vertex), static_cast<int>(vertex_count),
                                          static_cast<int>(instance_count), static_cast<uint32_t>(first_instance));
    });
}

void opengl::commands_implementation::draw_indexed(size_t index_count, size_t instance_count, ptrdiff_t first_index, ptrdiff_t first_vertex,
                                                   ptrdiff_t first_instance)
{
    _queue.emplace_back(
        [ =, draw_mode = current_draw_mode(), ty = _element_type.load(), off = _element_offset.load(), ts = _element_type_size.load() ] {
            glDrawElementsInstancedBaseVertexBaseInstance(draw_mode, index_count, ty, reinterpret_cast<const void*>(off + ts * first_index),
                                                          instance_count, first_vertex, first_instance);
        });
}

void opengl::commands_implementation::reset()
{
    _queue.clear();
    _curr_pipeline = nullptr;
}

void opengl::commands_implementation::execute()
{
    for (auto& q : _queue) {
        q();
    }
    _curr_pipeline = nullptr;

    static state_info default_state;
    apply(default_state);
}

void opengl::commands_implementation::bind_descriptors(descriptor_set* sets, int count)
{
    for (int s = 0; s < count; ++s) {
        const auto& set = sets[s];
        for (int i = 0; i < set.type_count(); ++i) {
            const auto type = static_cast<descriptor_type>(i);

            switch (type)
            {
            case descriptor_type::uniform_buffer:
            {
                for (auto & [ binding, object ] : set.bindings(descriptor_type(i))) {
                    // TODO
                    if (object.has_value() && object.type() == typeid(mygl::buffer))
                        _queue.emplace_back(
                            [ =, obj = std::any_cast<mygl::buffer>(object) ] { glBindBufferBase(GL_UNIFORM_BUFFER, binding, obj); });
                }
            }
            break;
            case descriptor_type::storage_buffer:
            {
                for (auto & [ binding, object ] : set.bindings(descriptor_type(i))) {
                    // TODO
                    if (object.has_value() && object.type() == typeid(mygl::buffer))
                        _queue.emplace_back(
                            [ =, obj = std::any_cast<mygl::buffer>(object) ] { glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, obj); });
                }
            }
            break;
            case descriptor_type::sampled_texture:
            {
                for (auto & [ binding, object ] : set.bindings(descriptor_type(i))) {
                    auto handles = std::any_cast<std::pair<std::any, std::any>>(object);

                    // TODO
                    if (handles.first.has_value() && handles.first.type() == typeid(mygl::texture) && handles.second.has_value()
                        && handles.second.type() == typeid(mygl::sampler))
                        _queue.emplace_back(
                            [ =, tex = std::any_cast<mygl::texture>(handles.first), smp = std::any_cast<mygl::sampler>(handles.second) ] {
                                glBindSampler(binding, smp);
                                glBindTextureUnit(binding, tex);
                            });
                }
            }
            break;
            default:;
            }
        }
    }
}

void opengl::commands_implementation::begin_pass(clear_value* values, int value_count, std::any fbo_handle)
{
    _queue.emplace_back(
        [ val = std::vector<clear_value>(values, values + 2), fbo = std::any_cast<mygl::framebuffer>(fbo_handle), value_count ] {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);

            for (int i = 0; i < value_count; ++i) {
                if (auto cptr = std::get_if<glm::vec4>(&val[i])) {
                    glClearNamedFramebufferfv(mygl::framebuffer::zero, GL_COLOR, i, value_ptr(*cptr));
                }
                else if (auto dptr = std::get_if<depth_stencil>(&val[i]))
                {
                    glClearNamedFramebufferfi(mygl::framebuffer::zero, GL_DEPTH_STENCIL, 0, dptr->depth, dptr->stencil);
                }
            }
        });
}

void opengl::commands_implementation::set_viewports(gfx::viewport* vps, int count, int first)
{
    for (int i = first; i < first + count; ++i) {
        _queue.emplace_back([ =, vp = vps[i] ] {
            glViewportIndexedf(i, vp.x, vp.y, vp.width, vp.height);
            glDepthRangeIndexed(i, vp.min_depth, vp.max_depth);
        });
    }
}

GLenum opengl::commands_implementation::current_draw_mode() const
{
    if (!_curr_pipeline) return GLenum(0);
    switch (_curr_pipeline->input().assembly_topology())
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
}
