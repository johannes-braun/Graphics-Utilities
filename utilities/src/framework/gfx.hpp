#pragma once

#include <experimental/filesystem>
#include <jpu/log.hpp>

namespace gfx
{
    namespace files = std::experimental::filesystem;

   /* struct file
    {
        constexpr static const char* global_res_directory = SOURCE_DIRECTORY "/global/res";
        constexpr static const char* local_res_directory = "../res";

        file(const files::path& path) 
            : path(path) 
        {
            if (exists(global_res_directory / path))
                this->path = global_res_directory / path;
            else if (exists(local_res_directory / path))
                this->path = local_res_directory / path;
            else
                tlog_e("GFX Core") << "File not found: " << path;
        }

        operator const files::path&() const noexcept { return path; }

        files::path path;
    };*/
}