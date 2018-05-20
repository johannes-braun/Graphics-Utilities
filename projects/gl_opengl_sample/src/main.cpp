#include <GLFW/glfw3.h>
#include <array>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#define STB_IMAGE_IMPLEMENTATION
#include <mygl/mygl.hpp>
#include <stb_image.h>

GLFWwindow* window;

struct vertex
{
    glm::vec4 position;
    glm::vec2 uv;
};
using index = uint8_t;

const std::array<vertex, 4> quad_vertices{vertex{{-0.5f, -0.5f, 0.f, 1.f}, {0.f, 0.f}},
                                          vertex{{0.5f, -0.5f, 0.f, 1.f}, {1.f, 0.f}},
                                          vertex{{0.5f, 0.5f, 0.f, 1.f}, {1.f, 1.f}},
                                          vertex{{-0.5f, 0.5f, 0.f, 1.f}, {0.f, 1.f}}};
const std::array<index, 6>  quad_indices{0, 1, 2, 0, 2, 3};

template<auto fun>
struct A
{
    void f() { fun(); }
};

void x()
{
    
}
void y() { A<&x>().f(); }

struct matrices
{
    alignas(16 /*bytes*/) uint64_t object_texture_handle;
    alignas(16 /*bytes*/) glm::mat4 view_projection;
    alignas(16 /*bytes*/) glm::mat4 model;
};

// Special construct for stbi resource management (RAII)
struct stbi_freer
{
    void operator()(uint8_t* d) { free(d); }
};
using stbi_data = std::unique_ptr<uint8_t, stbi_freer>;

// Define framebuffer targets beforehand to have global variables.
// Usually this is kind of a bad way but enough here.
mygl::texture     msaa_color_attachment;
mygl::texture     msaa_depth_attachment;
mygl::framebuffer framebuffer;

constexpr const char* shader_header = R"glsl(
#version 450 core
#extension GL_ARB_separate_shader_objects : require
#extension GL_ARB_bindless_texture : require
layout(bindless_sampler) uniform;
layout(bindless_image) uniform;
)glsl";

constexpr const char* vertex_shader_code = R"glsl( #line 1 "my_shader.vert"
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 uv;

layout(binding = 0) uniform ObjectData 
{
    sampler2D object_texture;
    mat4 view_projection;
    mat4 model;
};

out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) out vec4 out_position;
layout(location = 1) out vec2 out_uv;

void main()
{
    out_position = model * position;
    out_uv = uv;
    gl_Position = view_projection * out_position;
}
)glsl";

constexpr const char* fragment_shader_code = R"glsl( #line 1 "my_shader.frag"
// points to the same location as the vertex shader output.
layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec2 vertex_uv;

layout(binding = 0) uniform ObjectData 
{
    sampler2D object_texture;
    mat4 view_projection;
    mat4 model;
};

layout(location = 0) out vec4 final_color;

void main()
{
    final_color = texture(object_texture, vertex_uv);
}
)glsl";

