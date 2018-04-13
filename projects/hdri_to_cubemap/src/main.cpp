#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <filesystem>
#include <cmath>
#include <array>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <iostream>
#include <jpu/log.hpp>
#include "tinyfd/tinyfiledialogs.h"

std::array<std::string, 6> sides {
	"posx", "negx", "negy", "posy", "posz", "negz"
};

glm::mat4 shadowMat(const glm::vec3 look_dir, const glm::vec3 up)
{
	const glm::vec3 side_dir = glm::normalize(glm::cross(look_dir, up));
	return glm::mat4(glm::mat3{ side_dir,up,-look_dir });
}

const std::array<glm::mat4, 6> matrices{
	shadowMat(glm::vec3{ 1,0,0 },glm::vec3{ 0,1,0 }),
	shadowMat(glm::vec3{ -1,0,0 },glm::vec3{ 0,1,0 }),
	shadowMat(glm::vec3{ 0,1,0 },glm::vec3{ 0,0,-1 }),
	shadowMat(glm::vec3{ 0,-1,0 },glm::vec3{ 0,0,1 }),
	shadowMat(glm::vec3{ 0,0,1 },glm::vec3{ 0,1,0 }),
	shadowMat(glm::vec3{ 0,0,-1 },glm::vec3{ 0,1,0 }),
};

const glm::mat4 projection = glm::perspective(glm::radians(90.f), 1.f, 0.01f, 100.f);

namespace fs = std::experimental::filesystem;
int main(int argc, char** argv)
{
    int resolution = 1024;
    //if(!fs::exists("settings.xml"))
    //{
    //    doc.append_child("settings").append_child("resolution").append_attribute("value").set_value(resolution);
    //    if(!doc.save_file("settings.xml"))
    //        log_w << "Could not save settings file.";
    //}
    //doc.load_file("settings.xml");
    //resolution = doc.child("settings").child("resolution").attribute("value").as_int(resolution);

    fs::path path;
    fs::path dest;
    if(argc < 2)
    {
        constexpr const char *fs[1] = { "*.hdr" };
        if (const auto src = tinyfd_openFileDialog("Open HDRI", "./", 1, fs, "HDRI", false))
        {
            path = src;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        path = argv[1];
    }

    if (const auto dst = tinyfd_selectFolderDialog("Select Target Folder", "."))
    {
        dest = dst;
    }
    else
    {
        dest = path;
        dest.replace_extension("");
    }

	fs::path ppng = dest / "png";
	fs::path phdr = dest / "hdr";
	fs::create_directories(phdr);
	fs::create_directories(ppng);

    stbi_hdr_to_ldr_gamma(1.f);
    int w, h;
    struct freer { void operator()(void* v) const { free(v); } };
	if(auto hdri = std::unique_ptr<float[], freer>(stbi_loadf(path.string().c_str(), &w, &h, nullptr, STBI_rgb)))
	{
        log_i << "Successfully loaded " << path;
		const auto getter = [&](glm::vec3 dir) {
			dir = glm::normalize(dir);
			float angle_a = std::atan2(dir.x, dir.z) / (2.0f * glm::pi<float>());
			float angle_b = 1-(0.5f - asin(dir.y) / glm::pi<float>());

			auto idx = (int(angle_b * h) + 10000 * h)%h * w + (int(angle_a * w) + 10000 * w) % w;
			return reinterpret_cast<glm::vec3*>(hdri.get())[idx];
		};

#pragma omp parallel for
		for(int side = 0; side < 6; ++side)
		{
			const auto matrix = glm::inverse(projection * matrices[side]);
			std::vector<glm::vec3> new_image(resolution * resolution);
			for (int y = 0; y < resolution; ++y)
			{
				for (int x = 0; x < resolution; ++x)
				{
					glm::vec4 dir = matrix * glm::vec4(static_cast<float>(x) / resolution * 2 - 1, static_cast<float>(y) / resolution * 2 - 1, 0, 1);
					new_image[y * resolution + x] = getter(dir / dir[3]);
				}
			}

#pragma omp critical 
			{
                const auto target = phdr.string() + "/" + sides[side] + ".hdr";
                log_i << "Writing " << target << " ...";
				stbi_write_hdr(target.c_str(), resolution, resolution, 3, reinterpret_cast<float*>(new_image.data()));
			}

			std::vector<glm::u8vec3> png(resolution * resolution);
			for (int i = 0; i < png.size(); ++i) 
				png[i] = static_cast<glm::u8vec3>(glm::clamp(new_image[i] * glm::vec3(255), glm::vec3(0), glm::vec3(255)));

#pragma omp critical 
			{
                const auto target = ppng.string() + "/" + sides[side] + ".png";
                log_i << "Writing " << target << " ...";
				stbi_write_png(target.c_str(), resolution, resolution, 3, png.data(), 0);
			}
		}
	}
    else
    {
        log_e << "Failed to load image from " << path;
    }

    std::cout << "\nPress [ENTER] to quit.";
    std::cin.ignore();
}