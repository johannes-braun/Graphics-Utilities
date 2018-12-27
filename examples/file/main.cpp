#include <cassert>
#include <filesystem>
#include <future>
#include <variant>

namespace gfx {
inline namespace v1 {
namespace experimental {

struct scene
{};

struct sound
{};

struct picture
{};

enum class file_type
{
    scene,
    sound,
    picture
};

struct file
{
    file& load(file_type type, const std::filesystem::path& path)
    {
        switch (type)
        {
        case file_type::scene: break;
        case file_type::sound: break;
        case file_type::picture: break;
        default: assert(false);
        }
        return *this;
    }

private:
    bool is_data_valid() const
    {
        struct validity_visitor
        {
            bool operator()(std::monostate) const noexcept { return true; }
            bool operator()(std::unique_ptr<scene const> const& s) const noexcept { return s != nullptr; }
            bool operator()(std::unique_ptr<sound const> const& s) const noexcept { return s != nullptr; }
            bool operator()(std::unique_ptr<picture const> const& p) const noexcept { return p != nullptr; }
        } is_valid;
        return std::visit(is_valid, _data);
    }

    template<typename T>
    using uptr = std::unique_ptr<T>;
    std::variant<std::monostate, uptr<scene const>, uptr<sound const>, uptr<picture const>> _data;
};

}    // namespace experimental
}    // namespace v1
}    // namespace gfx

int main(int argc, char** argv)
{
    gfx::experimental::file file;
    file.load(gfx::experimental::file_type::scene, "test.png");
}
