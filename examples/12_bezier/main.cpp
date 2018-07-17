#include <gfx/gfx.hpp>
#include <random>
#include <utility>

gfx::vertex3d lerp(const gfx::vertex3d& a, const gfx::vertex3d& b, float t)
{
    auto va     = a;
    va.position = mix(a.position, b.position, t);
    va.uv       = mix(a.uv, b.uv, t);
    va.normal   = normalize(mix(a.normal, b.normal, t));
    return va;
}

template<typename Iter, typename LerpFun,
         typename = std::enable_if_t<
             /* lerp(value_type, value_type, float) -> value_type */
             std::is_convertible_v<std::decay_t<decltype(*std::declval<Iter>())>,
                                   decltype(std::declval<LerpFun>()(*std::declval<Iter>(), *std::declval<Iter>(), float()))>>>
auto de_casteljau(Iter begin, Iter end, float t, LerpFun&& lerp_fun)
{
    std::vector<std::decay_t<decltype(*std::declval<Iter>())>> vs(begin, end);

    while (vs.size() > 1)
	{
        for (int i = 0; i < vs.size() - 1; ++i) {
            vs[i] = lerp_fun(vs[i], vs[i + 1], t);
        }
		vs.pop_back();
	}
    return vs[0];
}

gfx::vertex3d de_casteljau(const std::initializer_list<gfx::vertex3d>& vertices, float t)
{
    return de_casteljau(std::begin(vertices), std::end(vertices), t, lerp);
}

int main()
{
    gfx::context_options opt;
    opt.window_title = "[12] Bezier";
    opt.debug        = true;
    auto context     = gfx::context::create(opt);
    context->make_current();

    std::mt19937                   gen(111);
    std::uniform_real_distribution dist(0.f, 10.f);
    gfx::hbuffer<gfx::vertex3d>    vertices;
    for (int i = 0; i < 5; ++i) {
        vertices.emplace_back(glm::vec3(dist(gen), dist(gen), dist(gen)));
    }

    constexpr int               resolution = 64;
    gfx::hbuffer<gfx::vertex3d> spline;
    for (int i = 0; i < resolution; ++i)
        spline.push_back(de_casteljau(vertices.begin(), vertices.end(), i / static_cast<float>(resolution), lerp));

    auto input = std::make_shared<gfx::vertex_input>();
    input->set_assembly(gfx::topology::line_strip);
    input->add_attribute(0, gfx::rgb32f, offsetof(gfx::vertex3d, position));
    input->set_binding_info(0, sizeof(gfx::vertex3d), gfx::input_rate::vertex);

    auto state                   = std::make_shared<gfx::state_info>();
    state->rasterizer.line_width = 4.f;

    gfx::graphics_pipeline pipeline(input, state);
    pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::text, "12_bezier/line.vert"));
    pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::text, "12_bezier/line.frag"));

    gfx::camera                     camera;
    gfx::camera_controller          controller;
    gfx::hbuffer<gfx::camera::data> camera_buffer{camera.info()};

    gfx::descriptor_set main_set;
    main_set.set(gfx::descriptor_type::uniform_buffer, 0, camera_buffer);

    gfx::commands cmd;

    gfx::clear_value clear[]{glm::vec4(1, 1, 1, 1), gfx::depth_stencil{0.f, 0}};
    cmd.begin_pass(clear, std::size(clear), mygl::framebuffer::zero);
    cmd.bind_descriptors(&main_set, 1);
    cmd.bind_pipeline(pipeline);
    cmd.bind_vertex_buffer(0, spline);
    cmd.draw(spline.size());

    while (context->run()) {
        controller.update(camera);
        camera_buffer[0] = camera.info();

        cmd.execute();
    }
}