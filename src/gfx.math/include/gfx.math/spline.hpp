#pragma once

#include "geometry.hpp"
#include <glm/glm.hpp>
#include <gsl/span>
#include <vector>

namespace gfx {
inline namespace v1 {

template<typename Iter, typename LerpFun,
         typename = std::enable_if_t<
             /* lerp(value_type, value_type, float) -> value_type */
             std::is_convertible_v<std::decay_t<decltype(*std::declval<Iter>())>,
                                   decltype(std::declval<LerpFun>()(*std::declval<Iter>(), *std::declval<Iter>(), float()))>>>
auto de_casteljau(Iter begin, Iter end, float t, LerpFun&& lerp_fun)
{
    std::vector<std::decay_t<decltype(*std::declval<Iter>())>> vs(begin, end);

    while (vs.size() > 1) {
        for (int i = 0; i < vs.size() - 1; ++i) {
            vs[i] = lerp_fun(vs[i], vs[i + 1], t);
        }
        vs.pop_back();
    }
    return vs[0];
}

vertex3d de_casteljau(gsl::span<vertex3d> vertices, float t);

class bspline
{
public:
    enum class type
    {
        periodic = 0,
        open
    };

    std::vector<vertex2d> build(type type, int order, int samples, glm::vec2 offset = {0, 0}) const;
    bspline&              add(const vertex2d& p);
    void                  add(const vertex2d& p, size_t after);
    void                  remove(size_t at);
    void                  highlight(size_t index, bool h = true);

    const std::vector<vertex2d>& points() const noexcept;
    vertex2d&                    operator[](size_t idx);

private:
    float cox_de_boor(float t, int cp, int o) const;

    mutable std::vector<float> _knots;
    std::vector<vertex2d>      _points;
    std::vector<vertex2d>      _selection_points;
};
}    // namespace v1
}    // namespace gfx
