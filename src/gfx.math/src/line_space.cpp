#include "datastructure/line_space.hpp"

namespace gfx
{
    line_space::line_space(int x, int y, int z)
        : _subdivision_x(x), _subdivision_y(y), _subdivision_z(z)
    {
    }

    void line_space::generate(const bvh<3>& bvh)
    {
        generate(bvh, bvh.get_bounds());
    }

    void line_space::generate(const bvh<3>& bvh, bounds3f bounds)
    {
        _bounds = bounds;
        glm::ivec3 subdiv;
        subdiv.x = _subdivision_x;
        subdiv.y = _subdivision_y;
        subdiv.z = _subdivision_z;

        static const std::array<glm::ivec2, 6> patch_indices{
            glm::ivec2{ 1, 2 }, glm::ivec2{ 0, 2 }, glm::ivec2{ 0, 1 },
            glm::ivec2{ 1, 2 }, glm::ivec2{ 0, 2 }, glm::ivec2{ 0, 1 },
        };

        const std::array<glm::ivec2, 6> patch_sizes{
            glm::ivec2{ subdiv[patch_indices[0].x], subdiv[patch_indices[0].y] },
            glm::ivec2{ subdiv[patch_indices[1].x], subdiv[patch_indices[1].y] },
            glm::ivec2{ subdiv[patch_indices[2].x], subdiv[patch_indices[2].y] },

            glm::ivec2{ subdiv[patch_indices[3].x], subdiv[patch_indices[3].y] },
            glm::ivec2{ subdiv[patch_indices[4].x], subdiv[patch_indices[4].y] },
            glm::ivec2{ subdiv[patch_indices[5].x], subdiv[patch_indices[5].y] },
        };

        const glm::vec3 bounds_size = bounds.size();

        std::atomic_bool empty = true;
        for (int s = 0; s < 6; ++s)
        {
            const glm::ivec2 patch_size_start = patch_sizes[s];
            for (int e = 0; e < 6; ++e)
            {
                if (s == e)
                    continue;

                _storages[s][e].resize(patch_sizes[s].x * patch_sizes[s].y * patch_sizes[e].x * patch_sizes[e].y);

                const glm::ivec2 patch_size_end = patch_sizes[e];

#pragma omp parallel for schedule(dynamic)
                for (int sid = 0; sid < patch_size_start.x * patch_size_start.y; ++sid)
                {
                    const int psx = sid % patch_size_start.y;
                    const int psy = sid / patch_size_start.y;

                    const int index_start = sid * patch_size_end.x * patch_size_end.y;
                    const int normal_axis_start = s % 3;
                    const float normal_offset_start = s <= 2 ? (bounds.min[normal_axis_start]) : (bounds.max[normal_axis_start]);

                    glm::vec3 start_center{ normal_offset_start };
                    start_center[patch_indices[s].x] = (psx + 0.5f) * (bounds_size[patch_indices[s].x] / patch_sizes[s].x) + bounds.min[patch_indices[s].x];
                    start_center[patch_indices[s].y] = (psy + 0.5f) * (bounds_size[patch_indices[s].y] / patch_sizes[s].y) + bounds.min[patch_indices[s].y];

                    for (int pex = 0; pex < patch_size_end.x; ++pex)
                        for (int pey = 0; pey < patch_size_end.y; ++pey)
                        {
                            const int normal_axis_end = e % 3;
                            const float normal_offset_end = e <= 2 ? (bounds.min[normal_axis_end]) : (bounds.max[normal_axis_end]);

                            glm::vec3 end_center{ normal_offset_end };
                            end_center[patch_indices[e].x] = (pex + 0.5f) * (bounds_size[patch_indices[e].x] / patch_sizes[e].x) + bounds.min[patch_indices[e].x];
                            end_center[patch_indices[e].y] = (pey + 0.5f) * (bounds_size[patch_indices[e].y] / patch_sizes[e].y) + bounds.min[patch_indices[e].y];

                            const glm::vec3 direction = normalize(end_center - start_center);
                            const glm::vec3 origin = start_center - 1e-2f*direction;

                            if (e == 2 && s == 5)
                                pex = pex;

                            const auto hit = bvh.intersect_ray(origin, direction, length(end_center - start_center) + 2 * 1e-2f);

                            // if first start-face and there is any hit...
                            if (hit)
                                empty = false;

                            _storages[s][e].at(index_start + (pey * patch_size_end.x + pex)).triangle = hit.hits ? hit.indices.front().index / 3 : -1;
                        }
                }
            }
        }
        _empty = empty;
    }

    const std::array<std::array<std::vector<line_space::line>, 6>, 6>& line_space::storage() const noexcept
    {
        return _storages;
    }

    int line_space::size_x() const noexcept
    {
        return _subdivision_x;
    }

    int line_space::size_y() const noexcept
    {
        return _subdivision_y;
    }

    int line_space::size_z() const noexcept
    {
        return _subdivision_z;
    }

    bool line_space::empty() const noexcept
    {
        return _empty;
    }

    const bounds3f& line_space::bounds() const noexcept
    {
        return _bounds;
    }
}