#include "../file.hpp"

namespace gfx
{

    file::file(const files::path& path)
        : path(path)
    {
        if (exists(global_res_directory / path))
            this->path = global_res_directory / path;
        else if (exists(local_res_directory / path))
            this->path = local_res_directory / path;
        else
            tlog_e("GFX Core") << "File not found: " << path;
    }

    file::operator const files::path&() const noexcept 
    {
        return path; 
    }
}