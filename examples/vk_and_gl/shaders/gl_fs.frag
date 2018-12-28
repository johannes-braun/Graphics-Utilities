#version 460 core
#define USE_OPENGL 1

#define LAYOUTS_DEFINED
#define layout_buffer_binding_models binding = 0
#define layout_buffer_binding_vertex binding = buffer_binding_vertex
#define layout_buffer_binding_element binding = buffer_binding_element
#define layout_buffer_binding_bvh binding = buffer_binding_bvh
#define layout_texture_binding_all binding = texture_binding_all
#define layout_shadow_map_binding binding = shadow_map_binding
#define layout_shadow_cam_binding binding = shadow_cam_binding

#include "def.hpp"
#include "instance.glsl"
#include "geometry.glsl"
#include "fs_ws.frag.glsl"
