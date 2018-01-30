#pragma once
#include <array>
#include "shader.hpp"
#include "glad/glad.h"
#include <jpu/memory>
#include <map>
#include "uniform.hpp"

namespace gl
{
    class pipeline : public jpu::ref_count
    {
    public:
        pipeline();
        virtual ~pipeline();

        operator unsigned() const { return _id; }

        void bind() const;
        int location(std::string_view name) const { return glGetUniformLocation(_id, name.data()); }
        shader* stage(shader_type s) const { return _shaders.count(s) != 0 ? _shaders.at(s).get() : nullptr; }
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
        void use_stages(TShaders ... shd)
        {
            _shaders.clear();
            for (auto s : std::initializer_list<shader*>{ shd... })
                use_shader(s);

            glValidateProgramPipeline(_id);

            if (int success = 0; glGetProgramPipelineiv(_id, GL_VALIDATE_STATUS, &success), !success)
            {
                int log_length;
                glGetProgramPipelineiv(_id, GL_INFO_LOG_LENGTH, &log_length);
                std::string log(log_length, ' ');
                glGetProgramPipelineInfoLog(_id, log_length, &log_length, log.data());
                glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, _id, GL_DEBUG_SEVERITY_HIGH, -1, log.c_str());

                throw std::runtime_error("Program pipeline validation failed: " + log);
            }
        }
    };

    class compute_pipeline : public pipeline
    {
    public:
        explicit compute_pipeline(shader* shader);

        void dispatch(const uint32_t count_x, const uint32_t count_y = 1, const uint32_t count_z = 1);

    private:
        std::array<int, 3> _group_sizes;
    };
}
