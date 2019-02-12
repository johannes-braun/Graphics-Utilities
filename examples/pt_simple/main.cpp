#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <map>
#include <memory>
#define MYGL_IMPLEMENTATION
#include "input/camera.hpp"
#include "input/input.hpp"
#include "input/input_glfw.hpp"
#include <gfx.ecs.components/basic.hpp>
#include <gfx.ecs.components/movement.hpp>
#include <gfx.ecs/ecs.hpp>
#include <mygl/mygl.hpp>
#include <random>

constexpr const char* screen = R"(
#version 460 core

layout(location=0) out vec2 uv;

void main()
{
    gl_Position = vec4(mix(-1, 3, gl_VertexID & 0x1), mix(-1, 3, (gl_VertexID >> 1) & 0x1), 0.f, 1.f);
    uv = ((gl_Position.xy+1)*0.5f);
	uv.y = 1-uv.y;
}
)";

constexpr const char* pt = R"(
#version 460 core

layout(location=0) in vec2 uv;

float prand1(uint n)
{
    const float g = 1.6180339887498948482f;
    return fract(0.5 + n / g);
}

vec2 prand2(uint n)
{
    const float g = 1.32471795724474602596;
    const float a1 = 1.0 / g;
    const float a2 = 1.0 / (g*g);
    return fract(0.5 + vec2(a1, a2) * n);
}

layout(location = 0) uniform uint cur_sample;
uniform mat4 inverse_view_projection;
uniform vec3 cam_pos;
uniform float rndm;

layout(rgba32f, binding = 0) uniform image2D fac_image;
layout(rgba32f, binding = 1) uniform image2D accum_image;
layout(rgba32f, binding = 2) uniform image2D i_orig_bounce;
layout(rgba32f, binding = 3) uniform image2D i_dir;

layout(location = 0) out vec4 out_color;

const vec3 box_normals[] = vec3[](
    vec3(-1, 0, 0), vec3(0, -1, 0), vec3(0, 0, -1),
    vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1)
);

int faceID(float t, float t1, float t2, float t3, float t4, float t5, float t6)
{
    return t == t1 ? 0                                                     // neg_x
                   : (t == t2 ? 3                                          // pos_x
                              : (t == t3 ? 1                               // neg_y
                                         : (t == t4 ? 4                    // pos_y
                                                    : (t == t5 ? 2         // neg_z
                                                               : 5))));    // pos_z
}

bool intersect_bounds(const vec3 origin, const vec3 direction,
                      const vec3 bounds_min, const vec3 bounds_max, const float max_distance, inout float min_distance,
                      inout float out_max_distance, inout int face_tmin, inout int face_tmax)
{
    vec3 inv_direction = 1.f / direction;

    // intersections with box planes parallel to x, y, z axis
    vec3 t135 = (bounds_min - origin) * inv_direction;
    vec3 t246 = (bounds_max - origin) * inv_direction;

    vec3 min_values = min(t135, t246);
    vec3 max_values = max(t135, t246);

    float tmin = max(max(min_values.x, min_values.y), min_values.z);
    float tmax = min(min(max_values.x, max_values.y), max_values.z);

    if(tmax >= 0 && tmin <= tmax && tmin <= max_distance)
    {
        face_tmin = faceID(tmin, t135.x, t246.x, t135.y, t246.y, t135.z, t246.z);
        face_tmax = faceID(tmax, t135.x, t246.x, t135.y, t246.y, t135.z, t246.z);
        min_distance     = min(tmin, tmax);
        out_max_distance = max(tmin, tmax);
        return true;
    }
    return false;
}
vec3 sample_cosine_hemisphere(vec2 uv)
{
    // (Uniformly) sample a point on the unit disk
    float r     = sqrt(uv.x);
    float theta = 2 * 3.14159265359f * uv.y;
    float x     = r * cos(theta);
    float y     = r * sin(theta);

    // Project point up to the unit sphere
    float z = float(sqrt(max(0.f, 1 - x * x - y * y)));
    return vec3(x, y, z);
}
vec3 bsdf_local_to_world(const in vec3 vector, const in vec3 normal)
{
    // Find an axis that is not parallel to normal
	vec3 perp = vec3(1, 0, 0);
    vec3 u = normalize(cross(normal, perp));
	for(int i=1; any(isnan(u)) && i < 3; ++i)
	{
		perp[i-1] = 0;
		perp[i] = 1;
		u = normalize(cross(normal, perp));
	}
    return normalize(mat3(u, cross(normal, u), normal) * vector);
}

