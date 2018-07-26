#include <algorithm>
#include <gfx/spline.hpp>
#include <numeric>

namespace gfx {
inline namespace v1 {
[[maybe_unused]] bspline& bspline::add(const vertex2d& p)
{
    _points.insert(_points.end(), p);
    _selection_points.insert(_selection_points.end(), p);
    return *this;
}
void bspline::add(const vertex2d& p, size_t after)
{
    _points.insert(_points.begin() + after, p);
    _selection_points.insert(_selection_points.begin() + after, p);
}
void bspline::remove(size_t at)
{
    _points.erase(_points.begin() + at);
    _selection_points.erase(_selection_points.begin() + at);
}

vertex3d lerp(const vertex3d& a, const vertex3d& b, float t)
{
    auto va     = a;
    va.position = mix(a.position, b.position, t);
    va.uv       = mix(a.uv, b.uv, t);
    va.normal   = normalize(mix(a.normal, b.normal, t));
    return va;
}

vertex3d de_casteljau(span<vertex3d> vertices, float t)
{
    return de_casteljau(std::begin(vertices), std::end(vertices), t, lerp);
}

std::vector<vertex2d> bspline::build(type type, int order, int samples, glm::vec2 offset) const
{
    if (order == 0) return _points;
    std::vector<vertex2d> sampled(samples);
    std::vector<vertex2d> points;

    float tmin = 0, tmax = 0;
    switch (type)
    {
    case type::periodic:
    {
        points.insert(points.begin(), _points.begin(), _points.end());
        points.insert(points.end(), _points.begin(), _points.begin() + order - 1);
        _knots.resize(points.size() + order);
        std::iota(_knots.begin(), _knots.end(), 0.f);
        tmin = float(order - 1);
        tmax = float(points.size());
    }
    break;

    case type::open:
    {
        points.insert(points.begin(), _points.begin(), _points.end());
        _knots.resize(points.size() + 2 * (order - 1) - (order - 2));
        std::generate(_knots.begin(), _knots.begin() + (order - 1), [&]() { return 0.f; });
        std::iota(_knots.begin() + (order - 1), _knots.end() - (order - 1), 0.f);
        std::generate(_knots.end() - (order - 1), _knots.end(), [&]() { return *(_knots.end() - (order - 1) - 1); });
        tmin = _knots.front();
        tmax = _knots.back() - 0.00001f;
    }
    break;
    }

#pragma omp parallel for schedule(static)
    for (int i = 0; i < samples; ++i) {
        const float t = std::min(glm::mix(tmin, tmax, float(i) / (samples - 1)), tmax);    // *(_knots.size() - 2 * (order)+1);
        sampled[i]    = std::accumulate(points.begin(), points.end(), vertex2d{offset, {}, glm::u8vec4(0)},
                                     [&, cp = 0 ](vertex2d c, const vertex2d& next) mutable {
                                         const float cdb = cox_de_boor(t, cp++, order);
                                         c.position += next.position * cdb;
                                         c.color += glm::vec4(next.color) * cdb;
                                         return c;
                                     });
    }

    return sampled;
}

const std::vector<vertex2d>& bspline::points() const noexcept
{
    return _points;
}

vertex2d& bspline::operator[](const size_t idx)
{
    return _points[idx];
}

void bspline::highlight(size_t index, bool h)
{
    _points[index].color = h ? glm::u8vec4(255, 128, 0, 255) : _selection_points[index].color;
}

float bspline::cox_de_boor(float t, int cp, int o) const
{
    if (o == 1) return (std::clamp<float>(t, _knots[cp], _knots[cp + 1]) == t && t != _knots[cp + 1]) ? 1.f : 0.f;

    const float n1 = cox_de_boor(t, cp, o - 1);
    const float n2 = cox_de_boor(t, cp + 1, o - 1);

    const float nom1 = (t - _knots[cp]) * n1;
    const float nom2 = (_knots[cp + o] - t) * n2;

    const float den1 = _knots[cp + o - 1] - _knots[cp];
    const float den2 = _knots[cp + o] - _knots[cp + 1];

    const auto div = [](float n, float d) { return (n * d == 0) ? 0 : (n / d); };
    return div(nom1, den1) + div(nom2, den2);
}
}    // namespace v1
}    // namespace gfx