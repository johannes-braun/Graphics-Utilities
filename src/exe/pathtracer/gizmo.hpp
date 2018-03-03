#pragma once
#include <res/transform.hpp>
#include <jpu/memory>
#include <opengl/pipeline.hpp>
#include <res/presets.hpp>
#include <opengl/buffer.hpp>
#include <opengl/vertex_array.hpp>
#include <jpu/impl/color/color.hpp>


namespace gfx
{
    enum class gizmo_translate_state : uint8_t
    {
        hover_none = 0,
        hover_center = 0b111,
        hover_x = 0b001,
        hover_y = 0b010,
        hover_z = 0b100,
        hover_xy = 0b011,
        hover_xz = 0b101,
        hover_yz = 0b110,

        hover_rx = 0b001000,
        hover_ry = 0b010000,
        hover_rz = 0b100000,
    };
    using gizmo_translate_state_flags = jpu::flags<uint8_t, gizmo_translate_state>;

    class gizmo
    {
    public:
        struct vertex
        {
            glm::vec3 position;
            glm::u8vec4 color;
        };
        using index = uint16_t;

        gizmo()
        {
            _translate_pipeline = jpu::make_ref<gl::graphics_pipeline>();
            _translate_pipeline->use_stages(new gl::shader("multi_gizmo/gizmo_translate.vert"), new gl::shader("multi_gizmo/gizmo_translate.frag"));

            std::array<vertex, 9 + 6 + 6> non_cube_vertices{
                // x-y-plane triangle
                vertex{ glm::vec3(0, 0, 0), glm::u8vec4(255, 255, 255, 120) },
                vertex{ glm::vec3(5, 0, 0), glm::u8vec4(255, 255, 255, 120) },
                vertex{ glm::vec3(0, 5, 0), glm::u8vec4(255, 255, 255, 120) },

                // x-z-plane triangle
                vertex{ glm::vec3(0, 0, 0), glm::u8vec4(255, 255, 255, 120) },
                vertex{ glm::vec3(5, 0, 0), glm::u8vec4(255, 255, 255, 120) },
                vertex{ glm::vec3(0, 0, 5), glm::u8vec4(255, 255, 255, 120) },

                // y-z-plane triangle
                vertex{ glm::vec3(0, 0, 0), glm::u8vec4(255, 255, 255, 120) },
                vertex{ glm::vec3(0, 5, 0), glm::u8vec4(255, 255, 255, 120) },
                vertex{ glm::vec3(0, 0, 5), glm::u8vec4(255, 255, 255, 120) },

                // Diagonal Lines
                vertex{ glm::vec3(5, 0, 0), glm::u8vec4(128, 0, 0, 255) },
                vertex{ glm::vec3(0, 5, 0), glm::u8vec4(0, 128, 0, 255) },
                vertex{ glm::vec3(5, 0, 0), glm::u8vec4(128, 0, 0, 255) },
                vertex{ glm::vec3(0, 0, 5), glm::u8vec4(0, 0, 128, 255) },
                vertex{ glm::vec3(0, 5, 0), glm::u8vec4(0, 128, 0, 255) },
                vertex{ glm::vec3(0, 0, 5), glm::u8vec4(0, 0, 128, 255) },

                // Axis Lines
                vertex{ glm::vec3(0, 0, 0), glm::u8vec4(0, 0, 0, 255) },
                vertex{ glm::vec3(10, 0, 0), glm::u8vec4(255, 0, 0, 255) },
                vertex{ glm::vec3(0, 0, 0), glm::u8vec4(0, 0, 0, 255) },
                vertex{ glm::vec3(0, 10, 0), glm::u8vec4(0, 255, 0, 255) },
                vertex{ glm::vec3(0, 0, 0), glm::u8vec4(0, 0, 0, 255) },
                vertex{ glm::vec3(0, 0, 10), glm::u8vec4(0, 0, 255, 255) }
            };

            std::array<index, 9+6+3> non_cube_indices{
                // plane triangles
                0, 1, 2, 3, 4, 5, 6, 7, 8,

                // diagonal lines and axis lines
                0, 1, 2, 3, 4, 5,

                // end points
                1, 3, 5
            };


            _vertices.resize(res::presets::cube::vertices.size());
            for (int i = 0; i < res::presets::cube::vertices.size(); ++i)
            {
                _vertices[i].position = 0.25f * (1.2f*res::presets::cube::vertices[i].position + 1.f);
                _vertices[i].color = glm::u8vec4(glm::vec4(64.f + 192.f*glm::vec3(glm::max(dot(glm::vec3(res::presets::cube::vertices[i].normal), normalize(glm::vec3(1, 2, 3))), 0.f)), 255.f));
            }
            _vertices.insert(_vertices.end(), non_cube_vertices.begin(), non_cube_vertices.end());
            const size_t last_vsize = _vertices.size();
            _vertices.resize(_vertices.size() + 60);
            for(int i=0; i<20; ++i)
            {
                _vertices[last_vsize + i].position.x = 10.f*sin(glm::radians(static_cast<float>(i * (360.f / 20.f))));
                _vertices[last_vsize + i].position.y = 0;
                _vertices[last_vsize + i].position.z = 10.f*cos(glm::radians(static_cast<float>(i * (360.f / 20.f))));
                _vertices[last_vsize + i].color = glm::u8vec4(0, 255, 0, 255);

                _vertices[last_vsize + 20 + i].position.x = 10.f*sin(glm::radians(static_cast<float>(i * (360.f / 20.f))));
                _vertices[last_vsize + 20 + i].position.y = 10.f*cos(glm::radians(static_cast<float>(i * (360.f / 20.f))));
                _vertices[last_vsize + 20 + i].position.z = 0;
                _vertices[last_vsize + 20 + i].color = glm::u8vec4(0, 0, 255, 255);

                _vertices[last_vsize + 40 + i].position.x = 0;
                _vertices[last_vsize + 40 + i].position.y = 10.f*sin(glm::radians(static_cast<float>(i * (360.f / 20.f))));
                _vertices[last_vsize + 40 + i].position.z = 10.f*cos(glm::radians(static_cast<float>(i * (360.f / 20.f))));
                _vertices[last_vsize + 40 + i].color = glm::u8vec4(255, 0, 0, 255);
            }

            _vertices_default = _vertices;
            for (auto&& v : _vertices)
                v.color = glm::u8vec4(255, 128, 0, 255);

            std::vector<index> indices(res::presets::cube::indices.size());
            _vertex_buffer = jpu::make_ref<gl::buffer>(_vertices_default, gl::buffer_flag_bits::dynamic_storage);
            for (int i = 0; i < res::presets::cube::indices.size(); ++i)
                indices[i] = res::presets::cube::indices[i];
            indices.insert(indices.end(), non_cube_indices.begin(), non_cube_indices.end());
            
            const size_t last_size = indices.size();
            indices.resize(indices.size() + 21);
            for (int i = 0; i < 20; ++i)
                indices[last_size + i] = i;
            _index_buffer = jpu::make_ref<gl::buffer>(indices);

            _vertex_array = jpu::make_ref<gl::vertex_array>();
            _vertex_array->set_element_buffer(*_index_buffer);
            _vertex_array->add_bindings({
                gl::vertex_attribute_binding(0, *_vertex_buffer, 0, 3, GL_FLOAT, sizeof(vertex), offsetof(vertex, position), false),
                gl::vertex_attribute_binding(1, *_vertex_buffer, 0, 4, GL_UNSIGNED_BYTE, sizeof(vertex), offsetof(vertex, color), true),
            });
        }

