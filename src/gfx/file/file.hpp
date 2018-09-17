#pragma once

#include "../math/geometry.hpp"

//#if __has_include(<filesystem>)
#include <filesystem>
//#else
//#include <experimental/filesystem>
//#if defined(__cpp_lib_experimental_filesystem)
// namespace std
//{
// namespace filesystem = experimental::filesystem;
//}
//#endif
//#endif

#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace gfx {
namespace files = std::filesystem;

enum class bits
{
    b8  = 8,
    b16 = 16,
    b32 = 32,
};

enum class popup_icon
{
    info,
    warning,
    error
};

enum class msg_icon
{
    info,
    warning,
    error,
    question
};

enum class msg_type
{
    ok,
    ok_cancel,
    yes_no,
    yes_no_cancel
};

enum class msg_result
{
    ok,
    cancel,
    yes,
    no
};

struct file
{
    constexpr static const char* local_res_directory = "../res";

                operator const files::path&() const noexcept;
    files::path path;

    file() = default;
    file(const files::path& path);

    static std::optional<file> open_dialog(const std::string& title, const files::path& default_path);
    static std::optional<file> open_dialog(const std::string& title, const files::path& default_path,
                                           const std::vector<std::string>& filters);
    static std::optional<file> open_dialog(const std::string& title, const files::path& default_path,
                                           const std::vector<std::string>& filters, const std::string& filter_description);
    static std::vector<file>   open_dialog_multi(const std::string& title, const files::path& default_path);
    static std::vector<file>   open_dialog_multi(const std::string& title, const files::path& default_path,
                                                 const std::vector<std::string>& filters);
    static std::vector<file>   open_dialog_multi(const std::string& title, const files::path& default_path,
                                                 const std::vector<std::string>& filters, const std::string& filter_description);

    static std::optional<file> save_dialog(const std::string& title, const files::path& default_path);
    static std::optional<file> save_dialog(const std::string& title, const files::path& default_path,
                                           const std::vector<std::string>& filters);
    static std::optional<file> save_dialog(const std::string& title, const files::path& default_path,
                                           const std::vector<std::string>& filters, const std::string& filter_description);

    static std::optional<files::path> folder_dialog(const std::string& title, const files::path& default_path);

    static void       popup(const std::string& title, const std::string& message, popup_icon icon);
    static msg_result message(const std::string& title, const std::string& message, msg_type type, msg_icon icon, int default_button = 0);
};

struct image_info
{
    uint32_t width    = 0;
    uint32_t height   = 0;
    uint16_t channels = 0;
};
using image_data = std::variant<std::vector<float>, std::vector<uint16_t>, std::vector<uint8_t>>;
struct image_file : file
{
    static image_info info(const files::path& path) noexcept;
    static void       save_png(const files::path& path, uint32_t width, uint32_t height, uint16_t channels, const uint8_t* data);
    static void       save_bmp(const files::path& path, uint32_t width, uint32_t height, uint16_t channels, const uint8_t* data);
    static void       save_tga(const files::path& path, uint32_t width, uint32_t height, uint16_t channels, const uint8_t* data);
    static void       save_jpg(const files::path& path, uint32_t width, uint32_t height, uint16_t channels, const uint8_t* data,
                               int quality = 95);
    static void       save_hdr(const files::path& path, uint32_t width, uint32_t height, uint16_t channels, const float* data);
    image_file() = default;
    image_file(const files::path& path, bits channel_bits, uint16_t channels);
    image_file(const files::path& svg, float raster_scale);
    image_file(image_file&& other) noexcept;
    image_file& operator=(image_file&& other) noexcept;

    void* bytes() const noexcept;
    int   pixel_count() const noexcept;

    uint32_t   width;
    uint32_t   height;
    uint16_t   channels;
    bits       channel_bits;
    image_data data;

private:
    void* _raw = nullptr;
};

struct audio_file : file
{
    audio_file(const files::path& path);
    void* bytes() const noexcept;

    uint16_t           channels;
    uint32_t           sample_rate;
    size_t             data_size;
    uint8_t            bits;
    uint32_t           format;
    std::vector<float> data;

private:
    void* _raw = nullptr;
};

struct scene_file : file
{
    struct material
    {
        std::string     name;
        glm::vec4       color_diffuse {0};
        glm::vec4       color_emissive {0};
        glm::vec4       color_reflective {0};
        glm::vec4       color_specular {0};
        glm::vec4       color_transparent {0};
        gfx::image_file texture_diffuse;
        gfx::image_file texture_bump;
    };

    /*struct mesh
    {
        std::string                name;
        const material*            material_ptr;
        uint32_t                   material_index;
        std::vector<gfx::index32>  indices;
        std::vector<gfx::vertex3d> vertices;
        gfx::transform             transform;
    };

    std::vector<mesh>     meshes;
    std::vector<material> materials;*/


    mesh3d                              mesh;
    std::vector<material>               materials;
    std::unordered_map<submesh3d*, u32> mesh_material_indices;

    scene_file(const files::path& path);
};
}    // namespace gfx