void main()
{
    const uint cs = cur_sample + uint(prand1(uint(gl_FragCoord.x)) * 138203 + prand1(uint(gl_FragCoord.y)) * 28103);

    vec4 orb = imageLoad(i_orig_bounce, ivec2(gl_FragCoord.xy));
    vec4 dir = imageLoad(i_dir, ivec2(gl_FragCoord.xy));
    vec4 fac = imageLoad(fac_image, ivec2(gl_FragCoord.xy));

    if(orb.w > 8)
        orb.w = 0;

    if(orb.w == 0)
    {
        vec2 img_size = imageSize(accum_image).xy;
        vec2 random_value = prand2(cs);
	    vec2 uv = vec2(((gl_FragCoord.xy + random_value*2-1) / vec2(img_size)) * 2 - 1);
        dir.xyz = vec3(inverse_view_projection * vec4(uv, 0.f, 1.f));
        orb.xyz = cam_pos;
        fac = vec4(1, 1, 1, 1);
        dir.w = 1.f;
    }

    float mind1 = 1.f / 0.f;
    float mind2 = 1.f / 0.f;
    int face_min1 = 0;
    int face_min2 = 0;
    int face_max = 0;
    float maxd = 1.f / 0.f;
    bool is1 = intersect_bounds(orb.xyz, normalize(dir.xyz), vec3(-4, -0.2, -4), vec3(4, 0.2, 4), maxd, mind1, maxd, face_min1, face_max);
    bool is2 = intersect_bounds(orb.xyz, normalize(dir.xyz), vec3(-1, 0.25, -1), vec3(1, 2, 1), maxd, mind2, maxd, face_min2, face_max);

    if(is1 || is2)
    {
        vec3 norm = box_normals[mind1 < mind2 ? face_min1 : face_min2];
        orb.xyz = orb.xyz + min(mind1, mind2) * normalize(dir.xyz) + 1e-3 * norm;
        dir.xyz = bsdf_local_to_world(sample_cosine_hemisphere(prand2(cs + uint(rndm * 13))), norm);
        fac *= (mind1 < mind2 ? vec4(1) : vec4(0.2f, 0.8f, 1.f, 1.f)) / 3.14159265359;
        orb.w = orb.w + 1;
        dir.w *= 1/(3.1415926535);
    }
    else
    {
        out_color = fac * vec4(0.8f, 0.9f, 0.95f, 1) / dir.w;
        orb.w = 0.f;
    }

    vec4 accum = imageLoad(accum_image, ivec2(gl_FragCoord.xy));
    accum += out_color;
    imageStore(accum_image, ivec2(gl_FragCoord.xy), accum);
    out_color = accum / cur_sample;

    imageStore(i_orig_bounce, ivec2(gl_FragCoord.xy), orb);
    imageStore(i_dir, ivec2(gl_FragCoord.xy), dir);
    imageStore(fac_image, ivec2(gl_FragCoord.xy), fac);
}
)";

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    const auto w = glfwCreateWindow(1280, 720, "Test", nullptr, nullptr);
    glfwMakeContextCurrent(w);
    mygl::load();

    std::mt19937 g;
    std::uniform_real_distribution<float> d(0, 1);

    gfx::ecs::ecs ecs;
    auto user_entity = ecs.create_entity_shared(gfx::transform_component{glm::vec3{0, 0, 4}, glm::vec3(3)}, gfx::projection_component{},
                                                gfx::grabbed_cursor_component{}, gfx::camera_controls{});
    gfx::ecs::system_list   main_systems;
    gfx::glfw_input_system  input(w);
    gfx::user_camera_system camera_system(input);
    main_systems.add(input);
    main_systems.add(camera_system);

    auto vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &screen, nullptr);
    glCompileShader(vs);

    auto fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &pt, nullptr);
    glCompileShader(fs);

    auto pr = glCreateProgram();
    glAttachShader(pr, vs);
    glAttachShader(pr, fs);
    glLinkProgram(pr);

    mygl::texture fac;
    glCreateTextures(GL_TEXTURE_2D, 1, &fac);
    glTextureStorage2D(fac, 1, GL_RGBA32F, 1280, 720);
    mygl::texture tex;
    glCreateTextures(GL_TEXTURE_2D, 1, &tex);
    glTextureStorage2D(tex, 1, GL_RGBA32F, 1280, 720);
    mygl::texture origbounce;
    glCreateTextures(GL_TEXTURE_2D, 1, &origbounce);
    glTextureStorage2D(origbounce, 1, GL_RGBA32F, 1280, 720);
    mygl::texture dir;
    glCreateTextures(GL_TEXTURE_2D, 1, &dir);
    glTextureStorage2D(dir, 1, GL_RGBA32F, 1280, 720);

    int  l;
    char buf[512];
    glGetShaderInfoLog(vs, 512, &l, buf);
    std::cout << buf << '\n';
    glGetShaderInfoLog(fs, 512, &l, buf);
    std::cout << buf << '\n';
    glGetProgramInfoLog(pr, 512, &l, buf);
    std::cout << buf << '\n';

    mygl::vertex_array arr;
    glCreateVertexArrays(1, &arr);

    int count = 0;
    glClearColor(1, 1, 1, 1);
    double last = glfwGetTime();
    while (!glfwWindowShouldClose(w))
    {
        const double delta = glfwGetTime() - last;
        last               = glfwGetTime();
        ecs.update(delta, main_systems);

        glm::vec4 clear(0, 0, 0, 0);
        glm::vec4 clear1(1);
        if (count == 0 || glfwGetKey(w, GLFW_KEY_R))
        {
            count = 0;
            glClearTexImage(fac, 0, GL_RGBA, GL_FLOAT, value_ptr(clear1));
            glClearTexImage(tex, 0, GL_RGBA, GL_FLOAT, value_ptr(clear));
            glClearTexImage(dir, 0, GL_RGBA, GL_FLOAT, value_ptr(clear));
            glClearTexImage(origbounce, 0, GL_RGBA, GL_FLOAT, value_ptr(clear));
        }

        count = (count + 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(pr);
        glUniform1ui(0, count);

        const auto iv = inverse(user_entity->get<gfx::transform_component>()->matrix());
        const auto ip = user_entity->get<gfx::projection_component>()->matrix();
        glUniformMatrix4fv(glGetUniformLocation(pr, "inverse_view_projection"), 1, false, value_ptr(inverse(ip * iv)));
        glUniform3fv(glGetUniformLocation(pr, "cam_pos"), 1, value_ptr(user_entity->get<gfx::transform_component>()->position));
        glUniform1f(glGetUniformLocation(pr, "rndm"), d(g));
        glBindVertexArray(arr);
        glBindImageTexture(0, fac, 0, false, 0, GL_READ_WRITE, GL_RGBA32F);
        glBindImageTexture(1, tex, 0, false, 0, GL_READ_WRITE, GL_RGBA32F);
        glBindImageTexture(2, origbounce, 0, false, 0, GL_READ_WRITE, GL_RGBA32F);
        glBindImageTexture(3, dir, 0, false, 0, GL_READ_WRITE, GL_RGBA32F);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(w);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}