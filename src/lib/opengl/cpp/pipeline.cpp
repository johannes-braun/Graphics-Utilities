#include "../pipeline.hpp"

namespace gl
{
    namespace v2
    {
        GLbitfield stage_bitfield(GLenum st) noexcept
        {
            switch (st)
            {
            case GL_VERTEX_SHADER:
                return GL_VERTEX_SHADER_BIT;
            case GL_FRAGMENT_SHADER:
                return GL_FRAGMENT_SHADER_BIT;
            case GL_GEOMETRY_SHADER:
                return GL_GEOMETRY_SHADER_BIT;
            case GL_TESS_CONTROL_SHADER:
                return GL_TESS_CONTROL_SHADER_BIT;
            case GL_TESS_EVALUATION_SHADER:
                return GL_TESS_EVALUATION_SHADER_BIT;
            case GL_COMPUTE_SHADER:
                return GL_COMPUTE_SHADER_BIT;
            default:
                return GL_ZERO;
            }
        }

        stage::stage(pipeline* pipeline, GLenum type)
            : _pipeline(pipeline), _type(type)
        {
        }

        stage& stage::operator=(std::shared_ptr<shader> shader)
        {
            assert(shader->type() == _type && "Incompatible shader type.");
            _shader = shader;
            glUseProgramStages(*_pipeline, stage_bitfield(_type), *_shader);
            glValidateProgramPipeline(*_pipeline);
            return *this;
        }

        stage& stage::operator=(nullptr_t)
        {
            glUseProgramStages(*_pipeline, stage_bitfield(_type), gl_shader_program_t::zero);
            glValidateProgramPipeline(*_pipeline);
            return *this;
        }

        stage::operator const std::shared_ptr<shader>&() const
        {
            return _shader;
        }

        shader* stage::operator->() const
        {
            return _shader.get();
        }

        shader* stage::get() const
        {
            return _shader.get();
        }

        stage::operator bool() const noexcept
        {
            return _shader != nullptr;
        }

        pipeline::pipeline() noexcept 
        {
            glCreateProgramPipelines(1, &_id);
        }

        pipeline::pipeline(const pipeline& other) noexcept
        {
            operator=(std::forward<const pipeline&>(other));
        }

        pipeline::pipeline(pipeline&& other) noexcept
        {
            operator=(std::forward<pipeline&&>(other));
        }

        pipeline& pipeline::operator=(const pipeline& other) noexcept
        {
            if (_id != gl_program_pipeline_t::zero)
                glDeleteProgramPipelines(1, &_id);
            if (other._id == gl_program_pipeline_t::zero)
            {
                _id = gl_program_pipeline_t::zero;
                return *this;
            }
            else glCreateProgramPipelines(1, &_id);
            
            for (auto&& pair : other._stages)
            {
                _stages.emplace(pair.first, stage(this, pair.first));
                _stages.at(pair.first) = static_cast<std::shared_ptr<shader>>(pair.second);
            }
            return *this;
        }

        pipeline& pipeline::operator=(pipeline&& other) noexcept
        {
            if (_id != gl_program_pipeline_t::zero)
                glDeleteProgramPipelines(1, &_id);
            _id = other._id;
            _stages = std::move(other._stages);
            other._id = gl_program_pipeline_t::zero;
            return *this;
        }

        pipeline::~pipeline() noexcept
        {
            if (_id != gl_program_pipeline_t::zero)
                glDeleteProgramPipelines(1, &_id);
        }

        stage& pipeline::at(GLenum stage)
        {
            if (_stages.count(stage) == 0)
                _stages.emplace(stage, gl::v2::stage(this, stage));
            return _stages.at(stage);
        }

        const std::shared_ptr<shader>& pipeline::at(GLenum stage) const
        {
            return _stages.at(stage);
        }

        stage& pipeline::operator[](GLenum stage)
        {
            return at(stage);
        }

        const std::shared_ptr<shader>& pipeline::operator[](GLenum stage) const
        {
            return at(stage);
        }

        void pipeline::bind() const noexcept
        {
            glBindProgramPipeline(_id);
        }

        pipeline::operator gl_program_pipeline_t() const noexcept
        {
            return _id;
        }

        void pipeline::reload() const noexcept
        {
            for (auto&& stage : _stages)
            {
                if (stage.second)
                {
                    stage.second->reload();
                    glUseProgramStages(_id, stage_bitfield(stage.first), *(stage.second.get()));
                }
            }
        }
    }






















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
