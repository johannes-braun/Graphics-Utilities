#pragma once

#include <glm/glm.hpp>
#include <numeric>
#include <vector>
#include <algorithm>

namespace gfx
{
    struct point
    {
        glm::vec2 position;
        glm::u8vec4 color;
    };

    class bspline
    {
    public:
        enum class type
        {
            periodic = 0,
            open
        };

        std::vector<point> build(type type, int order, int samples, glm::vec2 offset = {0, 0}) const;
        void add(const point& p);
        void add(const point& p, size_t after);
        void remove(size_t at);
        void highlight(size_t index, bool h = true);

        const std::vector<point>& points() const noexcept;
        point& operator[](size_t idx);

    private:
        float cox_de_boor(float t, int cp, int o) const;

        mutable std::vector<float> _knots;
        std::vector<point> _points;
        std::vector<point> _selection_points;
    };
}