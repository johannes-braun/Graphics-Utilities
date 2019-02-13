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
#include <gfx.file/file.hpp>

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
layout(binding = 0) uniform samplerCube cubemap;

layout(rgba32f, binding = 0) uniform image2D fac_image;
layout(rgba32f, binding = 1) uniform image2D accum_image;
layout(rgba32f, binding = 2) uniform image2D i_orig_bounce;
layout(rgba32f, binding = 3) uniform image2D i_dir;
layout(rgba32f, binding = 4) uniform image2D i_arb;

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

struct hit_t
{
    int id;
    bool hits;
    vec3 pos;
    vec3 norm;
    float t;
};

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
hit_t ray_aabb_hit(int id, vec3 r0, vec3 rd, vec3 bmin, vec3 bmax)
{
    int fmin, fmax;
    float tmin = 0, tmax = 0;
    hit_t hit;
    hit.id = id;
    if(!intersect_bounds(r0, rd, bmin, bmax, 1.f / 0.f, tmin, tmax, fmin, fmax))
    {
        hit.hits = false;
        return hit;
    }
    if(tmin < 0)
    {
        hit.t = tmax;
        hit.norm = box_normals[fmax];
    }
    else
    {
        hit.t = tmin;
        hit.norm = box_normals[fmin];
    }
    hit.pos = r0 + hit.t * rd + 1e-3 * hit.norm;
    hit.hits = true;
    return hit;
}

hit_t ray_oobb_hit(int id, vec3 r0, vec3 rd, vec3 bmin, vec3 bmax, mat4 tf)
{
    const mat4 itf = inverse(tf);
    
    hit_t hit = ray_aabb_hit(id, (itf * vec4(r0, 1)).xyz, (itf * vec4(rd, 0)).xyz, bmin, bmax);
    if(!hit.hits)
        return hit;
    hit.norm = (tf * vec4(hit.norm, 0)).xyz;
    hit.pos = (tf * vec4(hit.pos, 1)).xyz;
    return hit;
}

bool intSphere( in vec4 sp, in vec3 ro, in vec3 rd, in float tm, out float t )
{
    bool  r = false;
    vec3  d = ro - sp.xyz;
    float b = dot(rd,d);
    float c = dot(d,d) - sp.w*sp.w;
    t = b*b-c;
    if( t > 0.0 )
    {
        t = -b-sqrt(t);
        r = (t > 0.0) && (t < tm);
    }

    return r;
}

hit_t ray_sphere_hit(int id, vec3 r0, vec3 rd, vec3 s0, float sr) {
    // - r0: ray origin
    // - rd: normalized ray direction
    // - s0: sphere center
    // - sr: sphere radius
    // - Returns distance from r0 to first intersecion with sphere,
    //   or -1.0 if no intersection.
    hit_t hit;
    hit.id = id;
   
    if(!intSphere(vec4(s0, sr), r0, rd, 1.f / 0.f, hit.t))
    {
        hit.hits = false;
        return hit;
    }

    hit.pos = r0 + hit.t * rd;
    hit.norm = normalize(hit.pos - s0);
    hit.pos += 1e-3 * hit.norm;
    hit.hits = true;
    return hit;
}

