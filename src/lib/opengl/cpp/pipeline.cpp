#include "../pipeline.hpp"

namespace gl
{
    pipeline::pipeline() noexcept
    {
        glCreateProgramPipelines(1, &_id);
    }

    pipeline::~pipeline() noexcept
    {
        glDeleteProgramPipelines(1, &_id);
    }

    pipeline::operator gl_program_pipeline_t() const noexcept
    {
        return _id;
    }

    shader* pipeline::stage(const GLenum s) const
    {
        return _shaders.count(s) != 0 ? _shaders.at(s).get() : nullptr;
    }

    void pipeline::bind() const noexcept
    {
        glBindProgramPipeline(_id);
    }

    void pipeline::reload_stages(const bool force) const
    {
        for (auto&& shader : _shaders)
        {
            shader.second->reload();

            glUseProgramStages(_id, stage_bits(shader.second->type()), *shader.second);
        }

        validate();
    }

    void pipeline::set_stages_enabled(const std::vector<GLenum>& stages, const bool enable)
    {
        for (auto stage : stages)
            set_stage_enabled(stage, enable);

        validate();
    }

    void pipeline::set_stage_enabled(GLenum stage, const bool enable)
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
            glUseProgramStages(_id, stage_bits(stage), gl_shader_program_t(0));
        }
    }

    void pipeline::use_shader(std::shared_ptr<shader> s)
    {
        glUseProgramStages(_id, stage_bits(s->type()), *s);
        _shaders[s->type()] = s;
    }

    GLbitfield pipeline::stage_bits(const GLenum t) noexcept
    {
        GLbitfield stage_bits = GL_ZERO;
        switch (t)
        {
        case GL_VERTEX_SHADER:
            stage_bits = GL_VERTEX_SHADER_BIT;
            break;
        case GL_FRAGMENT_SHADER:
            stage_bits = GL_FRAGMENT_SHADER_BIT;
            break;
        case GL_GEOMETRY_SHADER:
            stage_bits = GL_GEOMETRY_SHADER_BIT;
            break;
        case GL_TESS_CONTROL_SHADER:
            stage_bits = GL_TESS_CONTROL_SHADER_BIT;
            break;
        case GL_TESS_EVALUATION_SHADER:
            stage_bits = GL_TESS_EVALUATION_SHADER_BIT;
            break;
        case GL_COMPUTE_SHADER:
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
            glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, static_cast<uint32_t>(_id), GL_DEBUG_SEVERITY_HIGH, -1, log.c_str());

            throw std::runtime_error("Program pipeline validation failed: " + log);
        }
    }

    void graphics_pipeline::set_input_format(const uint32_t attribute, const int components, const GLenum type, const bool normalized) const noexcept
    {
        _vertex_array.set_format(attribute, components, type, normalized);
    }

    void graphics_pipeline::disable_input(const uint32_t attribute) const noexcept
    {
        glDisableVertexArrayAttrib(_vertex_array, attribute);
    }

    void graphics_pipeline::set_input_buffer(const uint32_t attribute, gl_buffer_t buffer, const size_t stride, const size_t offset) const noexcept
    {
        _vertex_array.set_vertex_buffer(attribute, buffer, stride, offset);
    }

    void graphics_pipeline::set_index_buffer(gl_buffer_t buffer, const index_type elem_type) const noexcept
    {
        _elem_type = elem_type;
        _vertex_array.set_element_buffer(buffer);
    }

    void graphics_pipeline::draw_indexed(const primitive p, const size_t elem_count, const size_t base_index, const uint32_t base_vertex,
        const uint32_t instance_count, const uint32_t base_instance) const noexcept
    {
        _vertex_array.bind();
        glDrawElementsInstancedBaseVertexBaseInstance(static_cast<GLenum>(p), static_cast<uint32_t>(elem_count),
            static_cast<GLenum>(_elem_type),
            reinterpret_cast<const void*>(base_index * std::max(
                static_cast<uint32_t>(_elem_type) - GL_UNSIGNED_BYTE, 1u)),
            static_cast<int>(instance_count), static_cast<int>(base_vertex), base_instance);
    }

    void graphics_pipeline::draw(const primitive p, const size_t vertex_count, const size_t first, const uint32_t instance_count,
        const uint32_t base_instance) const noexcept
    {
        _vertex_array.bind();
        glDrawArraysInstancedBaseInstance(static_cast<GLenum>(p), static_cast<int>(first), static_cast<int>(vertex_count), static_cast<int>(instance_count), base_instance);
    }

    compute_pipeline::compute_pipeline(std::shared_ptr<shader> shader) : pipeline()
    {
        use_shader(shader);

        validate();

        glGetProgramiv(*shader, GL_COMPUTE_WORK_GROUP_SIZE, _group_sizes.data());
        if (_group_sizes[0] * _group_sizes[1] * _group_sizes[2] > 32)
            glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PERFORMANCE, static_cast<uint32_t>(_id), GL_DEBUG_SEVERITY_LOW, -1,
                "Group sizes result in a group invocation count higher than the pre-defined wave size. This might lead to a slower dispatch. Consider using smaller group sizes.");
    }

    void compute_pipeline::dispatch(const uint32_t count_x, const uint32_t count_y, const uint32_t count_z) noexcept
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
