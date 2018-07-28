#define GFX_EXPOSE_APIS
#include <gfx/gfx.hpp>
#include <gfx/res.hpp>
#include <runnable.hpp>

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
const material copper(glm::vec3{0, 0, 0},
	material::make_f0(glm::vec3(0.40835f, 0.67693f, 0.24197f), glm::vec3(3.0988f, 2.6248f, 2.2921f)), 0.f, 0.f);
const material cobalt(glm::vec3{0, 0, 0}, material::make_f0(glm::vec3(2.1609f, 2.0524f, 1.7365f), glm::vec3(4.0371f, 3.8242f, 3.2745f)),
	0.f, 0.f);
const material palladium(glm::vec3{0, 0, 0},
	material::make_f0(glm::vec3(1.7160f, 1.6412f, 1.4080f), glm::vec3(4.1177f, 3.8455f, 3.2540f)), 0.f, 0.f);
const material cellulose(glm::vec3{0, 0, 0}, material::make_f0(glm::vec3(1.4696f, 1.4720f, 1.4796f), glm::vec3(0.f)), 0.f, 0.f);

void runnable::init(gfx::context_options& options)
{
    options.window_title        = "[13] Image Based Lighting";
    options.debug               = true;
    options.framebuffer_samples = 8;
}

void runnable::run()
{
	gfx::ressources res;
	gfx::image& base_cubemap = res.cubemaps_hdr["arboretum/hdr"];
    gfx::image_view base_cubemap_view(gfx::imgv_type::image_cube, base_cubemap);
    gfx::sampler    sampler;

    // --------------------------------------------------------------------------------------------------------------------
    // Generate Filtered Cubemap
    // --------------------------------------------------------------------------------------------------------------------

    constexpr uint32_t sampling_size = 32;
    gfx::image         filtered_cubemap(gfx::img_type::image2d, gfx::rgba16f, gfx::extent(sampling_size, sampling_size, 6), 1);
    gfx::image_view    filtered_cubemap_view(gfx::imgv_type::image_cube, filtered_cubemap);
    gfx::image         specular_cubemap(gfx::img_type::image2d, gfx::rgba16f, base_cubemap.extents(), base_cubemap.levels());
    gfx::image_view    specular_cubemap_view(gfx::imgv_type::image_cube, specular_cubemap);

    gfx::framebuffer fbo(sampling_size, sampling_size);
    fbo.attach(gfx::attachment::color, 0, filtered_cubemap_view);

    gfx::graphics_pipeline filter_pipeline{gfx::vertex_input(gfx::topology::point_list), gfx::state_info()};
    filter_pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::text, "13_ibl/cubemap_filter.vert"));
    filter_pipeline.attach(gfx::shader_type::geom, gfx::shader(gfx::shader_format::text, "13_ibl/cubemap_filter.geom"));
    filter_pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::text, "13_ibl/cubemap_filter_diffuse.frag"));

    gfx::graphics_pipeline specular_pipeline{gfx::vertex_input(gfx::topology::point_list), gfx::state_info()};
    specular_pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::text, "13_ibl/cubemap_filter.vert"));
    specular_pipeline.attach(gfx::shader_type::geom, gfx::shader(gfx::shader_format::text, "13_ibl/cubemap_filter.geom"));
    specular_pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::text, "13_ibl/cubemap_filter_specular.frag"));

    gfx::descriptor_set filter_set;
    filter_set.set(gfx::descriptor_type::sampled_texture, 0, base_cubemap_view, sampler);
    gfx::commands cmd;

    gfx::viewport    filter_viewport(0, 0, sampling_size, sampling_size, 0.01f, 100.f);
    cmd.begin_pass(fbo);
	cmd.bind_descriptors({ &filter_set, 1 });
    cmd.bind_pipeline(filter_pipeline);
	cmd.set_viewports({ &filter_viewport, 1 }, 0);
    cmd.draw(6, 1);
    cmd.end_pass();
    cmd.execute();

    gfx::hbuffer<float> roughness(1);
    filter_set.set(gfx::descriptor_type::uniform_buffer, 0, roughness);

	gfx::fence spec_fence;
    for (int l = 0; l < specular_cubemap.levels(); ++l) {
        gfx::image_view  cmv(gfx::imgv_type::image_cube, specular_cubemap.level(l), 1, 6);
        gfx::framebuffer cfbo(1024 >> l, 1024 >> l);
        cfbo.attach(gfx::attachment::color, 0, cmv);

		roughness[0] = l / (9.f);
        gfx::viewport viewport(0, 0, 1024 >> l, 1024 >> l, 0.01f, 100.f);
        cmd.begin_pass(cfbo);
		cmd.bind_descriptors({ &filter_set, 1 });
        cmd.bind_pipeline(specular_pipeline);
		cmd.set_viewports({ &viewport, 1 }, 0);
        cmd.draw(6, 1);
        cmd.end_pass();
        cmd.execute(spec_fence);

		spec_fence.wait();
    }

    // --------------------------------------------------------------------------------------------------------------------
    // Generate LUT for BRDF
    // --------------------------------------------------------------------------------------------------------------------

    gfx::image      brdf_lut(gfx::img_type::image2d, gfx::rg16f, gfx::extent(512, 512, 1), 1);
    gfx::image_view brdf_lut_view(gfx::imgv_type::image2d, brdf_lut);

    gfx::framebuffer lut_framebuffer(512, 512);
    lut_framebuffer.attach(gfx::attachment::color, 0, brdf_lut_view);

    gfx::viewport          lut_viewport(0, 0, 512, 512, 0.01f, 100.f);
    gfx::graphics_pipeline lut_pipeline{gfx::vertex_input(gfx::topology::triangle_list), gfx::state_info()};
    lut_pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::text, "postfx/screen.vert"));
    lut_pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::text, "13_ibl/brdf_lut.frag"));

    cmd.reset();
    cmd.begin_pass(lut_framebuffer);
    cmd.bind_pipeline(lut_pipeline);
	cmd.set_viewports({ &lut_viewport, 1 }, 0);
    cmd.draw(3);
    cmd.end_pass();
    cmd.execute();

    gfx::scene_file& scene = res.scenes["bunny.dae"];
	gfx::mesh3d mesh = scene.mesh;
	mesh.collapse();

    gfx::buffer<gfx::vertex3d> mesh_vertex_buffer(gfx::buffer_usage::vertex, mesh.vertices);
    gfx::buffer<gfx::index32>  mesh_index_buffer(gfx::buffer_usage::index, mesh.indices);
    gfx::hbuffer<material>     mesh_material_buffer_local;
    gfx::buffer<material>      mesh_material_buffer(gfx::buffer_usage::storage, 1);

    auto& mat = mesh_material_buffer_local.emplace_back(cellulose);
    mat.set_f0_roughness(cellulose.f0(), 0.2f);
    mat.set_albedo_transparency({0.3f, 0.18f, 0.12f}, 0.f);

    const auto mesh_vertex_input = std::make_shared<gfx::vertex_input>();
    mesh_vertex_input->add_attribute(0, gfx::rgb32f, offsetof(gfx::vertex3d, position));
    mesh_vertex_input->add_attribute(0, gfx::rgb32f, offsetof(gfx::vertex3d, uv));
    mesh_vertex_input->add_attribute(0, gfx::rgb32f, offsetof(gfx::vertex3d, normal));
    mesh_vertex_input->set_binding_info(0, sizeof(gfx::vertex3d), gfx::input_rate::vertex);

    const auto mesh_render_state                         = std::make_shared<gfx::state_info>();
    mesh_render_state->multisample.sample_shading_enable = true;
    mesh_render_state->multisample.samples               = gfx::sample_count::x8;

    const auto cubemap_render_state                        = std::make_shared<gfx::state_info>(*mesh_render_state);
    cubemap_render_state->depth_stencil.depth_write_enable = false;

    gfx::graphics_pipeline mesh_pipeline(mesh_vertex_input, mesh_render_state);
    mesh_pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::text, "13_ibl/mesh_unique.vert"));
    mesh_pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::text, "13_ibl/mesh_unique.frag"));

    gfx::graphics_pipeline cubemap_pipeline(gfx::vertex_input{}, cubemap_render_state);
    cubemap_pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::text, "13_ibl/skybox.vert"));
    cubemap_pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::text, "13_ibl/skybox.frag"));

    struct lighting
    {
        float gamma;
        float exposure;
    };
    gfx::hbuffer<lighting> lighting_buffer{{2.2f, 1.f}};

    gfx::descriptor_set mesh_cam_descriptor;
    mesh_cam_descriptor.set(gfx::descriptor_type::uniform_buffer, 0, *camera_buffer);
    mesh_cam_descriptor.set(gfx::descriptor_type::uniform_buffer, 1, lighting_buffer);
    mesh_cam_descriptor.set(gfx::descriptor_type::storage_buffer, 0, mesh_material_buffer);
    mesh_cam_descriptor.set(gfx::descriptor_type::sampled_texture, 0, filtered_cubemap_view, sampler);
    mesh_cam_descriptor.set(gfx::descriptor_type::sampled_texture, 1, specular_cubemap_view, sampler);
    mesh_cam_descriptor.set(gfx::descriptor_type::sampled_texture, 2, brdf_lut_view, sampler);

    gfx::descriptor_set cubemap_set;
    cubemap_set.set(gfx::descriptor_type::uniform_buffer, 0, *camera_buffer);
    cubemap_set.set(gfx::descriptor_type::uniform_buffer, 1, lighting_buffer);
    cubemap_set.set(gfx::descriptor_type::sampled_texture, 0, base_cubemap_view, sampler);

    while (frame()) {
        static glm::vec3 ior_n(0.24197f, 0.42108f, 1.3737f);
        static glm::vec3 ior_k(2.9152f, 2.3459f, 1.7704f);
        glm::vec3        albedo = mat.albedo();
        float            t      = mat.transparency();
        float            r      = mat.roughness();

        ImGui::Begin("Settings");
		ImGui::Value("Delta", static_cast<float>(context->delta() * 1000.f));
		ImGui::Separator();
        ImGui::ColorEdit3("Albedo", glm::value_ptr(albedo));
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

        cmd.reset();
        cmd.begin_pass(main_framebuffer());
		cmd.bind_descriptors({ &cubemap_set, 1 });
		cmd.set_viewports({ &main_viewport, 1 }, 0);
        cmd.bind_pipeline(cubemap_pipeline);
        cmd.draw(3);

        cmd.bind_descriptors({&mesh_cam_descriptor, 1});
        cmd.bind_pipeline(mesh_pipeline);
		cmd.set_viewports({ &main_viewport, 1 }, 0);
        cmd.bind_vertex_buffer(0, mesh_vertex_buffer);
        cmd.bind_index_buffer(mesh_index_buffer, gfx::index_type::uint32);
        cmd.draw_indexed(mesh_index_buffer.capacity());
        cmd.end_pass();
        cmd.execute();
    }
}