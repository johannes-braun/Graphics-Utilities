#pragma once

#include "shader.hpp"
#include "uniform.hpp"

#include <jpu/memory>
#include <jpu/log>
#include <array>
#include <map>
#include <set>

namespace gl
{
    class pipeline : public jpu::ref_count
    {
    public:
        pipeline();
        virtual ~pipeline();

        operator unsigned() const;

        void bind() const;
        int location(std::string_view name) const;
        shader* stage(shader_type s) const;
        void reload_stages(bool force = false) const;
        
        void set_stages_enabled(const std::vector<shader_type>& stages, bool enable);

    protected:
        void set_stage_enabled(shader_type stage, bool enable);
        void use_shader(shader* s);
        static uint32_t stage_bits(shader_type t);
        void validate() const;

        uint32_t _id;
        std::map<shader_type, jpu::ref_ptr<shader>> _shaders;
        std::set<shader_type> _disabled_stages;
    };

    class graphics_pipeline : public pipeline
    {
    public:
        template<typename... TShaders>
        void use_stages(TShaders ... shd);
    };

    class compute_pipeline : public pipeline
    {
    public:
        explicit compute_pipeline(shader* shader);
        void dispatch(uint32_t count_x, uint32_t count_y = 1, uint32_t count_z = 1);
        const std::array<int, 3>& work_group_sizes() const { return _group_sizes; }

    private:
        std::array<int, 3> _group_sizes;
    };
}

#include "pipeline.inl"