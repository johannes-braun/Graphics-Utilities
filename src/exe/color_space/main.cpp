#include <jpu/memory>
#include <jpu/data>
#include "io/window.hpp"
#include "io/camera.hpp"
#include "stb_image.h"
#include "stb_image_write.h"
#include "res/image.hpp"
#include "res/presets.hpp"
#include "tinyfd/tinyfiledialogs.h"

#include "eigen3/Eigen/Eigenvalues"

jpu::ref_ptr<io::window> main_window;
jpu::named_vector<std::string, jpu::ref_ptr<gl::graphics_pipeline>> graphics_pipelines;
bool is_iconified = false;

struct pat_result
{
    glm::mat4 with_eigenvectors;
    glm::mat4 with_cov_columns;

    glm::mat4 with_eigenvectors_pom;
    glm::mat4 with_cov_columns_pom;

    glm::mat4 with_eigenvectors_pom_rgb;
};

pat_result principal_axis_transformation(glm::u8vec3* pixels, const size_t count)
{
    using namespace glm;

    const dmat3 pom_mat = transpose(dmat3(0.5f, -1.f, 0.5f, -0.875f, 0, 0.875f, 1.5f, 1.5f, 1.5f));

    dvec3 mu = dvec3(0);
    dvec3 mu_pom = dvec3(0);
    for (int i = 0; i < count; ++i)
    {
        mu += dvec3(pixels[i]) / 255.0;
        mu_pom += pom_mat * (dvec3(pixels[i]) / 255.0);
    }
    mu /= count;
    mu_pom /= count;
    const double inv = 1 / 255.0;

    dmat4 tr0 = dmat4(1.0);
    tr0[3] = dvec4(-mu, 1);

    dmat4 tr1 = dmat4(1.0);
    tr1[3] = dvec4(normalize(dvec3(1)) * length(mu), 1);

    dmat4 tr0_pom = dmat4(1.f);
    tr0_pom[3] = dvec4(-mu_pom, 1);

    dmat4 tr1_pom = dmat4(1.0);
    tr1_pom[3] = dvec4(normalize(vec3(0, 0, 1)) * length(mu_pom), 1);

    log_i << to_string(mu_pom) << " versus " << to_string(pom_mat * mu);
    log_i << to_string(inverse(pom_mat) * mu_pom) << " versus " << to_string(mu);

    dmat3 rot_pom(0);
    dmat3 rot(0);
    for (int i = 0; i<count; ++i)
    {
        const dvec3 diff = dvec3(pixels[i]) * inv - mu;
        rot += outerProduct(diff, diff);

        const dvec3 diff_pom = pom_mat * (dvec3(pixels[i]) * inv) - mu_pom;
        rot_pom += outerProduct(diff_pom, diff_pom);
    }
    rot /= count;
    rot_pom /= count;

    dmat3 rot_pom_next = pom_mat * rot * transpose(pom_mat);
    Eigen::Matrix3d cov_matrix = reinterpret_cast<Eigen::Matrix3d&>(rot);
    Eigen::Matrix3d cov_matrix_pom = reinterpret_cast<Eigen::Matrix3d&>(rot_pom);
    Eigen::Matrix3d cov_matrix_pom_ext = reinterpret_cast<Eigen::Matrix3d&>(rot_pom_next);
    Eigen::EigenSolver<decltype(cov_matrix)> solver;

    solver.compute(cov_matrix);
    Eigen::Matrix3d evm = solver.eigenvectors().real();
    Eigen::Vector3d evm_val = solver.eigenvalues().real();
    dmat3 eigenvectormatrix = reinterpret_cast<dmat3&>(evm);

    solver.compute(cov_matrix_pom);
    Eigen::Matrix3d evm_pom = solver.eigenvectors().real();
    Eigen::Vector3d evm_pom_val = solver.eigenvalues().real();
    dmat3 eigenvectormatrix_pom = reinterpret_cast<dmat3&>(evm_pom);

    Eigen::Matrix3d pom_mat_eigen = reinterpret_cast<const Eigen::Matrix3d&>(pom_mat);
    solver.compute(pom_mat_eigen);
    Eigen::Matrix3d pom_eigen_mat = solver.eigenvectors().real();
    Eigen::Vector3d pom_eigen_mat_val = solver.eigenvalues().real();
    const dmat3 pom_eigen_mat_glm = reinterpret_cast<dmat3&>(pom_eigen_mat);

    solver.compute(cov_matrix_pom_ext);
    Eigen::Matrix3d evm_pom_next = solver.eigenvectors().real();
    Eigen::Vector3d evm_pom_next_val = solver.eigenvalues().real();
    const dmat3 eigenvectormatrix_pom_next = reinterpret_cast<dmat3&>(evm_pom_next);

    dmat3 eigenvectormatrix_construct;
    eigenvectormatrix_construct[0] = transpose(inverse(pom_mat)) * eigenvectormatrix[0];
    eigenvectormatrix_construct[1] = transpose(inverse(pom_mat)) * eigenvectormatrix[1];
    eigenvectormatrix_construct[2] = transpose(inverse(pom_mat)) * eigenvectormatrix[2];

    log_i << "EVMP: " << to_string(eigenvectormatrix_pom);
    log_i << "EVMP: " << to_string(eigenvectormatrix_pom_next);
    log_i << "EVMP: " << to_string(eigenvectormatrix_construct);
    log_i << "EVMP: " << to_string(pom_eigen_mat_glm);

    pat_result result;

    {
        dvec3 ha = normalize(rot[0]);
        double len = length(rot[0]);
        if (length(rot[1]) > len)
        {
            len = length(rot[1]);
            ha = rot[1];
        }
        if (length(rot[2]) > len)
        {
            ha = rot[2];
        }
        ha = normalize(ha);
        const double angle = dot(ha, normalize(dvec3(1)));
        const dvec3 axis = cross(ha, normalize(dvec3(1)));
        result.with_cov_columns = tr1 * toMat4(angleAxis(angle, axis)) * tr0;
    }

    {
        dvec3 ha = normalize(eigenvectormatrix[0]);
        double len = length(evm_val[0]);
        if (length(evm_val[1]) > len)
        {
            len = length(evm_val[1]);
            ha = eigenvectormatrix[1];
        }
        if (length(evm_val[2]) > len)
        {
            ha = eigenvectormatrix[2];
        }
        ha = normalize(ha);
        const double angle = dot(ha, normalize(dvec3(1)));
        const dvec3 axis = cross(ha, normalize(dvec3(1)));
        result.with_eigenvectors = tr1 * toMat4(angleAxis(angle, axis)) * tr0;
    }

    {
        dvec3 ha = normalize(rot_pom[0]);
        double len = length(rot_pom[0]);
        if (length(rot_pom[1]) > len)
        {
            len = length(rot_pom[1]);
            ha = rot_pom[1];
        }
        if (length(rot[2]) > len)
        {
            ha = rot_pom[2];
        }
        ha = normalize(ha);
        double angle = dot(ha, normalize(dvec3(1)));
        const dvec3 axis = (angle>0) ? cross(ha, normalize(dvec3(0, 0, 1))) : cross(-ha, normalize(dvec3(0, 0, 1)));
        angle = abs(angle);
        result.with_cov_columns_pom = dmat4(inverse(pom_mat)) * tr1_pom * toMat4(angleAxis(angle, axis)) * tr0_pom * dmat4(pom_mat);
    }

    {
        dvec3 ha = normalize(eigenvectormatrix_pom[0]);
        double len = length(evm_pom_val[0]);
        if (length(evm_pom_val[1]) > len)
        {
            len = length(evm_pom_val[1]);
            ha = eigenvectormatrix_pom[1];
        }
        if (length(evm_pom_val[2]) > len)
        {
            ha = eigenvectormatrix_pom[2];
        }
        ha = normalize(ha);
        double angle = dot(ha, normalize(dvec3(0, 0, 1)));
        const dvec3 axis = (angle>0) ? cross(ha, normalize(dvec3(0, 0, 1))) : cross(-ha, normalize(dvec3(0, 0, 1)));
        angle = abs(angle);

        result.with_eigenvectors_pom = dmat4(inverse(pom_mat)) * tr1_pom * toMat4(angleAxis(angle, axis)) * tr0_pom * dmat4(pom_mat);
    }

    {
        const dmat3 ppt = rot * pom_mat * transpose(pom_mat);
        dvec3 ha = normalize(ppt * eigenvectormatrix[0]);
        double len = length(ppt * eigenvectormatrix_construct[0]);
        if (length(ppt * eigenvectormatrix_construct[1]) > len)
        {
            len = length(ppt * eigenvectormatrix_construct[1]);
            ha = ppt * eigenvectormatrix[1];
        }
        if (length(ppt * eigenvectormatrix_construct[2]) > len)
        {
            ha = ppt * eigenvectormatrix[2];
        }
        ha = normalize(ha);
        double angle = dot(ha, normalize(dvec3(1)));
        const dvec3 axis = (angle>0) ? cross(ha, normalize(dvec3(1))) : cross(-ha, normalize(dvec3(1)));
        angle = abs(angle);

        result.with_eigenvectors_pom_rgb = tr1 * toMat4(angleAxis(angle, axis)) * tr0;
    }

    return result;
}