        void reassign_vertices(std::initializer_list<size_t> indices, const bool to_default)
        {
            if (to_default)
            {
                for(const auto idx : indices)
                    _vertex_buffer->assign(_vertices_default.data() + res::presets::cube::vertices.size() + idx, 1, (res::presets::cube::vertices.size() + idx) * sizeof(vertex));
            }
            else
            {
                for (const auto idx : indices)
                    _vertex_buffer->assign(_vertices.data() + res::presets::cube::vertices.size() + idx, 1, (res::presets::cube::vertices.size() + idx) * sizeof(vertex));
            }
        }

        void change_hover_state(const gizmo_translate_state_flags new_state)
        {
            if (_last_hover_state == new_state)
                return;

            if((_last_hover_state & gizmo_translate_state::hover_x) && !(new_state & gizmo_translate_state::hover_x))
            {
                reassign_vertices({ 9, 11, 15, 16 }, true);
            }
            if ((_last_hover_state & gizmo_translate_state::hover_y) && !(new_state & gizmo_translate_state::hover_y))
            {
                reassign_vertices({ 10, 13, 17, 18 }, true);
            }
            if ((_last_hover_state & gizmo_translate_state::hover_z) && !(new_state & gizmo_translate_state::hover_z))
            {
                reassign_vertices({ 12, 14, 19, 20 }, true);
            }
            if ((_last_hover_state & gizmo_translate_state::hover_ry) && !(new_state & gizmo_translate_state::hover_ry))
            {
                _vertex_buffer->assign(_vertices_default.data() + res::presets::cube::vertices.size() + 21, 20, (res::presets::cube::vertices.size() + 21) * sizeof(vertex));
            }
            if ((_last_hover_state & gizmo_translate_state::hover_rz) && !(new_state & gizmo_translate_state::hover_rz))
            {
                _vertex_buffer->assign(_vertices_default.data() + res::presets::cube::vertices.size() + 41, 20, (res::presets::cube::vertices.size() + 41) * sizeof(vertex));
            }
            if ((_last_hover_state & gizmo_translate_state::hover_rx) && !(new_state & gizmo_translate_state::hover_rx))
            {
                _vertex_buffer->assign(_vertices_default.data() + res::presets::cube::vertices.size() + 61, 20, (res::presets::cube::vertices.size() + 61) * sizeof(vertex));
            }

            if ((new_state & gizmo_translate_state::hover_x) && !(_last_hover_state & gizmo_translate_state::hover_x))
            {
                reassign_vertices({ 9, 11, 15, 16 }, false);
            }
            if ((new_state & gizmo_translate_state::hover_y) && !(_last_hover_state & gizmo_translate_state::hover_y))
            {
                reassign_vertices({ 10, 13, 17, 18 }, false);
            }
            if ((new_state & gizmo_translate_state::hover_z) && !(_last_hover_state & gizmo_translate_state::hover_z))
            {
                reassign_vertices({ 12, 14, 19, 20 }, false);
            }
            if ((new_state & gizmo_translate_state::hover_ry) && !(_last_hover_state & gizmo_translate_state::hover_ry))
            {
                _vertex_buffer->assign(_vertices.data() + res::presets::cube::vertices.size() + 21, 20, (res::presets::cube::vertices.size() + 21) * sizeof(vertex));
            }
            if ((new_state & gizmo_translate_state::hover_rz) && !(_last_hover_state & gizmo_translate_state::hover_rz))
            {
                _vertex_buffer->assign(_vertices.data() + res::presets::cube::vertices.size() + 41, 20, (res::presets::cube::vertices.size() + 41) * sizeof(vertex));
            }
            if ((new_state & gizmo_translate_state::hover_rx) && !(_last_hover_state & gizmo_translate_state::hover_rx))
            {
                _vertex_buffer->assign(_vertices.data() + res::presets::cube::vertices.size() + 61, 20, (res::presets::cube::vertices.size() + 61) * sizeof(vertex));
            }

            _last_hover_state = new_state;
        }

