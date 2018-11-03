#pragma once
#include <executable.hpp>
#include <gfx.legacy/gfx.hpp>
#include "shaders/shaders.hpp"

class graphics
{
public:
    graphics(executable& exe) : exe(exe)
    {
        default_sampler.set_wrap(gfx::wrap::u, gfx::wrap_mode::repeat);
        default_sampler.set_wrap(gfx::wrap::v, gfx::wrap_mode::repeat);
        shadow_pass_layout.set_depth_stencil_attachment(gfx::d32f);

        camera_bindings = gfx::binding_layout{};
        camera_bindings.push(gfx::binding_type::uniform_buffer);    // camera data
        shadow_bindings = gfx::binding_layout{};
        shadow_bindings
            .push(gfx::binding_type::sampled_image)      // shadow map
            .push(gfx::binding_type::uniform_buffer);    // shadow camera infos

		time_bindings = gfx::binding_layout{};
		time_bindings.push(gfx::binding_type::uniform_buffer);

        create_mesh_pipelines();
        create_sky_pipeline();
        create_terrain_pipelines();
    }

    void create_mesh_pipelines()
    {
        mesh_bindings = gfx::binding_layout{};
		mesh_style_bindings = gfx::binding_layout{};

        gfx::pipe_state::vertex_input mesh_input;
        mesh_input.attributes.emplace_back(0, 0, gfx::rgb32f, offsetof(gfx::vertex3d, position));
        mesh_input.attributes.emplace_back(1, 0, gfx::rg32f, offsetof(gfx::vertex3d, uv));
        mesh_input.attributes.emplace_back(2, 0, gfx::rgb32f, offsetof(gfx::vertex3d, normal));
        mesh_input.bindings.emplace_back(0, sizeof(gfx::vertex3d));
        mesh_bindings.push(gfx::binding_type::storage_buffer);
		mesh_style_bindings.push(gfx::binding_type::sampled_image); // overlay texture;
        gfx::pipe_state::binding_layouts layouts;
        layouts.layouts.push_back(&camera_bindings);
        layouts.layouts.push_back(&mesh_bindings);
        layouts.layouts.push_back(&mesh_style_bindings);
        layouts.layouts.push_back(&shadow_bindings);
        layouts.layouts.push_back(&time_bindings);
        gfx::pipe_state::depth_stencil depth_stencil;
        depth_stencil.depth_test_enable = true;
        gfx::pipe_state::rasterizer raster;
        raster.cull = gfx::cull_mode::back;
        gfx::pipe_state mesh_state;
        mesh_state.state_bindings      = &layouts;
        mesh_state.state_depth_stencil = &depth_stencil;
        mesh_state.state_multisample   = &exe.msaa_state;
        mesh_state.state_rasterizer    = &raster;
        mesh_state.state_vertex_input  = &mesh_input;
        const auto shaders             = {
            gfx::shader(gfx::shader_type::vert, gfx::spirv::proto::shaders::mesh_vert),
            gfx::shader(gfx::shader_type::frag, gfx::spirv::proto::shaders::mesh_frag),
        };
        mesh_render_pipeline = std::make_unique<gfx::graphics_pipeline>(mesh_state, exe.pass_layout, shaders);
        mesh_cull_pipeline   = std::make_unique<gfx::compute_pipeline>(layouts, gfx::shader(gfx::shader_type::comp, gfx::spirv::proto::shaders::cull_comp));

        raster.cull                       = gfx::cull_mode::none;
        mesh_state.state_multisample      = nullptr;
        raster.depth_bias_enable          = true;
        raster.depth_bias_constant_factor = -1.f;
        raster.depth_bias_slope_factor    = -1.f;
        const auto shadow_shaders         = {
            gfx::shader(gfx::shader_type::vert, gfx::spirv::proto::shaders::mesh_shadow_vert),
            gfx::shader(gfx::shader_type::frag, gfx::spirv::proto::shaders::mesh_shadow_frag),
        };
        mesh_shadow_pipeline = std::make_unique<gfx::graphics_pipeline>(mesh_state, shadow_pass_layout, shadow_shaders);
    }
    void create_sky_pipeline()
    {
        gfx::pipe_state::binding_layouts sky_binding_state;
        sky_binding_state.layouts.push_back(&camera_bindings);
		sky_binding_state.layouts.push_back(&time_bindings);
        gfx::pipe_state::depth_stencil sky_depth;
        sky_depth.depth_write_enable = false;
        sky_depth.depth_test_enable  = true;
        gfx::pipe_state sky_state;
        sky_state.state_bindings      = &sky_binding_state;
        sky_state.state_depth_stencil = &sky_depth;
        sky_state.state_multisample   = &exe.msaa_state;
        const auto shaders            = {gfx::shader(gfx::shader_type::vert, gfx::spirv::proto::shaders::skybox_vert),
                              gfx::shader(gfx::shader_type::frag, gfx::spirv::proto::shaders::skybox_frag)};
        sky_pipeline                  = std::make_unique<gfx::graphics_pipeline>(sky_state, exe.pass_layout, shaders);
    }
    void create_terrain_pipelines()
    {
        terrain_info_bindings   = gfx::binding_layout{};
        terrain_style_bindings  = gfx::binding_layout{};

        /*
        0) info: X
            0 - heightmap
            1 - TerrainInfo
        1) camera: X
            0 - camera
        2) style:
            0 - bump
            1 - color
        3) shadow:
            0 - map
            1 - cameras

		-------------------------------------------

		

         */
		
        terrain_info_bindings
            .push(gfx::binding_type::sampled_image)      // heightmap
            .push(gfx::binding_type::uniform_buffer);    // terrain info
        terrain_style_bindings
            .push(gfx::binding_type::sampled_image)     // bumpmap
            .push(gfx::binding_type::sampled_image);    // colormap;

        gfx::pipe_state::binding_layouts terrain_layouts;
        terrain_layouts.layouts.push_back(&terrain_info_bindings);
        terrain_layouts.layouts.push_back(&camera_bindings);
        gfx::pipe_state::vertex_input terrain_input;
        terrain_input.attributes.emplace_back(0, 0, gfx::rg32f, 0);
        terrain_input.bindings.emplace_back(0, sizeof(glm::vec2), gfx::input_rate::vertex);
        gfx::pipe_state::input_assembly terrain_assembly;
        terrain_assembly.primitive_topology = gfx::topology::triangle_list;
        gfx::pipe_state::rasterizer terrain_rasterizer;
        terrain_rasterizer.polygon_mode = gfx::poly_mode::fill;
        gfx::pipe_state terrain_state;
        terrain_state.state_rasterizer     = &terrain_rasterizer;
        terrain_state.state_vertex_input   = &terrain_input;
        terrain_state.state_input_assembly = &terrain_assembly;
        terrain_state.state_bindings       = &terrain_layouts;

        terrain_rasterizer.depth_bias_enable          = true;
        terrain_rasterizer.depth_bias_constant_factor = -8.f;
        terrain_rasterizer.depth_bias_slope_factor    = -8.f;

        const auto terrain_shaders_shadow = {
            gfx::shader(gfx::shader_type::vert, gfx::spirv::proto::shaders::terrain_shadow_vert),
            gfx::shader(gfx::shader_type::frag, gfx::spirv::proto::shaders::mesh_shadow_frag),
        };
        terrain_shadow_pipeline = std::make_unique<gfx::graphics_pipeline>(terrain_state, shadow_pass_layout, terrain_shaders_shadow);

        terrain_rasterizer.depth_bias_enable = false;

        const auto terrain_shaders = {
            gfx::shader(gfx::shader_type::vert, gfx::spirv::proto::shaders::terrain_vert),
            gfx::shader(gfx::shader_type::frag, gfx::spirv::proto::shaders::terrain_frag),
        };
        terrain_state.state_multisample = &exe.msaa_state;
		terrain_layouts.layouts.push_back(&terrain_style_bindings);
		terrain_layouts.layouts.push_back(&shadow_bindings);
		terrain_layouts.layouts.push_back(&time_bindings);
        terrain_rasterizer.cull = gfx::cull_mode::back;
        terrain_render_pipeline = std::make_unique<gfx::graphics_pipeline>(terrain_state, exe.pass_layout, terrain_shaders);
    }

    executable& exe;

    gfx::binding_layout camera_bindings;
    gfx::binding_layout mesh_bindings;
    gfx::binding_layout mesh_style_bindings;
    gfx::binding_layout shadow_bindings;
    gfx::binding_layout terrain_info_bindings;
    gfx::binding_layout terrain_style_bindings;
	gfx::binding_layout time_bindings;

    std::unique_ptr<gfx::graphics_pipeline> mesh_render_pipeline;
    std::unique_ptr<gfx::graphics_pipeline> mesh_shadow_pipeline;
    std::unique_ptr<gfx::graphics_pipeline> terrain_render_pipeline;
    std::unique_ptr<gfx::graphics_pipeline> terrain_shadow_pipeline;
    std::unique_ptr<gfx::graphics_pipeline> sky_pipeline;
    std::unique_ptr<gfx::compute_pipeline>  mesh_cull_pipeline;

    gfx::sampler           default_sampler;
    gfx::renderpass_layout shadow_pass_layout;
};