hit_t nearest_hit(const in hit_t a, const in hit_t b)
{
    return (a.hits && b.hits && a.t < b.t) || !b.hits ? a : b;
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
vec2 ggx_importance_sample(const in vec2 random_sample, const in float roughness)
{
    float phi    = random_sample.y * 2.f * 3.14156235659f;
    float rough2 = roughness * roughness;
    float theta  = atan(sqrt(rough2 * random_sample.x / (1 - random_sample.x)));
    return vec2(phi, theta);
}
vec3 ggx_importance_hemisphere(const in vec2 importance_sample)
{
    float phi      = importance_sample.x;
    float cosTheta = cos(importance_sample.y);
    float sinTheta = sin(importance_sample.y);
    return vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
}
// The GGX chi function
float DistributionGGX(vec3 N, vec3 H, float a)
{
	if(a < 0.0001f)
		return 1.f;

	const float PI =  3.14159265359f;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}

vec3 tonemapFilmic(const vec3 color) {
	vec3 x = max(vec3(0.0), color - 0.004);
	return (x * (6.2 * x + 0.5)) / (x * (6.2 * x + 1.7) + 0.06);
}
mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main()
{
    const uint cs = cur_sample + uint(prand1(uint(gl_FragCoord.x)) * 138203 + prand1(uint(gl_FragCoord.y)) * 28103);

    vec4 orb = imageLoad(i_orig_bounce, ivec2(gl_FragCoord.xy));
    vec4 dir = imageLoad(i_dir, ivec2(gl_FragCoord.xy));
    vec4 fac = imageLoad(fac_image, ivec2(gl_FragCoord.xy));
    vec4 accum = imageLoad(accum_image, ivec2(gl_FragCoord.xy));
    vec4 arb = imageLoad(i_arb, ivec2(gl_FragCoord.xy));

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
        arb.x += 1.f;
    }

    const vec3 ndir = normalize(dir.xyz);
    
    vec3 colors[] = {
        vec3(1.f, 1.f, 1.f),
        vec3(1.f, 0.4f, 0.f),
        vec3(0.5f, 0.2f, 0.8f),
        vec3(0.96f, 0.95f, 0.98f),
        vec3(0.96f, 0.89f, 0.85f),
        vec3(0.2f, 0.5f, 0.6f)
    };
    bool metal[] = {
        true,
        false,
        false,
        true,
        true,
        true
    };
    float rough[] = {
        0.8f,
        0.1f,
        0.01f,
        0.01f,
        0.3f,
        0.2f
    };
    
    mat4 rot = rotationMatrix(normalize(vec3(1, 0.8f, 0.2)), -40.f * 3.14159265359 / 360);
    mat4 rot2 = rotationMatrix(normalize(vec3(0, 0.7f, 1.2)), -60.f * 3.14159265359 / 360);
    hit_t all_hits[] = {
        ray_oobb_hit(0, orb.xyz, ndir, vec3(-4, -0.2, -4), vec3(4, 0.2, 4), rot),
        ray_aabb_hit(1, orb.xyz, ndir, vec3(-1, 0.35, -1), vec3(1, 2, 1)),
        ray_oobb_hit(2, orb.xyz, ndir, vec3(-0.4, 0, -0.4), vec3(0.4, 4, 0.4), rot2),
        ray_sphere_hit(3, orb.xyz, ndir, vec3(1, 1.4, 1), 1.f),
        ray_sphere_hit(4, orb.xyz, ndir, vec3(1, 2.8, 1), 0.6f),
        ray_sphere_hit(5, orb.xyz, ndir, vec3(1, 3.5, 1), 0.3f),
    };

    hit_t nearest;
    nearest.hits = false;
    for(int i=0; i<all_hits.length(); ++i)
    {
        nearest = nearest_hit(nearest, all_hits[i]);
    }

    if(nearest.hits)
    {
        vec3 norm = nearest.norm;
        orb.xyz = nearest.pos;
            
        const float alpha = rough[nearest.id] * rough[nearest.id];
        float D = DistributionGGX(norm, dir.xyz, alpha);
        if(isnan(D) || isinf(D) || D < 1e-5) D = 1.f;
        const vec2 smp = prand2(cs + uint(rndm * 30));
        const vec3 msn = normalize(bsdf_local_to_world(ggx_importance_hemisphere(ggx_importance_sample(smp, alpha)), norm));

        const float ior_in = 1.f;
        const float ior_out = 1.5f;
        float F0 = pow((ior_in-ior_out)/(ior_in+ior_out), 2);
	    F0 = mix(F0, 1.0, 0.0f);
        float fresnel = F0 + (1 - F0) * pow(1.0 - max(dot(-dir.xyz, msn), 0), 5.0);

        dir.xyz = reflect(normalize(dir.xyz), msn);

        if(!metal[nearest.id] && prand1(cs + uint(rndm * 777)) > fresnel)
        {
            dir.xyz = bsdf_local_to_world(sample_cosine_hemisphere(prand2(cs + uint(rndm * 13))), norm);
            fac *= vec4(colors[nearest.id], 1);
            D = 3.14159265359;
        }

        fac *= D;// * abs(dot(norm, dir.xyz));
        orb.w = orb.w + 1;
        dir.w *= D;// * abs(dot(norm, dir.xyz));
    }
    else
    {
        const vec3 background = (texture(cubemap, dir.xyz).xyz); //vec4(0.8f, 0.9f, 0.95f, 1)
        out_color = fac * vec4(background, 1) / dir.w;
        accum += vec4(tonemapFilmic(out_color.xyz), out_color.w);
        orb.w = 0.f;
    }

    imageStore(accum_image, ivec2(gl_FragCoord.xy), accum);
    out_color = accum / (arb.x);

    out_color.xyz = out_color.xyz;

    imageStore(i_orig_bounce, ivec2(gl_FragCoord.xy), orb);
    imageStore(i_dir, ivec2(gl_FragCoord.xy), dir);
    imageStore(fac_image, ivec2(gl_FragCoord.xy), fac);
    imageStore(i_arb, ivec2(gl_FragCoord.xy), arb);
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
    mygl::texture arb;
    glCreateTextures(GL_TEXTURE_2D, 1, &arb);
    glTextureStorage2D(arb, 1, GL_RGBA32F, 1280, 720);

    int  l;
    char buf[512];
    glGetShaderInfoLog(vs, 512, &l, buf);
    std::cout << buf << '\n';
    glGetShaderInfoLog(fs, 512, &l, buf);
    std::cout << buf << '\n';
    glGetProgramInfoLog(pr, 512, &l, buf);
    std::cout << buf << '\n';

    gfx::image_file cpx("moulton_station_train_tunnel_west_16k.hdr/hdr/posx.hdr", gfx::bits::b32, 4);
    gfx::image_file cnx("moulton_station_train_tunnel_west_16k.hdr/hdr/negx.hdr", gfx::bits::b32, 4);
    gfx::image_file cpy("moulton_station_train_tunnel_west_16k.hdr/hdr/posy.hdr", gfx::bits::b32, 4);
    gfx::image_file cny("moulton_station_train_tunnel_west_16k.hdr/hdr/negy.hdr", gfx::bits::b32, 4);
    gfx::image_file cpz("moulton_station_train_tunnel_west_16k.hdr/hdr/posz.hdr", gfx::bits::b32, 4);
    gfx::image_file cnz("moulton_station_train_tunnel_west_16k.hdr/hdr/negz.hdr", gfx::bits::b32, 4);

    mygl::texture cubemap;
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &cubemap);
    glTextureStorage2D(cubemap, 1, GL_RGBA32F, cpx.width, cpx.height);
    glTextureSubImage3D(cubemap, 0, 0, 0, 0, cpx.width, cpx.height, 1, GL_RGBA, GL_FLOAT, cpx.bytes());
    glTextureSubImage3D(cubemap, 0, 0, 0, 1, cpx.width, cpx.height, 1, GL_RGBA, GL_FLOAT, cnx.bytes());
    glTextureSubImage3D(cubemap, 0, 0, 0, 2, cpx.width, cpx.height, 1, GL_RGBA, GL_FLOAT, cpy.bytes());
    glTextureSubImage3D(cubemap, 0, 0, 0, 3, cpx.width, cpx.height, 1, GL_RGBA, GL_FLOAT, cny.bytes());
    glTextureSubImage3D(cubemap, 0, 0, 0, 4, cpx.width, cpx.height, 1, GL_RGBA, GL_FLOAT, cpz.bytes());
    glTextureSubImage3D(cubemap, 0, 0, 0, 5, cpx.width, cpx.height, 1, GL_RGBA, GL_FLOAT, cnz.bytes());

    mygl::sampler cubemap_sampler;
    glCreateSamplers(1, &cubemap_sampler);
    glSamplerParameteri(cubemap_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(cubemap_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(cubemap_sampler, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glSamplerParameteri(cubemap_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(cubemap_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

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
            glClearTexImage(arb, 0, GL_RGBA, GL_FLOAT, value_ptr(clear));
            glClearTexImage(origbounce, 0, GL_RGBA, GL_FLOAT, value_ptr(clear));
        }

        count = (count + 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(pr);
        glUniform1ui(0, count);
        glBindTextureUnit(0, cubemap);
        glBindSampler(0, cubemap_sampler);

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
        glBindImageTexture(4, arb, 0, false, 0, GL_READ_WRITE, GL_RGBA32F);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(w);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}