        void update(const glm::mat4& view, const glm::mat4& projection, const bool mouse_button_down, const float mouse_x, const float mouse_y)
        {
            if (!transform)
                return;
            const glm::mat4 mvp = projection * view * static_cast<glm::mat4>(*transform);
            const float aspect = projection[1][1] / projection[0][0];
            const float scale = (mvp * glm::vec4(0, 0, 0, 1)).w * 0.015f;
            const glm::mat4 scaled_mvp = mvp * glm::scale(glm::vec3(scale));
            _translate_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("model_view_projection") = scaled_mvp;

            glm::vec2 uv = (glm::vec2(mouse_x, mouse_y) - 0.5f) * 2.f;
            uv.y = - uv.y;
            const glm::vec4 unnormalized_origin = inverse(projection * view) * glm::vec4(uv, 0, 1);
            const glm::vec3 world_direction = normalize(glm::vec3(unnormalized_origin) / unnormalized_origin.w);
            const glm::vec3 world_origin = inverse(view)[3];
            const glm::vec3 mouse_direction = inverse(transpose(inverse(translate(transform->position) * glm::scale(glm::vec3(scale))))) * glm::vec4(world_direction, 0);
            const glm::vec3 mouse_origin = inverse(translate(transform->position) * glm::scale(glm::vec3(scale))) * glm::vec4(world_origin, 1);
            
            const float side_size = .3f;
            const float grip_size = .8f;
            const float grip_pad = .8f;
            if (!mouse_button_down)
                _click_lock = false;

            if (_click_lock || !_pressed)
            {
                gizmo_translate_state_flags this_frame_state = gizmo_translate_state::hover_none;
                {
                    const glm::vec3 bmax{ 1.4f };
                    const glm::vec3 bmin{ -0.4f };
                    if (intersect_bounds(mouse_origin, mouse_direction, bmin, bmax, std::numeric_limits<float>::max()))
                        this_frame_state |= gizmo_translate_state::hover_center;
                }

                if (!(this_frame_state & gizmo_translate_state::hover_x) || !(this_frame_state & gizmo_translate_state::hover_y))
                {
                    // x-axis
                    const glm::vec3 bmax{ 5.f, 5.f, side_size };
                    const glm::vec3 bmin{ grip_pad + grip_size, grip_pad + grip_size, -side_size };
                    if (intersect_bounds(mouse_origin, mouse_direction, bmin, bmax, std::numeric_limits<float>::max()))
                        this_frame_state |= gizmo_translate_state::hover_xy;
                }
                if (!(this_frame_state & gizmo_translate_state::hover_x) || !(this_frame_state & gizmo_translate_state::hover_z))
                {
                    // x-axis
                    const glm::vec3 bmax{ 5.f, side_size, 5.f };
                    const glm::vec3 bmin{ grip_pad + grip_size, -side_size, grip_pad + grip_size };
                    if (intersect_bounds(mouse_origin, mouse_direction, bmin, bmax, std::numeric_limits<float>::max()))
                        this_frame_state |= gizmo_translate_state::hover_xz;
                }
                if (!(this_frame_state & gizmo_translate_state::hover_y) || !(this_frame_state & gizmo_translate_state::hover_z))
                {
                    // x-axis
                    const glm::vec3 bmax{ side_size, 5.f, 5.f };
                    const glm::vec3 bmin{ -side_size, grip_pad + grip_size, grip_pad + grip_size };
                    if (intersect_bounds(mouse_origin, mouse_direction, bmin, bmax, std::numeric_limits<float>::max()))
                        this_frame_state |= gizmo_translate_state::hover_yz;
                }

                if ((this_frame_state & gizmo_translate_state::hover_x) != gizmo_translate_state::hover_x)
                {
                    // x-axis
                    const glm::vec3 bmax{ 10.f, grip_size, grip_size };
                    const glm::vec3 bmin{ -grip_size };
                    if (intersect_bounds(mouse_origin, mouse_direction, bmin, bmax, std::numeric_limits<float>::max()))
                        this_frame_state |= gizmo_translate_state::hover_x;
                }
                if (!(this_frame_state & gizmo_translate_state::hover_y))
                {
                    // y-axis
                    const glm::vec3 bmax{ grip_size, 10.f, grip_size };
                    const glm::vec3 bmin{ -grip_size };
                    if (intersect_bounds(mouse_origin, mouse_direction, bmin, bmax, std::numeric_limits<float>::max()))
                        this_frame_state |= gizmo_translate_state::hover_y;
                }
                if (!(this_frame_state & gizmo_translate_state::hover_z))
                {
                    // z-axis
                    const glm::vec3 bmax{ grip_size, grip_size, 10.f };
                    const glm::vec3 bmin{ -grip_size };
                    if (intersect_bounds(mouse_origin, mouse_direction, bmin, bmax, std::numeric_limits<float>::max()))
                        this_frame_state |= gizmo_translate_state::hover_z;
                }

                if(!this_frame_state)
                {
                    float tmins[3];
                    const auto dcheck = [&](float t)
                    {
                        return glm::abs(glm::clamp(glm::length(mouse_origin + mouse_direction * t), 20.f - grip_pad, 20.f + grip_pad) - 20.f) < grip_size;
                    };
                    gizmo_translate_state_flags tstate;
                    {
                        const glm::vec3 bmax{ 10.f, grip_pad, 10.f };
                        const glm::vec3 bmin{ -10.f, -grip_pad, -10.f };
                        if (intersect_bounds(mouse_origin, mouse_direction, bmin, bmax, std::numeric_limits<float>::max(), &tmins[0]) && dcheck(tmins[0]))
                            tstate |= gizmo_translate_state::hover_ry;
                    }
                    {
                        const glm::vec3 bmax{ 10.f, 10.f, grip_pad };
                        const glm::vec3 bmin{ -10.f, -10.f, -grip_pad };
                        if (intersect_bounds(mouse_origin, mouse_direction, bmin, bmax, std::numeric_limits<float>::max(), &tmins[1]) && dcheck(tmins[1]))
                            tstate |= gizmo_translate_state::hover_rz;
                    }
                    {
                        const glm::vec3 bmax{ grip_pad, 10.f, 10.f };
                        const glm::vec3 bmin{ -grip_pad, -10.f, -10.f };
                        if (intersect_bounds(mouse_origin, mouse_direction, bmin, bmax, std::numeric_limits<float>::max(), &tmins[2]) && dcheck(tmins[2]))
                            tstate |= gizmo_translate_state::hover_rx;
                    }

                    float mint = std::numeric_limits<float>::max();
                    for(uint8_t i = 0b001000, j=0; i != 0b1000000; i<<=1, ++j)
                    {
                        if(tstate & gizmo_translate_state(i) && tmins[j] < mint)
                        {
                            mint = tmins[j];
                            this_frame_state = gizmo_translate_state(i);
                        }

                    }
                }

                change_hover_state(this_frame_state);
                _first_down = true;
                if (this_frame_state && mouse_button_down)
                    _pressed = true;
                else if (mouse_button_down)
                    _click_lock = true;
            }
            else
            {
                const float drag_speed = 1.f;

                if (!mouse_button_down)
                    _pressed = false;
                const glm::vec4 x_axis(1, 0, 0, 0);
                const glm::vec4 y_axis(0, 1, 0, 0);
                const glm::vec4 z_axis(0, 0, 1, 0);
                const glm::vec4 sx = scaled_mvp * x_axis;
                const glm::vec4 sy = scaled_mvp * y_axis;
                const glm::vec4 sz = scaled_mvp * z_axis;
                uv.x *= 2.f*aspect;
                const float mvx = drag_speed * dot(glm::vec2(sx), uv);
                const float mvy = drag_speed * dot(glm::vec2(sy), uv);
                const float mvz = drag_speed * dot(glm::vec2(sz), uv);
             
                if (_first_down)
                {
                    _down_offset = glm::vec3(0);
                    if (_last_hover_state & gizmo_translate_state::hover_x)
                        _down_offset.x = mvx;
                    if (_last_hover_state & gizmo_translate_state::hover_y)
                        _down_offset.y = mvy;
                    if (_last_hover_state & gizmo_translate_state::hover_z)
                        _down_offset.z = mvz;
                }
                _first_down = false;
                const float x = mvx - _down_offset.x;
                const float y = mvy - _down_offset.y;
                const float z = mvz - _down_offset.z;
                transform->position += glm::vec3(glm::scale(glm::vec3(20.f)) * glm::vec4(
                    _last_hover_state & gizmo_translate_state::hover_x ? x : 0,
                    _last_hover_state & gizmo_translate_state::hover_y ? y : 0, 
                    _last_hover_state & gizmo_translate_state::hover_z ? z : 0, 0));

                _down_offset = glm::vec3(0);
                if (_last_hover_state & gizmo_translate_state::hover_x)
                    _down_offset.x = mvx;
                if (_last_hover_state & gizmo_translate_state::hover_y)
                    _down_offset.y = mvy;
                if (_last_hover_state & gizmo_translate_state::hover_z)
                    _down_offset.z = mvz;
            }
        }

