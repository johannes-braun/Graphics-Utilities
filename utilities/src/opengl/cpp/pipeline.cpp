#include "../pipeline.hpp"

namespace gl
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
        glEnableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
        glEnableClientState(GL_ELEMENT_ARRAY_UNIFIED_NV);
        glEnableClientState(GL_UNIFORM_BUFFER_UNIFIED_NV);
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
            _stages.emplace(stage, gl::stage(this, stage));
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
        glBindVertexArray(gl_vertex_array_t::zero);
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

    void pipeline::draw(GLenum primitive, size_t count, size_t first) const noexcept
    {
        glDrawArrays(primitive, int(first), int(count));
    }

    void pipeline::draw_instanced(GLenum primitive, size_t count, size_t instances, size_t first_vertex, size_t first_instance) const noexcept
    {
        glDrawArraysInstancedBaseInstance(primitive, first_vertex, count, instances, first_instance);
    }

    compute_pipeline::compute_pipeline(const std::shared_ptr<shader>& shader) noexcept
        : _shader(shader)
    {
        glCreateProgramPipelines(1, &_id);
        glUseProgramStages(_id, GL_COMPUTE_SHADER_BIT, *_shader);
        glGetProgramiv(*shader, GL_COMPUTE_WORK_GROUP_SIZE, &_group_sizes[0]);
    }

    compute_pipeline::compute_pipeline(const compute_pipeline& other) noexcept
    {
        operator=(std::forward<const compute_pipeline&>(other));
    }

    compute_pipeline::compute_pipeline(compute_pipeline&& other) noexcept
    {
        operator=(std::forward<compute_pipeline&&>(other));
    }

    compute_pipeline& compute_pipeline::operator=(const compute_pipeline& other) noexcept
    {
        if (_id != gl_program_pipeline_t::zero)
            glDeleteProgramPipelines(1, &_id);
        if (other._id == gl_program_pipeline_t::zero)
        {
            _id = gl_program_pipeline_t::zero;
            return *this;
        }
        else glCreateProgramPipelines(1, &_id);
        _group_sizes = other._group_sizes;
        _shader = other._shader;
        return *this;
    }

    compute_pipeline& compute_pipeline::operator=(compute_pipeline&& other) noexcept
    {
        if (_id != gl_program_pipeline_t::zero)
            glDeleteProgramPipelines(1, &_id);
        _id = other._id;
        _group_sizes = std::move(other._group_sizes);
        _shader = std::move(other._shader);
        other._id = gl_program_pipeline_t::zero;
        return *this;
    }

    compute_pipeline::~compute_pipeline() noexcept
    {
        if (_id != gl_program_pipeline_t::zero)
            glDeleteProgramPipelines(1, &_id);
    }

    void compute_pipeline::dispatch(uint32_t count_x, uint32_t count_y, uint32_t count_z) noexcept
    {
        const static auto invocation_count = [](uint32_t global, uint32_t local) {
            return (global + local - 1) / local;
        };

        glBindProgramPipeline(_id);
        glDispatchCompute(
            invocation_count(count_x, _group_sizes[0]),
            invocation_count(count_y, _group_sizes[1]),
            invocation_count(count_z, _group_sizes[2]));
    }

    compute_pipeline::operator gl_program_pipeline_t() const noexcept
    {
        return _id;
    }

    const glm::ivec3& compute_pipeline::group_sizes() const noexcept
    {
        return _group_sizes;
    }

    const std::shared_ptr<shader>& compute_pipeline::stage() const noexcept
    {
        return _shader;
    }

    shader* compute_pipeline::operator->() const noexcept
    {
        return _shader.get();
    }

    void compute_pipeline::reload() const noexcept
    {
        _shader->reload();
        glUseProgramStages(_id, GL_COMPUTE_SHADER_BIT, *_shader);
    }
}
