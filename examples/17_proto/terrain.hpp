#pragma once
#include <gfx/gfx.hpp>

struct terrain_info
{
	float chunk_size;
	int   chunk_count;
};
class terrain
{
public:
	terrain(const std::filesystem::path& path, float chunk_size, int chunk_count)
		: _info{ {chunk_size, chunk_count} }
		, _chunks(gfx::buffer_usage::vertex, chunk_count * chunk_count * 6)
		, _heightmap_local(gfx::r8unorm, path)
		, _heightmap(_heightmap_local)
		, _heightmap_view(gfx::imgv_type::image2d, _heightmap)
	{
		_sampler.set_wrap(gfx::wrap::u, gfx::wrap_mode::clamp_to_edge);
		_sampler.set_wrap(gfx::wrap::v, gfx::wrap_mode::clamp_to_edge);

		gfx::hbuffer<glm::vec2> chunks_local(6 * chunk_count * chunk_count);
		for (int x = 0; x < chunk_count; ++x)
		{
			for (int y = 0; y < chunk_count; ++y)
			{
				chunks_local[6 * x + 6 * chunk_count * y + 0] =
					glm::vec2((x - (chunk_count / 2.f)) * chunk_size, (y - (chunk_count / 2.f)) * chunk_size)
					+ chunk_size * glm::vec2(0, 0);
				chunks_local[6 * x + 6 * chunk_count * y + 1] =
					glm::vec2((x - (chunk_count / 2.f)) * chunk_size, (y - (chunk_count / 2.f)) * chunk_size)
					+ chunk_size * glm::vec2(0, 1);
				chunks_local[6 * x + 6 * chunk_count * y + 2] =
					glm::vec2((x - (chunk_count / 2.f)) * chunk_size, (y - (chunk_count / 2.f)) * chunk_size)
					+ chunk_size * glm::vec2(1, 1);

				chunks_local[6 * x + 6 * chunk_count * y + 3] =
					glm::vec2((x - (chunk_count / 2.f)) * chunk_size, (y - (chunk_count / 2.f)) * chunk_size)
					+ chunk_size * glm::vec2(0, 0);
				chunks_local[6 * x + 6 * chunk_count * y + 4] =
					glm::vec2((x - (chunk_count / 2.f)) * chunk_size, (y - (chunk_count / 2.f)) * chunk_size)
					+ chunk_size * glm::vec2(1, 1);
				chunks_local[6 * x + 6 * chunk_count * y + 5] =
					glm::vec2((x - (chunk_count / 2.f)) * chunk_size, (y - (chunk_count / 2.f)) * chunk_size)
					+ chunk_size * glm::vec2(1, 0);
			}
		}

		_chunks << chunks_local;
	}

	auto& chunk_buffer() { return _chunks; }

	auto terrain_height(glm::vec2 xz) const
	{
		glm::vec2 xzi;
		const glm::vec2 fract{ glm::modf(xz / chunk_size(), xzi) };

		const glm::vec2 min = xzi * chunk_size();
		const glm::vec2 max = min + chunk_size();

		const glm::vec2 v00(min.x, min.y);
		const glm::vec2 v10(max.x, min.y);
		const glm::vec2 v01(min.x, max.y);
		const glm::vec2 v11(max.x, max.y);

		const auto b00 = _heightmap_local.load(glm::vec3(v00 / 1500.f * glm::vec2(_heightmap_local.extents().vec), 0)).r;
		const auto b10 = _heightmap_local.load(glm::vec3(v10 / 1500.f * glm::vec2(_heightmap_local.extents().vec), 0)).r;
		const auto b01 = _heightmap_local.load(glm::vec3(v01 / 1500.f * glm::vec2(_heightmap_local.extents().vec), 0)).r;
		const auto b11 = _heightmap_local.load(glm::vec3(v11 / 1500.f * glm::vec2(_heightmap_local.extents().vec), 0)).r;

		return 80 * glm::mix(glm::mix(b00, b10, fract.x), glm::mix(b01, b11, fract.x), fract.y);
	};

	float                             chunk_size() const noexcept { return _info[0].chunk_size; }
	int                               chunk_count() const noexcept { return _info[0].chunk_count; }
	const gfx::hbuffer<terrain_info>& info() const noexcept { return _info; }
	const gfx::image_view&            map_view() const noexcept { return _heightmap_view; }
	const gfx::sampler&               sampler() const noexcept { return _sampler; }


private:
	gfx::hbuffer<terrain_info> _info;
	gfx::buffer<glm::vec2>     _chunks;
	gfx::himage                _heightmap_local;
	gfx::image                 _heightmap;
	gfx::image_view            _heightmap_view;
	gfx::sampler               _sampler;
};
