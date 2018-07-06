#define GFX_EXPOSE_APIS
#include <gfx/gfx.hpp>
#include <gfx/graphics/opengl/state_info.hpp>

#include <opengl/opengl.hpp>

int main()
{
    // --- Context ---
    gfx::context_options opt;
    opt.window_title        = "[07] Meshing";
    opt.debug               = true;
    opt.framebuffer_samples = 8;
    auto context            = gfx::context::create(opt);
    context->make_current();

    // --- Simple triangle fan ---
    gfx::device_buffer<gfx::vertex3d> graph_plane(gfx::buffer_usage::vertex, {{{0, 0, 0}}, {{1, 0, 0}}, {{0, 1, 0}}, {{1, 1, 0}}});
    gfx::vertex_input                 plane_vao;
    plane_vao.set_assembly(gfx::topology::triangle_strip);
    plane_vao.add_attribute(0, gfx::format::rgb32f, offsetof(gfx::vertex3d, position));
    plane_vao.set_binding_info(0, sizeof(gfx::vertex3d), gfx::input_rate::vertex);

    gfx::device_image grid_image(gfx::host_image(gfx::rgb8unorm, "grid.jpg"));
    gfx::image_view   grid_view(gfx::imgv_type::image_2d, grid_image.pixel_format(), grid_image, 0, grid_image.levels(), 0, 1);
    gfx::sampler      sampler;

    gl::pipeline grid;
    grid[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("07_meshing/grid.vert");
    grid[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("07_meshing/grid.frag");

    struct camera_data
    {
        glm::mat4 view;
        glm::mat4 proj;
    };
    gfx::host_buffer<camera_data> camera_buffer(1);
    gfx::camera                   camera;
    gfx::camera_controller        controller;

    gfx::state_info grid_state;
    grid_state.rasterizer.cull                   = gfx::cull_mode::none;
    grid_state.multisample.sample_shading_enable = true;
    grid_state.multisample.samples               = gfx::sample_count::x8;

    gfx::device_image color_attachment(gfx::img_type::image2d, gfx::rgba16f, {1280, 720, 1}, gfx::sample_count::x8);
    gfx::device_image depth_attachment(gfx::img_type::image2d, gfx::d32f, {1280, 720, 1}, gfx::sample_count::x8);
    gfx::device_image resolve_attachment(gfx::img_type::image2d, gfx::rgba16f, {1280, 720, 1}, 1);

    gl::framebuffer render_fbo;
    glNamedFramebufferTexture(render_fbo, GL_COLOR_ATTACHMENT0, color_attachment, 0);
    glNamedFramebufferTexture(render_fbo, GL_DEPTH_ATTACHMENT, depth_attachment, 0);

    gl::framebuffer resolve_fbo;
    glNamedFramebufferTexture(resolve_fbo, GL_COLOR_ATTACHMENT0, resolve_attachment, 0);

    while(context->run())
    {
        render_fbo.set_drawbuffer(GL_COLOR_ATTACHMENT0);
        render_fbo.bind();
        render_fbo.clear(0, {0.1f, 0.1f, 0.1f, 1.f});
        render_fbo.clear(0.f, 0);

        controller.update(camera);
        camera_buffer[0].view = glm::inverse(camera.transform_mode.matrix());
        camera_buffer[0].proj = camera.projection_mode.matrix();

        gfx::apply(grid_state);
        plane_vao.bind_vertex_buffer(0, graph_plane, 0);
        glBindTextureUnit(0, grid_view);
        glBindSampler(0, sampler);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, camera_buffer);
        grid.bind();
        plane_vao.draw(4);

        // RESOLVE
        render_fbo.set_readbuffer(GL_COLOR_ATTACHMENT0);
        resolve_fbo.set_drawbuffer(GL_COLOR_ATTACHMENT0);
        render_fbo.blit(resolve_fbo, 0, 0, 1280, 720, 0, 0, 1280, 720, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        resolve_fbo.set_readbuffer(GL_COLOR_ATTACHMENT0);
        render_fbo.set_drawbuffer(GL_COLOR_ATTACHMENT0);

        resolve_fbo.blit(gl::framebuffer::zero(), 0, 0, 1280, 720, 0, 0, 1280, 720, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
}