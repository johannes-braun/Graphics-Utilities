#pragma once

#include "shader.hpp"
#include "uniform.hpp"
#include "buffer.hpp"

#include <array>
#include <map>
#include <set>

#include <type_traits>

namespace gl
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

        static void reload_all();
        void reload() const noexcept;
        void bind() const noexcept;
        operator mygl::pipeline() const noexcept;

        template<typename T>
        void bind_attribute(uint32_t index, const buffer<T>& buffer, int components, GLenum type, size_t offset = 0, size_t stride = sizeof(T)) const noexcept;

        template<typename T>
        void bind_attribute(uint32_t index, const buffer<T>& buffer, int components, GLenum type, bool normalized, size_t offset = 0, size_t stride = sizeof(T)) const noexcept;

        template<typename T>
        void bind_attribute(uint32_t index, uint32_t divisor, const buffer<T>& buffer, int components, GLenum type, size_t offset = 0, size_t stride = sizeof(T)) const noexcept;

        template<typename T>
        void bind_attribute(uint32_t index, uint32_t divisor, const buffer<T>& buffer, int components, GLenum type, bool normalized, size_t offset = 0, size_t stride = sizeof(T)) const noexcept;

        template<typename T>
        void bind_uniform_buffer(uint32_t index, const buffer<T>& buffer, size_t offset = 0, size_t size = ~0) const noexcept;

        void draw(GLenum primitive, size_t count, size_t first = 0) const noexcept;
        void draw_instanced(GLenum primitive, size_t count, size_t instances, size_t first_vertex = 0, size_t first_instance = 0) const noexcept;

        template<typename T>
        void draw(GLenum primitive, const buffer<T>& index_buffer, GLenum type, size_t count = ~0ull, size_t first_index = 0, size_t first_vertex = 0) const noexcept;

    private:
        static std::unordered_set<pipeline*> _all_pipelines;
        static std::mutex _all_pipelines_mutex;

        std::map<GLenum, stage> _stages;
        mygl::pipeline _id;
    };

    class compute_pipeline
    {
    public:
        compute_pipeline(const std::shared_ptr<shader>& shader) noexcept;
        compute_pipeline(const compute_pipeline& other) noexcept;
        compute_pipeline(compute_pipeline&& other) noexcept;
        compute_pipeline& operator=(const compute_pipeline& other) noexcept;
        compute_pipeline& operator=(compute_pipeline&& other) noexcept;
        ~compute_pipeline() noexcept;

        void dispatch(uint32_t count_x, uint32_t count_y = 1, uint32_t count_z = 1) noexcept;
        operator mygl::pipeline() const noexcept;
        const glm::ivec3& group_sizes() const noexcept;
        void reload() const noexcept;

        const std::shared_ptr<shader>& stage() const noexcept;
        shader* operator->() const noexcept;

        template<typename T>
        void bind_uniform_buffer(uint32_t index, const buffer<T>& buffer, size_t offset = 0, size_t size = ~0) const noexcept;

    private:
        glm::ivec3 _group_sizes;
        mygl::pipeline _id;
        std::shared_ptr<shader> _shader;
    };
}

#include "impl/pipeline.inl"