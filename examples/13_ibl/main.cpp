#include <executable.hpp>
#include "shaders/shaders.hpp"
#include <gfx.shaders/shaders.hpp>

struct material
{
    static constexpr glm::vec3 make_f0(glm::vec3 n, glm::vec3 k)
    {
        const glm::vec3 k2(k.x * k.x, k.y * k.y, k.z * k.z);
        const glm::vec3 omn(1.f - n.x, 1.f - n.y, 1.f - n.z);
        const glm::vec3 opn(1.f + n.x, 1.f + n.y, 1.f + n.z);

        return glm::vec3((omn.x * omn.x + k2.x) / (opn.x * opn.x + k2.x), (omn.y * omn.y + k2.y) / (opn.y * opn.y + k2.y),
                         (omn.z * omn.z + k2.z) / (opn.z * opn.z + k2.z));
    }

    constexpr glm::uint pack(glm::vec4 const& v)
    {
        glm::uint result = 0;
        result |= glm::uint(std::min(255.f, std::max(v.x * 255.f, 0.f))) << 0;
        result |= glm::uint(std::min(255.f, std::max(v.y * 255.f, 0.f))) << 8;
        result |= glm::uint(std::min(255.f, std::max(v.z * 255.f, 0.f))) << 16;
        result |= glm::uint(std::min(255.f, std::max(v.w * 255.f, 0.f))) << 24;
        return result;
    }

    material() = default;

    material(glm::vec3 color, glm::vec3 f0, float tr, float rough) : packed_logf0_roughness(0), packed_albedo_transparency(0)
    {
        set_albedo_transparency(color, tr);
        set_f0_roughness(f0, rough);
    }

    void set_albedo_transparency(const glm::vec3& color, float t) { packed_albedo_transparency = pack(glm::vec4(color, t)); }
    void set_f0_roughness(const glm::vec3& val, float r)
    {
        packed_logf0_roughness = pack(glm::vec4(glm::vec3(std::log(val.x + 1), std::log(val.y + 1), std::log(val.z + 1)), r));
    }

    glm::vec3 albedo() const noexcept { return glm::unpackUnorm4x8(packed_albedo_transparency); }
    glm::vec3 f0() const noexcept
    {
        return glm::pow(glm::vec3(glm::e<float>()), glm::vec3(glm::unpackUnorm4x8(packed_logf0_roughness))) - 1.f;
    }
    float transparency() const noexcept { return glm::unpackUnorm4x8(packed_albedo_transparency).a; }
    float roughness() const noexcept { return glm::unpackUnorm4x8(packed_logf0_roughness).a; }

    uint32_t packed_logf0_roughness;
    uint32_t packed_albedo_transparency;
};

const material gold(glm::vec3{0, 0, 0}, material::make_f0(glm::vec3(0.24197f, 0.42108f, 1.3737f), glm::vec3(2.9152f, 2.3459f, 1.7704f)),
                    0.f, 0.f);
const material copper(glm::vec3{0, 0, 0}, material::make_f0(glm::vec3(0.40835f, 0.67693f, 0.24197f), glm::vec3(3.0988f, 2.6248f, 2.2921f)),
                      0.f, 0.f);
const material cobalt(glm::vec3{0, 0, 0}, material::make_f0(glm::vec3(2.1609f, 2.0524f, 1.7365f), glm::vec3(4.0371f, 3.8242f, 3.2745f)),
                      0.f, 0.f);
const material palladium(glm::vec3{0, 0, 0}, material::make_f0(glm::vec3(1.7160f, 1.6412f, 1.4080f), glm::vec3(4.1177f, 3.8455f, 3.2540f)),
                         0.f, 0.f);
const material cellulose(glm::vec3{0, 0, 0}, material::make_f0(glm::vec3(1.4696f, 1.4720f, 1.4796f), glm::vec3(0.f)), 0.f, 0.f);

void executable::init(gfx::context_options& options)
{
    options.window_title        = "[13] Image Based Lighting";
    options.debug               = true;
    options.framebuffer_samples = gfx::sample_count::x8;
    options.graphics_api        = gfx::gapi::vulkan;
}

