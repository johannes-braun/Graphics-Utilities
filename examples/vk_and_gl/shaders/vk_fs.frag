#version 460 core
#define USE_VULKAN 1

#define LAYOUTS_DEFINED
#define layout_buffer_binding_models set = 1, binding = 0
#define layout_buffer_binding_vertex set = 1, binding = buffer_binding_vertex
#define layout_buffer_binding_element set = 1, binding = buffer_binding_element
#define layout_buffer_binding_bvh set = 1, binding = buffer_binding_bvh
#define layout_texture_binding_all set = 2, binding = texture_binding_all

#include "def.hpp"
#include "instance.glsl"
#include "geometry.glsl"
#include "fs.frag.glsl"
