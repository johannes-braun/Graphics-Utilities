#include "xspirv.hpp"
#include <fstream>
#include <memory>
#include <iterator>
#include <cassert>
#include <jpu/log>
#include <sstream>

namespace xspirv
{
    namespace fs = std::experimental::filesystem;
	const fs::path shader_temp_file = fs::temp_directory_path() / temp_file_name;



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
        auto str = call(glslc_path, std::string(" ") + default_flags + " -M " + path.string() + "");
        return std::vector<std::string>{ ++std::istream_iterator<std::string>(str), std::istream_iterator<std::string>() };
    }

    std::vector<uint32_t> load_file(const std::experimental::filesystem::path& file_path,
        const std::vector<std::experimental::filesystem::path>& include_directories,
        const std::vector<definition>& definitions)
	{
		const fs::path vtxshd = file_path.string() + file_extension;

		assert(fs::exists(file_path) && "File could not be found");

		bool new_shader_file = !fs::exists(vtxshd);

		// Get file version
		std::fstream shader_file;
		shader_file.open(vtxshd, std::ios::in | std::ios::binary);

		const auto dep_sorter = [](const std::tuple<long long, fs::path>& lhs, const std::tuple<long long, fs::path>& rhs) {
			return std::greater<long long>()(std::get<long long>(lhs), std::get<long long>(rhs));
		};

		std::vector<uint32_t> result;

		std::vector<std::tuple<long long, fs::path>> saved_dependencies;
		if (!new_shader_file)
		{
			// read existing file
			uint32_t version;
			shader_file.read(reinterpret_cast<char*>(&version), sizeof version);

			if (version == file_version)
			{
				log_i << "Loading cached vtxshd file";

				uint32_t num_dependencies;
				shader_file.read(reinterpret_cast<char*>(&num_dependencies), sizeof num_dependencies);

				while (num_dependencies-- > 0)
				{
					long long edit_time;
					shader_file.read(reinterpret_cast<char*>(&edit_time), sizeof edit_time);

					uint32_t dependency_length;
					shader_file.read(reinterpret_cast<char*>(&dependency_length), sizeof dependency_length);

					std::string dependency(dependency_length, ' ');
					shader_file.read(dependency.data(), dependency_length);

					saved_dependencies.push_back(std::make_tuple(edit_time, dependency));
				}
				std::sort(saved_dependencies.begin(), saved_dependencies.end(), dep_sorter);

				uint32_t bin_size;
				shader_file.read(reinterpret_cast<char*>(&bin_size), sizeof bin_size);
				result.resize(static_cast<uint32_t>(bin_size * sizeof(char) / static_cast<float>(sizeof(uint32_t))));
				shader_file.read(reinterpret_cast<char*>(result.data()), bin_size);
			}
			else
			{
				new_shader_file = true;
			}
		}
		shader_file.close();

		std::vector<std::tuple<long long, fs::path>> current_dependencies;
		auto dependencies = get_dependencies(glslc_location, file_path);
		for (auto&& dependency : dependencies)
		{
			long long edit_time = std::chrono::system_clock::to_time_t(fs::last_write_time(fs::path(dependency)));
			current_dependencies.push_back(std::make_tuple(edit_time, fs::path(dependency).string()));
		}
		std::sort(current_dependencies.begin(), current_dependencies.end(), dep_sorter);

		if (current_dependencies.size() == saved_dependencies.size())
		{
			for (int i = 0; i < current_dependencies.size(); ++i)
			{
				if (std::get<long long>(current_dependencies[i]) != std::get<long long>(saved_dependencies[i]) ||
					std::get<fs::path>(current_dependencies[i]) != std::get<fs::path>(saved_dependencies[i]))
				{
					new_shader_file = true;
					break;
				}
			}
		}
		else new_shader_file = true;

		if (new_shader_file)
		{
			log_i << "Creating updated vtxshd file";

			shader_file.open(vtxshd, std::ios::out | std::ios::binary | std::ios::trunc);
			shader_file.write(reinterpret_cast<const char*>(&file_version), sizeof file_version);
			uint32_t count = static_cast<uint32_t>(current_dependencies.size());
			shader_file.write(reinterpret_cast<char*>(&count), sizeof count);

			for (auto&& dependency : current_dependencies)
			{
				shader_file.write(reinterpret_cast<char*>(&std::get<long long>(dependency)), sizeof(long long));
				auto name = std::get<fs::path>(dependency);
				uint32_t length = static_cast<uint32_t>(name.string().size());
				shader_file.write(reinterpret_cast<char*>(&length), sizeof length);
				shader_file.write(name.string().data(), length);
			}

            std::string includes;
            for(auto&& inc : include_directories)
            {
                includes += " -I" + fs::absolute(inc).string();
            }

            std::string defines;
            for (auto&& def : definitions)
            {
                defines += " -D'" + def.name;
                if(def.info.parameters.size() == 0)
                {
                    defines += "(";
                    defines += def.info.parameters.at(0);
                    for(int i=1; i<def.info.parameters.size(); ++i)
                    {
                        defines += "," + def.info.parameters.at(i);
                    }
                    defines += ")";
                }
                if(def.info.replacement != "")
                {
                    defines += "=" + def.info.replacement;
                }
                defines += '\' ';
            }
					
			system(cmd(glslc_location, " -std=450core -o " + shader_temp_file.string() + " -mfmt=bin" + includes + defines + " " + fs::absolute(file_path).string() + "").c_str());
			std::ifstream tmp(shader_temp_file, std::ios::in | std::ios::binary);
			tmp.ignore(std::numeric_limits<std::streamsize>::max());
			const std::streamsize length = tmp.gcount();
			tmp.clear();   //  Since ignore will have set eof.
			tmp.seekg(0, std::ios_base::beg);
			result.resize(length / sizeof(uint32_t));
			tmp.read(reinterpret_cast<char*>(result.data()), length);

			uint32_t size = static_cast<uint32_t>(length);
			shader_file.write(reinterpret_cast<char*>(&size), sizeof size);
			shader_file.write(reinterpret_cast<const char*>(result.data()), size);
			tmp.close();

			remove(shader_temp_file);
		}

		return result;
	}
}
