#pragma once

#include "geometry.hpp"

#include <experimental/filesystem>
#include <variant>
#include <vector>
#include <map>
#include <string>
#include <optional>

#include <stb_image.h>
#include <stb_image_write.h>
#include <nanosvg.h>
#include <nanosvgrast.h>
#include <tinyfd/tinyfiledialogs.h>

namespace gfx
{
    namespace files = std::experimental::filesystem;

    enum class bits
    {
        b8 = 8,
        b16 = 16,
        b32 = 32,
    };


    struct file
    {
        constexpr static const char* global_res_directory = SOURCE_DIRECTORY "/global/res";
        constexpr static const char* local_res_directory = "../res";

        operator const files::path&() const noexcept;
        files::path path;

        file() = default;
        file(const files::path& path);

        static std::optional<file> open_dialog(const std::string& title, const files::path& default_path);
        static std::optional<file> open_dialog(const std::string& title, const files::path& default_path, const std::vector<std::string>& filters);
        static std::optional<file> open_dialog(const std::string& title, const files::path& default_path, const std::vector<std::string>& filters, const std::string& filter_description);
        static std::vector<file>   open_dialog_multi(const std::string& title, const files::path& default_path);
        static std::vector<file>   open_dialog_multi(const std::string& title, const files::path& default_path, const std::vector<std::string>& filters);
        static std::vector<file>   open_dialog_multi(const std::string& title, const files::path& default_path, const std::vector<std::string>& filters, const std::string& filter_description);

        static std::optional<file> save_dialog(const std::string& title, const files::path& default_path);
        static std::optional<file> save_dialog(const std::string& title, const files::path& default_path, const std::vector<std::string>& filters);
        static std::optional<file> save_dialog(const std::string& title, const files::path& default_path, const std::vector<std::string>& filters, const std::string& filter_description);
    };


    struct image_info
    {
        uint32_t width      = 0;
        uint32_t height     = 0;
        uint16_t channels   = 0;
    };
    using image_data = std::variant<std::vector<float>, std::vector<uint16_t>, std::vector<uint8_t>>;
    struct image_file : file
    {
        static image_info info(const files::path& path) noexcept;
        static void save_png(const files::path& path, uint32_t width, uint32_t height, uint16_t channels, const uint8_t* data);
        static void save_bmp(const files::path& path, uint32_t width, uint32_t height, uint16_t channels, const uint8_t* data);
        static void save_tga(const files::path& path, uint32_t width, uint32_t height, uint16_t channels, const uint8_t* data);
        static void save_jpg(const files::path& path, uint32_t width, uint32_t height, uint16_t channels, const uint8_t* data, int quality = 95);
        static void save_hdr(const files::path& path, uint32_t width, uint32_t height, uint16_t channels, const float* data);

        image_file(const files::path& path, bits channel_bits, uint16_t channels);
        image_file(const files::path& svg, float raster_scale);
        void* bytes() const noexcept;
        int pixel_count() const noexcept;

        uint32_t width;
        uint32_t height;
        uint16_t channels;
        bits channel_bits;
        image_data data;

    private:
        void* _raw = nullptr;
    };


    struct audio_file : file
    {
        audio_file(const files::path& path);
        void* bytes() const noexcept;

        uint16_t channels;
        uint32_t sample_rate;
        size_t data_size;
        uint32_t format;
        std::vector<uint8_t> data;

    private:
        void* _raw = nullptr;
    };


    struct scene_file : file
    {
        struct material { };

        struct mesh
        {
            std::string                 name;
            const material*             material;
            std::vector<gfx::index32>   indices;
            std::vector<gfx::vertex>    vertices;
            gfx::transform              transform;
        };

        std::vector<mesh>       meshes;
        std::vector<material>   materials;

        scene_file(const files::path& path);
    };
}