int main()
{
    gl::setup_shader_paths("../shaders");

    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    main_window = jpu::make_ref<io::window>(io::api::opengl, 1280, 720, "My Window");
    main_window->load_icon("../res/ui/logo.png");
    main_window->limit_framerate(60.0);
    main_window->set_cursor(new io::cursor("../res/cursor.png", 0, 0));

    glfwSetKeyCallback(*main_window, [](GLFWwindow*, int key, int, int action, int mods) {
        if (main_window->gui()->key_action(key, action, mods))
            return;

        if (action == GLFW_PRESS && key == GLFW_KEY_P)
            for (auto&& p : graphics_pipelines)
                p->reload_stages();
    });

    glfwSetScrollCallback(*main_window, [](GLFWwindow*, double x, double y) {
        main_window->gui()->scrolled(y);
    });

    glfwSetCharCallback(*main_window, [](GLFWwindow*, uint32_t ch) {
        if (main_window->gui()->char_input(static_cast<wchar_t>(ch)))
            return;
    });

    glfwSetMouseButtonCallback(*main_window, [](GLFWwindow*, int btn, int action, int mods) {
        if (main_window->gui()->mouse_button_action(btn, action, mods))
            return;
    });

    const auto sampler = jpu::make_ref<gl::sampler>();
    sampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    sampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    sampler->set(GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
    sampler->set(GL_TEXTURE_CUBE_MAP_SEAMLESS, 16);
    sampler->set(GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
    sampler->set(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    sampler->set(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    auto texture = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
    int iw, ih;
    constexpr const char *fs[2] = {
        "*.jpg", "*.png"
    };
    const auto src_data = tinyfd_openFileDialog("Open Image", "../res/", 2, fs, "Images", false);
    if (!src_data) return 0;
    auto data = res::stbi_data(stbi_load(src_data, &iw, &ih, nullptr, 3));
    texture->storage_2d(iw, ih, GL_RGB8);
    texture->assign_2d(GL_RGB, GL_UNSIGNED_BYTE, data.get());
    texture->generate_mipmaps();

    pat_result pat = principal_axis_transformation(reinterpret_cast<glm::u8vec3*>(data.get()), iw * ih);

    glm::vec3 mu = glm::vec3(0);
    for (int i = 0; i<iw*ih; ++i)
        mu += reinterpret_cast<glm::u8vec3*>(data.get())[i];
    mu /= iw * ih * 255;

    auto grid_image = load_image("../res/grid.jpg", res::image_type::unsigned_byte, res::image_components::rgb_alpha);
    const auto grid = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
    grid->storage_2d(grid_image.width, grid_image.height, GL_RGBA8);
    grid->assign_2d(GL_RGBA, GL_UNSIGNED_BYTE, grid_image.data.get());
    grid->generate_mipmaps();

    auto points_pipeline = graphics_pipelines.push("Points Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    points_pipeline->use_stages(
        new gl::shader("../shaders/color_space/points.vert"),
        new gl::shader("../shaders/color_space/points.frag")
    );

    auto center_pipeline = graphics_pipelines.push("Center Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    center_pipeline->use_stages(
        new gl::shader("../shaders/color_space/center_point.vert"),
        new gl::shader("../shaders/color_space/center_point.frag")
    );

    auto gizmo_pipeline = graphics_pipelines.push("Gizmo Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    gizmo_pipeline->use_stages(
        new gl::shader("../shaders/color_space/gizmo.vert"),
        new gl::shader("../shaders/color_space/gizmo.frag")
    );

    auto cube_pipeline = graphics_pipelines.push("Cube Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    cube_pipeline->use_stages(
        new gl::shader("../shaders/color_space/cube.vert"),
        new gl::shader("../shaders/color_space/cube.frag")
    );

    auto img_pipeline = graphics_pipelines.push("Image Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    img_pipeline->use_stages(
        new gl::shader("../shaders/postprocess/screen.vert"),
        new gl::shader("../shaders/color_space/image.frag")
    );

    const auto vbo = jpu::make_ref<gl::buffer>(res::presets::cube::vertices);
    const auto ibo = jpu::make_ref<gl::buffer>(res::presets::cube::indices);
    const auto cube_vao = jpu::make_ref<gl::vertex_array>();
    cube_vao->add_bindings({
        gl::vertex_attribute_binding(0, *vbo, 0, 4, GL_FLOAT, sizeof(res::vertex), offsetof(res::vertex, position), false),
        gl::vertex_attribute_binding(0, *vbo, 0, 2, GL_FLOAT, sizeof(res::vertex), offsetof(res::vertex, uv), false),
    });
    cube_vao->set_element_buffer(*ibo);

    log_i << "Reached here";

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    //glDepthFunc(GL_GEQUAL);
    //glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
    glClearColor(0.2f, 0.2f, 0.2f, 1.f);
    //glClearDepth(0);
    io::camera cam;
    io::default_cam_controller cam_controller;
    cam.transform.position = glm::vec3(0, 0, 5);
    const auto vao = jpu::make_ref<gl::vertex_array>();
    const auto line_vao = jpu::make_ref<gl::vertex_array>();
    
    glLineWidth(4.f);

    while (main_window->update())
    {
        const auto id = sampler->sample_texture(texture.get());
        ImGui::Begin("Bla");
        ImGui::Value("DT", 1000.f * static_cast<float>(main_window->delta_time()));
        static bool hat_en = true;
        ImGui::Checkbox("Show HAT", &hat_en);
        static int mode = 0;
        ImGui::Combo("Mode", &mode, " Default (Cov) \0 Default (Eigen) \0 POM (Cov) \0 POM (Eigen) \0 POM (RGB) \0");

        static float scale = 1.f;
        ImGui::DragFloat("Scale", &scale, 0.01f, 0.f, 100.f);

        if(ImGui::Button("Load other"))
        {
            if (const auto src = tinyfd_openFileDialog("Open Image", "../res/", 2, fs, "Images", false))
            {
                data = res::stbi_data(stbi_load(src, &iw, &ih, nullptr, 3));
                texture = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
                texture->storage_2d(iw, ih, GL_RGB8);
                texture->assign_2d(GL_RGB, GL_UNSIGNED_BYTE, data.get());
                texture->generate_mipmaps();
                pat = principal_axis_transformation(reinterpret_cast<glm::u8vec3*>(data.get()), iw * ih);
                mu = glm::vec3(0);
                for (int i = 0; i < iw*ih; ++i)
                    mu += reinterpret_cast<glm::u8vec3*>(data.get())[i];
                mu /= iw * ih * 255;
            }
        }
        
        glm::mat4 patmat = glm::mat4(1.f); 
        
        switch(mode)
        {
        case 0: patmat = pat.with_cov_columns; break;
        case 1: patmat = pat.with_eigenvectors; break;
        case 2: patmat = pat.with_cov_columns_pom; break;
        case 3: patmat = pat.with_eigenvectors_pom; break;
        case 4: patmat = pat.with_eigenvectors_pom_rgb; break;
        default:;
        }

        if(ImGui::Button("Save..."))
        {
            glm::u8vec3* data_cast = reinterpret_cast<glm::u8vec3*>(data.get());
            std::vector<glm::u8vec3> new_img(iw * ih);
            for (int x = 0; x < iw; ++x)
            {
                for (int y = 0; y<ih; ++y)
                {
                    const auto idx = x + y * iw;
                    new_img[idx] = clamp(glm::vec3(patmat * glm::vec4(glm::vec3(data_cast[x + y * iw]) / 255.f, 1)) * 255.f, glm::vec3(0), glm::vec3(255.f));
                }
            }
            constexpr const char *ext[1] = {
                "*.png"
            };
            if(const auto dst = tinyfd_saveFileDialog("Save output", "../res/", 1, ext, "PNG"))
                stbi_write_png(dst, iw, ih, 3, new_img.data(), 0);
        }
        ImGui::End();

        cam_controller.update(cam, *main_window, main_window->delta_time());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        points_pipeline->bind();
        points_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("hat_mat") = hat_en ? patmat : glm::mat4(1.0);
        points_pipeline->stage(gl::shader_type::vertex)->get_uniform<uint64_t>("picture") = id;
        points_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("view_projection") = cam.projection() * cam.view();
        vao->bind();
        glDrawArrays(GL_POINTS, 0, texture->width() * texture->height());

        glDisable(GL_DEPTH_TEST);
        center_pipeline->bind();
        center_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("hat_mat") = hat_en ? patmat : glm::mat4(1.0);
        center_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("view_projection") = cam.projection() * cam.view();
        center_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::vec3>("center") = mu;
        glDrawArrays(GL_POINTS, 0, 1);
        glEnable(GL_DEPTH_TEST);

        gizmo_pipeline->bind();
        gizmo_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("view_projection") = cam.projection() * cam.view();
        vao->bind();
        glDrawArrays(GL_LINES, 0, 6);

        glFrontFace(GL_CW);
        cube_pipeline->bind();
        cube_pipeline->stage(gl::shader_type::vertex)->get_uniform<glm::mat4>("view_projection") = cam.projection() * cam.view();
        cube_pipeline->stage(gl::shader_type::fragment)->get_uniform<uint64_t>("tex") = sampler->sample_texture(grid.get());
        cube_pipeline->stage(gl::shader_type::fragment)->get_uniform<glm::vec4>("tint") = glm::vec4(1, 1, 1, 1);
        cube_vao->bind();
        glDrawElements(GL_TRIANGLES, static_cast<int>(res::presets::cube::indices.size()), GL_UNSIGNED_INT, nullptr);
        glFrontFace(GL_CCW);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        cube_pipeline->stage(gl::shader_type::fragment)->get_uniform<glm::vec4>("tint") = glm::vec4(1, 1, 1, -1);
        glDrawElements(GL_TRIANGLES, static_cast<int>(res::presets::cube::indices.size()), GL_UNSIGNED_INT, nullptr);
        glDisable(GL_BLEND);

        glDisable(GL_DEPTH_TEST);
        glViewport(0, 0, static_cast<int>(texture->width() *scale), static_cast<int>(texture->height() *scale));
        img_pipeline->bind();
        img_pipeline->stage(gl::shader_type::fragment)->get_uniform<glm::mat4>("hat_mat") = hat_en ? patmat : glm::mat4(1.0);
        img_pipeline->stage(gl::shader_type::fragment)->get_uniform<uint64_t>("tex") = id;
        vao->bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);

        int w, h;
        glfwGetFramebufferSize(*main_window, &w, &h);
        glViewport(0, 0, w, h);
        glEnable(GL_DEPTH_TEST);
    }
}