        void render() const
        {
            if (!transform)
                return;
            const auto get_i = [](GLenum e) { int i; glGetIntegerv(e, &i); return i; };
            const int last_program = get_i(GL_CURRENT_PROGRAM);
            const int last_blend_src_rgb = get_i(GL_BLEND_SRC_RGB);
            const int last_blend_dst_rgb = get_i(GL_BLEND_DST_RGB);
            const int last_blend_src_alpha = get_i(GL_BLEND_SRC_ALPHA);
            const int last_blend_dst_alpha = get_i(GL_BLEND_DST_ALPHA);
            const int last_blend_equation_rgb = get_i(GL_BLEND_EQUATION_RGB);
            const int last_blend_equation_alpha = get_i(GL_BLEND_EQUATION_ALPHA);
            const int last_enable_blend = glIsEnabled(GL_BLEND);
            const int last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
            const int last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
            int last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);

            _translate_pipeline->bind();
            _vertex_array->bind();
            index* begin = nullptr;
            int base = 0;
            glDisable(GL_BLEND);
            glDrawElementsBaseVertex(GL_TRIANGLES, res::presets::cube::indices.size(), GL_UNSIGNED_SHORT, begin, base);
            begin += res::presets::cube::indices.size();
            base += res::presets::cube::vertices.size();
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDrawElementsBaseVertex(GL_TRIANGLES, 9, GL_UNSIGNED_SHORT, begin, base);
            begin += 9;
            base += 9;
            glDisable(GL_BLEND);
            glLineWidth(2.f);
            glDrawElementsBaseVertex(GL_LINES, 6, GL_UNSIGNED_SHORT, begin, base);
            base += 6;
            glLineWidth(6.f);
            glDrawElementsBaseVertex(GL_LINES, 6, GL_UNSIGNED_SHORT, begin, base);
            begin += 6;
            glPointSize(12.f);
            glDrawElementsBaseVertex(GL_POINTS, 3, GL_UNSIGNED_SHORT, begin, base);
            base += 6;
            begin += 3;
            glLineWidth(2.f);
            glDrawElementsBaseVertex(GL_LINE_LOOP, 20, GL_UNSIGNED_SHORT, begin, base);
            base += 20;
            glDrawElementsBaseVertex(GL_LINE_LOOP, 20, GL_UNSIGNED_SHORT, begin, base);
            base += 20;
            glDrawElementsBaseVertex(GL_LINE_LOOP, 20, GL_UNSIGNED_SHORT, begin, base);
            base += 20;
            begin += 20;

