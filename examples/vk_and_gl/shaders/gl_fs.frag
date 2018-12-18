#version 460 core
#include "instance.glsl"
#include "geometry.glsl"
#include "def.hpp"

#define LAYOUTS_DEFINED
#define layout_buffer_binding_models binding = 0
#define layout_buffer_binding_vertex binding = buffer_binding_vertex
#define layout_buffer_binding_element binding = buffer_binding_element
#define layout_buffer_binding_bvh binding = buffer_binding_bvh

#include "fs.frag.glsl"