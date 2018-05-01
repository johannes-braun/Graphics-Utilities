#include <memory>

#include <opengl/texture.hpp>
#include <opengl/framebuffer.hpp>

#include <gfx/data/bvh.hpp>
#include <gfx/file.hpp>
#include <gfx/geometry.hpp>
#include <gfx/window.hpp>
#include <gfx/camera.hpp>

int main()
{
    gl::shader::set_include_directories(std::vector<gfx::files::path>{ "../shd", SOURCE_DIRECTORY "/global/shd" });
    auto main_window = std::make_shared<gfx::window>(gfx::apis::opengl::name, "CPU Raytracer", 480, 320);

    gfx::camera camera;
    gfx::camera_controller controller(main_window);

    gfx::scene_file geom("bunny.dae");
    std::vector<gfx::vertex3d> vertices(geom.meshes.begin()->vertices.begin(), geom.meshes.begin()->vertices.end());
    std::vector<gfx::index32> indices(geom.meshes.begin()->indices.begin(), geom.meshes.begin()->indices.end());

    gfx::bvh<3> gen_bvh(gfx::shape::triangle, gfx::bvh_mode::persistent_iterators);
    gen_bvh.sort(indices.begin(), indices.end(), [&](uint32_t index) { return vertices[index].position; });

    const auto texture = std::make_shared<gl::texture>(GL_TEXTURE_2D, 480, 320, GL_RGBA32F, 1);
    gl::framebuffer framebuffer;
    framebuffer[GL_COLOR_ATTACHMENT0] = texture;

    int y = 0;
    while (main_window->update())
    {
        controller.update(camera);
        const glm::mat4 camera_matrix = inverse(camera.projection() * glm::mat4(glm::mat3(camera.view())));

        for (int x = 0; x < 480; ++x)
        {
            glm::vec2 uv = 2.f * glm::vec2(x / 480.f, y / 320.f) - 1.f;
            glm::vec3 direction = normalize(glm::vec3(camera_matrix * glm::vec4(uv, 0, 1)));
            glm::vec3 origin = glm::vec3(camera.transform.position);

            const auto hit = gen_bvh.intersect_ray(origin, direction, 100000.f);
                        
            glm::vec4 color(0, 0, 0, 1);
            
            if (hit)
            {
                const glm::vec3 normal = normalize([&, begin = hit.indices.begin()]() mutable {
                    return vertices[indices[(begin++)->index]].normal + vertices[indices[(begin++)->index]].normal + vertices[indices[(begin++)->index]].normal;
                }());
                color = glm::vec4(normal, 1);
            }

            texture->at(x, y, 0) = color;
        }
        y = (y + 1) % 320;

        framebuffer.blit(nullptr, GL_COLOR_BUFFER_BIT);
    }

    return 0;
}