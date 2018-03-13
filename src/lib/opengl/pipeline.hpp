#pragma once

#include "shader.hpp"
#include "uniform.hpp"
#include "vertex_array.hpp"
#include "buffer.hpp"

#include <jpu/memory.hpp>
#include <jpu/log.hpp>
#include <array>
#include <map>
#include <set>

#include <type_traits>
namespace gl
{
    namespace v2
    {
        class pipeline;
        class stage
        {
        public:
            friend pipeline;

            stage() = delete;
            stage(const stage&) = delete;
            stage(stage&&) = default;
            stage& operator=(const stage&) = delete;
            stage& operator=(stage&&) = delete;

            stage& operator=(std::shared_ptr<shader> shader);
            stage& operator=(nullptr_t);
            operator const std::shared_ptr<shader>&() const;
            shader* operator->() const;
            shader* get() const;
            operator bool() const noexcept;

        private:
            stage(pipeline* pipeline, GLenum bits);
            pipeline* _pipeline;
            GLenum _type;
            std::shared_ptr<shader> _shader;
        };

        class pipeline
        {
        public:
            pipeline() noexcept;
            pipeline(const pipeline& other) noexcept;
            pipeline(pipeline&& other) noexcept;
            pipeline& operator=(const pipeline& other) noexcept;
            pipeline& operator=(pipeline&& other) noexcept;
            ~pipeline() noexcept;

            stage& at(GLenum stage);
            const std::shared_ptr<shader>& at(GLenum stage) const;
            stage& operator[](GLenum stage);
            const std::shared_ptr<shader>& operator[](GLenum stage) const;

            void reload() const noexcept;
            void bind() const noexcept;
            operator gl_program_pipeline_t() const noexcept;

        private:
            std::map<GLenum, stage> _stages;
            gl_program_pipeline_t _id;
        };
    }


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
        pipeline() noexcept;
        virtual ~pipeline() noexcept;

        operator gl_program_pipeline_t() const noexcept;

        void bind() const noexcept;
        shader* stage(GLenum stype) const;
        void reload_stages(bool force = false) const;
        
        void set_stages_enabled(const std::vector<GLenum>& stages, bool enable);

    protected:
        void set_stage_enabled(GLenum stage, bool enable);
        void use_shader(std::shared_ptr<shader> s);
        static GLbitfield stage_bits(GLenum st) noexcept;
        void validate() const;

        gl_program_pipeline_t _id;
        std::map<GLenum, std::shared_ptr<shader>> _shaders;
        std::set<GLenum> _disabled_stages;
    };

    class graphics_pipeline : public pipeline
    {
        template<typename T, typename... As> struct are_same : std::conjunction<std::is_same<T, As>...> {};
    public:
        template<typename... TShaders>
        std::enable_if_t<are_same<std::shared_ptr<shader>, TShaders...>::value> use_stages(TShaders ... shd);
        template<typename T>
        uniform<T> get_uniform(GLenum stage, const char* name);

        void disable_input(uint32_t attribute) const noexcept;
        void set_input_format(uint32_t attribute, int components, GLenum type, bool normalized) const noexcept;
        void set_input_buffer(uint32_t attribute, gl_buffer_t buffer, size_t stride, size_t offset = 0) const noexcept;
        void set_index_buffer(gl_buffer_t buffer, index_type elem_type) const noexcept;

        void draw_indexed(primitive p, size_t elem_count, size_t base_index = 0, uint32_t base_vertex = 0,
                          uint32_t instance_count = 1, uint32_t base_instance = 0) const noexcept;
        void draw(primitive p, size_t vertex_count, size_t first = 0, uint32_t instance_count = 1,
                  uint32_t base_instance = 0) const noexcept;

    private:
        mutable index_type _elem_type = index_type::u32;
        vertex_array _vertex_array;
    };

    class compute_pipeline : public pipeline
    {
    public:
        explicit compute_pipeline(std::shared_ptr<shader> shader);
        void dispatch(uint32_t count_x, uint32_t count_y = 1, uint32_t count_z = 1) noexcept;
        const std::array<int, 3>& work_group_sizes() const noexcept { return _group_sizes; }
        template<typename T>
        uniform<T> get_uniform(const char* name);

    private:
        std::array<int, 3> _group_sizes;
    };
}

#include "impl/pipeline.inl"