int main(int argc, const char** args)
{
    // Create Window
    glfwInit();
    glfwSetErrorCallback([](int, const char* message) { std::cout << message << '\n'; });
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(1280, 720, "A Triangle", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Load OpenGL functions from system processes
    mygl::load();

    // Add debug callback
    glDebugMessageCallback([](GLenum       source,
                              GLenum       type,
                              unsigned int id,
                              GLenum       severity,
                              int          length,
                              const char*  message,
                              const void*  userParam) { std::cout << message << '\n'; },
                           nullptr);

    // Create your buffers
    constexpr int vertex_buffer_index = 0;
    constexpr int index_buffer_index  = 1;
    mygl::buffer  buffers[2];
    glCreateBuffers(2, buffers);

    // Allocate Buffer Memory and assign data
    const ptrdiff_t vertex_buffer_size = quad_vertices.size() * sizeof(vertex);
    const ptrdiff_t index_buffer_size  = quad_indices.size() * sizeof(index);
    glNamedBufferStorage(
            buffers[vertex_buffer_index], vertex_buffer_size, quad_vertices.data(), GL_NONE);
    glNamedBufferStorage(
            buffers[index_buffer_index], index_buffer_size, quad_indices.data(), GL_NONE);

    // Create Vertex Array to reference data as attributes
    constexpr int attribute_position    = 0;
    constexpr int attribute_uv          = 1;
    constexpr int vertex_buffer_binding = 0;

    mygl::vertex_array vertex_array;
    glCreateVertexArrays(1, &vertex_array);
    // A position is an unnormalized 4D float vector starting at a specific offset in the struct.
    glEnableVertexArrayAttrib(vertex_array, attribute_position);
    glVertexArrayAttribFormat(vertex_array,
                              attribute_position,
                              glm::vec4::length(),
                              GL_FLOAT,
                              false,
                              offsetof(vertex, position));
    glVertexArrayAttribBinding(vertex_array, attribute_position, vertex_buffer_binding);
    // A uv coordinate is also an unnormalized float object, but it's a 2D vector.
    glEnableVertexArrayAttrib(vertex_array, attribute_uv);
    glVertexArrayAttribFormat(
            vertex_array, attribute_uv, glm::vec2::length(), GL_FLOAT, false, offsetof(vertex, uv));
    glVertexArrayAttribBinding(vertex_array, attribute_uv, vertex_buffer_binding);
    // Assign (or bind) our buffers to the vertex array.
    glVertexArrayVertexBuffer(
            vertex_array, vertex_buffer_binding, buffers[vertex_buffer_index], 0, sizeof(vertex));
    glVertexArrayElementBuffer(vertex_array, buffers[index_buffer_index]);

    // Create shaders
    constexpr const char* vertex_shader_sources[2] = {shader_header, vertex_shader_code};
    mygl::shader_program  vertex_shader =
            glCreateShaderProgramv(GL_VERTEX_SHADER, 2, vertex_shader_sources);
    constexpr const char* fragment_shader_sources[2] = {shader_header, fragment_shader_code};
    mygl::shader_program  fragment_shader =
            glCreateShaderProgramv(GL_FRAGMENT_SHADER, 2, fragment_shader_sources);

    const auto check_shader = [](mygl::shader_program program) {
        if(int status; glGetProgramiv(program, GL_LINK_STATUS, &status), !status)
        {
            std::string log;
            int         log_length;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
            log.resize(log_length);
            glGetProgramInfoLog(program, log_length, &log_length, log.data());
            std::cerr << log << '\n';
            throw std::runtime_error("Shader compilation failed.");
        }
    };
    check_shader(vertex_shader);
    check_shader(fragment_shader);

    // Create a pipeline for our shaders to run in
    mygl::pipeline pipeline;
    glCreateProgramPipelines(1, &pipeline);
    glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vertex_shader);
    glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fragment_shader);

    // Add a "host-visible" (and mapped) uniform buffer for the mvp matrix
    mygl::buffer matrix_buffer;
    glCreateBuffers(1, &matrix_buffer);
    // Make the buffer dynamic, so we can assign data to it after allocation, enable writing when
    // mapped and leave the buffer data mapped if done so.
    glNamedBufferStorage(matrix_buffer,
                         sizeof(matrices),
                         nullptr,
                         GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
    matrices* mapped_matrix_buffer =
            static_cast<matrices*>(glMapNamedBuffer(matrix_buffer, GL_WRITE_ONLY));
    mapped_matrix_buffer->model           = glm::mat4(1.f);
    mapped_matrix_buffer->view_projection = glm::mat4(1.f);

    // Just because we can, we put a texture onto the quad
    mygl::texture texture;
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    int       texture_width, texture_height;
    stbi_data texture_data(stbi_load("../res/brick.png",
                                     &texture_width,
                                     &texture_height,
                                     nullptr,
                                     STBI_rgb_alpha)); // put in your own path.
    glTextureStorage2D(texture,
                       int(1 + std::floor(std::log2(std::max(texture_width, texture_height)))),
                       GL_RGBA8,
                       texture_width,
                       texture_height);
    glTextureSubImage2D(texture,
                        0,
                        0,
                        0,
                        texture_width,
                        texture_height,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE,
                        texture_data.get());
    glGenerateTextureMipmap(texture);

    // A texture needs to be sampled. So add a sampler.
    mygl::sampler sampler;
    glCreateSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY, 16.f);
    mapped_matrix_buffer->object_texture_handle = glGetTextureSamplerHandleARB(texture, sampler);
    glMakeTextureHandleResidentARB(mapped_matrix_buffer->object_texture_handle);

    // Create framebuffer attachments and the framebuffer itself.
    glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &msaa_color_attachment);
    glTextureStorage2DMultisample(msaa_color_attachment, 8, GL_RGBA16F, 1280, 720, true);
    glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &msaa_depth_attachment);
    glTextureStorage2DMultisample(msaa_depth_attachment, 8, GL_DEPTH24_STENCIL8, 1280, 720, true);
    glCreateFramebuffers(1, &framebuffer);
    glNamedFramebufferTexture(framebuffer, GL_COLOR_ATTACHMENT0, msaa_color_attachment, 0);
    glNamedFramebufferTexture(framebuffer, GL_DEPTH_STENCIL_ATTACHMENT, msaa_depth_attachment, 0);

    // draw to and read from color attachments. The depth buffer is used implicitly
    glNamedFramebufferDrawBuffer(framebuffer, GL_COLOR_ATTACHMENT0);
    glNamedFramebufferReadBuffer(framebuffer, GL_COLOR_ATTACHMENT0);

    // In order to resize the window, we have to register a glfw callback
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, const int x, const int y) {
        glDeleteFramebuffers(1, &framebuffer);
        glDeleteTextures(1, &msaa_color_attachment);
        glDeleteTextures(1, &msaa_depth_attachment);

        glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &msaa_color_attachment);
        glTextureStorage2DMultisample(msaa_color_attachment, 8, GL_RGBA16F, x, y, true);

        glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &msaa_depth_attachment);
        glTextureStorage2DMultisample(msaa_depth_attachment, 8, GL_DEPTH24_STENCIL8, x, y, true);

        glCreateFramebuffers(1, &framebuffer);
        glNamedFramebufferTexture(framebuffer, GL_COLOR_ATTACHMENT0, msaa_color_attachment, 0);
        glNamedFramebufferTexture(
                framebuffer, GL_DEPTH_STENCIL_ATTACHMENT, msaa_depth_attachment, 0);
        glViewportIndexedf(0, 0, 0, float(x), float(y));
    });

    const glm::vec4 clear_color(0.6f, 0.8f, 0.9f, 1.f);
    float           last_time  = float(glfwGetTime());
    float           delta_time = 0.f;
    while(!glfwWindowShouldClose(window))
    {
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        delta_time = float(glfwGetTime() - last_time);
        last_time  = float(glfwGetTime());

        // Animate the quad (assign to mapped buffer, therefore automatically synchronized)
        mapped_matrix_buffer->model *=
                glm::rotate(glm::radians(80.f) * delta_time, glm::vec3(0, 0, 1));
        mapped_matrix_buffer->view_projection =
                glm::perspectiveFov(glm::radians(60.f),
                                    static_cast<float>(w),
                                    static_cast<float>(h),
                                    0.01f,
                                    100.f) * // projection
                glm::lookAt(glm::vec3(0, 0, 4.f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); // view

        // Draw the quad into the framebuffer.
        // This is the only place you find any glBindXYZ commands in this example.
        glClearNamedFramebufferfv(framebuffer, GL_COLOR, 0, glm::value_ptr(clear_color));
        glClearNamedFramebufferfi(framebuffer, GL_DEPTH_STENCIL, 0, 1.f, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glBindProgramPipeline(pipeline);
        glBindVertexArray(vertex_array);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, matrix_buffer);
        glDrawElements(GL_TRIANGLES,
                       int(quad_indices.size()),
                       /*uint8_t, therefore */ GL_UNSIGNED_BYTE,
                       nullptr);

        glBlitNamedFramebuffer(framebuffer,
                               mygl::framebuffer::zero,
                               0,
                               0,
                               w,
                               h,
                               0,
                               0,
                               w,
                               h,
                               GL_COLOR_BUFFER_BIT,
                               GL_NEAREST);

        // Either comment out or put your own framerate cap in.
        // Helps in simple applications to not make your GPU scream in pain from all those fps.
        double wait_start = glfwGetTime();
        while(glfwGetTime() - wait_start < 1.0 / 60.0)
            glfwPollEvents();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &msaa_color_attachment);
    glDeleteTextures(1, &msaa_depth_attachment);
    glDeleteTextures(1, &texture);
    glDeleteSamplers(1, &sampler);
    glDeleteProgram(fragment_shader);
    glDeleteProgram(vertex_shader);
    glDeleteProgramPipelines(1, &pipeline);
    glDeleteVertexArrays(1, &vertex_array);
    glDeleteBuffers(1, &matrix_buffer);
    glDeleteBuffers(2, buffers);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}