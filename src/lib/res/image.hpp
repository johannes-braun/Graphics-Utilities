#pragma once

#include <filesystem>
#include <jpu/memory>
#include <opengl/texture.hpp>

namespace res
{
    namespace filesystem = std::experimental::filesystem;

    struct stbi_free_deleter { void operator()(void* d) const { free(d); } };
    using stbi_data = std::unique_ptr<void, stbi_free_deleter>;

    jpu::ref_ptr<gl::texture> load_texture(const filesystem::path& path, GLenum internal_format, GLenum format, GLenum type);
    int save_texture(gl::texture* texture, const filesystem::path& path, GLenum format, int level = 0, int jpg_quality = 95);
}