            glUseProgram(last_program);
            glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
            glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
            last_enable_blend ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
            last_enable_cull_face ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
            last_enable_depth_test ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
            glPolygonMode(GL_FRONT_AND_BACK, last_polygon_mode[0]);
        }

        gizmo(const gizmo& other) = default;
        gizmo(gizmo&& other) noexcept = default;
        gizmo& operator=(const gizmo& other) = default;
        gizmo& operator=(gizmo&& other) noexcept = default;
        ~gizmo() = default;

        res::transform* transform = nullptr;

    private:
        static bool intersect_bounds(const glm::vec3 origin, const glm::vec3 direction,
            const glm::vec3 bounds_min, const glm::vec3 bounds_max, const float max_distance, float* tmin = nullptr)
        {
            const glm::vec3 inv_direction = 1.f / direction;

            //intersections with box planes parallel to x, y, z axis
            const glm::vec3 t135 = (bounds_min - origin) * inv_direction;
            const glm::vec3 t246 = (bounds_max - origin) * inv_direction;

            const glm::vec3 min_values = min(t135, t246);
            const glm::vec3 max_values = max(t135, t246);

            const float t = glm::max(glm::max(min_values.x, min_values.y), min_values.z);
            const float tmax = glm::min(glm::min(max_values.x, max_values.y), max_values.z);
            if (tmin) *tmin = tmax;
            return tmax >= 0 && t <= tmax && t <= max_distance;
        }
        
        bool _pressed = false;
        bool _first_down = false;
        bool _click_lock = false;
        glm::vec3 _down_offset;
        gizmo_translate_state_flags _last_hover_state;
        std::vector<vertex> _vertices;
        std::vector<vertex> _vertices_default;
        jpu::ref_ptr<gl::graphics_pipeline> _translate_pipeline;
        jpu::ref_ptr<gl::buffer> _vertex_buffer;
        jpu::ref_ptr<gl::buffer> _index_buffer;
        jpu::ref_ptr<gl::vertex_array> _vertex_array;
    };
}
