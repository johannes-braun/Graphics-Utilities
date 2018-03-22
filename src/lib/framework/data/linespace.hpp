#pragma once

#include "bvh.hpp"

#include <memory>
#include <opengl/buffer.hpp>

namespace gfx
{
    using line_space_bounds = bvh<3>::bounds;

    class line_space
    {
#pragma pack(push, 1)
        struct buffer_data
        {
            line_space_bounds bounds;
            int x;
            int y;
            int z;
            int _p;
            uint64_t storages[6][6];
        };
#pragma pack(pop, 1)

    public:
        using data_type = buffer_data;
        struct line
        {
            int triangle;
        };

        line_space(int x, int y, int z)
        {
            const std::array<int, 6> sizes
            {
                y * z,
                x * z,
                x * y,

                y * z,
                x * z,
                x * y,
            };
            _buffer_data.x = x;
            _buffer_data.y = y;
            _buffer_data.z = z;

            _memory_consumption = 0;
            for (int s = 0; s < 6; ++s)
                for (int e = 0; e < 6; ++e)
                {
                    _storages[s][e] = s == e ? nullptr : std::make_unique<gl::buffer<line>>(sizes[s] * sizes[e], GL_DYNAMIC_STORAGE_BIT);
                    _memory_consumption += s == e ? 0 : _storages[s][e]->size() * sizeof(line);
                }
            log_i << "Line Space {" << x << ", " << y << ", " << z << "} uses " << _memory_consumption << " Bytes of memory.";
        }
        void build(const bvh<3>& bvh)
        {
            build(bvh, bvh.get_bounds());
        }

        void build(const bvh<3>& bvh, line_space_bounds bounds)
        {
            glm::ivec3 subdiv;
            subdiv.x = _buffer_data.x;
            subdiv.y = _buffer_data.y;
            subdiv.z = _buffer_data.z;

            const std::array<glm::ivec2, 6> patch_indices
            {
                glm::ivec2{ 0, 1 },
                glm::ivec2{ 0, 2 },
                glm::ivec2{ 1, 2 },

                glm::ivec2{ 0, 1 },
                glm::ivec2{ 0, 2 },
                glm::ivec2{ 1, 2 },
            };

            const std::array<glm::ivec2, 6> patch_sizes
            {
                glm::ivec2{ subdiv.x, subdiv.y },
                glm::ivec2{ subdiv.x, subdiv.z },
                glm::ivec2{ subdiv.y, subdiv.z },

                glm::ivec2{ subdiv.x, subdiv.y },
                glm::ivec2{ subdiv.x, subdiv.z },
                glm::ivec2{ subdiv.y, subdiv.z },
            };

            _buffer_data.bounds = bounds;
            const glm::vec4 bounds_size = bounds.size();

//#pragma omp parallel for schedule(dynamic)
            for (int s = 0; s < 6; ++s)
            {
                const glm::ivec2 patch_size_start = patch_sizes[s];
                for (int e = 0; e < 6; ++e)
                {
                    if (s == e)
                        continue;

                    const glm::ivec2 patch_size_end = patch_sizes[e];

                    for(int psx = 0; psx < patch_size_start.x; ++psx)
                    for (int psy = 0; psy < patch_size_start.y; ++psy)
                    {
                        const int index_start = (psy * patch_size_start.x + psx) * patch_size_end.x * patch_size_end.y;
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

                            const auto hit = bvh.intersect_ray(origin, direction, length(end_center - start_center) + 2*1e-2f);


                            _storages[s][e]->at(index_start + (pey * patch_size_end.x + pex)).triangle = hit.hits ? hit.indices.front().index / 3 : -1;
                        }
                    }
                    _storages[s][e]->synchronize();
                }
            }

            for (int s = 0; s < 6; ++s)
                for (int e = 0; e < 6; ++e)
                    _buffer_data.storages[s][e] = e == s ? 0ui64 : _storages[s][e]->handle();
        }

        const buffer_data& get_data() const noexcept {
            return _buffer_data;
        }

    private:
        size_t _memory_consumption;
        std::array<std::array<std::unique_ptr<gl::buffer<line>>, 6>, 6> _storages;
        buffer_data _buffer_data;
    };
}