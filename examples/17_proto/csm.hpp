#pragma once

#include "graphics.hpp"

class csm
{
	const float vps = 80.f;
public:

    csm(graphics& core, uint32_t resolution = 1024, int cascades = 5)
          : shadow_set(core.shadow_bindings), _core(core), _cascades(cascades), _camera_data(gfx::buffer_usage::uniform, cascades)
    {
        shadow_map = std::make_unique<gfx::image>(gfx::img_type::attachment, gfx::d32f, gfx::extent(resolution, resolution, _cascades), 1);
        shadow_map_view = std::make_unique<gfx::image_view>(gfx::imgv_type::image2d_array, *shadow_map);
        for (int i = 0; i < _cascades; ++i)
        {
            shadow_map_framebuffers.emplace_back(shadow_map->extents().width, shadow_map->extents().height, 1, core.shadow_pass_layout);
            auto& view = shadow_map_attachments.emplace_back(gfx::imgv_type::image2d, gfx::d32f, *shadow_map, 0, 1, i, 1);
            shadow_map_framebuffers[i].attach(gfx::attachment::depth_stencil, 0, view, gfx::depth_stencil());
        }
        shadow_sampler.set_wrap(gfx::wrap::u, gfx::wrap_mode::clamp_to_edge);
        shadow_sampler.set_wrap(gfx::wrap::v, gfx::wrap_mode::clamp_to_edge);
        shadow_sampler.set_compare(true, gfx::compare_op::less_or_equal);
        shadow_sampler.set_anisotropy(false, 0.f);

        init_cameras();

		shadow_set.bind(0, *shadow_map_view, shadow_sampler);
		shadow_set.bind(1, _camera_data);
    }

    void init_cameras()
    {
        light_cameras.resize(_cascades);
        light_camera_matrices.resize(_cascades);
        for (auto i = 0ull; i < std::size(light_cameras); ++i)
        {
            light_cameras[i].second.value.position = glm::vec3(100, -150, 100);
            light_cameras[i].second.value.rotation =
                glm::quatLookAt(normalize(glm::vec3(0) - light_cameras[i].second.value.position), glm::vec3(0, 1, 0));
            const float vpss            = vps * (1 << i);
            light_cameras[i].first.projection = gfx::projection(-vpss * 0.5f, vpss * 0.5f, -vpss * 0.5f, vpss * 0.5f, -1000.f, 1000.f);

			auto& lci = light_camera_matrices[i];
			lci.do_cull = true;
			lci.view = inverse(light_cameras[i].second.value.matrix());
			lci.projection = light_cameras[i].first.projection.matrix();
			lci.position = light_cameras[i].second.value.position;
        }
        _camera_data.update(light_camera_matrices);

        for (int i = 0; i < std::size(light_cameras); ++i)
            light_camera_sets.emplace_back(_core.camera_bindings).bind(0, 0, _camera_data, 1, 0 * i);
    }

    template<typename FunPre, typename FunDraw>
    void render(gfx::commands& cmd, glm::vec3 position, FunPre&& preDraw, FunDraw&& draw)
    {
		for (auto i = 0ull; i < std::size(light_cameras); ++i) {
			const float vpss = vps * (1 << i);

			light_cameras[i].second.value.position =
				(glm::vec3(glm::ivec3(position * vpss / shadow_map->extents().width)) * shadow_map->extents().width / vpss)
				+ 500.f * (light_cameras[i].second.value.rotation * glm::vec3(0, 0, 1));

			auto& lci = light_camera_matrices[i];
			lci.do_cull = true;
			lci.view = inverse(light_cameras[i].second.value.matrix());
			lci.projection = light_cameras[i].first.projection.matrix();
			lci.position = light_cameras[i].second.value.position;
			cmd.update_buffer(_camera_data, 0, lci);
			preDraw(light_camera_sets[i]);
			cmd.begin_pass(shadow_map_framebuffers[i]);
			draw(light_camera_sets[i]);
			cmd.end_pass();
		}
		cmd.update_buffer(_camera_data, 0, gfx::u32(light_camera_matrices.size()), light_camera_matrices.data());
    }

	gfx::binding_set                 shadow_set;
	std::vector<std::pair<gfx::camera_component, gfx::transform_component>>           light_cameras;	
    std::vector<gfx::camera_matrices> light_camera_matrices;
    std::unique_ptr<gfx::image>      shadow_map;
    std::unique_ptr<gfx::image_view> shadow_map_view;
    std::vector<gfx::binding_set>    light_camera_sets;
    std::vector<gfx::framebuffer>    shadow_map_framebuffers;
    std::vector<gfx::image_view>     shadow_map_attachments;
    gfx::sampler                     shadow_sampler;

private:
    graphics&                                    _core;
    int                                          _cascades;
    gfx::buffer<gfx::camera_matrices> _camera_data;
};