void executable::run()
{
    gfx::image&     base_cubemap = res.cubemaps_hdr["arboretum/hdr"];
    gfx::image_view base_cubemap_view(gfx::imgv_type::image_cube, base_cubemap);
    gfx::sampler    sampler;

    // --------------------------------------------------------------------------------------------------------------------
    // Generate Filtered Cubemap
    // --------------------------------------------------------------------------------------------------------------------

    constexpr uint32_t sampling_size = 32;
    gfx::image         filtered_cubemap(gfx::img_type::attachment, gfx::rgba16f, gfx::extent(sampling_size, sampling_size, 6), 1);
    gfx::image_view    filtered_cubemap_view(gfx::imgv_type::image_cube, filtered_cubemap);
    gfx::image         specular_cubemap(gfx::img_type::attachment, gfx::rgba16f, base_cubemap.extents(), base_cubemap.levels());
    gfx::image_view    specular_cubemap_view(gfx::imgv_type::image_cube, specular_cubemap);

	{
		gfx::renderpass_layout prefilter_pass_layout;
		prefilter_pass_layout.add_color_attachment(gfx::rgba16f);

		gfx::framebuffer fbo(sampling_size, sampling_size, 6, prefilter_pass_layout);
		fbo.attach(gfx::attachment::color, 0, filtered_cubemap_view);

		gfx::pipe_state::input_assembly points_input;
		points_input.primitive_topology = gfx::topology::point_list;

		gfx::binding_layout filter_layout;
		filter_layout.push(gfx::binding_type::sampled_image);
		gfx::pipe_state::binding_layouts filter_layouts;
		filter_layouts.layouts.push_back(&filter_layout);

		gfx::pipe_state::render_area filter_area;
		filter_area.scissors.emplace_back(glm::vec2{ 0, 0 }, glm::vec2{ sampling_size, sampling_size });
		filter_area.viewports.emplace_back(0.f, 0.f, float(sampling_size), float(sampling_size), 0.f, 1.f);

		gfx::pipe_state filter_state;
		filter_state.state_input_assembly = &points_input;
		filter_state.state_bindings       = &filter_layouts;
		filter_state.state_render_area    = &filter_area;

		gfx::graphics_pipeline filter_pipeline(filter_state, prefilter_pass_layout,
			{ gfx::shader(gfx::shader_type::vert, gfx::spirv::ibl::shaders::cubemap_filter_vert),
			 gfx::shader(gfx::shader_type::geom,  gfx::spirv::ibl::shaders::cubemap_filter_geom),
			 gfx::shader(gfx::shader_type::frag, gfx::spirv::ibl::shaders::cubemap_filter_diffuse_frag) });

		gfx::binding_layout filter_layout_spec;
		filter_layout_spec.push(gfx::binding_type::sampled_image).push(gfx::binding_type::uniform_buffer);
		filter_layouts.layouts[0]      = &filter_layout_spec;
		filter_state.state_render_area = nullptr;

		gfx::graphics_pipeline specular_pipeline(filter_state, prefilter_pass_layout,
			{ gfx::shader(gfx::shader_type::vert, gfx::spirv::ibl::shaders::cubemap_filter_vert),
			 gfx::shader(gfx::shader_type::geom,  gfx::spirv::ibl::shaders::cubemap_filter_geom),
			 gfx::shader(gfx::shader_type::frag, gfx::spirv::ibl::shaders::cubemap_filter_specular_frag) });


		gfx::binding_set filter_set(filter_layout);
		filter_set.bind(0, base_cubemap_view, sampler);

		struct specular_info
		{
			gfx::hbuffer<float> roughness;
			gfx::binding_set    set;
			gfx::image_view     view;
			gfx::framebuffer    cfbo;
		};

		std::vector<specular_info> infos;
		infos.reserve(specular_cubemap.levels());
		for (int l = 0; l < specular_cubemap.levels(); ++l) {
			infos.emplace_back(specular_info{ {l / (9.f)},
											 gfx::binding_set(filter_layout_spec),
											 gfx::image_view(gfx::imgv_type::image_cube, specular_cubemap.level(l), 1, 6),
											 gfx::framebuffer(1024 >> l, 1024 >> l, 6, prefilter_pass_layout) });
			infos.back().set.bind(0, base_cubemap_view, sampler);
			infos.back().set.bind(1, infos.back().roughness);
			infos.back().cfbo.attach(gfx::attachment::color, 0, infos.back().view);
		}

		gfx::commands cmd_diff(gfx::commands_type::graphics);

		cmd_diff.begin();
		cmd_diff.begin_pass(fbo);
		cmd_diff.bind_pipeline(filter_pipeline, { &filter_set });
		cmd_diff.draw(6);
		cmd_diff.end_pass();
		for (int l = 0; l < specular_cubemap.levels(); ++l) {
			gfx::viewport viewport(0, 0, 1024 >> l, 1024 >> l, 0.f, 1.f);
			gfx::rect2f   scissor({ 0, 0 }, { 1024 >> l, 1024 >> l });
			cmd_diff.begin_pass(infos[l].cfbo);
			cmd_diff.bind_pipeline(specular_pipeline, { &infos[l].set });
			cmd_diff.set_viewports(0, { &viewport, 1 }, { &scissor, 1 });
			cmd_diff.draw(6);
			cmd_diff.end_pass();
		}
		cmd_diff.end();
		cmd_diff.execute();
	}

    // --------------------------------------------------------------------------------------------------------------------
    // Generate LUT for BRDF
    // --------------------------------------------------------------------------------------------------------------------
	gfx::image      brdf_lut(gfx::img_type::attachment, gfx::rg16f, gfx::extent(512, 512, 1), 1);
	gfx::image_view brdf_lut_view(gfx::imgv_type::image2d, brdf_lut);

	{
		gfx::renderpass_layout lut_pass_layout;
		lut_pass_layout.add_color_attachment(gfx::rg16f);

		gfx::framebuffer lut_framebuffer(512, 512, 1, lut_pass_layout);
		lut_framebuffer.attach(gfx::attachment::color, 0, brdf_lut_view);

		gfx::pipe_state::render_area lut_area;
		lut_area.viewports.emplace_back(0, 0, 512, 512, 0.f, 1.f);
		lut_area.scissors.emplace_back(glm::vec2(0), glm::vec2(512));

		gfx::pipe_state lut_state;
		lut_state.state_render_area = &lut_area;

		gfx::viewport(0, 0, 512, 512, 0.01f, 100.f);
		gfx::graphics_pipeline lut_pipeline(
			lut_state, lut_pass_layout,
			{ gfx::shader(gfx::shader_type::vert, gfx::spirv::core::screen_vert), gfx::shader(gfx::shader_type::frag, gfx::spirv::ibl::shaders::brdf_lut_frag) });

		gfx::commands cmd_lut(gfx::commands_type::graphics);
		cmd_lut.reset();
		cmd_lut.begin();
		cmd_lut.begin_pass(lut_framebuffer);
		cmd_lut.bind_pipeline(lut_pipeline);
		cmd_lut.draw(3);
		cmd_lut.end_pass();
		cmd_lut.end();
		cmd_lut.execute();
	}

    gfx::scene_file& scene = res.scenes["bunny.dae"];
    gfx::mesh3d      mesh  = scene.mesh;
    mesh.collapse();

    gfx::buffer<gfx::vertex3d> mesh_vertex_buffer(gfx::buffer_usage::vertex, mesh.vertices);
    gfx::buffer<gfx::index32>  mesh_index_buffer(gfx::buffer_usage::index, mesh.indices);
    gfx::hbuffer<material>     mesh_material_buffer_local;
    gfx::buffer<material>      mesh_material_buffer(gfx::buffer_usage::storage, 1);

    auto& mat = mesh_material_buffer_local.emplace_back(gold);
    mat.set_f0_roughness(gold.f0(), 0.2f);
    mat.set_albedo_transparency({0.3f, 0.18f, 0.12f}, 0.f);

    gfx::binding_layout mesh_binding_layout;
    mesh_binding_layout.push(gfx::binding_type::uniform_buffer)
        .push(gfx::binding_type::uniform_buffer)
        .push(gfx::binding_type::storage_buffer)
        .push(gfx::binding_type::sampled_image)
        .push(gfx::binding_type::sampled_image)
        .push(gfx::binding_type::sampled_image);

    gfx::binding_layout cubemap_binding_layout;
    cubemap_binding_layout.push(gfx::binding_type::uniform_buffer)
        .push(gfx::binding_type::uniform_buffer)
        .push(gfx::binding_type::sampled_image);

    gfx::pipe_state::vertex_input mesh_vertex_input;
    mesh_vertex_input.attributes.emplace_back(0, 0, gfx::rgb32f, offsetof(gfx::vertex3d, position));
    mesh_vertex_input.attributes.emplace_back(1, 0, gfx::rgb32f, offsetof(gfx::vertex3d, uv));
    mesh_vertex_input.attributes.emplace_back(2, 0, gfx::rgb32f, offsetof(gfx::vertex3d, normal));
    mesh_vertex_input.bindings.emplace_back(0, sizeof(gfx::vertex3d));

    gfx::pipe_state::depth_stencil mesh_depth;
    mesh_depth.depth_test_enable = true;

    gfx::pipe_state::binding_layouts bindings;
    bindings.layouts.push_back(&mesh_binding_layout);

    gfx::pipe_state::rasterizer mesh_raster;
    mesh_raster.cull = gfx::cull_mode::back;

    gfx::pipe_state mesh_state;
    mesh_state.state_vertex_input  = &mesh_vertex_input;
    mesh_state.state_depth_stencil = &mesh_depth;
    mesh_state.state_multisample   = &msaa_state;
    mesh_state.state_bindings      = &bindings;
    mesh_state.state_rasterizer    = &mesh_raster;

    gfx::pipe_state cubemap_state;
    cubemap_state.state_multisample = &msaa_state;
    cubemap_state.state_bindings    = &bindings;

    gfx::graphics_pipeline mesh_pipeline(
        mesh_state, pass_layout,
        {gfx::shader(gfx::shader_type::vert, gfx::spirv::ibl::shaders::mesh_unique_vert), gfx::shader(gfx::shader_type::frag, gfx::spirv::ibl::shaders::mesh_unique_frag)});

    bindings.layouts[0] = &cubemap_binding_layout;

    gfx::graphics_pipeline cubemap_pipeline(
        cubemap_state, pass_layout,
        {gfx::shader(gfx::shader_type::vert, gfx::spirv::ibl::shaders::skybox_vert), gfx::shader(gfx::shader_type::frag, gfx::spirv::ibl::shaders::skybox_frag)});

    struct lighting
    {
        float gamma;
        float exposure;
    };
    gfx::hbuffer<lighting> lighting_buffer{{2.2f, 1.f}};

    gfx::binding_set mesh_cam_set(mesh_binding_layout);
    mesh_cam_set.bind(0, *camera_buffer);
    mesh_cam_set.bind(1, lighting_buffer);
    mesh_cam_set.bind(2, mesh_material_buffer);
    mesh_cam_set.bind(3, filtered_cubemap_view, sampler);
    mesh_cam_set.bind(4, specular_cubemap_view, sampler);
    mesh_cam_set.bind(5, brdf_lut_view, sampler);

    gfx::binding_set cubemap_set(cubemap_binding_layout);
    cubemap_set.bind(0, *camera_buffer);
    cubemap_set.bind(1, lighting_buffer);
    cubemap_set.bind(2, base_cubemap_view, sampler);

    while (frame()) {
        static glm::vec3 ior_n(0.24197f, 0.42108f, 1.3737f);
        static glm::vec3 ior_k(2.9152f, 2.3459f, 1.7704f);
        glm::vec3        albedo = mat.albedo();
        float            t      = mat.transparency();
        float            r      = mat.roughness();

        ImGui::Begin("Settings");
        ImGui::Value("Delta", static_cast<float>(context->delta() * 1000.f));
        ImGui::Separator();
        ImGui::ColorEdit3("Albedo", glm::value_ptr(albedo), ImGuiColorEditFlags_Float);
        ImGui::ColorEdit3("IOR (n)", glm::value_ptr(ior_n), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
        ImGui::ColorEdit3("IOR (k)", glm::value_ptr(ior_k), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
        ImGui::DragFloat("Transparency", &t, 0.01f, 0.f, 1.f);
        ImGui::DragFloat("Roughness", &r, 0.01f, 0.f, 1.f);
        ImGui::Separator();
        ImGui::DragFloat("Gamma", &lighting_buffer[0].gamma, 0.01f, 0.f, 100.f);
        ImGui::DragFloat("Exposure", &lighting_buffer[0].exposure, 0.01f, 0.f, 100.f);
        ImGui::End();

        mat.set_albedo_transparency(albedo, t);
        mat.set_f0_roughness(material::make_f0(ior_n, ior_k), r);

        mesh_material_buffer << mesh_material_buffer_local;

        current_command->begin_pass(*current_framebuffer);
        current_command->bind_pipeline(cubemap_pipeline, {&cubemap_set});
        current_command->draw(3);

        current_command->bind_pipeline(mesh_pipeline, {&mesh_cam_set});
        current_command->bind_vertex_buffer(mesh_vertex_buffer, 0);
        current_command->bind_index_buffer(mesh_index_buffer, gfx::index_type::uint32);
        current_command->draw_indexed(mesh_index_buffer.capacity());
        current_command->end_pass();
    }
}