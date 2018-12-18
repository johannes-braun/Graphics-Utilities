#version 460 core
#include "instance.glsl"
#include "geometry.glsl"
#include "def.hpp"

#define LAYOUTS_DEFINED
#define layout_buffer_binding_models set = 1, binding = 0
#define layout_buffer_binding_vertex set = 1, binding = buffer_binding_vertex
#define layout_buffer_binding_element set = 1, binding = buffer_binding_element
#define layout_buffer_binding_bvh set = 1, binding = buffer_binding_bvh

#include "fs.frag.glsl"