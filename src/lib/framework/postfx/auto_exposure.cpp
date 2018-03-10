#include "auto_exposure.hpp"
#include <numeric>

namespace gfx::fx
{
    auto_exposure::auto_exposure()
        : _luminance_compute_pipeline(new gl::shader("postprocess/tonemap/sample_luminance.comp")),
        _luminance_sample_buffer(std::array<float, 32>{}, gl::buffer_flag_bits::map_dynamic_persistent_read),
        _luminance(0)
    {
        _tonemap_pipeline.use_stages(screen_vertex_shader(), new gl::shader("postprocess/filter/const_multiply.frag"));
    }
    void auto_exposure::run(const std::array<jpu::ref_ptr<gl::texture>, 2>& base_attachments, postprocess_provider & provider, double delta_time)
    {
        provider.begin_draw();
        constexpr auto adaption_speed = 0.4f;
        constexpr auto brightness = 2.5f;
        const auto new_luma = std::accumulate(_luminance_sample_buffer.data_as<float>(), _luminance_sample_buffer.data_as<float>() + 32, 0.f) * (1 / 32.f);
        _luminance = glm::mix(glm::mix(_luminance, new_luma, adaption_speed * delta_time), new_luma, adaption_speed * delta_time);
        const auto half_inv_luma = 0.5f / _luminance;
        const auto luminance = half_inv_luma * (half_inv_luma * brightness + 1.f) / (half_inv_luma + 1.f);
        _luminance_sample_buffer.bind(GL_SHADER_STORAGE_BUFFER, 1);
        _luminance_compute_pipeline.bind();
        _luminance_compute_pipeline.get_uniform<uint64_t>("src_texture") = sampler.sample_texture(provider.last_target());
        _luminance_compute_pipeline.dispatch(1);

        _tonemap_pipeline.bind();
        _tonemap_pipeline.get_uniform<uint64_t>(gl::shader_type::fragment, "src_textures[0]") = sampler.sample_texture(provider.last_target());
        _tonemap_pipeline.get_uniform<float>(gl::shader_type::fragment, "factor") = luminance;
        _tonemap_pipeline.draw(gl::primitive::triangles, 3);
        provider.end_draw();
    }
    void auto_exposure::resize(int x, int y)
    {
    }
    void auto_exposure::reload_pipelines()
    {
        _luminance_compute_pipeline.reload_stages();
        _tonemap_pipeline.reload_stages();
    }
}