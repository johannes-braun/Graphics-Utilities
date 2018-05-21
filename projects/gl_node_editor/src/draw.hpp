#pragma once

#include <glm/glm.hpp>
#include <gfx/geometry.hpp>
#include <vector>
#include <gfx/spline.hpp>
#include <algorithm>

namespace gfx::draw
{
    struct info
    {
        std::vector<vertex2d> vertices;
        std::vector<index16>  indices;
    };
    enum class side
    {
        center = 0,
        inside,
        outside
    };

    inline std::vector<vertex2d> make_open_spline(const std::vector<vertex2d>& points, const int samples, const int order = 3)
    {
        gfx::bspline spline;
        for (const auto& p : points)
            spline.add(p);
        return spline.build(bspline::type::open, std::min(order, int(points.size())), samples);
    }

    inline std::vector<vertex2d> make_circle(const vertex2d& center, const float radius, const int samples)
    {
        std::vector<vertex2d> vertices(samples);
        for (int i=0; i<samples; ++i)
        {
            const float t = 2*glm::pi<float>() * i / float(samples - 2);
            vertex2d& v = vertices[i];
            v.position.x = radius * std::cos(t);
            v.position.y = radius * std::sin(t);
            v.position += center.position;
            v.uv ={ 0, 0 };
            v.color = center.color;
        }
        return vertices;
    }

    inline info as_quickfill(const std::vector<vertex2d>& line)
    {
        info fill_info;
        fill_info.vertices.resize(line.size()+1);
        fill_info.indices.resize(3 * line.size());

        vertex2d center;
        glm::vec4 col(0);
        for (const auto& v : line)
        {
            center.position += v.position;
            center.uv += v.uv;
            col += v.color;
        }
        center.position /= line.size();
        center.uv /= line.size();
        center.color = col / line.size();

        fill_info.vertices[0] = center;
        std::copy(line.begin(), line.end(), fill_info.vertices.begin()+1);

        for (int i=0; i<fill_info.vertices.size()-2; ++i)
        {
            fill_info.indices[3 * i + 0] = 0;
            fill_info.indices[3 * i + 1] = i+1;
            fill_info.indices[3 * i + 2] = i+2;
        }
        return fill_info;
    }

    inline info as_stroke(const std::vector<vertex2d>& line, const float radius, const side stroke_side = side::center)
    {
        info stroke_info;
        stroke_info.vertices.resize(2 * line.size());
        for (size_t i=0; i<line.size()-1; ++i)
        {
            glm::vec2 dir = line[i+1].position - line[i].position;
            dir =glm::normalize(glm::vec2{ dir.y, -dir.x });

            if (i > 2)
            {
                glm::vec2 dir2 = line[i-1].position - line[i-2].position;
                dir2 =glm::normalize(glm::vec2{ dir2.y, -dir2.x });
                dir = glm::normalize(dir2 + dir);
            }

            const float rad_outer = stroke_side == side::center ? radius : (stroke_side == side::outside ? 2 * radius : 0);
            const float rad_inner = stroke_side == side::center ? radius : (stroke_side == side::outside ? 0 : 2 * radius);

            gfx::vertex2d p = line[i];
            gfx::vertex2d p0 = p;
            p0.position += dir*rad_outer;
            gfx::vertex2d p1 = p;
            p1.position -= dir*rad_inner;

            stroke_info.vertices[2*i] = p0;
            stroke_info.vertices[2*i + 1] = p1;

            if (line.size()-1 == i+1)
            {
                p = line[i+1];
                p0 = p;
                p0.position += dir*rad_outer;
                p1 = p;
                p1.position -= dir*rad_inner;

                stroke_info.vertices[2 * i + 2] = p0;
                stroke_info.vertices[2 * i + 3] = p1;
            }
        }
        stroke_info.indices.resize((stroke_info.vertices.size() - 2) * 3);
        for (size_t i=0; i<stroke_info.vertices.size() - 2; ++i)
        {
            const auto m = 1-(i&1);
            stroke_info.indices[i * 3 + 0] = static_cast<uint16_t>(i + 1 - m);
            stroke_info.indices[i * 3 + 1] = static_cast<uint16_t>(i + m);
            stroke_info.indices[i * 3 + 2] = static_cast<uint16_t>(i + 2);
        }

        return stroke_info;
    }
}
