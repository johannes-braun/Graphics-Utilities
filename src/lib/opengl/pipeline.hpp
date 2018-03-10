#pragma once

#include "shader.hpp"
#include "uniform.hpp"

#include <jpu/memory.hpp>
#include <jpu/log.hpp>
#include <array>
#include <map>
#include <set>
#include "vertex_array.hpp"
#include "buffer.hpp"

namespace gl
{
    enum class primitive
    {
        points = GL_POINTS,
        line_strip = GL_LINE_STRIP, 
        line_loop = GL_LINE_LOOP, 
        lines = GL_LINES, 
        line_strip_adj = GL_LINE_STRIP_ADJACENCY, 
        lines_adj = GL_LINES_ADJACENCY, 
        triangle_strip = GL_TRIANGLE_STRIP, 
        triangle_fan = GL_TRIANGLE_FAN, 
        triangles = GL_TRIANGLES,
        triangle_strip_adj = GL_TRIANGLE_STRIP_ADJACENCY, 
        triangles_adj = GL_TRIANGLES_ADJACENCY,
        patches = GL_PATCHES
    };

    enum class basic_primitive
    {
        points = GL_POINTS,
        lines = GL_LINES,
        lines_adj = GL_LINES_ADJACENCY,
        triangles = GL_TRIANGLES,
        triangles_adj = GL_TRIANGLES_ADJACENCY,
        patches = GL_PATCHES
    };

    enum class index_type
    {
        u8 = GL_UNSIGNED_BYTE, 
        u16 = GL_UNSIGNED_SHORT, 
        u32 = GL_UNSIGNED_INT
    };

    class pipeline : public jpu::ref_count
    {
    public:
        pipeline();
        virtual ~pipeline();

        operator gl_program_pipeline_t() const;

        void bind() const;
        shader* stage(shader_type s) const;
        void reload_stages(bool force = false) const;
        
        void set_stages_enabled(const std::vector<shader_type>& stages, bool enable);

    protected:
        void set_stage_enabled(shader_type stage, bool enable);
        void use_shader(shader* s);
        static GLbitfield stage_bits(shader_type t);
        void validate() const;

        gl_program_pipeline_t _id;
        std::map<shader_type, jpu::ref_ptr<shader>> _shaders;
        std::set<shader_type> _disabled_stages;
    };

    class graphics_pipeline : public pipeline
    {
    public:
        template<typename... TShaders>
        void use_stages(TShaders ... shd);
        template<typename T>
        uniform<T> get_uniform(shader_type s, const char* name);

        void disable_input(uint32_t attribute) const;
        void set_input_format(uint32_t attribute, int components, GLenum type, bool normalized) const;
        void set_input_buffer(uint32_t attribute, const buffer* buffer, size_t stride, size_t offset = 0) const;
        void set_index_buffer(const buffer* buffer, index_type elem_type) const;

        void draw_indexed(primitive p, size_t elem_count, size_t base_index = 0, uint32_t base_vertex = 0,
                          uint32_t instance_count = 1, uint32_t base_instance = 0) const;
        void draw(primitive p, size_t vertex_count, size_t first = 0, uint32_t instance_count = 1,
                  uint32_t base_instance = 0) const;

    private:
        mutable index_type _elem_type = index_type::u32;
        vertex_array _vertex_array;
    };

    class compute_pipeline : public pipeline
    {
    public:
        explicit compute_pipeline(shader* shader);
        void dispatch(uint32_t count_x, uint32_t count_y = 1, uint32_t count_z = 1);
        const std::array<int, 3>& work_group_sizes() const { return _group_sizes; }
        template<typename T>
        uniform<T> get_uniform(const char* name);

    private:
        std::array<int, 3> _group_sizes;
    };
}

#include "pipeline.inl"