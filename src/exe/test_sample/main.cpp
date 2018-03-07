//#include <atomic>
#include <array>
//#include <jpu/log>
//#include <jpu/memory>
//#include <io/window.hpp>
//#include <jpu/data>
//#include <stack>
//#include <opengl/query.hpp>
#include <numeric>
//#include <res/image.hpp>
#include <pugixml.hpp>
#include <fstream>
#include <set>
#include <vector>

#include "of.hpp"
//
//constexpr int logn = 20;
//std::vector<float> data1(1 << logn);
//std::vector<float> data2(1 << logn);
//
//void bitonic(const int logn, uint32_t* a, const size_t len)
//{
//    for (int p = 0; p < logn; ++p)
//    {
//        for (int q = 0; q <= p; ++q)
//        {
//            const int d = 1 << (p - q);
//
//#pragma omp parallel for schedule(static)
//            for (int id = 0; id < len / 2; ++id)
//            {
//
//                const int gid = (id + d * (id / d));
//                const bool up = ((gid >> p) & 2) == 0;
//
//                if ((a[gid] > a[gid | d]) == up)
//                    std::swap(a[gid], a[gid | d]);
//            }
//        }
//    }
//}
//
//jpu::ref_ptr<io::window> main_window;
//jpu::named_vector<std::string, jpu::ref_ptr<gl::graphics_pipeline>> graphics_pipelines;
//
//int main__dep()
//{
//    gl::shader::set_include_directories("../shaders");
//
//    res::image icon = load_image("../res/ui/logo.png", res::image_type::u8, res::image_components::rgb_alpha);
//    res::image cursor = load_image("../res/cursor.png", res::image_type::u8, res::image_components::rgb_alpha);
//
//    main_window = jpu::make_ref<io::window>(io::api::opengl, 1280, 720, "My Window");
//    main_window->set_icon(icon.width, icon.height, icon.data.get());
//    main_window->set_cursor(new io::cursor(cursor.width, cursor.height, cursor.data.get(), 0, 0));
//    main_window->set_max_framerate(60.f);
//
//    data2 = data1;
//
//    std::generate(data1.begin(), data1.end(), []() { return static_cast<float>(rand()); });
//    auto buf = jpu::make_ref<gl::buffer>(data1);
//    buf->bind(0, GL_SHADER_STORAGE_BUFFER);
//    glFinish();
//
//    const auto sort_bitonic = [&]() {
//        auto cmp = jpu::make_ref<gl::compute_pipeline>(new gl::shader("sort/bitonic.comp"));
//        std::generate(data1.begin(), data1.end(), []() { return rand(); });
//        buf = jpu::make_ref<gl::buffer>(data1);
//        buf->bind(0, GL_SHADER_STORAGE_BUFFER);
//        glFinish();
//        const int trg = data1.size() >> 1;
//        auto puni = cmp->get_uniform<int>("p");
//        auto quni = cmp->get_uniform<int>("q");
//        auto duni = cmp->get_uniform<int>("d");
//
//        gl::query query(GL_TIME_ELAPSED);
//        cmp->bind();
//        query.begin();
//        for (int p = 0; p < logn; ++p)
//        {
//            puni = p;
//            for (int q = 0; q <= p; ++q)
//            {
//                const int d = 1 << (p - q);
//                duni = d;
//                quni = q;
//                cmp->dispatch(trg);
//                glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
//            }
//        }
//        query.end();
//        uint64_t time = query.get_uint64();
//        log_i << time / 1000000.f;
//    };
//
//    std::string bla = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
//    std::string blu = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
//    std::string bli = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
//
//    log_i << bla << ", " << blu << ", " << bli;
//
//    system("pause");
//    return 0;
//}

