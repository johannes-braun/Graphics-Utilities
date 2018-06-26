#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "geometry.hpp"

namespace gfx
{
    class bspline
    {
    public:
        enum class type
        {
            periodic = 0,
            open
        };

        std::vector<vertex2d> build(type type, int order, int samples, glm::vec2 offset = {0, 0}) const;
        bspline& add(const vertex2d& p);
        void add(const vertex2d& p, size_t after);
        void remove(size_t at);
        void highlight(size_t index, bool h = true);

        const std::vector<vertex2d>& points() const noexcept;
        vertex2d& operator[](size_t idx);

    private:
        float cox_de_boor(float t, int cp, int o) const;

        mutable std::vector<float> _knots;
        std::vector<vertex2d> _points;
        std::vector<vertex2d> _selection_points;
    };
}
