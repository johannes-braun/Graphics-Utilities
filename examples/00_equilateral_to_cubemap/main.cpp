#include <array>
#include <cmath>
#include <filesystem>
#include <gfx/gfx.hpp>
#include <gfx/log.hpp>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

std::array<std::string, 6> sides{"posx", "negx", "negy", "posy", "posz", "negz"};

glm::mat4 make_matrix(const glm::vec3 look_dir, const glm::vec3 up)
{
    return glm::mat4(glm::mat3{normalize(cross(look_dir, up)), up, -look_dir});
}

const std::array<glm::mat4, 6> matrices{
    make_matrix({1, 0, 0}, {0, 1, 0}),  make_matrix({-1, 0, 0}, {0, 1, 0}), make_matrix({0, 1, 0}, {0, 0, -1}),
    make_matrix({0, -1, 0}, {0, 0, 1}), make_matrix({0, 0, 1}, {0, 1, 0}),  make_matrix({0, 0, -1}, {0, 1, 0}),
};

const glm::mat4 projection = glm::perspective(glm::radians(90.f), 1.f, 0.01f, 100.f);

namespace fs = std::filesystem;
int main(const int argc, char** argv)
{
    const int resolution = 1024;

    gfx::file path;
    fs::path  dest;
    if (argc < 2)
    {
        if (const auto src = gfx::file::open_dialog("Open HDRI", "./", {"*.hdr"}, {"HDRI Images"}))
            path = src.value();
        else
            return 0;
    }
    else
    {
        path = gfx::file(argv[1]);
    }

    if (const auto dst = gfx::file::folder_dialog("Select Target Folder", ".")) { dest = dst.value(); }
    else
    {
        dest = path;
        dest.replace_extension("");
    }

    fs::path ppng = dest / "png";
    fs::path phdr = dest / "hdr";
    create_directories(phdr);
    create_directories(ppng);

    struct freer
    {
        void operator()(void* v) const { free(v); }
    };
    if (gfx::image_file hdri = gfx::image_file(path, gfx::bits::b32, 3); hdri.bytes())
    {
        gfx::ilog << "Successfully loaded " << path.path;
        const auto getter = [&](glm::vec3 dir) {
            dir                 = normalize(dir);
            const float angle_a = std::atan2(dir.x, dir.z) / (2.0f * glm::pi<float>());
            const float angle_b = 1 - (0.5f - asin(dir.y) / glm::pi<float>());

            const auto idx = (int(angle_b * hdri.height) + 10000 * hdri.height) % hdri.height * hdri.width
                             + (int(angle_a * hdri.width) + 10000 * hdri.width) % hdri.width;
            return reinterpret_cast<glm::vec3*>(hdri.bytes())[idx];
        };

#pragma omp parallel for schedule(static)
        for (int side = 0; side < 6; ++side)
        {
            const auto             matrix = inverse(projection * matrices[side]);
            std::vector<glm::vec3> new_image(resolution * resolution);
            for (int y = 0; y < resolution; ++y)
            {
                for (int x = 0; x < resolution; ++x)
                {
                    glm::vec4 dir =
                        matrix * glm::vec4(static_cast<float>(x) / resolution * 2 - 1, static_cast<float>(y) / resolution * 2 - 1, 0, 1);
                    new_image[y * resolution + x] = getter(dir / dir[3]);
                }
            }

#pragma omp critical
            {
                const auto target = phdr.string() + "/" + sides[side] + ".hdr";
                gfx::ilog << "Writing " << target << " ...";
                gfx::image_file::save_hdr(target, resolution, resolution, 3, reinterpret_cast<float*>(new_image.data()));
            }

            std::vector<glm::u8vec3> png(resolution * resolution);
            for (int i = 0; i < png.size(); ++i)
                png[i] = static_cast<glm::u8vec3>(
                    glm::clamp(glm::pow(new_image[i], glm::vec3(1.f / 1.75f)) * glm::vec3(255), glm::vec3(0), glm::vec3(255)));

#pragma omp critical
            {
                const auto target = ppng.string() + "/" + sides[side] + ".png";
                gfx::ilog << "Writing " << target << " ...";
                gfx::image_file::save_png(target, resolution, resolution, 3, reinterpret_cast<const uint8_t*>(png.data()));
            }
        }
    }
    else
    {
        gfx::elog << "Failed to load image from " << path.path;
    }

    gfx::hlog("success") << "Press [ENTER] to quit.";
    std::cin.ignore();
}