int main()
{
    pugi::xml_document doc;
    doc.load_file("../res/gl.xml");

    std::ofstream of("../../src/exe/test_sample/of.hpp", std::ios::out);
    of << R"cpp(#pragma once

)cpp";

    std::set<std::string> enums;
    std::set<std::string> commands;

    pugi::xml_node registry = doc.child("registry");
    pugi::xml_node extensions = registry.child("extensions");
    pugi::xml_node types = registry.child("types");

    bool core = true;
    bool khrplatform = false;
    std::set<std::string> enable_extensions = {
        "GL_ARB_bindless_texture",
        "GL_ARB_direct_state_access",
        "GL_ARB_compute_variable_group_size",
        "GL_ARB_vertex_attrib_binding",
        "GL_EXT_texture_filter_anisotropic",
        "GL_KHR_debug",
        "GL_NVX_gpu_memory_info",
        "GL_NV_bindless_multi_draw_indirect",
        "GL_NV_bindless_multi_draw_indirect_count",
        "GL_NV_command_list",
        "GL_NV_path_rendering",
        "GL_NV_path_rendering_shared_edge",
        "GL_NV_shader_buffer_load",
        "GL_NV_shader_buffer_store",
        "GL_NV_uniform_buffer_unified_memory",
        "GL_NV_vertex_buffer_unified_memory"
    };

    of << R"cpp(#define GL_APIENTRY *
)cpp";

    for (pugi::xml_node type : types.children("type"))
    {
        if (!khrplatform && (std::strcmp(type.attribute("name").as_string(), "khrplatform") == 0 || std::strcmp(type.attribute("requires").as_string(), "khrplatform") == 0))
            continue;
        for (pugi::xml_node tc : type.children())
        {
            if (std::strcmp(tc.name(), "apientry") == 0)
                of << "GL_APIENTRY";
            else if (std::strcmp(tc.name(), "name") == 0)
                of << tc.first_child().value();
            else
                of << tc.value();
        }
        of << "\n";
    }
    of << "\n";

    for (auto&& ext : enable_extensions)
    {
        of << "#define " << ext << " " << 1 << "\n";
    }

    for (pugi::xml_node feature : registry.children("feature"))
    {
        if (std::strcmp(feature.attribute("api").as_string(), "gl") != 0)
            continue;

        for (pugi::xml_node require : feature.children("require"))
        {
            if (!core || (!require.attribute("profile") || std::strcmp(require.attribute("profile").as_string(), "core") == 0))
            {
                if (auto comment = require.attribute("comment"))
                {
                    std::string comment_str = comment.as_string();
                    if (memcmp("Reuse ", comment_str.data(), 6) == 0)
                    {
                        std::string sub = comment_str.substr(6, comment_str.size() - 6);
                        const char* beg = sub.data();
                        const char* end = sub.data() + sub.size();
                        if (memcmp(beg, "tokens ", 7) == 0)
                            beg += 12;
                        else if (memcmp(beg, "commands ", 9) == 0)
                            beg += 14;
                        if (*(end-1) == ')')
                        {
                            while (*end != '(')
                                --end;
                            end -= 1;
                        }
                        sub = std::string(beg, end);
                        auto found = sub.find_first_of('-');
                        if(found != std::string::npos)
                        {
                            sub.resize(found - 1);
                        }

                        enable_extensions.emplace(sub);
                        of << "#define GL_" << sub << " " << 1 << "\n";
                        of << "extern bool enable_" << sub << ";\n";
                    }
                }

                for (pugi::xml_node enumerator : require.children("enum"))
                    enums.emplace(enumerator.attribute("name").as_string());
                for (pugi::xml_node command : require.children("command"))
                    commands.emplace(command.attribute("name").as_string());
            }
        }
        for (pugi::xml_node remove : feature.children("remove"))
        {
            if (!core || (!remove.attribute("profile") || std::strcmp(remove.attribute("profile").as_string(), "core") == 0))
            {
                for (pugi::xml_node enumerator : remove.children("enum"))
                    enums.erase(enumerator.attribute("name").as_string());
                for (pugi::xml_node command : remove.children("command"))
                    commands.erase(command.attribute("name").as_string());
            }
        }
    }
    of << "\n";
    of << "\n";

    for (pugi::xml_node extension : extensions)
    {
        const char* name = extension.attribute("name").as_string();
        std::set<std::string>::iterator it;
        if ((it = enable_extensions.find(name)) != enable_extensions.end())
        {
            for (pugi::xml_node require : extension.children("require"))
            {
                for (pugi::xml_node enumerator : require.children("enum"))
                    enums.emplace(enumerator.attribute("name").as_string());
                for (pugi::xml_node command : require.children("command"))
                    commands.emplace(command.attribute("name").as_string());
            }
        }
    }

    of << R"cpp(enum GLenum_values {
)cpp";
    for (pugi::xml_node enums_node : registry)
    {
        bool has = false;
        for (pugi::xml_node enums_val : enums_node.children("enum"))
        {
            auto it = enums.begin();
            if ((it = enums.find(enums_val.attribute("name").as_string())) != enums.end())
            {
                of << "    " << *it << " = " << enums_val.attribute("value").as_string() << ",\n";
            }
        }
    }
    of << R"cpp(};
    
// Functions
    
)cpp";

    auto child = doc.child("registry").find_child_by_attribute("commands", "namespace", "GL");
    for (auto&& c : child.children("command"))
    {
        auto proto = c.child("proto");
        std::string pname = proto.child("name").first_child().value();
        auto it = commands.begin();
        if ((it = commands.find(pname)) == commands.end())
            continue;

        of << "inline ";
        if (auto pt = proto.child("ptype"))
            of << "" << pt.first_child().value();
        else of << "" << proto.first_child().value();
        of << "(*" << proto.child("name").first_child().value() << ")(";
        bool first = true;
        for (auto&& p : c.children("param"))
        {
            if (!first) of << ", ";

            if (auto pt = p.child("ptype"))
                of << "" << pt.first_child().value();
            else of << "" << p.first_child().value();

            of << " " << p.child("name").first_child().value();
            first = false;
        }
        of << ");\n";
    }

    of << R"cpp(
void* get_func(const char* name)
{
    return nullptr;
}
)cpp";


    of << "\nvoid load(){\n";
    for (auto&& command : commands)
    {
        of << "    " << command << " = reinterpret_cast<decltype(" << command << ")>(get_func(\"" << command << "\"));\n";
    }
    of << "}\n";

    system("pause");
    return 0;
}