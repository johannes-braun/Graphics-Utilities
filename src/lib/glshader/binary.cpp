#include "binary.hpp"
#include <iterator>
#include <sstream>
#include <tuple>
#include <fstream>
#include <glad/glad.h>
#include "preprocessor.hpp"
#include "jpu/impl/log/log.hpp"
#include <snappy.h>
#include "config.hpp"

namespace glshader
{
    namespace fs = std::experimental::filesystem;

    struct shader_file_header
    {
        shader_format type;             // SPRV or GBIN.
        uint32_t version;               // version as 100*maj + 10*min + 1*rev.

        uint32_t info_tag;              // INFO
        uint32_t dependencies_length;   // byte size of dependencies
        uint32_t dependencies_count;
        uint32_t binary_format;         // If binary received from opengl, this contains the binary format GLenum
        uint32_t binary_length;         // byte size of binary

        uint32_t data_tag;              // DATA
    };

    std::string cmd(const fs::path& glslc_path, const std::string& args)
    {
        return glslc_path.string() + " " + args + " ";
    }

    std::stringstream call(const fs::path& glslc_path, const std::string& args)
    {
        std::unique_ptr<FILE, decltype(&_pclose)> file(_popen(cmd(glslc_path, args).c_str(), "r"), &_pclose);
        std::stringstream str;
        for (char line[32]; fgets(line, sizeof line, file.get());)
            str << line;
        return str;
    }

    std::vector<std::string> get_dependencies(const fs::path& glslc_path, std::experimental::filesystem::path path)
    {
        auto str = call(glslc_path, std::string(" ") + glslc_default_flags + " -M " + path.string() + "");
        return std::vector<std::string>{ ++std::istream_iterator<std::string>(str), std::istream_iterator<std::string>() };
    }

    std::tuple<std::vector<uint8_t>, std::vector<fs::path>> load_spirv(const std::experimental::filesystem::path& file_path,
        const std::vector<std::experimental::filesystem::path>& include_directories,
        const std::vector<definition>& definitions)
    {
        std::vector<std::string> dependencies = get_dependencies(glslc_location, file_path);
        std::vector<uint8_t> spirv;
        std::string includes;
        for (auto&& inc : include_directories)
        {
            includes += " -I" + fs::absolute(inc).string();
        }
        std::string defines;
        for (auto&& def : definitions)
        {
            defines += " -D'" + def.name;
            if (def.info.parameters.size() == 0)
            {
                defines += "(";
                defines += def.info.parameters.at(0);
                for (int i = 1; i < def.info.parameters.size(); ++i)
                {
                    defines += "," + def.info.parameters.at(i);
                }
                defines += ")";
            }
            if (def.info.replacement != "")
            {
                defines += "=" + def.info.replacement;
            }
            defines += "\' ";
        }
        system(cmd(glslc_location, std::string(" -std=450core -o ") + glslc_temp_file_name + " -mfmt=bin" + includes + defines + " " + fs::absolute(file_path).string() + "").c_str());
        std::ifstream tmp(glslc_temp_file_name, std::ios::in | std::ios::binary);
        tmp.ignore(std::numeric_limits<std::streamsize>::max());
        const std::streamsize length = tmp.gcount();
        tmp.clear();   //  Since ignore will have set eof.
        tmp.seekg(0, std::ios_base::beg);
        spirv.resize(length);
        tmp.read(reinterpret_cast<char*>(spirv.data()), length);
        tmp.close();
        fs::remove(glslc_temp_file_name);
        return std::make_tuple(spirv, std::vector<fs::path>(dependencies.begin(), dependencies.end()));
    }

    std::tuple<std::vector<uint8_t>, uint32_t, std::vector<fs::path>> load_glbin(const std::experimental::filesystem::path& file_path,
        const std::vector<std::experimental::filesystem::path>& include_directories,
        const std::vector<definition>& definitions)
    {
        auto proc = preprocessor::preprocess_file(file_path, include_directories, definitions);

        // Precompile shader
        auto src = opengl_prefix + proc.contents + opengl_postfix;
        auto src_ptr = src.data();

        const auto shader_type = [&, extension = file_path.extension()] {
            if (extension == ".vert")
                return GL_VERTEX_SHADER;
            if (extension == ".frag")
                return GL_FRAGMENT_SHADER;
            if (extension == ".geom")
                return GL_GEOMETRY_SHADER;
            if (extension == ".tesc")
                return GL_TESS_CONTROL_SHADER;
            if (extension == ".tese")
                return GL_TESS_EVALUATION_SHADER;
            if (extension == ".comp")
                return GL_COMPUTE_SHADER;
            return 0;
        }();

        auto id = glCreateShaderProgramv(shader_type, 1, &src_ptr);
        if (int success = 0; glGetProgramiv(id, GL_LINK_STATUS, &success), !success)
        {
            int log_length;
            glGetProgramiv(id, GL_INFO_LOG_LENGTH, &log_length);
            std::string log(log_length, ' ');
            glGetProgramInfoLog(id, log_length, &log_length, log.data());
            glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, id, GL_DEBUG_SEVERITY_HIGH, -1, log.c_str());
            glDeleteProgram(id);

            throw std::runtime_error("Program linking failed: " + log);
        }

