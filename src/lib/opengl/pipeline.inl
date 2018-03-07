#pragma once

namespace gl
{
    template<typename... TShaders>
    void graphics_pipeline::use_stages(TShaders ... shd)
    {
        _shaders.clear();
        const std::initializer_list<shader*> list{ shd... };
        for (auto s : list)
            use_shader(s);

        glValidateProgramPipeline(_id);

        if (int success = 0; glGetProgramPipelineiv(_id, GL_VALIDATE_STATUS, &success), !success)
        {
            int log_length;
            glGetProgramPipelineiv(_id, GL_INFO_LOG_LENGTH, &log_length);
            std::string log(log_length, ' ');
            glGetProgramPipelineInfoLog(_id, log_length, &log_length, log.data());
            glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, static_cast<GLenum>(_id), GL_DEBUG_SEVERITY_HIGH, -1, log.c_str());

            throw std::runtime_error("Program pipeline validation failed: " + log);
        }
    }

    template <typename T>
    uniform<T> graphics_pipeline::get_uniform(const shader_type s, const char* name)
    {
        return stage(s)->get_uniform<T>(name);
    }

    template <typename T>
    uniform<T> compute_pipeline::get_uniform(const char* name)
    {
        return stage(shader_type::compute)->get_uniform<T>(name);
    }
}
