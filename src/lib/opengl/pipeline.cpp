#include "pipeline.hpp"
#include <glad/glad.h>

namespace gl
{
    pipeline::pipeline()
    {
        glCreateProgramPipelines(1, &_id);
    }

    pipeline::~pipeline()
    {
        glDeleteProgramPipelines(1, &_id);
    }

    pipeline::operator unsigned() const
    {
        return _id;
    }

    int pipeline::location(std::string_view name) const
    {
        return glGetUniformLocation(_id, name.data());
    }

    shader* pipeline::stage(const shader_type s) const
    {
        return _shaders.count(s) != 0 ? _shaders.at(s).get() : nullptr;
    }

    void pipeline::bind() const
    {
        glBindProgramPipeline(_id);
    }

    void pipeline::reload_stages(const bool force) const
    {
        for (auto&& shader : _shaders)
        {
            shader.second->reload(force);

            glUseProgramStages(_id, stage_bits(shader.second->type()), *shader.second);
        }

        validate();
    }

    void pipeline::set_stages_enabled(const std::vector<shader_type>& stages, const bool enable)
    {
        for (auto stage : stages)
            set_stage_enabled(stage, enable);

        validate();
    }

    void pipeline::set_stage_enabled(shader_type stage, const bool enable)
    {
        if (enable)
        {
            if (_disabled_stages.count(stage) == 0)
                return;

            _disabled_stages.erase(stage);
            auto&& s = _shaders.at(stage);
            glUseProgramStages(_id, stage_bits(s->type()), *s);
        }
        else
        {
            if (_disabled_stages.count(stage) != 0)
                return;

            _disabled_stages.emplace(stage);
            glUseProgramStages(_id, stage_bits(stage), 0);
        }
    }

    void pipeline::use_shader(shader* s)
    {
        glUseProgramStages(_id, stage_bits(s->type()), *s);
        s->inc_ref();
        _shaders[s->type()] = s;
    }

    uint32_t pipeline::stage_bits(const shader_type t)
    {
        uint32_t stage_bits = 0;
        switch (t)
        {
        case shader_type::vertex:
            stage_bits = GL_VERTEX_SHADER_BIT;
            break;
        case shader_type::fragment:
            stage_bits = GL_FRAGMENT_SHADER_BIT;
            break;
        case shader_type::geometry:
            stage_bits = GL_GEOMETRY_SHADER_BIT;
            break;
        case shader_type::tesselation_control:
            stage_bits = GL_TESS_CONTROL_SHADER_BIT;
            break;
        case shader_type::tesselation_evaluation:
            stage_bits = GL_TESS_EVALUATION_SHADER_BIT;
            break;
        case shader_type::compute:
            stage_bits = GL_COMPUTE_SHADER_BIT;
            break;
        }
        return stage_bits;
    }

    void pipeline::validate() const
    {
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

    void graphics_pipeline::set_input_format(const uint32_t attribute, const int components, const uint32_t type, const bool normalized) const
    {
        _vertex_array.set_format(attribute, components, type, normalized);
    }

    void graphics_pipeline::disable_input(const uint32_t attribute) const
    {
        glDisableVertexArrayAttrib(_vertex_array, attribute);
    }

    void graphics_pipeline::set_input_buffer(const uint32_t attribute, const buffer* buffer, const size_t stride, const size_t offset) const
    {
        _vertex_array.set_vertex_buffer(attribute, *buffer, stride, offset);
    }

    void graphics_pipeline::set_index_buffer(const buffer* buffer, const index_type elem_type) const
    {
        _elem_type = elem_type;
        _vertex_array.set_element_buffer(*buffer);
    }

    void graphics_pipeline::set_index_buffer(const buffer* buffer, index_type elem_type, const size_t stride, const size_t offset) const
    {
        _elem_type = elem_type;
        _vertex_array.set_element_buffer(*buffer, static_cast<uint32_t>(elem_type), stride, offset);
    }

    void graphics_pipeline::draw_indexed(const primitive p, const size_t elem_count, const size_t base_index, const uint32_t base_vertex,
        const uint32_t instance_count, const uint32_t base_instance) const
    {
        _vertex_array.bind();
        glDrawElementsInstancedBaseVertexBaseInstance(static_cast<uint32_t>(p), static_cast<uint32_t>(elem_count),
            static_cast<uint32_t>(_elem_type),
            reinterpret_cast<const void*>(base_index * std::max(
                static_cast<uint32_t>(_elem_type) - GL_UNSIGNED_BYTE, 1u)),
            static_cast<int>(instance_count), static_cast<int>(base_vertex), base_instance);
    }

    void graphics_pipeline::draw(const primitive p, const size_t vertex_count, const size_t first, const uint32_t instance_count,
        const uint32_t base_instance) const
    {
        _vertex_array.bind();
        glDrawArraysInstancedBaseInstance(static_cast<uint32_t>(p), static_cast<int>(first), static_cast<int>(vertex_count), static_cast<int>(instance_count), base_instance);
    }

    compute_pipeline::compute_pipeline(shader* shader) : pipeline()
    {
        use_shader(shader);

        validate();

        glGetProgramiv(*shader, GL_COMPUTE_WORK_GROUP_SIZE, _group_sizes.data());
        if (_group_sizes[0] * _group_sizes[1] * _group_sizes[2] > 32)
            glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PERFORMANCE, _id, GL_DEBUG_SEVERITY_LOW, -1,
                "Group sizes result in a group invocation count higher than the pre-defined wave size. This might lead to a slower dispatch. Consider using smaller group sizes.");
    }

    void compute_pipeline::dispatch(const uint32_t count_x, const uint32_t count_y, const uint32_t count_z)
    {
        const static auto invocation_count = [](uint32_t global, uint32_t local) {
            return (global + local - 1) / local;
        };

        glDispatchCompute(
            invocation_count(count_x, _group_sizes[0]),
            invocation_count(count_y, _group_sizes[1]),
            invocation_count(count_z, _group_sizes[2]));
    }
}