        std::vector<uint8_t> target;
        int length;
        glGetProgramiv(id, GL_PROGRAM_BINARY_LENGTH, &length);
        target.resize(length);
        GLenum fmt;
        glGetProgramBinary(id, static_cast<int>(target.capacity()), &length, &fmt, target.data());
        glDeleteProgram(id);
        std::vector<fs::path> deps;
        deps.push_back(file_path);
        deps.insert(deps.end(), proc.dependencies.begin(), proc.dependencies.end());
        return std::make_tuple(std::move(target), fmt, deps);
    }



    binary_shader load_binary_shader(shader_format type,
        const std::experimental::filesystem::path& src,
        const std::vector<std::experimental::filesystem::path>& include_directories,
        const std::vector<definition>& definitions,
        bool force)
    {
        fs::path dst = fs::absolute(src);
        const auto hash = std::hash<std::string>()(dst.string());
        if(!fs::exists(shader_cache_dir))
        {
            fs::create_directories(shader_cache_dir);
        }
        dst = fs::path(shader_cache_dir) / (std::to_string(hash) + ".shader");
        uint32_t format = 0;
        std::vector<uint8_t> bin;

        bool reload = false;
        if (!force && fs::exists(dst))
        {
            shader_file_header header;
            std::ifstream input(dst, std::ios::binary);
            input.read(reinterpret_cast<char*>(&header), sizeof(header));

            if (header.type != type || header.info_tag != make_tag("INFO") || header.data_tag != make_tag("DATA"))
                reload = true;

            if (!reload)
            {
                /*std::vector<char> dep_data(header.dependencies_length);
                input.read(dep_data.data(), header.dependencies_length);*/

                for (int i = 0; i < static_cast<int>(header.dependencies_count); ++i)
                {
                    std::time_t last_write{ 0 };
                    uint32_t slength{ 0 };

                    input.read(reinterpret_cast<char*>(&last_write), sizeof(last_write));
                    input.read(reinterpret_cast<char*>(&slength), sizeof(slength));
                    std::string dep_file;
                    dep_file.resize(slength);
                    input.read(dep_file.data(), slength);

                    // Treat missing dependency as non-needed.
                    if (!fs::exists(dep_file))
                        continue;

                    if (std::chrono::system_clock::to_time_t(fs::last_write_time(dep_file)) != last_write)
                    {
                        reload = true;
                        break;
                    }
                }
            }
            if (!reload)
            {
                log_i << "Load cached shader binary...";
                std::vector<char> compressed(header.binary_length);
                input.read(reinterpret_cast<char*>(compressed.data()), header.binary_length);

                size_t com_size;
                snappy::GetUncompressedLength(
                    compressed.data(),
                    compressed.size(),
                    &com_size
                );
                bin.resize(com_size);
                snappy::RawUncompress(compressed.data(), compressed.size(), reinterpret_cast< char*>(bin.data()));
                format = header.binary_format;
            }
        }
        else
        {
            reload = true;
        }

        if (reload)
        {
            shader_file_header header;
            header.type = type;
            header.info_tag = make_tag("INFO");
            header.data_tag = make_tag("DATA");
            std::vector<fs::path> dependencies;

            switch (header.type)
            {
            case shader_format::gl_binary:
            {
                log_i << "Compile OpenGL shader binary...";
                auto[b, fmt, ds] = load_glbin(src, include_directories, definitions);
                bin = std::move(b);
                dependencies = std::move(ds);
                format = fmt;
            } break;
            case shader_format::spirv:
            {
                log_i << "Compile SPIR-V shader binary...";
                auto[b, ds] = load_spirv(src, include_directories, definitions);
                bin = std::move(b);
                dependencies = std::move(ds);
            } break;
            }

            std::stringstream buf;
            for (auto dep : dependencies)
            {
                std::time_t t = std::chrono::system_clock::to_time_t(fs::last_write_time(dep));
                auto str = dep.string();
                uint32_t len = static_cast<uint32_t>(str.length());
                buf.write(reinterpret_cast<const char*>(&t), sizeof(t));
                buf.write(reinterpret_cast<const char*>(&len), sizeof(len));
                buf.write(str.data(), str.length());
            }

            auto deps = buf.str();


            std::string out_compressed;
            snappy::Compress(reinterpret_cast<const char*>(bin.data()), bin.size(), &out_compressed);
            header.version = 100;
            header.dependencies_length = static_cast<uint32_t>(deps.size());
            header.dependencies_count = static_cast<uint32_t>(dependencies.size());
            header.binary_format = format;
            header.binary_length = static_cast<uint32_t>(out_compressed.size());

            std::ofstream out(dst, std::ios::binary);
            out.write(reinterpret_cast<const char*>(&header), sizeof(header));
            out.write(deps.data(), deps.size());
            out.write(reinterpret_cast<const char*>(out_compressed.data()), out_compressed.size());
            out.close();

            log_i << "Shader compilation successful";
        }

        return { type, format, bin };
    }
}
