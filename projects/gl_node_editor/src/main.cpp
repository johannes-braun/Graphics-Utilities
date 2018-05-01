#include <gfx/window.hpp>
#include <gfx/imgui.hpp>
#include <gfx/spline.hpp>
#include <opengl/framebuffer.hpp>
#include <numeric>
#include <random>

#include <ui/draw_list.hpp>
#include "draw.hpp"

class node
{
public:
    void add_out_connection(const gfx::vertex2d& p)
    {
        _out_connections.emplace_back(p);

        gfx::bspline& circle = _out_connection_points.emplace_back();
        circle.add(gfx::vertex2d{ p.position + glm::vec2(-8, -8), p.color })
            .add(gfx::vertex2d{ p.position + glm::vec2(-8, 8), p.color })
            .add(gfx::vertex2d{ p.position + glm::vec2(8, 8), p.color })
            .add(gfx::vertex2d{ p.position + glm::vec2(8, -8), p.color });
    }
    void add_in_connection(const gfx::vertex2d& p)
    {
        _in_connections.emplace_back(p);

        gfx::bspline& circle = _in_connection_points.emplace_back();
        circle.add(gfx::vertex2d{ p.position + glm::vec2(-8, -8), p.color })
            .add(gfx::vertex2d{ p.position + glm::vec2(-8, 8), p.color })
            .add(gfx::vertex2d{ p.position + glm::vec2(8, 8), p.color })
            .add(gfx::vertex2d{ p.position + glm::vec2(8, -8), p.color });
    }

    const std::vector<gfx::vertex2d>& out_connections() const noexcept
    {
        return _out_connections;
    }
    const std::vector<gfx::vertex2d>& in_connections() const noexcept
    {
        return _in_connections;
    }

    const std::vector<gfx::bspline>& out_connection_circles() const noexcept
    {
        return _out_connection_points;
    }
    const std::vector<gfx::bspline>& in_connection_circles() const noexcept
    {
        return _in_connection_points;
    }

private:
    std::vector<gfx::vertex2d> _out_connections;
    std::vector<gfx::vertex2d> _in_connections;

    std::vector<gfx::bspline> _out_connection_points;
    std::vector<gfx::bspline> _in_connection_points;
};

int main()
{
    gfx::window_hints hints;
    hints[GLFW_SAMPLES] = 8;
    auto window = std::make_shared<gfx::window>(gfx::apis::opengl::name, "[gfx] Splines", 1280, 720, hints);
    gfx::imgui gui(window);

    gl::shader::set_include_directories(std::vector<gfx::files::path>{ "../shd", SOURCE_DIRECTORY "/global/shd" });

    std::vector<gfx::bspline> connection_circles;
    std::vector<node> nodes;
    {
        node& n = nodes.emplace_back();
        n.add_out_connection(gfx::vertex2d{ glm::vec2(50, 50), glm::u8vec4(255, 0, 0, 255) });
        n.add_out_connection(gfx::vertex2d{ glm::vec2(50, 100), glm::u8vec4(0, 255, 0, 255) });
        n.add_out_connection(gfx::vertex2d{ glm::vec2(50, 150), glm::u8vec4(0, 0, 255, 255) });

        node& n2 = nodes.emplace_back();
        n2.add_in_connection(gfx::vertex2d{ glm::vec2(400, 150), glm::u8vec4(0, 255, 0, 255) });
        n2.add_in_connection(gfx::vertex2d{ glm::vec2(400, 200), glm::u8vec4(0, 0, 255, 255) });
        n2.add_in_connection(gfx::vertex2d{ glm::vec2(400, 250), glm::u8vec4(255, 0, 0, 255) });
        n2.add_in_connection(gfx::vertex2d{ glm::vec2(400, 300), glm::u8vec4(0, 255, 0, 255) });
        n2.add_in_connection(gfx::vertex2d{ glm::vec2(400, 350), glm::u8vec4(255, 0, 0, 255) });
    }

    gl::buffer<gfx::vertex2d> node_connections_buffer(GL_DYNAMIC_STORAGE_BIT);

    gl::buffer<gfx::vertex2d> point_buffer(GL_DYNAMIC_STORAGE_BIT);

    gl::buffer<glm::mat4> globals_buffer(1, GL_DYNAMIC_STORAGE_BIT);
    globals_buffer[0] = glm::mat4(1.f);
    globals_buffer[0][0][0] = 2.0f / 1280;
    globals_buffer[0][1][1] = -2.0f / 720;
    globals_buffer[0][2] = glm::vec4{ 0.0f, 0.0f, -1.0f, 0.0f };
    globals_buffer[0][3] = glm::vec4{ -1.0f, 1.0f, 0.0f, 1.0f };
    globals_buffer.synchronize();

    gl::pipeline spline_pipeline;
    spline_pipeline[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("spline.vert");
    spline_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("spline.frag");

    std::default_random_engine eng;
    std::uniform_real_distribution<float> dst(0.f, 1.f);

    std::vector<gfx::bspline> connection_splines;

    /*std::vector<gfx::vertex2d> line = gfx::bspline()
        .add(gfx::vertex2d({ 200, 200 }, {}, { 255, 128, 0, 255 }))
        .add(gfx::vertex2d({ 400, 200 }, {}, { 255, 128, 0, 255 }))
        .add(gfx::vertex2d({ 400, 500 }, {}, { 255, 128, 0, 255 }))
        .add(gfx::vertex2d({ 200, 500 }, {}, { 255, 128, 0, 255 }))
        .build(gfx::bspline::type::open, 3, 32);

    enum class side
    {
        center = 0,
        inside,
        outside
    };
    const auto triangulate_stroke = [](const std::vector<gfx::vertex2d>& line, const float radius, const side stroke_side = side::center)
    {
        std::vector<gfx::vertex2d> triangulated(2*line.size());
        for (size_t i=0; i<line.size()-1; ++i)
        {
            glm::vec2 dir = line[i+1].position - line[i].position;
            dir =glm::normalize(glm::vec2{ dir.y, -dir.x });

            const float rad_outer = stroke_side == side::center ? radius : (stroke_side == side::outside ? 2 * radius : 0);
            const float rad_inner = stroke_side == side::center ? radius : (stroke_side == side::outside ? 0 : 2 * radius);

            gfx::vertex2d p = line[i];
            gfx::vertex2d p0 = p;
            p0.position += dir*rad_outer;
            gfx::vertex2d p1 = p;
            p1.position -= dir*rad_inner;

            triangulated[2*i] = p0;
            triangulated[2*i + 1] = p1;

            if (line.size()-1 == i+1)
            {
                p = line[i+1];
                p0 = p;
                p0.position += dir*rad_outer;
                p1 = p;
                p1.position -= dir*rad_inner;

                triangulated[2 * i + 2] = p0;
                triangulated[2 * i + 3] = p1;
            }
        }
        return triangulated;
    };
    const auto emulate_triangle_strip = [](size_t vertices)
    {
        std::vector<gfx::index32> indices((vertices-2) * 3);
        for(size_t i=0; i<vertices-2; ++i)
        {
            const auto m = i&1;
            indices[i * 3 + 0] = i + 1 - m;
            indices[i * 3 + 1] = i + m;
            indices[i * 3 + 2] = i + 2;
        }
        return indices;
    };

    const auto tri = triangulate_stroke(line, 8);
    const auto idx = emulate_triangle_strip(tri.size());*/
    
    struct draw_cmd
    {
        uint32_t idx_count;
        uint32_t vtx_count;
    };
    gfx::draw::info draw_info;
    std::vector<draw_cmd> commands;
    const auto clear_infos = [&]()
    {
        draw_info.indices.clear();
        draw_info.vertices.clear();
        commands.clear();
    };
    const auto submit_info = [&](const gfx::draw::info& inf)
    {
        draw_cmd cmd;
        cmd.idx_count = inf.indices.size();
        cmd.vtx_count = inf.vertices.size();
        commands.emplace_back(cmd);
        draw_info.indices.insert(draw_info.indices.end(), inf.indices.begin(), inf.indices.end());
        draw_info.vertices.insert(draw_info.vertices.end(), inf.vertices.begin(), inf.vertices.end());
    };

    gl::buffer<gfx::index16> index_buffer(GL_DYNAMIC_STORAGE_BIT);
    gl::buffer<gfx::vertex2d> vertex_buffer(GL_DYNAMIC_STORAGE_BIT);

    while (window->update())
    {
        gui.new_frame();
        glDisable(GL_CULL_FACE);
        gl::framebuffer::zero().clear(0, { 1, 0.95f, 1, 1 });
        spline_pipeline.bind();
        spline_pipeline.bind_uniform_buffer(0, globals_buffer);

        double mx, my;
        glfwGetCursorPos(*window, &mx, &my);
        const glm::vec2 mpos(mx, my);
        glm::u8vec4 col(255, 0, 0, 255);

        bool hover = false;
        static glm::u8vec4 hcol(255, 0, 0, 255);

        size_t pt = 0;
        const node* cn = nullptr;
        for (const auto& n : nodes)
        {
            pt = 0;
            for (const auto& oc : n.out_connections())
            {
                if (glm::distance2(oc.position, mpos) < float(8 << 5))
                {
                    col ={ 255, 255, 255, 255 };
                    hcol = oc.color;
                    hover = true;
                    cn = &n;
                    break;
                }
                ++pt;
            }
            if (cn) break;
        }

        static bool down = false;
        if (hover && !down && glfwGetMouseButton(*window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            gfx::bspline& spl = connection_splines.emplace_back();
            if (cn != nullptr)
            {
                const gfx::vertex2d& p = cn->out_connections()[pt];
                spl.add(p);
                spl.add(p);
                spl.add(p);
                spl.add(p);
                down = true;
            }
        }

        static bool connected = false;
        if (down && glfwGetMouseButton(*window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_RELEASE)
        {
            connected = false;
            gfx::bspline& spl = connection_splines.back();
            glm::vec2 ppos = mpos;
            for (const auto& n : nodes)
            {
                if (&n != &nodes.front())
                    for (const auto& cp : n.in_connections())
                    {
                        if (cp.color == hcol && glm::distance2(cp.position, ppos) < float(8 << 4))
                        {
                            ppos = cp.position;
                            connected = true;
                            break;
                        }
                    }
            }

            spl[3].position = ppos;

            const float dist = glm::distance(spl[0].position, spl[3].position);
            const float dstep = dist / 3.f;
            spl[1].position = spl[0].position + glm::vec2(dstep, 0);
            spl[2].position = spl[3].position - glm::vec2(dstep, 0);
        }
        if (glfwGetMouseButton(*window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            if (down && !connected)
                connection_splines.erase(std::prev(connection_splines.end()));
            down = false;
        }

        point_buffer.clear();

        for (int i=0; i<connection_splines.size(); ++i)
        {
            const auto sp = connection_splines[i].build(gfx::bspline::type::open, 4, 20);
            point_buffer.insert(point_buffer.end(), sp.begin(), sp.end());
        }
        for (const auto& n : nodes)
        {
            for (int i=0; i<n.in_connections().size(); ++i)
            {
                const auto& nic = n.in_connections()[i];
                const auto& nisp = n.in_connection_circles()[i].build(gfx::bspline::type::periodic, 4, 16);

                point_buffer.push_back(gfx::vertex2d{ nic.position, nic.color });
                point_buffer.insert(point_buffer.end(), nisp.begin(), nisp.end());
            }
            for (int i=0; i<n.out_connections().size(); ++i)
            {
                const auto& nic = n.out_connections()[i];
                const auto& nisp = n.out_connection_circles()[i].build(gfx::bspline::type::periodic, 4, 16);

                point_buffer.push_back(gfx::vertex2d{ nic.position, (&n == cn && pt == i) ? col : nic.color });
                point_buffer.insert(point_buffer.end(), nisp.begin(), nisp.end());
            }
        }

        spline_pipeline.bind_attribute(0, 0, point_buffer, 2, GL_FLOAT, offsetof(gfx::vertex2d, position));
        spline_pipeline.bind_attribute(1, 0, point_buffer, 4, GL_UNSIGNED_BYTE, true, offsetof(gfx::vertex2d, color));

        glLineWidth(6.f);
        glPointSize(6.f);
        for (int i=0; i<connection_splines.size(); ++i)
        {
            spline_pipeline.draw(GL_LINE_STRIP, 20, i*20);
            spline_pipeline.draw(GL_POINTS, 20, i*20);
        }

        int cbrb = connection_splines.size() * 20;
        for (const auto& n : nodes)
        {
            for (int i=0; i<n.in_connections().size(); ++i)
            {
                spline_pipeline.draw(GL_TRIANGLE_FAN, 17, cbrb);
                cbrb += 17;
            }
            for (int i=0; i<n.out_connections().size(); ++i)
            {
                spline_pipeline.draw(GL_TRIANGLE_FAN, 17, cbrb);
                cbrb += 17;
            }
        }

        glm::vec2 beg ={ 200, 200 };
        glm::vec2 end ={ mx, my };
        float len = glm::distance(beg, end);

        std::vector<gfx::vertex2d> control_points{
            gfx::vertex2d(beg,{},{ 255, 128, 0, 255 }),
            gfx::vertex2d(beg + glm::vec2(len/1.5, 0),{},{ 255, 128, 0, 255 }),
            gfx::vertex2d(end - glm::vec2(len/1.5, 0),{},{ 255, 128, 0, 255 }),
            gfx::vertex2d(end,{},{ 255, 128, 0, 255 }),
        };

        submit_info(gfx::draw::as_stroke(gfx::draw::make_open_spline(control_points, 40, 4), 2.f));
        submit_info(gfx::draw::as_quickfill(gfx::draw::make_circle({ {200, 200}, {},{255, 128, 0, 255 } }, 3.f, 16)));
        submit_info(gfx::draw::as_stroke(gfx::draw::make_circle({ {200, 200}, {},{ 25, 25, 25, 255 } }, 3.f, 16), 1.5f, gfx::draw::side::outside));

        submit_info(gfx::draw::as_quickfill(gfx::draw::make_circle({ { mx, my },{},{ 25, 25, 25, 255 } }, 3.f, 16)));

        vertex_buffer.clear();
        vertex_buffer.insert(vertex_buffer.begin(), draw_info.vertices.begin(), draw_info.vertices.end());
        index_buffer.clear();
        index_buffer.insert(index_buffer.begin(), draw_info.indices.begin(), draw_info.indices.end());

        spline_pipeline.bind_attribute(0, 0, vertex_buffer, 2, GL_FLOAT, offsetof(gfx::vertex2d, position));
        spline_pipeline.bind_attribute(1, 0, vertex_buffer, 4, GL_UNSIGNED_BYTE, true, offsetof(gfx::vertex2d, color));

        uint32_t voff = 0, ioff = 0;
        for (int i=0; i<commands.size(); ++i)
        {
            const auto& cmd = commands[i];
            spline_pipeline.draw(GL_TRIANGLES, index_buffer, GL_UNSIGNED_SHORT, cmd.idx_count, ioff, voff);
            voff += cmd.vtx_count;
            ioff += cmd.idx_count;
        }
        clear_infos();

        gui.render();
    }
}