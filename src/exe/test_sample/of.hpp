#pragma once

#define GL_APIENTRY *
#include <stddef.h>
#ifndef GLEXT_64_TYPES_DEFINED
/* This code block is duplicated in glxext.h, so must be protected */
#define GLEXT_64_TYPES_DEFINED
/* Define int32_t, int64_t, and uint64_t types for UST/MSC */
/* (as used in the GL_EXT_timer_query extension). */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#include <inttypes.h>
#elif defined(__sun__) || defined(__digital__)
#include <inttypes.h>
#if defined(__STDC__)
#if defined(__arch64__) || defined(_LP64)
typedef long int int64_t;
typedef unsigned long int uint64_t;
#else
typedef long long int int64_t;
typedef unsigned long long int uint64_t;
#endif /* __arch64__ */
#endif /* __STDC__ */
#elif defined( __VMS ) || defined(__sgi)
#include <inttypes.h>
#elif defined(__SCO__) || defined(__USLC__)
#include <stdint.h>
#elif defined(__UNIXOS2__) || defined(__SOL64__)
typedef long int int32_t;
typedef long long int int64_t;
typedef unsigned long long int uint64_t;
#elif defined(_WIN32) && defined(__GNUC__)
#include <stdint.h>
#elif defined(_WIN32)
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
/* Fallback if nothing above works */
#include <inttypes.h>
#endif
#endif
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLclampx;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void *GLeglClientBufferEXT;
typedef void *GLeglImageOES;
typedef char GLchar;
typedef char GLcharARB;
#ifdef __APPLE__
typedef void *GLhandleARB;
#else
typedef unsigned int GLhandleARB;
#endif
typedef unsigned short GLhalfARB;
typedef unsigned short GLhalf;
typedef GLint GLfixed;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;
typedef int64_t GLint64;
typedef uint64_t GLuint64;
typedef ptrdiff_t GLintptrARB;
typedef ptrdiff_t GLsizeiptrARB;
typedef int64_t GLint64EXT;
typedef uint64_t GLuint64EXT;
typedef struct __GLsync *GLsync;
struct _cl_context;
struct _cl_event;
typedef void (GL_APIENTRY *GLDEBUGPROC)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef void (GL_APIENTRY *GLDEBUGPROCARB)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef void (GL_APIENTRY *GLDEBUGPROCKHR)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef void (GL_APIENTRY *GLDEBUGPROCAMD)(GLuint id,GLenum category,GLenum severity,GLsizei length,const GLchar *message,void *userParam);
typedef unsigned short GLhalfNV;
typedef GLintptr GLvdpauSurfaceNV;
typedef void (GL_APIENTRY *GLVULKANPROCNV)(void);

#define GL_ARB_bindless_texture 1
#define GL_ARB_compute_variable_group_size 1
#define GL_ARB_direct_state_access 1
#define GL_ARB_vertex_attrib_binding 1
#define GL_EXT_texture_filter_anisotropic 1
#define GL_KHR_debug 1
#define GL_NVX_gpu_memory_info 1
#define GL_NV_bindless_multi_draw_indirect 1
#define GL_NV_bindless_multi_draw_indirect_count 1
#define GL_NV_command_list 1
#define GL_NV_path_rendering 1
#define GL_NV_path_rendering_shared_edge 1
#define GL_NV_shader_buffer_load 1
#define GL_NV_shader_buffer_store 1
#define GL_NV_uniform_buffer_unified_memory 1
#define GL_NV_vertex_buffer_unified_memory 1
#define GL_ARB_depth_buffer_float 1
extern bool enable_ARB_depth_buffer_float;
#define GL_ARB_framebuffer_object 1
extern bool enable_ARB_framebuffer_object;
#define GL_ARB_framebuffer_sRGB 1
extern bool enable_ARB_framebuffer_sRGB;
#define GL_ARB_half_float_vertex 1
extern bool enable_ARB_half_float_vertex;
#define GL_ARB_map_buffer_range 1
extern bool enable_ARB_map_buffer_range;
#define GL_ARB_texture_compression_rgtc 1
extern bool enable_ARB_texture_compression_rgtc;
#define GL_ARB_texture_rg 1
extern bool enable_ARB_texture_rg;
#define GL_ARB_vertex_array_object 1
extern bool enable_ARB_vertex_array_object;
#define GL_ARB_copy_buffer 1
extern bool enable_ARB_copy_buffer;
#define GL_ARB_uniform_buffer_object 1
extern bool enable_ARB_uniform_buffer_object;
#define GL_ARB_depth_clamp 1
extern bool enable_ARB_depth_clamp;
#define GL_ARB_draw_elements_base_vertex 1
extern bool enable_ARB_draw_elements_base_vertex;
#define GL_ARB_fragment_coord_conventions 1
extern bool enable_ARB_fragment_coord_conventions;
#define GL_ARB_provoking_vertex 1
extern bool enable_ARB_provoking_vertex;
#define GL_ARB_seamless_cube_map 1
extern bool enable_ARB_seamless_cube_map;
#define GL_ARB_sync 1
extern bool enable_ARB_sync;
#define GL_ARB_texture_multisample 1
extern bool enable_ARB_texture_multisample;
#define GL_ARB_blend_func_extended 1
extern bool enable_ARB_blend_func_extended;
#define GL_ARB_explicit_attrib_location 1
extern bool enable_ARB_explicit_attrib_location;
#define GL_ARB_occlusion_query2 1
extern bool enable_ARB_occlusion_query2;
#define GL_ARB_sampler_objects 1
extern bool enable_ARB_sampler_objects;
#define GL_ARB_shader_bit_encoding 1
extern bool enable_ARB_shader_bit_encoding;
#define GL_ARB_texture_rgb10_a2ui 1
extern bool enable_ARB_texture_rgb10_a2ui;
#define GL_ARB_texture_swizzle 1
extern bool enable_ARB_texture_swizzle;
#define GL_ARB_timer_query 1
extern bool enable_ARB_timer_query;
#define GL_ARB_vertex_type_2_10_10_10_rev 1
extern bool enable_ARB_vertex_type_2_10_10_10_rev;
#define GL_ARB_draw_buffers_blend 1
extern bool enable_ARB_draw_buffers_blend;
#define GL_ARB_draw_indirect 1
extern bool enable_ARB_draw_indirect;
#define GL_ARB_gpu_shader5 1
extern bool enable_ARB_gpu_shader5;
#define GL_ARB_gpu_shader_fp64 1
extern bool enable_ARB_gpu_shader_fp64;
#define GL_ARB_shader_subroutine 1
extern bool enable_ARB_shader_subroutine;
#define GL_ARB_tessellation_shader 1
extern bool enable_ARB_tessellation_shader;
#define GL_ARB_texture_buffer_object_rgb32 1
extern bool enable_ARB_texture_buffer_object_rgb32;
#define GL_ARB_texture_cube_map_array 1
extern bool enable_ARB_texture_cube_map_array;
#define GL_ARB_texture_gather 1
extern bool enable_ARB_texture_gather;
#define GL_ARB_texture_query_lod 1
extern bool enable_ARB_texture_query_lod;
#define GL_ARB_transform_feedback2 1
extern bool enable_ARB_transform_feedback2;
#define GL_ARB_transform_feedback3 1
extern bool enable_ARB_transform_feedback3;
#define GL_ARB_ES2_compatibility 1
extern bool enable_ARB_ES2_compatibility;
#define GL_ARB_get_program_binary 1
extern bool enable_ARB_get_program_binary;
#define GL_ARB_separate_shader_objects 1
extern bool enable_ARB_separate_shader_objects;
#define GL_ARB_shader_precision 1
extern bool enable_ARB_shader_precision;
#define GL_ARB_vertex_attrib_64bit 1
extern bool enable_ARB_vertex_attrib_64bit;
#define GL_ARB_viewport_array 1
extern bool enable_ARB_viewport_array;
#define GL_ARB_ES2_compatibility 1
extern bool enable_ARB_ES2_compatibility;
#define GL_ARB_get_program_binary 1
extern bool enable_ARB_get_program_binary;
#define GL_ARB_separate_shader_objects 1
extern bool enable_ARB_separate_shader_objects;
#define GL_ARB_shader_precision 1
extern bool enable_ARB_shader_precision;
#define GL_ARB_vertex_attrib_64bit 1
extern bool enable_ARB_vertex_attrib_64bit;
#define GL_ARB_viewport_array 1
extern bool enable_ARB_viewport_array;
#define GL_ARB_base_instance 1
extern bool enable_ARB_base_instance;
#define GL_ARB_shading_language_420pack 1
extern bool enable_ARB_shading_language_420pack;
#define GL_ARB_transform_feedback_instanced 1
extern bool enable_ARB_transform_feedback_instanced;
#define GL_ARB_compressed_texture_pixel_storage 1
extern bool enable_ARB_compressed_texture_pixel_storage;
#define GL_ARB_conservative_depth 1
extern bool enable_ARB_conservative_depth;
#define GL_ARB_internalformat_query 1
extern bool enable_ARB_internalformat_query;
#define GL_ARB_map_buffer_alignment 1
extern bool enable_ARB_map_buffer_alignment;
#define GL_ARB_shader_atomic_counters 1
extern bool enable_ARB_shader_atomic_counters;
#define GL_ARB_shader_image_load_store 1
extern bool enable_ARB_shader_image_load_store;
#define GL_ARB_shading_language_packing 1
extern bool enable_ARB_shading_language_packing;
#define GL_ARB_texture_compression_bptc 1
extern bool enable_ARB_texture_compression_bptc;
#define GL_ARB_texture_storage 1
extern bool enable_ARB_texture_storage;
#define GL_ARB_base_instance 1
extern bool enable_ARB_base_instance;
#define GL_ARB_compressed_texture_pixel_storage 1
extern bool enable_ARB_compressed_texture_pixel_storage;
#define GL_ARB_conservative_depth 1
extern bool enable_ARB_conservative_depth;
#define GL_ARB_internalformat_query 1
extern bool enable_ARB_internalformat_query;
#define GL_ARB_map_buffer_alignment 1
extern bool enable_ARB_map_buffer_alignment;
#define GL_ARB_shader_atomic_counters 1
extern bool enable_ARB_shader_atomic_counters;
#define GL_ARB_shader_image_load_store 1
extern bool enable_ARB_shader_image_load_store;
#define GL_ARB_shading_language_420pack 1
extern bool enable_ARB_shading_language_420pack;
#define GL_ARB_shading_language_packing 1
extern bool enable_ARB_shading_language_packing;
#define GL_ARB_texture_storage 1
extern bool enable_ARB_texture_storage;
#define GL_ARB_transform_feedback_instanced 1
extern bool enable_ARB_transform_feedback_instanced;
#define GL_ARB_arrays_of_arrays 1
extern bool enable_ARB_arrays_of_arrays;
#define GL_ARB_fragment_layer_viewport 1
extern bool enable_ARB_fragment_layer_viewport;
#define GL_ARB_shader_image_size 1
extern bool enable_ARB_shader_image_size;
#define GL_ARB_ES3_compatibility 1
extern bool enable_ARB_ES3_compatibility;
#define GL_ARB_clear_buffer_object 1
extern bool enable_ARB_clear_buffer_object;
#define GL_ARB_compute_shader 1
extern bool enable_ARB_compute_shader;
#define GL_ARB_copy_image 1
extern bool enable_ARB_copy_image;
#define GL_KHR_debug 1
extern bool enable_KHR_debug;
#define GL_ARB_explicit_uniform_location 1
extern bool enable_ARB_explicit_uniform_location;
#define GL_ARB_framebuffer_no_attachments 1
extern bool enable_ARB_framebuffer_no_attachments;
#define GL_ARB_internalformat_query2 1
extern bool enable_ARB_internalformat_query2;
#define GL_ARB_invalidate_subdata 1
extern bool enable_ARB_invalidate_subdata;
#define GL_ARB_multi_draw_indirect 1
extern bool enable_ARB_multi_draw_indirect;
#define GL_ARB_program_interface_query 1
extern bool enable_ARB_program_interface_query;
#define GL_ARB_robust_buffer_access_behavior 1
extern bool enable_ARB_robust_buffer_access_behavior;
#define GL_ARB_shader_storage_buffer_object 1
extern bool enable_ARB_shader_storage_buffer_object;
#define GL_ARB_stencil_texturing 1
extern bool enable_ARB_stencil_texturing;
#define GL_ARB_texture_buffer_range 1
extern bool enable_ARB_texture_buffer_range;
#define GL_ARB_texture_query_levels 1
extern bool enable_ARB_texture_query_levels;
#define GL_ARB_texture_storage_multisample 1
extern bool enable_ARB_texture_storage_multisample;
#define GL_ARB_texture_view 1
extern bool enable_ARB_texture_view;
#define GL_ARB_vertex_attrib_binding 1
extern bool enable_ARB_vertex_attrib_binding;
#define GL_ARB_arrays_of_arrays 1
extern bool enable_ARB_arrays_of_arrays;
#define GL_ARB_clear_buffer_object 1
extern bool enable_ARB_clear_buffer_object;
#define GL_ARB_compute_shader 1
extern bool enable_ARB_compute_shader;
#define GL_ARB_copy_image 1
extern bool enable_ARB_copy_image;
#define GL_ARB_ES3_compatibility 1
extern bool enable_ARB_ES3_compatibility;
#define GL_ARB_explicit_uniform_location 1
extern bool enable_ARB_explicit_uniform_location;
#define GL_ARB_fragment_layer_viewport 1
extern bool enable_ARB_fragment_layer_viewport;
#define GL_ARB_framebuffer_no_attachments 1
extern bool enable_ARB_framebuffer_no_attachments;
#define GL_ARB_internalformat_query2 1
extern bool enable_ARB_internalformat_query2;
#define GL_ARB_invalidate_subdata 1
extern bool enable_ARB_invalidate_subdata;
#define GL_ARB_multi_draw_indirect 1
extern bool enable_ARB_multi_draw_indirect;
#define GL_ARB_program_interface_query 1
extern bool enable_ARB_program_interface_query;
#define GL_ARB_robust_buffer_access_behavior 1
extern bool enable_ARB_robust_buffer_access_behavior;
#define GL_ARB_shader_image_size 1
extern bool enable_ARB_shader_image_size;
#define GL_ARB_shader_storage_buffer_object 1
extern bool enable_ARB_shader_storage_buffer_object;
#define GL_ARB_stencil_texturing 1
extern bool enable_ARB_stencil_texturing;
#define GL_ARB_texture_buffer_range 1
extern bool enable_ARB_texture_buffer_range;
#define GL_ARB_texture_query_levels 1
extern bool enable_ARB_texture_query_levels;
#define GL_ARB_texture_storage_multisample 1
extern bool enable_ARB_texture_storage_multisample;
#define GL_ARB_texture_view 1
extern bool enable_ARB_texture_view;
#define GL_ARB_vertex_attrib_binding 1
extern bool enable_ARB_vertex_attrib_binding;
#define GL_KHR_debug 1
extern bool enable_KHR_debug;
#define GL_GL_ARB_buffer_storage 1
extern bool enable_GL_ARB_buffer_storage;
#define GL_GL_ARB_clear_texture 1
extern bool enable_GL_ARB_clear_texture;
#define GL_GL_ARB_enhanced_layouts 1
extern bool enable_GL_ARB_enhanced_layouts;
#define GL_GL_ARB_multi_bind 1
extern bool enable_GL_ARB_multi_bind;
#define GL_GL_ARB_query_buffer_object 1
extern bool enable_GL_ARB_query_buffer_object;
#define GL_GL_ARB_texture_mirror_clamp_to_edge 1
extern bool enable_GL_ARB_texture_mirror_clamp_to_edge;
#define GL_GL_ARB_texture_stencil8 1
extern bool enable_GL_ARB_texture_stencil8;
#define GL_GL_ARB_vertex_type_10f_11f_11f_rev 1
extern bool enable_GL_ARB_vertex_type_10f_11f_11f_rev;
#define GL_GL_ARB_clip_control 1
extern bool enable_GL_ARB_clip_control;
#define GL_GL_ARB_conditional_render_inverted 1
extern bool enable_GL_ARB_conditional_render_inverted;
#define GL_GL_ARB_cull_distance 1
extern bool enable_GL_ARB_cull_distance;
#define GL_GL_ARB_direct_state_access 1
extern bool enable_GL_ARB_direct_state_access;
#define GL_GL_ARB_ES3_1_compatibility 1
extern bool enable_GL_ARB_ES3_1_compatibility;
#define GL_GL_ARB_get_texture_sub_image 1
extern bool enable_GL_ARB_get_texture_sub_image;
#define GL_GL_ARB_robustness 1
extern bool enable_GL_ARB_robustness;
#define GL_GL_ARB_texture_barrier 1
extern bool enable_GL_ARB_texture_barrier;
#define GL_GL_KHR_context_flush_control 1
extern bool enable_GL_KHR_context_flush_control;
#define GL_GL_KHR_context_flush_control 1
extern bool enable_GL_KHR_context_flush_control;
#define GL_GL_ARB_gl_spirv 1
extern bool enable_GL_ARB_gl_spirv;
#define GL_GL_ARB_indirect_parameters 1
extern bool enable_GL_ARB_indirect_parameters;
#define GL_GL_KHR_no_error 1
extern bool enable_GL_KHR_no_error;
#define GL_GL_ARB_pipeline_statistics_query 1
extern bool enable_GL_ARB_pipeline_statistics_query;
#define GL_GL_ARB_polygon_offset_clamp 1
extern bool enable_GL_ARB_polygon_offset_clamp;
#define GL_GL_ARB_shader_atomic_counter_ops 1
extern bool enable_GL_ARB_shader_atomic_counter_ops;
#define GL_GL_ARB_shader_draw_parameters 1
extern bool enable_GL_ARB_shader_draw_parameters;
#define GL_GL_ARB_shader_group_vote 1
extern bool enable_GL_ARB_shader_group_vote;
#define GL_GL_ARB_spirv_extensions 1
extern bool enable_GL_ARB_spirv_extensions;
#define GL_GL_ARB_texture_filter_anisotropic 1
extern bool enable_GL_ARB_texture_filter_anisotropic;
#define GL_GL_ARB_transform_feedback_overflow_query 1
extern bool enable_GL_ARB_transform_feedback_overflow_query;


enum GLenum_values {
    GL_DEPTH_BUFFER_BIT = 0x00000100,
    GL_STENCIL_BUFFER_BIT = 0x00000400,
    GL_COLOR_BUFFER_BIT = 0x00004000,
    GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT = 0x00000001,
    GL_CONTEXT_FLAG_DEBUG_BIT = 0x00000002,
    GL_CONTEXT_FLAG_DEBUG_BIT_KHR = 0x00000002,
    GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT = 0x00000004,
    GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT_ARB = 0x00000004,
    GL_CONTEXT_FLAG_NO_ERROR_BIT = 0x00000008,
    GL_CONTEXT_FLAG_NO_ERROR_BIT_KHR = 0x00000008,
    GL_CONTEXT_CORE_PROFILE_BIT = 0x00000001,
    GL_CONTEXT_COMPATIBILITY_PROFILE_BIT = 0x00000002,
    GL_MAP_READ_BIT = 0x0001,
    GL_MAP_WRITE_BIT = 0x0002,
    GL_MAP_INVALIDATE_RANGE_BIT = 0x0004,
    GL_MAP_INVALIDATE_BUFFER_BIT = 0x0008,
    GL_MAP_FLUSH_EXPLICIT_BIT = 0x0010,
    GL_MAP_UNSYNCHRONIZED_BIT = 0x0020,
    GL_MAP_PERSISTENT_BIT = 0x0040,
    GL_MAP_COHERENT_BIT = 0x0080,
    GL_DYNAMIC_STORAGE_BIT = 0x0100,
    GL_CLIENT_STORAGE_BIT = 0x0200,
    GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT = 0x00000001,
    GL_ELEMENT_ARRAY_BARRIER_BIT = 0x00000002,
    GL_UNIFORM_BARRIER_BIT = 0x00000004,
    GL_TEXTURE_FETCH_BARRIER_BIT = 0x00000008,
    GL_SHADER_GLOBAL_ACCESS_BARRIER_BIT_NV = 0x00000010,
    GL_SHADER_IMAGE_ACCESS_BARRIER_BIT = 0x00000020,
    GL_COMMAND_BARRIER_BIT = 0x00000040,
    GL_PIXEL_BUFFER_BARRIER_BIT = 0x00000080,
    GL_TEXTURE_UPDATE_BARRIER_BIT = 0x00000100,
    GL_BUFFER_UPDATE_BARRIER_BIT = 0x00000200,
    GL_FRAMEBUFFER_BARRIER_BIT = 0x00000400,
    GL_TRANSFORM_FEEDBACK_BARRIER_BIT = 0x00000800,
    GL_ATOMIC_COUNTER_BARRIER_BIT = 0x00001000,
    GL_SHADER_STORAGE_BARRIER_BIT = 0x00002000,
    GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT = 0x00004000,
    GL_QUERY_BUFFER_BARRIER_BIT = 0x00008000,
    GL_ALL_BARRIER_BITS = 0xFFFFFFFF,
    GL_SYNC_FLUSH_COMMANDS_BIT = 0x00000001,
    GL_VERTEX_SHADER_BIT = 0x00000001,
    GL_FRAGMENT_SHADER_BIT = 0x00000002,
    GL_GEOMETRY_SHADER_BIT = 0x00000004,
    GL_TESS_CONTROL_SHADER_BIT = 0x00000008,
    GL_TESS_EVALUATION_SHADER_BIT = 0x00000010,
    GL_COMPUTE_SHADER_BIT = 0x00000020,
    GL_ALL_SHADER_BITS = 0xFFFFFFFF,
    GL_BOLD_BIT_NV = 0x01,
    GL_ITALIC_BIT_NV = 0x02,
    GL_GLYPH_WIDTH_BIT_NV = 0x01,
    GL_GLYPH_HEIGHT_BIT_NV = 0x02,
    GL_GLYPH_HORIZONTAL_BEARING_X_BIT_NV = 0x04,
    GL_GLYPH_HORIZONTAL_BEARING_Y_BIT_NV = 0x08,
    GL_GLYPH_HORIZONTAL_BEARING_ADVANCE_BIT_NV = 0x10,
    GL_GLYPH_VERTICAL_BEARING_X_BIT_NV = 0x20,
    GL_GLYPH_VERTICAL_BEARING_Y_BIT_NV = 0x40,
    GL_GLYPH_VERTICAL_BEARING_ADVANCE_BIT_NV = 0x80,
    GL_GLYPH_HAS_KERNING_BIT_NV = 0x100,
    GL_FONT_X_MIN_BOUNDS_BIT_NV = 0x00010000,
    GL_FONT_Y_MIN_BOUNDS_BIT_NV = 0x00020000,
    GL_FONT_X_MAX_BOUNDS_BIT_NV = 0x00040000,
    GL_FONT_Y_MAX_BOUNDS_BIT_NV = 0x00080000,
    GL_FONT_UNITS_PER_EM_BIT_NV = 0x00100000,
    GL_FONT_ASCENDER_BIT_NV = 0x00200000,
    GL_FONT_DESCENDER_BIT_NV = 0x00400000,
    GL_FONT_HEIGHT_BIT_NV = 0x00800000,
    GL_FONT_MAX_ADVANCE_WIDTH_BIT_NV = 0x01000000,
    GL_FONT_MAX_ADVANCE_HEIGHT_BIT_NV = 0x02000000,
    GL_FONT_UNDERLINE_POSITION_BIT_NV = 0x04000000,
    GL_FONT_UNDERLINE_THICKNESS_BIT_NV = 0x08000000,
    GL_FONT_HAS_KERNING_BIT_NV = 0x10000000,
    GL_FONT_NUM_GLYPH_INDICES_BIT_NV = 0x20000000,
    GL_TERMINATE_SEQUENCE_COMMAND_NV = 0x0000,
    GL_NOP_COMMAND_NV = 0x0001,
    GL_DRAW_ELEMENTS_COMMAND_NV = 0x0002,
    GL_DRAW_ARRAYS_COMMAND_NV = 0x0003,
    GL_DRAW_ELEMENTS_STRIP_COMMAND_NV = 0x0004,
    GL_DRAW_ARRAYS_STRIP_COMMAND_NV = 0x0005,
    GL_DRAW_ELEMENTS_INSTANCED_COMMAND_NV = 0x0006,
    GL_DRAW_ARRAYS_INSTANCED_COMMAND_NV = 0x0007,
    GL_ELEMENT_ADDRESS_COMMAND_NV = 0x0008,
    GL_ATTRIBUTE_ADDRESS_COMMAND_NV = 0x0009,
    GL_UNIFORM_ADDRESS_COMMAND_NV = 0x000A,
    GL_BLEND_COLOR_COMMAND_NV = 0x000B,
    GL_STENCIL_REF_COMMAND_NV = 0x000C,
    GL_LINE_WIDTH_COMMAND_NV = 0x000D,
    GL_POLYGON_OFFSET_COMMAND_NV = 0x000E,
    GL_ALPHA_REF_COMMAND_NV = 0x000F,
    GL_VIEWPORT_COMMAND_NV = 0x0010,
    GL_SCISSOR_COMMAND_NV = 0x0011,
    GL_FRONT_FACE_COMMAND_NV = 0x0012,
    GL_CLOSE_PATH_NV = 0x00,
    GL_MOVE_TO_NV = 0x02,
    GL_RELATIVE_MOVE_TO_NV = 0x03,
    GL_LINE_TO_NV = 0x04,
    GL_RELATIVE_LINE_TO_NV = 0x05,
    GL_HORIZONTAL_LINE_TO_NV = 0x06,
    GL_RELATIVE_HORIZONTAL_LINE_TO_NV = 0x07,
    GL_VERTICAL_LINE_TO_NV = 0x08,
    GL_RELATIVE_VERTICAL_LINE_TO_NV = 0x09,
    GL_QUADRATIC_CURVE_TO_NV = 0x0A,
    GL_RELATIVE_QUADRATIC_CURVE_TO_NV = 0x0B,
    GL_CUBIC_CURVE_TO_NV = 0x0C,
    GL_RELATIVE_CUBIC_CURVE_TO_NV = 0x0D,
    GL_SMOOTH_QUADRATIC_CURVE_TO_NV = 0x0E,
    GL_RELATIVE_SMOOTH_QUADRATIC_CURVE_TO_NV = 0x0F,
    GL_SMOOTH_CUBIC_CURVE_TO_NV = 0x10,
    GL_RELATIVE_SMOOTH_CUBIC_CURVE_TO_NV = 0x11,
    GL_SMALL_CCW_ARC_TO_NV = 0x12,
    GL_RELATIVE_SMALL_CCW_ARC_TO_NV = 0x13,
    GL_SMALL_CW_ARC_TO_NV = 0x14,
    GL_RELATIVE_SMALL_CW_ARC_TO_NV = 0x15,
    GL_LARGE_CCW_ARC_TO_NV = 0x16,
    GL_RELATIVE_LARGE_CCW_ARC_TO_NV = 0x17,
    GL_LARGE_CW_ARC_TO_NV = 0x18,
    GL_RELATIVE_LARGE_CW_ARC_TO_NV = 0x19,
    GL_CONIC_CURVE_TO_NV = 0x1A,
    GL_RELATIVE_CONIC_CURVE_TO_NV = 0x1B,
    GL_SHARED_EDGE_NV = 0xC0,
    GL_ROUNDED_RECT_NV = 0xE8,
    GL_RELATIVE_ROUNDED_RECT_NV = 0xE9,
    GL_ROUNDED_RECT2_NV = 0xEA,
    GL_RELATIVE_ROUNDED_RECT2_NV = 0xEB,
    GL_ROUNDED_RECT4_NV = 0xEC,
    GL_RELATIVE_ROUNDED_RECT4_NV = 0xED,
    GL_ROUNDED_RECT8_NV = 0xEE,
    GL_RELATIVE_ROUNDED_RECT8_NV = 0xEF,
    GL_RESTART_PATH_NV = 0xF0,
    GL_DUP_FIRST_CUBIC_CURVE_TO_NV = 0xF2,
    GL_DUP_LAST_CUBIC_CURVE_TO_NV = 0xF4,
    GL_RECT_NV = 0xF6,
    GL_RELATIVE_RECT_NV = 0xF7,
    GL_CIRCULAR_CCW_ARC_TO_NV = 0xF8,
    GL_CIRCULAR_CW_ARC_TO_NV = 0xFA,
    GL_CIRCULAR_TANGENT_ARC_TO_NV = 0xFC,
    GL_ARC_TO_NV = 0xFE,
    GL_RELATIVE_ARC_TO_NV = 0xFF,
    GL_FALSE = 0,
    GL_NO_ERROR = 0,
    GL_ZERO = 0,
    GL_NONE = 0,
    GL_TRUE = 1,
    GL_ONE = 1,
    GL_INVALID_INDEX = 0xFFFFFFFF,
    GL_TIMEOUT_IGNORED = 0xFFFFFFFFFFFFFFFF,
    GL_POINTS = 0x0000,
    GL_LINES = 0x0001,
    GL_LINE_LOOP = 0x0002,
    GL_LINE_STRIP = 0x0003,
    GL_TRIANGLES = 0x0004,
    GL_TRIANGLE_STRIP = 0x0005,
    GL_TRIANGLE_FAN = 0x0006,
    GL_QUADS = 0x0007,
    GL_LINES_ADJACENCY = 0x000A,
    GL_LINE_STRIP_ADJACENCY = 0x000B,
    GL_TRIANGLES_ADJACENCY = 0x000C,
    GL_TRIANGLE_STRIP_ADJACENCY = 0x000D,
    GL_PATCHES = 0x000E,
    GL_NEVER = 0x0200,
    GL_LESS = 0x0201,
    GL_EQUAL = 0x0202,
    GL_LEQUAL = 0x0203,
    GL_GREATER = 0x0204,
    GL_NOTEQUAL = 0x0205,
    GL_GEQUAL = 0x0206,
    GL_ALWAYS = 0x0207,
    GL_SRC_COLOR = 0x0300,
    GL_ONE_MINUS_SRC_COLOR = 0x0301,
    GL_SRC_ALPHA = 0x0302,
    GL_ONE_MINUS_SRC_ALPHA = 0x0303,
    GL_DST_ALPHA = 0x0304,
    GL_ONE_MINUS_DST_ALPHA = 0x0305,
    GL_DST_COLOR = 0x0306,
    GL_ONE_MINUS_DST_COLOR = 0x0307,
    GL_SRC_ALPHA_SATURATE = 0x0308,
    GL_FRONT_LEFT = 0x0400,
    GL_FRONT_RIGHT = 0x0401,
    GL_BACK_LEFT = 0x0402,
    GL_BACK_RIGHT = 0x0403,
    GL_FRONT = 0x0404,
    GL_BACK = 0x0405,
    GL_LEFT = 0x0406,
    GL_RIGHT = 0x0407,
    GL_FRONT_AND_BACK = 0x0408,
    GL_INVALID_ENUM = 0x0500,
    GL_INVALID_VALUE = 0x0501,
    GL_INVALID_OPERATION = 0x0502,
    GL_STACK_OVERFLOW = 0x0503,
    GL_STACK_OVERFLOW_KHR = 0x0503,
    GL_STACK_UNDERFLOW = 0x0504,
    GL_STACK_UNDERFLOW_KHR = 0x0504,
    GL_OUT_OF_MEMORY = 0x0505,
    GL_INVALID_FRAMEBUFFER_OPERATION = 0x0506,
    GL_CONTEXT_LOST = 0x0507,
    GL_CW = 0x0900,
    GL_CCW = 0x0901,
    GL_POINT_SIZE = 0x0B11,
    GL_POINT_SIZE_RANGE = 0x0B12,
    GL_SMOOTH_POINT_SIZE_RANGE = 0x0B12,
    GL_POINT_SIZE_GRANULARITY = 0x0B13,
    GL_SMOOTH_POINT_SIZE_GRANULARITY = 0x0B13,
    GL_LINE_SMOOTH = 0x0B20,
    GL_LINE_WIDTH = 0x0B21,
    GL_LINE_WIDTH_RANGE = 0x0B22,
    GL_SMOOTH_LINE_WIDTH_RANGE = 0x0B22,
    GL_LINE_WIDTH_GRANULARITY = 0x0B23,
    GL_SMOOTH_LINE_WIDTH_GRANULARITY = 0x0B23,
    GL_POLYGON_MODE = 0x0B40,
    GL_POLYGON_SMOOTH = 0x0B41,
    GL_CULL_FACE = 0x0B44,
    GL_CULL_FACE_MODE = 0x0B45,
    GL_FRONT_FACE = 0x0B46,
    GL_DEPTH_RANGE = 0x0B70,
    GL_DEPTH_TEST = 0x0B71,
    GL_DEPTH_WRITEMASK = 0x0B72,
    GL_DEPTH_CLEAR_VALUE = 0x0B73,
    GL_DEPTH_FUNC = 0x0B74,
    GL_STENCIL_TEST = 0x0B90,
    GL_STENCIL_CLEAR_VALUE = 0x0B91,
    GL_STENCIL_FUNC = 0x0B92,
    GL_STENCIL_VALUE_MASK = 0x0B93,
    GL_STENCIL_FAIL = 0x0B94,
    GL_STENCIL_PASS_DEPTH_FAIL = 0x0B95,
    GL_STENCIL_PASS_DEPTH_PASS = 0x0B96,
    GL_STENCIL_REF = 0x0B97,
    GL_STENCIL_WRITEMASK = 0x0B98,
    GL_VIEWPORT = 0x0BA2,
    GL_PATH_MODELVIEW_STACK_DEPTH_NV = 0x0BA3,
    GL_PATH_PROJECTION_STACK_DEPTH_NV = 0x0BA4,
    GL_PATH_MODELVIEW_MATRIX_NV = 0x0BA6,
    GL_PATH_PROJECTION_MATRIX_NV = 0x0BA7,
    GL_DITHER = 0x0BD0,
    GL_BLEND_DST = 0x0BE0,
    GL_BLEND_SRC = 0x0BE1,
    GL_BLEND = 0x0BE2,
    GL_LOGIC_OP_MODE = 0x0BF0,
    GL_COLOR_LOGIC_OP = 0x0BF2,
    GL_DRAW_BUFFER = 0x0C01,
    GL_READ_BUFFER = 0x0C02,
    GL_SCISSOR_BOX = 0x0C10,
    GL_SCISSOR_TEST = 0x0C11,
    GL_COLOR_CLEAR_VALUE = 0x0C22,
    GL_COLOR_WRITEMASK = 0x0C23,
    GL_DOUBLEBUFFER = 0x0C32,
    GL_STEREO = 0x0C33,
    GL_LINE_SMOOTH_HINT = 0x0C52,
    GL_POLYGON_SMOOTH_HINT = 0x0C53,
    GL_UNPACK_SWAP_BYTES = 0x0CF0,
    GL_UNPACK_LSB_FIRST = 0x0CF1,
    GL_UNPACK_ROW_LENGTH = 0x0CF2,
    GL_UNPACK_SKIP_ROWS = 0x0CF3,
    GL_UNPACK_SKIP_PIXELS = 0x0CF4,
    GL_UNPACK_ALIGNMENT = 0x0CF5,
    GL_PACK_SWAP_BYTES = 0x0D00,
    GL_PACK_LSB_FIRST = 0x0D01,
    GL_PACK_ROW_LENGTH = 0x0D02,
    GL_PACK_SKIP_ROWS = 0x0D03,
    GL_PACK_SKIP_PIXELS = 0x0D04,
    GL_PACK_ALIGNMENT = 0x0D05,
    GL_MAX_CLIP_DISTANCES = 0x0D32,
    GL_MAX_TEXTURE_SIZE = 0x0D33,
    GL_PATH_MAX_MODELVIEW_STACK_DEPTH_NV = 0x0D36,
    GL_PATH_MAX_PROJECTION_STACK_DEPTH_NV = 0x0D38,
    GL_MAX_VIEWPORT_DIMS = 0x0D3A,
    GL_SUBPIXEL_BITS = 0x0D50,
    GL_TEXTURE_1D = 0x0DE0,
    GL_TEXTURE_2D = 0x0DE1,
    GL_TEXTURE_WIDTH = 0x1000,
    GL_TEXTURE_HEIGHT = 0x1001,
    GL_TEXTURE_INTERNAL_FORMAT = 0x1003,
    GL_TEXTURE_BORDER_COLOR = 0x1004,
    GL_TEXTURE_TARGET = 0x1006,
    GL_DONT_CARE = 0x1100,
    GL_FASTEST = 0x1101,
    GL_NICEST = 0x1102,
    GL_BYTE = 0x1400,
    GL_UNSIGNED_BYTE = 0x1401,
    GL_SHORT = 0x1402,
    GL_UNSIGNED_SHORT = 0x1403,
    GL_INT = 0x1404,
    GL_UNSIGNED_INT = 0x1405,
    GL_FLOAT = 0x1406,
    GL_2_BYTES_NV = 0x1407,
    GL_3_BYTES_NV = 0x1408,
    GL_4_BYTES_NV = 0x1409,
    GL_DOUBLE = 0x140A,
    GL_HALF_FLOAT = 0x140B,
    GL_FIXED = 0x140C,
    GL_UNSIGNED_INT64_ARB = 0x140F,
    GL_CLEAR = 0x1500,
    GL_AND = 0x1501,
    GL_AND_REVERSE = 0x1502,
    GL_COPY = 0x1503,
    GL_AND_INVERTED = 0x1504,
    GL_NOOP = 0x1505,
    GL_XOR = 0x1506,
    GL_OR = 0x1507,
    GL_NOR = 0x1508,
    GL_EQUIV = 0x1509,
    GL_INVERT = 0x150A,
    GL_OR_REVERSE = 0x150B,
    GL_COPY_INVERTED = 0x150C,
    GL_OR_INVERTED = 0x150D,
    GL_NAND = 0x150E,
    GL_SET = 0x150F,
    GL_PATH_MODELVIEW_NV = 0x1700,
    GL_PATH_PROJECTION_NV = 0x1701,
    GL_TEXTURE = 0x1702,
    GL_COLOR = 0x1800,
    GL_DEPTH = 0x1801,
    GL_STENCIL = 0x1802,
    GL_STENCIL_INDEX = 0x1901,
    GL_DEPTH_COMPONENT = 0x1902,
    GL_RED = 0x1903,
    GL_GREEN = 0x1904,
    GL_BLUE = 0x1905,
    GL_ALPHA = 0x1906,
    GL_RGB = 0x1907,
    GL_RGBA = 0x1908,
    GL_POINT = 0x1B00,
    GL_LINE = 0x1B01,
    GL_FILL = 0x1B02,
    GL_KEEP = 0x1E00,
    GL_REPLACE = 0x1E01,
    GL_INCR = 0x1E02,
    GL_DECR = 0x1E03,
    GL_VENDOR = 0x1F00,
    GL_RENDERER = 0x1F01,
    GL_VERSION = 0x1F02,
    GL_EXTENSIONS = 0x1F03,
    GL_EYE_LINEAR_NV = 0x2400,
    GL_OBJECT_LINEAR_NV = 0x2401,
    GL_NEAREST = 0x2600,
    GL_LINEAR = 0x2601,
    GL_NEAREST_MIPMAP_NEAREST = 0x2700,
    GL_LINEAR_MIPMAP_NEAREST = 0x2701,
    GL_NEAREST_MIPMAP_LINEAR = 0x2702,
    GL_LINEAR_MIPMAP_LINEAR = 0x2703,
    GL_TEXTURE_MAG_FILTER = 0x2800,
    GL_TEXTURE_MIN_FILTER = 0x2801,
    GL_TEXTURE_WRAP_S = 0x2802,
    GL_TEXTURE_WRAP_T = 0x2803,
    GL_REPEAT = 0x2901,
    GL_POLYGON_OFFSET_UNITS = 0x2A00,
    GL_POLYGON_OFFSET_POINT = 0x2A01,
    GL_POLYGON_OFFSET_LINE = 0x2A02,
    GL_R3_G3_B2 = 0x2A10,
    GL_CLIP_DISTANCE0 = 0x3000,
    GL_CLIP_DISTANCE1 = 0x3001,
    GL_CLIP_DISTANCE2 = 0x3002,
    GL_CLIP_DISTANCE3 = 0x3003,
    GL_CLIP_DISTANCE4 = 0x3004,
    GL_CLIP_DISTANCE5 = 0x3005,
    GL_CLIP_DISTANCE6 = 0x3006,
    GL_CLIP_DISTANCE7 = 0x3007,
    GL_CONSTANT_COLOR = 0x8001,
    GL_ONE_MINUS_CONSTANT_COLOR = 0x8002,
    GL_CONSTANT_ALPHA = 0x8003,
    GL_ONE_MINUS_CONSTANT_ALPHA = 0x8004,
    GL_BLEND_COLOR = 0x8005,
    GL_FUNC_ADD = 0x8006,
    GL_MIN = 0x8007,
    GL_MAX = 0x8008,
    GL_BLEND_EQUATION = 0x8009,
    GL_BLEND_EQUATION_RGB = 0x8009,
    GL_FUNC_SUBTRACT = 0x800A,
    GL_FUNC_REVERSE_SUBTRACT = 0x800B,
    GL_UNSIGNED_BYTE_3_3_2 = 0x8032,
    GL_UNSIGNED_SHORT_4_4_4_4 = 0x8033,
    GL_UNSIGNED_SHORT_5_5_5_1 = 0x8034,
    GL_UNSIGNED_INT_8_8_8_8 = 0x8035,
    GL_UNSIGNED_INT_10_10_10_2 = 0x8036,
    GL_POLYGON_OFFSET_FILL = 0x8037,
    GL_POLYGON_OFFSET_FACTOR = 0x8038,
    GL_RGB4 = 0x804F,
    GL_RGB5 = 0x8050,
    GL_RGB8 = 0x8051,
    GL_RGB10 = 0x8052,
    GL_RGB12 = 0x8053,
    GL_RGB16 = 0x8054,
    GL_RGBA2 = 0x8055,
    GL_RGBA4 = 0x8056,
    GL_RGB5_A1 = 0x8057,
    GL_RGBA8 = 0x8058,
    GL_RGB10_A2 = 0x8059,
    GL_RGBA12 = 0x805A,
    GL_RGBA16 = 0x805B,
    GL_TEXTURE_RED_SIZE = 0x805C,
    GL_TEXTURE_GREEN_SIZE = 0x805D,
    GL_TEXTURE_BLUE_SIZE = 0x805E,
    GL_TEXTURE_ALPHA_SIZE = 0x805F,
    GL_PROXY_TEXTURE_1D = 0x8063,
    GL_PROXY_TEXTURE_2D = 0x8064,
    GL_TEXTURE_BINDING_1D = 0x8068,
    GL_TEXTURE_BINDING_2D = 0x8069,
    GL_TEXTURE_BINDING_3D = 0x806A,
    GL_PACK_SKIP_IMAGES = 0x806B,
    GL_PACK_IMAGE_HEIGHT = 0x806C,
    GL_UNPACK_SKIP_IMAGES = 0x806D,
    GL_UNPACK_IMAGE_HEIGHT = 0x806E,
    GL_TEXTURE_3D = 0x806F,
    GL_PROXY_TEXTURE_3D = 0x8070,
    GL_TEXTURE_DEPTH = 0x8071,
    GL_TEXTURE_WRAP_R = 0x8072,
    GL_MAX_3D_TEXTURE_SIZE = 0x8073,
    GL_VERTEX_ARRAY = 0x8074,
    GL_VERTEX_ARRAY_KHR = 0x8074,
    GL_MULTISAMPLE = 0x809D,
    GL_SAMPLE_ALPHA_TO_COVERAGE = 0x809E,
    GL_SAMPLE_ALPHA_TO_ONE = 0x809F,
    GL_SAMPLE_COVERAGE = 0x80A0,
    GL_SAMPLE_BUFFERS = 0x80A8,
    GL_SAMPLES = 0x80A9,
    GL_SAMPLE_COVERAGE_VALUE = 0x80AA,
    GL_SAMPLE_COVERAGE_INVERT = 0x80AB,
    GL_BLEND_DST_RGB = 0x80C8,
    GL_BLEND_SRC_RGB = 0x80C9,
    GL_BLEND_DST_ALPHA = 0x80CA,
    GL_BLEND_SRC_ALPHA = 0x80CB,
    GL_BGR = 0x80E0,
    GL_BGRA = 0x80E1,
    GL_MAX_ELEMENTS_VERTICES = 0x80E8,
    GL_MAX_ELEMENTS_INDICES = 0x80E9,
    GL_PARAMETER_BUFFER = 0x80EE,
    GL_PARAMETER_BUFFER_ARB = 0x80EE,
    GL_PARAMETER_BUFFER_BINDING = 0x80EF,
    GL_PARAMETER_BUFFER_BINDING_ARB = 0x80EF,
    GL_POINT_FADE_THRESHOLD_SIZE = 0x8128,
    GL_CLAMP_TO_BORDER = 0x812D,
    GL_CLAMP_TO_EDGE = 0x812F,
    GL_TEXTURE_MIN_LOD = 0x813A,
    GL_TEXTURE_MAX_LOD = 0x813B,
    GL_TEXTURE_BASE_LEVEL = 0x813C,
    GL_TEXTURE_MAX_LEVEL = 0x813D,
    GL_DEPTH_COMPONENT16 = 0x81A5,
    GL_DEPTH_COMPONENT24 = 0x81A6,
    GL_DEPTH_COMPONENT32 = 0x81A7,
    GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING = 0x8210,
    GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE = 0x8211,
    GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE = 0x8212,
    GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE = 0x8213,
    GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE = 0x8214,
    GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE = 0x8215,
    GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE = 0x8216,
    GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE = 0x8217,
    GL_FRAMEBUFFER_DEFAULT = 0x8218,
    GL_FRAMEBUFFER_UNDEFINED = 0x8219,
    GL_DEPTH_STENCIL_ATTACHMENT = 0x821A,
    GL_MAJOR_VERSION = 0x821B,
    GL_MINOR_VERSION = 0x821C,
    GL_NUM_EXTENSIONS = 0x821D,
    GL_CONTEXT_FLAGS = 0x821E,
    GL_BUFFER_IMMUTABLE_STORAGE = 0x821F,
    GL_BUFFER_STORAGE_FLAGS = 0x8220,
    GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED = 0x8221,
    GL_COMPRESSED_RED = 0x8225,
    GL_COMPRESSED_RG = 0x8226,
    GL_RG = 0x8227,
    GL_RG_INTEGER = 0x8228,
    GL_R8 = 0x8229,
    GL_R16 = 0x822A,
    GL_RG8 = 0x822B,
    GL_RG16 = 0x822C,
    GL_R16F = 0x822D,
    GL_R32F = 0x822E,
    GL_RG16F = 0x822F,
    GL_RG32F = 0x8230,
    GL_R8I = 0x8231,
    GL_R8UI = 0x8232,
    GL_R16I = 0x8233,
    GL_R16UI = 0x8234,
    GL_R32I = 0x8235,
    GL_R32UI = 0x8236,
    GL_RG8I = 0x8237,
    GL_RG8UI = 0x8238,
    GL_RG16I = 0x8239,
    GL_RG16UI = 0x823A,
    GL_RG32I = 0x823B,
    GL_RG32UI = 0x823C,
    GL_DEBUG_OUTPUT_SYNCHRONOUS = 0x8242,
    GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR = 0x8242,
    GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH = 0x8243,
    GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_KHR = 0x8243,
    GL_DEBUG_CALLBACK_FUNCTION = 0x8244,
    GL_DEBUG_CALLBACK_FUNCTION_KHR = 0x8244,
    GL_DEBUG_CALLBACK_USER_PARAM = 0x8245,
    GL_DEBUG_CALLBACK_USER_PARAM_KHR = 0x8245,
    GL_DEBUG_SOURCE_API = 0x8246,
    GL_DEBUG_SOURCE_API_KHR = 0x8246,
    GL_DEBUG_SOURCE_WINDOW_SYSTEM = 0x8247,
    GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR = 0x8247,
    GL_DEBUG_SOURCE_SHADER_COMPILER = 0x8248,
    GL_DEBUG_SOURCE_SHADER_COMPILER_KHR = 0x8248,
    GL_DEBUG_SOURCE_THIRD_PARTY = 0x8249,
    GL_DEBUG_SOURCE_THIRD_PARTY_KHR = 0x8249,
    GL_DEBUG_SOURCE_APPLICATION = 0x824A,
    GL_DEBUG_SOURCE_APPLICATION_KHR = 0x824A,
    GL_DEBUG_SOURCE_OTHER = 0x824B,
    GL_DEBUG_SOURCE_OTHER_KHR = 0x824B,
    GL_DEBUG_TYPE_ERROR = 0x824C,
    GL_DEBUG_TYPE_ERROR_KHR = 0x824C,
    GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR = 0x824D,
    GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR = 0x824D,
    GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR = 0x824E,
    GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR = 0x824E,
    GL_DEBUG_TYPE_PORTABILITY = 0x824F,
    GL_DEBUG_TYPE_PORTABILITY_KHR = 0x824F,
    GL_DEBUG_TYPE_PERFORMANCE = 0x8250,
    GL_DEBUG_TYPE_PERFORMANCE_KHR = 0x8250,
    GL_DEBUG_TYPE_OTHER = 0x8251,
    GL_DEBUG_TYPE_OTHER_KHR = 0x8251,
    GL_LOSE_CONTEXT_ON_RESET = 0x8252,
    GL_LOSE_CONTEXT_ON_RESET_ARB = 0x8252,
    GL_GUILTY_CONTEXT_RESET = 0x8253,
    GL_GUILTY_CONTEXT_RESET_ARB = 0x8253,
    GL_INNOCENT_CONTEXT_RESET = 0x8254,
    GL_INNOCENT_CONTEXT_RESET_ARB = 0x8254,
    GL_UNKNOWN_CONTEXT_RESET = 0x8255,
    GL_UNKNOWN_CONTEXT_RESET_ARB = 0x8255,
    GL_RESET_NOTIFICATION_STRATEGY = 0x8256,
    GL_RESET_NOTIFICATION_STRATEGY_ARB = 0x8256,
    GL_PROGRAM_BINARY_RETRIEVABLE_HINT = 0x8257,
    GL_PROGRAM_SEPARABLE = 0x8258,
    GL_ACTIVE_PROGRAM = 0x8259,
    GL_PROGRAM_PIPELINE_BINDING = 0x825A,
    GL_MAX_VIEWPORTS = 0x825B,
    GL_VIEWPORT_SUBPIXEL_BITS = 0x825C,
    GL_VIEWPORT_BOUNDS_RANGE = 0x825D,
    GL_LAYER_PROVOKING_VERTEX = 0x825E,
    GL_VIEWPORT_INDEX_PROVOKING_VERTEX = 0x825F,
    GL_UNDEFINED_VERTEX = 0x8260,
    GL_NO_RESET_NOTIFICATION = 0x8261,
    GL_NO_RESET_NOTIFICATION_ARB = 0x8261,
    GL_MAX_COMPUTE_SHARED_MEMORY_SIZE = 0x8262,
    GL_MAX_COMPUTE_UNIFORM_COMPONENTS = 0x8263,
    GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS = 0x8264,
    GL_MAX_COMPUTE_ATOMIC_COUNTERS = 0x8265,
    GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS = 0x8266,
    GL_COMPUTE_WORK_GROUP_SIZE = 0x8267,
    GL_DEBUG_TYPE_MARKER = 0x8268,
    GL_DEBUG_TYPE_MARKER_KHR = 0x8268,
    GL_DEBUG_TYPE_PUSH_GROUP = 0x8269,
    GL_DEBUG_TYPE_PUSH_GROUP_KHR = 0x8269,
    GL_DEBUG_TYPE_POP_GROUP = 0x826A,
    GL_DEBUG_TYPE_POP_GROUP_KHR = 0x826A,
    GL_DEBUG_SEVERITY_NOTIFICATION = 0x826B,
    GL_DEBUG_SEVERITY_NOTIFICATION_KHR = 0x826B,
    GL_MAX_DEBUG_GROUP_STACK_DEPTH = 0x826C,
    GL_MAX_DEBUG_GROUP_STACK_DEPTH_KHR = 0x826C,
    GL_DEBUG_GROUP_STACK_DEPTH = 0x826D,
    GL_DEBUG_GROUP_STACK_DEPTH_KHR = 0x826D,
    GL_MAX_UNIFORM_LOCATIONS = 0x826E,
    GL_INTERNALFORMAT_SUPPORTED = 0x826F,
    GL_INTERNALFORMAT_PREFERRED = 0x8270,
    GL_INTERNALFORMAT_RED_SIZE = 0x8271,
    GL_INTERNALFORMAT_GREEN_SIZE = 0x8272,
    GL_INTERNALFORMAT_BLUE_SIZE = 0x8273,
    GL_INTERNALFORMAT_ALPHA_SIZE = 0x8274,
    GL_INTERNALFORMAT_DEPTH_SIZE = 0x8275,
    GL_INTERNALFORMAT_STENCIL_SIZE = 0x8276,
    GL_INTERNALFORMAT_SHARED_SIZE = 0x8277,
    GL_INTERNALFORMAT_RED_TYPE = 0x8278,
    GL_INTERNALFORMAT_GREEN_TYPE = 0x8279,
    GL_INTERNALFORMAT_BLUE_TYPE = 0x827A,
    GL_INTERNALFORMAT_ALPHA_TYPE = 0x827B,
    GL_INTERNALFORMAT_DEPTH_TYPE = 0x827C,
    GL_INTERNALFORMAT_STENCIL_TYPE = 0x827D,
    GL_MAX_WIDTH = 0x827E,
    GL_MAX_HEIGHT = 0x827F,
    GL_MAX_DEPTH = 0x8280,
    GL_MAX_LAYERS = 0x8281,
    GL_MAX_COMBINED_DIMENSIONS = 0x8282,
    GL_COLOR_COMPONENTS = 0x8283,
    GL_DEPTH_COMPONENTS = 0x8284,
    GL_STENCIL_COMPONENTS = 0x8285,
    GL_COLOR_RENDERABLE = 0x8286,
    GL_DEPTH_RENDERABLE = 0x8287,
    GL_STENCIL_RENDERABLE = 0x8288,
    GL_FRAMEBUFFER_RENDERABLE = 0x8289,
    GL_FRAMEBUFFER_RENDERABLE_LAYERED = 0x828A,
    GL_FRAMEBUFFER_BLEND = 0x828B,
    GL_READ_PIXELS = 0x828C,
    GL_READ_PIXELS_FORMAT = 0x828D,
    GL_READ_PIXELS_TYPE = 0x828E,
    GL_TEXTURE_IMAGE_FORMAT = 0x828F,
    GL_TEXTURE_IMAGE_TYPE = 0x8290,
    GL_GET_TEXTURE_IMAGE_FORMAT = 0x8291,
    GL_GET_TEXTURE_IMAGE_TYPE = 0x8292,
    GL_MIPMAP = 0x8293,
    GL_MANUAL_GENERATE_MIPMAP = 0x8294,
    GL_AUTO_GENERATE_MIPMAP = 0x8295,
    GL_COLOR_ENCODING = 0x8296,
    GL_SRGB_READ = 0x8297,
    GL_SRGB_WRITE = 0x8298,
    GL_FILTER = 0x829A,
    GL_VERTEX_TEXTURE = 0x829B,
    GL_TESS_CONTROL_TEXTURE = 0x829C,
    GL_TESS_EVALUATION_TEXTURE = 0x829D,
    GL_GEOMETRY_TEXTURE = 0x829E,
    GL_FRAGMENT_TEXTURE = 0x829F,
    GL_COMPUTE_TEXTURE = 0x82A0,
    GL_TEXTURE_SHADOW = 0x82A1,
    GL_TEXTURE_GATHER = 0x82A2,
    GL_TEXTURE_GATHER_SHADOW = 0x82A3,
    GL_SHADER_IMAGE_LOAD = 0x82A4,
    GL_SHADER_IMAGE_STORE = 0x82A5,
    GL_SHADER_IMAGE_ATOMIC = 0x82A6,
    GL_IMAGE_TEXEL_SIZE = 0x82A7,
    GL_IMAGE_COMPATIBILITY_CLASS = 0x82A8,
    GL_IMAGE_PIXEL_FORMAT = 0x82A9,
    GL_IMAGE_PIXEL_TYPE = 0x82AA,
    GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_TEST = 0x82AC,
    GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_TEST = 0x82AD,
    GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_WRITE = 0x82AE,
    GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_WRITE = 0x82AF,
    GL_TEXTURE_COMPRESSED_BLOCK_WIDTH = 0x82B1,
    GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT = 0x82B2,
    GL_TEXTURE_COMPRESSED_BLOCK_SIZE = 0x82B3,
    GL_CLEAR_BUFFER = 0x82B4,
    GL_TEXTURE_VIEW = 0x82B5,
    GL_VIEW_COMPATIBILITY_CLASS = 0x82B6,
    GL_FULL_SUPPORT = 0x82B7,
    GL_CAVEAT_SUPPORT = 0x82B8,
    GL_IMAGE_CLASS_4_X_32 = 0x82B9,
    GL_IMAGE_CLASS_2_X_32 = 0x82BA,
    GL_IMAGE_CLASS_1_X_32 = 0x82BB,
    GL_IMAGE_CLASS_4_X_16 = 0x82BC,
    GL_IMAGE_CLASS_2_X_16 = 0x82BD,
    GL_IMAGE_CLASS_1_X_16 = 0x82BE,
    GL_IMAGE_CLASS_4_X_8 = 0x82BF,
    GL_IMAGE_CLASS_2_X_8 = 0x82C0,
    GL_IMAGE_CLASS_1_X_8 = 0x82C1,
    GL_IMAGE_CLASS_11_11_10 = 0x82C2,
    GL_IMAGE_CLASS_10_10_10_2 = 0x82C3,
    GL_VIEW_CLASS_128_BITS = 0x82C4,
    GL_VIEW_CLASS_96_BITS = 0x82C5,
    GL_VIEW_CLASS_64_BITS = 0x82C6,
    GL_VIEW_CLASS_48_BITS = 0x82C7,
    GL_VIEW_CLASS_32_BITS = 0x82C8,
    GL_VIEW_CLASS_24_BITS = 0x82C9,
    GL_VIEW_CLASS_16_BITS = 0x82CA,
    GL_VIEW_CLASS_8_BITS = 0x82CB,
    GL_VIEW_CLASS_S3TC_DXT1_RGB = 0x82CC,
    GL_VIEW_CLASS_S3TC_DXT1_RGBA = 0x82CD,
    GL_VIEW_CLASS_S3TC_DXT3_RGBA = 0x82CE,
    GL_VIEW_CLASS_S3TC_DXT5_RGBA = 0x82CF,
    GL_VIEW_CLASS_RGTC1_RED = 0x82D0,
    GL_VIEW_CLASS_RGTC2_RG = 0x82D1,
    GL_VIEW_CLASS_BPTC_UNORM = 0x82D2,
    GL_VIEW_CLASS_BPTC_FLOAT = 0x82D3,
    GL_VERTEX_ATTRIB_BINDING = 0x82D4,
    GL_VERTEX_ATTRIB_RELATIVE_OFFSET = 0x82D5,
    GL_VERTEX_BINDING_DIVISOR = 0x82D6,
    GL_VERTEX_BINDING_OFFSET = 0x82D7,
    GL_VERTEX_BINDING_STRIDE = 0x82D8,
    GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET = 0x82D9,
    GL_MAX_VERTEX_ATTRIB_BINDINGS = 0x82DA,
    GL_TEXTURE_VIEW_MIN_LEVEL = 0x82DB,
    GL_TEXTURE_VIEW_NUM_LEVELS = 0x82DC,
    GL_TEXTURE_VIEW_MIN_LAYER = 0x82DD,
    GL_TEXTURE_VIEW_NUM_LAYERS = 0x82DE,
    GL_TEXTURE_IMMUTABLE_LEVELS = 0x82DF,
    GL_BUFFER = 0x82E0,
    GL_BUFFER_KHR = 0x82E0,
    GL_SHADER = 0x82E1,
    GL_SHADER_KHR = 0x82E1,
    GL_PROGRAM = 0x82E2,
    GL_PROGRAM_KHR = 0x82E2,
    GL_QUERY = 0x82E3,
    GL_QUERY_KHR = 0x82E3,
    GL_PROGRAM_PIPELINE = 0x82E4,
    GL_PROGRAM_PIPELINE_KHR = 0x82E4,
    GL_MAX_VERTEX_ATTRIB_STRIDE = 0x82E5,
    GL_SAMPLER = 0x82E6,
    GL_SAMPLER_KHR = 0x82E6,
    GL_DISPLAY_LIST = 0x82E7,
    GL_MAX_LABEL_LENGTH = 0x82E8,
    GL_MAX_LABEL_LENGTH_KHR = 0x82E8,
    GL_NUM_SHADING_LANGUAGE_VERSIONS = 0x82E9,
    GL_QUERY_TARGET = 0x82EA,
    GL_TRANSFORM_FEEDBACK_OVERFLOW = 0x82EC,
    GL_TRANSFORM_FEEDBACK_OVERFLOW_ARB = 0x82EC,
    GL_TRANSFORM_FEEDBACK_STREAM_OVERFLOW = 0x82ED,
    GL_TRANSFORM_FEEDBACK_STREAM_OVERFLOW_ARB = 0x82ED,
    GL_VERTICES_SUBMITTED = 0x82EE,
    GL_VERTICES_SUBMITTED_ARB = 0x82EE,
    GL_PRIMITIVES_SUBMITTED = 0x82EF,
    GL_PRIMITIVES_SUBMITTED_ARB = 0x82EF,
    GL_VERTEX_SHADER_INVOCATIONS = 0x82F0,
    GL_VERTEX_SHADER_INVOCATIONS_ARB = 0x82F0,
    GL_TESS_CONTROL_SHADER_PATCHES = 0x82F1,
    GL_TESS_CONTROL_SHADER_PATCHES_ARB = 0x82F1,
    GL_TESS_EVALUATION_SHADER_INVOCATIONS = 0x82F2,
    GL_TESS_EVALUATION_SHADER_INVOCATIONS_ARB = 0x82F2,
    GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED = 0x82F3,
    GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED_ARB = 0x82F3,
    GL_FRAGMENT_SHADER_INVOCATIONS = 0x82F4,
    GL_FRAGMENT_SHADER_INVOCATIONS_ARB = 0x82F4,
    GL_COMPUTE_SHADER_INVOCATIONS = 0x82F5,
    GL_COMPUTE_SHADER_INVOCATIONS_ARB = 0x82F5,
    GL_CLIPPING_INPUT_PRIMITIVES = 0x82F6,
    GL_CLIPPING_INPUT_PRIMITIVES_ARB = 0x82F6,
    GL_CLIPPING_OUTPUT_PRIMITIVES = 0x82F7,
    GL_CLIPPING_OUTPUT_PRIMITIVES_ARB = 0x82F7,
    GL_MAX_CULL_DISTANCES = 0x82F9,
    GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES = 0x82FA,
    GL_CONTEXT_RELEASE_BEHAVIOR = 0x82FB,
    GL_CONTEXT_RELEASE_BEHAVIOR_KHR = 0x82FB,
    GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH = 0x82FC,
    GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_KHR = 0x82FC,
    GL_UNSIGNED_BYTE_2_3_3_REV = 0x8362,
    GL_UNSIGNED_SHORT_5_6_5 = 0x8363,
    GL_UNSIGNED_SHORT_5_6_5_REV = 0x8364,
    GL_UNSIGNED_SHORT_4_4_4_4_REV = 0x8365,
    GL_UNSIGNED_SHORT_1_5_5_5_REV = 0x8366,
    GL_UNSIGNED_INT_8_8_8_8_REV = 0x8367,
    GL_UNSIGNED_INT_2_10_10_10_REV = 0x8368,
    GL_MIRRORED_REPEAT = 0x8370,
    GL_ALIASED_LINE_WIDTH_RANGE = 0x846E,
    GL_TEXTURE0 = 0x84C0,
    GL_TEXTURE1 = 0x84C1,
    GL_TEXTURE2 = 0x84C2,
    GL_TEXTURE3 = 0x84C3,
    GL_TEXTURE4 = 0x84C4,
    GL_TEXTURE5 = 0x84C5,
    GL_TEXTURE6 = 0x84C6,
    GL_TEXTURE7 = 0x84C7,
    GL_TEXTURE8 = 0x84C8,
    GL_TEXTURE9 = 0x84C9,
    GL_TEXTURE10 = 0x84CA,
    GL_TEXTURE11 = 0x84CB,
    GL_TEXTURE12 = 0x84CC,
    GL_TEXTURE13 = 0x84CD,
    GL_TEXTURE14 = 0x84CE,
    GL_TEXTURE15 = 0x84CF,
    GL_TEXTURE16 = 0x84D0,
    GL_TEXTURE17 = 0x84D1,
    GL_TEXTURE18 = 0x84D2,
    GL_TEXTURE19 = 0x84D3,
    GL_TEXTURE20 = 0x84D4,
    GL_TEXTURE21 = 0x84D5,
    GL_TEXTURE22 = 0x84D6,
    GL_TEXTURE23 = 0x84D7,
    GL_TEXTURE24 = 0x84D8,
    GL_TEXTURE25 = 0x84D9,
    GL_TEXTURE26 = 0x84DA,
    GL_TEXTURE27 = 0x84DB,
    GL_TEXTURE28 = 0x84DC,
    GL_TEXTURE29 = 0x84DD,
    GL_TEXTURE30 = 0x84DE,
    GL_TEXTURE31 = 0x84DF,
    GL_ACTIVE_TEXTURE = 0x84E0,
    GL_PATH_TRANSPOSE_MODELVIEW_MATRIX_NV = 0x84E3,
    GL_PATH_TRANSPOSE_PROJECTION_MATRIX_NV = 0x84E4,
    GL_MAX_RENDERBUFFER_SIZE = 0x84E8,
    GL_COMPRESSED_RGB = 0x84ED,
    GL_COMPRESSED_RGBA = 0x84EE,
    GL_TEXTURE_COMPRESSION_HINT = 0x84EF,
    GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER = 0x84F0,
    GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER = 0x84F1,
    GL_TEXTURE_RECTANGLE = 0x84F5,
    GL_TEXTURE_BINDING_RECTANGLE = 0x84F6,
    GL_PROXY_TEXTURE_RECTANGLE = 0x84F7,
    GL_MAX_RECTANGLE_TEXTURE_SIZE = 0x84F8,
    GL_DEPTH_STENCIL = 0x84F9,
    GL_UNSIGNED_INT_24_8 = 0x84FA,
    GL_MAX_TEXTURE_LOD_BIAS = 0x84FD,
    GL_TEXTURE_MAX_ANISOTROPY = 0x84FE,
    GL_TEXTURE_MAX_ANISOTROPY_EXT = 0x84FE,
    GL_MAX_TEXTURE_MAX_ANISOTROPY = 0x84FF,
    GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT = 0x84FF,
    GL_TEXTURE_LOD_BIAS = 0x8501,
    GL_INCR_WRAP = 0x8507,
    GL_DECR_WRAP = 0x8508,
    GL_TEXTURE_CUBE_MAP = 0x8513,
    GL_TEXTURE_BINDING_CUBE_MAP = 0x8514,
    GL_TEXTURE_CUBE_MAP_POSITIVE_X = 0x8515,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X = 0x8516,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y = 0x8517,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y = 0x8518,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z = 0x8519,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z = 0x851A,
    GL_PROXY_TEXTURE_CUBE_MAP = 0x851B,
    GL_MAX_CUBE_MAP_TEXTURE_SIZE = 0x851C,
    GL_PRIMARY_COLOR_NV = 0x852C,
    GL_SECONDARY_COLOR_NV = 0x852D,
    GL_CONSTANT_NV = 0x8576,
    GL_PRIMARY_COLOR = 0x8577,
    GL_SRC1_ALPHA = 0x8589,
    GL_VERTEX_ARRAY_BINDING = 0x85B5,
    GL_VERTEX_ATTRIB_ARRAY_ENABLED = 0x8622,
    GL_VERTEX_ATTRIB_ARRAY_SIZE = 0x8623,
    GL_VERTEX_ATTRIB_ARRAY_STRIDE = 0x8624,
    GL_VERTEX_ATTRIB_ARRAY_TYPE = 0x8625,
    GL_CURRENT_VERTEX_ATTRIB = 0x8626,
    GL_VERTEX_PROGRAM_POINT_SIZE = 0x8642,
    GL_PROGRAM_POINT_SIZE = 0x8642,
    GL_VERTEX_ATTRIB_ARRAY_POINTER = 0x8645,
    GL_DEPTH_CLAMP = 0x864F,
    GL_TEXTURE_COMPRESSED_IMAGE_SIZE = 0x86A0,
    GL_TEXTURE_COMPRESSED = 0x86A1,
    GL_NUM_COMPRESSED_TEXTURE_FORMATS = 0x86A2,
    GL_COMPRESSED_TEXTURE_FORMATS = 0x86A3,
    GL_PROGRAM_BINARY_LENGTH = 0x8741,
    GL_MIRROR_CLAMP_TO_EDGE = 0x8743,
    GL_VERTEX_ATTRIB_ARRAY_LONG = 0x874E,
    GL_BUFFER_SIZE = 0x8764,
    GL_BUFFER_USAGE = 0x8765,
    GL_NUM_PROGRAM_BINARY_FORMATS = 0x87FE,
    GL_PROGRAM_BINARY_FORMATS = 0x87FF,
    GL_STENCIL_BACK_FUNC = 0x8800,
    GL_STENCIL_BACK_FAIL = 0x8801,
    GL_STENCIL_BACK_PASS_DEPTH_FAIL = 0x8802,
    GL_STENCIL_BACK_PASS_DEPTH_PASS = 0x8803,
    GL_RGBA32F = 0x8814,
    GL_RGB32F = 0x8815,
    GL_RGBA16F = 0x881A,
    GL_RGB16F = 0x881B,
    GL_MAX_DRAW_BUFFERS = 0x8824,
    GL_DRAW_BUFFER0 = 0x8825,
    GL_DRAW_BUFFER1 = 0x8826,
    GL_DRAW_BUFFER2 = 0x8827,
    GL_DRAW_BUFFER3 = 0x8828,
    GL_DRAW_BUFFER4 = 0x8829,
    GL_DRAW_BUFFER5 = 0x882A,
    GL_DRAW_BUFFER6 = 0x882B,
    GL_DRAW_BUFFER7 = 0x882C,
    GL_DRAW_BUFFER8 = 0x882D,
    GL_DRAW_BUFFER9 = 0x882E,
    GL_DRAW_BUFFER10 = 0x882F,
    GL_DRAW_BUFFER11 = 0x8830,
    GL_DRAW_BUFFER12 = 0x8831,
    GL_DRAW_BUFFER13 = 0x8832,
    GL_DRAW_BUFFER14 = 0x8833,
    GL_DRAW_BUFFER15 = 0x8834,
    GL_BLEND_EQUATION_ALPHA = 0x883D,
    GL_TEXTURE_DEPTH_SIZE = 0x884A,
    GL_TEXTURE_COMPARE_MODE = 0x884C,
    GL_TEXTURE_COMPARE_FUNC = 0x884D,
    GL_COMPARE_REF_TO_TEXTURE = 0x884E,
    GL_TEXTURE_CUBE_MAP_SEAMLESS = 0x884F,
    GL_QUERY_COUNTER_BITS = 0x8864,
    GL_CURRENT_QUERY = 0x8865,
    GL_QUERY_RESULT = 0x8866,
    GL_QUERY_RESULT_AVAILABLE = 0x8867,
    GL_MAX_VERTEX_ATTRIBS = 0x8869,
    GL_VERTEX_ATTRIB_ARRAY_NORMALIZED = 0x886A,
    GL_MAX_TESS_CONTROL_INPUT_COMPONENTS = 0x886C,
    GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS = 0x886D,
    GL_MAX_TEXTURE_IMAGE_UNITS = 0x8872,
    GL_GEOMETRY_SHADER_INVOCATIONS = 0x887F,
    GL_ARRAY_BUFFER = 0x8892,
    GL_ELEMENT_ARRAY_BUFFER = 0x8893,
    GL_ARRAY_BUFFER_BINDING = 0x8894,
    GL_ELEMENT_ARRAY_BUFFER_BINDING = 0x8895,
    GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING = 0x889F,
    GL_READ_ONLY = 0x88B8,
    GL_WRITE_ONLY = 0x88B9,
    GL_READ_WRITE = 0x88BA,
    GL_BUFFER_ACCESS = 0x88BB,
    GL_BUFFER_MAPPED = 0x88BC,
    GL_BUFFER_MAP_POINTER = 0x88BD,
    GL_TIME_ELAPSED = 0x88BF,
    GL_STREAM_DRAW = 0x88E0,
    GL_STREAM_READ = 0x88E1,
    GL_STREAM_COPY = 0x88E2,
    GL_STATIC_DRAW = 0x88E4,
    GL_STATIC_READ = 0x88E5,
    GL_STATIC_COPY = 0x88E6,
    GL_DYNAMIC_DRAW = 0x88E8,
    GL_DYNAMIC_READ = 0x88E9,
    GL_DYNAMIC_COPY = 0x88EA,
    GL_PIXEL_PACK_BUFFER = 0x88EB,
    GL_PIXEL_UNPACK_BUFFER = 0x88EC,
    GL_PIXEL_PACK_BUFFER_BINDING = 0x88ED,
    GL_PIXEL_UNPACK_BUFFER_BINDING = 0x88EF,
    GL_DEPTH24_STENCIL8 = 0x88F0,
    GL_TEXTURE_STENCIL_SIZE = 0x88F1,
    GL_SRC1_COLOR = 0x88F9,
    GL_ONE_MINUS_SRC1_COLOR = 0x88FA,
    GL_ONE_MINUS_SRC1_ALPHA = 0x88FB,
    GL_MAX_DUAL_SOURCE_DRAW_BUFFERS = 0x88FC,
    GL_VERTEX_ATTRIB_ARRAY_INTEGER = 0x88FD,
    GL_VERTEX_ATTRIB_ARRAY_DIVISOR = 0x88FE,
    GL_MAX_ARRAY_TEXTURE_LAYERS = 0x88FF,
    GL_MIN_PROGRAM_TEXEL_OFFSET = 0x8904,
    GL_MAX_PROGRAM_TEXEL_OFFSET = 0x8905,
    GL_SAMPLES_PASSED = 0x8914,
    GL_GEOMETRY_VERTICES_OUT = 0x8916,
    GL_GEOMETRY_INPUT_TYPE = 0x8917,
    GL_GEOMETRY_OUTPUT_TYPE = 0x8918,
    GL_SAMPLER_BINDING = 0x8919,
    GL_CLAMP_READ_COLOR = 0x891C,
    GL_FIXED_ONLY = 0x891D,
    GL_UNIFORM_BUFFER = 0x8A11,
    GL_UNIFORM_BUFFER_BINDING = 0x8A28,
    GL_UNIFORM_BUFFER_START = 0x8A29,
    GL_UNIFORM_BUFFER_SIZE = 0x8A2A,
    GL_MAX_VERTEX_UNIFORM_BLOCKS = 0x8A2B,
    GL_MAX_GEOMETRY_UNIFORM_BLOCKS = 0x8A2C,
    GL_MAX_FRAGMENT_UNIFORM_BLOCKS = 0x8A2D,
    GL_MAX_COMBINED_UNIFORM_BLOCKS = 0x8A2E,
    GL_MAX_UNIFORM_BUFFER_BINDINGS = 0x8A2F,
    GL_MAX_UNIFORM_BLOCK_SIZE = 0x8A30,
    GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS = 0x8A31,
    GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS = 0x8A32,
    GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS = 0x8A33,
    GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT = 0x8A34,
    GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH = 0x8A35,
    GL_ACTIVE_UNIFORM_BLOCKS = 0x8A36,
    GL_UNIFORM_TYPE = 0x8A37,
    GL_UNIFORM_SIZE = 0x8A38,
    GL_UNIFORM_NAME_LENGTH = 0x8A39,
    GL_UNIFORM_BLOCK_INDEX = 0x8A3A,
    GL_UNIFORM_OFFSET = 0x8A3B,
    GL_UNIFORM_ARRAY_STRIDE = 0x8A3C,
    GL_UNIFORM_MATRIX_STRIDE = 0x8A3D,
    GL_UNIFORM_IS_ROW_MAJOR = 0x8A3E,
    GL_UNIFORM_BLOCK_BINDING = 0x8A3F,
    GL_UNIFORM_BLOCK_DATA_SIZE = 0x8A40,
    GL_UNIFORM_BLOCK_NAME_LENGTH = 0x8A41,
    GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS = 0x8A42,
    GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES = 0x8A43,
    GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER = 0x8A44,
    GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER = 0x8A45,
    GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER = 0x8A46,
    GL_FRAGMENT_SHADER = 0x8B30,
    GL_VERTEX_SHADER = 0x8B31,
    GL_MAX_FRAGMENT_UNIFORM_COMPONENTS = 0x8B49,
    GL_MAX_VERTEX_UNIFORM_COMPONENTS = 0x8B4A,
    GL_MAX_VARYING_FLOATS = 0x8B4B,
    GL_MAX_VARYING_COMPONENTS = 0x8B4B,
    GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS = 0x8B4C,
    GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = 0x8B4D,
    GL_SHADER_TYPE = 0x8B4F,
    GL_FLOAT_VEC2 = 0x8B50,
    GL_FLOAT_VEC3 = 0x8B51,
    GL_FLOAT_VEC4 = 0x8B52,
    GL_INT_VEC2 = 0x8B53,
    GL_INT_VEC3 = 0x8B54,
    GL_INT_VEC4 = 0x8B55,
    GL_BOOL = 0x8B56,
    GL_BOOL_VEC2 = 0x8B57,
    GL_BOOL_VEC3 = 0x8B58,
    GL_BOOL_VEC4 = 0x8B59,
    GL_FLOAT_MAT2 = 0x8B5A,
    GL_FLOAT_MAT3 = 0x8B5B,
    GL_FLOAT_MAT4 = 0x8B5C,
    GL_SAMPLER_1D = 0x8B5D,
    GL_SAMPLER_2D = 0x8B5E,
    GL_SAMPLER_3D = 0x8B5F,
    GL_SAMPLER_CUBE = 0x8B60,
    GL_SAMPLER_1D_SHADOW = 0x8B61,
    GL_SAMPLER_2D_SHADOW = 0x8B62,
    GL_SAMPLER_2D_RECT = 0x8B63,
    GL_SAMPLER_2D_RECT_SHADOW = 0x8B64,
    GL_FLOAT_MAT2x3 = 0x8B65,
    GL_FLOAT_MAT2x4 = 0x8B66,
    GL_FLOAT_MAT3x2 = 0x8B67,
    GL_FLOAT_MAT3x4 = 0x8B68,
    GL_FLOAT_MAT4x2 = 0x8B69,
    GL_FLOAT_MAT4x3 = 0x8B6A,
    GL_DELETE_STATUS = 0x8B80,
    GL_COMPILE_STATUS = 0x8B81,
    GL_LINK_STATUS = 0x8B82,
    GL_VALIDATE_STATUS = 0x8B83,
    GL_INFO_LOG_LENGTH = 0x8B84,
    GL_ATTACHED_SHADERS = 0x8B85,
    GL_ACTIVE_UNIFORMS = 0x8B86,
    GL_ACTIVE_UNIFORM_MAX_LENGTH = 0x8B87,
    GL_SHADER_SOURCE_LENGTH = 0x8B88,
    GL_ACTIVE_ATTRIBUTES = 0x8B89,
    GL_ACTIVE_ATTRIBUTE_MAX_LENGTH = 0x8B8A,
    GL_FRAGMENT_SHADER_DERIVATIVE_HINT = 0x8B8B,
    GL_SHADING_LANGUAGE_VERSION = 0x8B8C,
    GL_CURRENT_PROGRAM = 0x8B8D,
    GL_IMPLEMENTATION_COLOR_READ_TYPE = 0x8B9A,
    GL_IMPLEMENTATION_COLOR_READ_FORMAT = 0x8B9B,
    GL_TEXTURE_RED_TYPE = 0x8C10,
    GL_TEXTURE_GREEN_TYPE = 0x8C11,
    GL_TEXTURE_BLUE_TYPE = 0x8C12,
    GL_TEXTURE_ALPHA_TYPE = 0x8C13,
    GL_TEXTURE_DEPTH_TYPE = 0x8C16,
    GL_UNSIGNED_NORMALIZED = 0x8C17,
    GL_TEXTURE_1D_ARRAY = 0x8C18,
    GL_PROXY_TEXTURE_1D_ARRAY = 0x8C19,
    GL_TEXTURE_2D_ARRAY = 0x8C1A,
    GL_PROXY_TEXTURE_2D_ARRAY = 0x8C1B,
    GL_TEXTURE_BINDING_1D_ARRAY = 0x8C1C,
    GL_TEXTURE_BINDING_2D_ARRAY = 0x8C1D,
    GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS = 0x8C29,
    GL_TEXTURE_BUFFER = 0x8C2A,
    GL_TEXTURE_BUFFER_BINDING = 0x8C2A,
    GL_MAX_TEXTURE_BUFFER_SIZE = 0x8C2B,
    GL_TEXTURE_BINDING_BUFFER = 0x8C2C,
    GL_TEXTURE_BUFFER_DATA_STORE_BINDING = 0x8C2D,
    GL_ANY_SAMPLES_PASSED = 0x8C2F,
    GL_SAMPLE_SHADING = 0x8C36,
    GL_MIN_SAMPLE_SHADING_VALUE = 0x8C37,
    GL_R11F_G11F_B10F = 0x8C3A,
    GL_UNSIGNED_INT_10F_11F_11F_REV = 0x8C3B,
    GL_RGB9_E5 = 0x8C3D,
    GL_UNSIGNED_INT_5_9_9_9_REV = 0x8C3E,
    GL_TEXTURE_SHARED_SIZE = 0x8C3F,
    GL_SRGB = 0x8C40,
    GL_SRGB8 = 0x8C41,
    GL_SRGB_ALPHA = 0x8C42,
    GL_SRGB8_ALPHA8 = 0x8C43,
    GL_COMPRESSED_SRGB = 0x8C48,
    GL_COMPRESSED_SRGB_ALPHA = 0x8C49,
    GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH = 0x8C76,
    GL_TRANSFORM_FEEDBACK_BUFFER_MODE = 0x8C7F,
    GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS = 0x8C80,
    GL_TRANSFORM_FEEDBACK_VARYINGS = 0x8C83,
    GL_TRANSFORM_FEEDBACK_BUFFER_START = 0x8C84,
    GL_TRANSFORM_FEEDBACK_BUFFER_SIZE = 0x8C85,
    GL_PRIMITIVES_GENERATED = 0x8C87,
    GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN = 0x8C88,
    GL_RASTERIZER_DISCARD = 0x8C89,
    GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS = 0x8C8A,
    GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS = 0x8C8B,
    GL_INTERLEAVED_ATTRIBS = 0x8C8C,
    GL_SEPARATE_ATTRIBS = 0x8C8D,
    GL_TRANSFORM_FEEDBACK_BUFFER = 0x8C8E,
    GL_TRANSFORM_FEEDBACK_BUFFER_BINDING = 0x8C8F,
    GL_POINT_SPRITE_COORD_ORIGIN = 0x8CA0,
    GL_LOWER_LEFT = 0x8CA1,
    GL_UPPER_LEFT = 0x8CA2,
    GL_STENCIL_BACK_REF = 0x8CA3,
    GL_STENCIL_BACK_VALUE_MASK = 0x8CA4,
    GL_STENCIL_BACK_WRITEMASK = 0x8CA5,
    GL_DRAW_FRAMEBUFFER_BINDING = 0x8CA6,
    GL_FRAMEBUFFER_BINDING = 0x8CA6,
    GL_RENDERBUFFER_BINDING = 0x8CA7,
    GL_READ_FRAMEBUFFER = 0x8CA8,
    GL_DRAW_FRAMEBUFFER = 0x8CA9,
    GL_READ_FRAMEBUFFER_BINDING = 0x8CAA,
    GL_RENDERBUFFER_SAMPLES = 0x8CAB,
    GL_DEPTH_COMPONENT32F = 0x8CAC,
    GL_DEPTH32F_STENCIL8 = 0x8CAD,
    GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE = 0x8CD0,
    GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME = 0x8CD1,
    GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL = 0x8CD2,
    GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE = 0x8CD3,
    GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER = 0x8CD4,
    GL_FRAMEBUFFER_COMPLETE = 0x8CD5,
    GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT = 0x8CD6,
    GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT = 0x8CD7,
    GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER = 0x8CDB,
    GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER = 0x8CDC,
    GL_FRAMEBUFFER_UNSUPPORTED = 0x8CDD,
    GL_MAX_COLOR_ATTACHMENTS = 0x8CDF,
    GL_COLOR_ATTACHMENT0 = 0x8CE0,
    GL_COLOR_ATTACHMENT1 = 0x8CE1,
    GL_COLOR_ATTACHMENT2 = 0x8CE2,
    GL_COLOR_ATTACHMENT3 = 0x8CE3,
    GL_COLOR_ATTACHMENT4 = 0x8CE4,
    GL_COLOR_ATTACHMENT5 = 0x8CE5,
    GL_COLOR_ATTACHMENT6 = 0x8CE6,
    GL_COLOR_ATTACHMENT7 = 0x8CE7,
    GL_COLOR_ATTACHMENT8 = 0x8CE8,
    GL_COLOR_ATTACHMENT9 = 0x8CE9,
    GL_COLOR_ATTACHMENT10 = 0x8CEA,
    GL_COLOR_ATTACHMENT11 = 0x8CEB,
    GL_COLOR_ATTACHMENT12 = 0x8CEC,
    GL_COLOR_ATTACHMENT13 = 0x8CED,
    GL_COLOR_ATTACHMENT14 = 0x8CEE,
    GL_COLOR_ATTACHMENT15 = 0x8CEF,
    GL_COLOR_ATTACHMENT16 = 0x8CF0,
    GL_COLOR_ATTACHMENT17 = 0x8CF1,
    GL_COLOR_ATTACHMENT18 = 0x8CF2,
    GL_COLOR_ATTACHMENT19 = 0x8CF3,
    GL_COLOR_ATTACHMENT20 = 0x8CF4,
    GL_COLOR_ATTACHMENT21 = 0x8CF5,
    GL_COLOR_ATTACHMENT22 = 0x8CF6,
    GL_COLOR_ATTACHMENT23 = 0x8CF7,
    GL_COLOR_ATTACHMENT24 = 0x8CF8,
    GL_COLOR_ATTACHMENT25 = 0x8CF9,
    GL_COLOR_ATTACHMENT26 = 0x8CFA,
    GL_COLOR_ATTACHMENT27 = 0x8CFB,
    GL_COLOR_ATTACHMENT28 = 0x8CFC,
    GL_COLOR_ATTACHMENT29 = 0x8CFD,
    GL_COLOR_ATTACHMENT30 = 0x8CFE,
    GL_COLOR_ATTACHMENT31 = 0x8CFF,
    GL_DEPTH_ATTACHMENT = 0x8D00,
    GL_STENCIL_ATTACHMENT = 0x8D20,
    GL_FRAMEBUFFER = 0x8D40,
    GL_RENDERBUFFER = 0x8D41,
    GL_RENDERBUFFER_WIDTH = 0x8D42,
    GL_RENDERBUFFER_HEIGHT = 0x8D43,
    GL_RENDERBUFFER_INTERNAL_FORMAT = 0x8D44,
    GL_STENCIL_INDEX1 = 0x8D46,
    GL_STENCIL_INDEX4 = 0x8D47,
    GL_STENCIL_INDEX8 = 0x8D48,
    GL_STENCIL_INDEX16 = 0x8D49,
    GL_RENDERBUFFER_RED_SIZE = 0x8D50,
    GL_RENDERBUFFER_GREEN_SIZE = 0x8D51,
    GL_RENDERBUFFER_BLUE_SIZE = 0x8D52,
    GL_RENDERBUFFER_ALPHA_SIZE = 0x8D53,
    GL_RENDERBUFFER_DEPTH_SIZE = 0x8D54,
    GL_RENDERBUFFER_STENCIL_SIZE = 0x8D55,
    GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE = 0x8D56,
    GL_MAX_SAMPLES = 0x8D57,
    GL_RGB565 = 0x8D62,
    GL_PRIMITIVE_RESTART_FIXED_INDEX = 0x8D69,
    GL_ANY_SAMPLES_PASSED_CONSERVATIVE = 0x8D6A,
    GL_MAX_ELEMENT_INDEX = 0x8D6B,
    GL_RGBA32UI = 0x8D70,
    GL_RGB32UI = 0x8D71,
    GL_RGBA16UI = 0x8D76,
    GL_RGB16UI = 0x8D77,
    GL_RGBA8UI = 0x8D7C,
    GL_RGB8UI = 0x8D7D,
    GL_RGBA32I = 0x8D82,
    GL_RGB32I = 0x8D83,
    GL_RGBA16I = 0x8D88,
    GL_RGB16I = 0x8D89,
    GL_RGBA8I = 0x8D8E,
    GL_RGB8I = 0x8D8F,
    GL_RED_INTEGER = 0x8D94,
    GL_GREEN_INTEGER = 0x8D95,
    GL_BLUE_INTEGER = 0x8D96,
    GL_RGB_INTEGER = 0x8D98,
    GL_RGBA_INTEGER = 0x8D99,
    GL_BGR_INTEGER = 0x8D9A,
    GL_BGRA_INTEGER = 0x8D9B,
    GL_INT_2_10_10_10_REV = 0x8D9F,
    GL_FRAMEBUFFER_ATTACHMENT_LAYERED = 0x8DA7,
    GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS = 0x8DA8,
    GL_FLOAT_32_UNSIGNED_INT_24_8_REV = 0x8DAD,
    GL_FRAMEBUFFER_SRGB = 0x8DB9,
    GL_COMPRESSED_RED_RGTC1 = 0x8DBB,
    GL_COMPRESSED_SIGNED_RED_RGTC1 = 0x8DBC,
    GL_COMPRESSED_RG_RGTC2 = 0x8DBD,
    GL_COMPRESSED_SIGNED_RG_RGTC2 = 0x8DBE,
    GL_SAMPLER_1D_ARRAY = 0x8DC0,
    GL_SAMPLER_2D_ARRAY = 0x8DC1,
    GL_SAMPLER_BUFFER = 0x8DC2,
    GL_SAMPLER_1D_ARRAY_SHADOW = 0x8DC3,
    GL_SAMPLER_2D_ARRAY_SHADOW = 0x8DC4,
    GL_SAMPLER_CUBE_SHADOW = 0x8DC5,
    GL_UNSIGNED_INT_VEC2 = 0x8DC6,
    GL_UNSIGNED_INT_VEC3 = 0x8DC7,
    GL_UNSIGNED_INT_VEC4 = 0x8DC8,
    GL_INT_SAMPLER_1D = 0x8DC9,
    GL_INT_SAMPLER_2D = 0x8DCA,
    GL_INT_SAMPLER_3D = 0x8DCB,
    GL_INT_SAMPLER_CUBE = 0x8DCC,
    GL_INT_SAMPLER_2D_RECT = 0x8DCD,
    GL_INT_SAMPLER_1D_ARRAY = 0x8DCE,
    GL_INT_SAMPLER_2D_ARRAY = 0x8DCF,
    GL_INT_SAMPLER_BUFFER = 0x8DD0,
    GL_UNSIGNED_INT_SAMPLER_1D = 0x8DD1,
    GL_UNSIGNED_INT_SAMPLER_2D = 0x8DD2,
    GL_UNSIGNED_INT_SAMPLER_3D = 0x8DD3,
    GL_UNSIGNED_INT_SAMPLER_CUBE = 0x8DD4,
    GL_UNSIGNED_INT_SAMPLER_2D_RECT = 0x8DD5,
    GL_UNSIGNED_INT_SAMPLER_1D_ARRAY = 0x8DD6,
    GL_UNSIGNED_INT_SAMPLER_2D_ARRAY = 0x8DD7,
    GL_UNSIGNED_INT_SAMPLER_BUFFER = 0x8DD8,
    GL_GEOMETRY_SHADER = 0x8DD9,
    GL_MAX_GEOMETRY_UNIFORM_COMPONENTS = 0x8DDF,
    GL_MAX_GEOMETRY_OUTPUT_VERTICES = 0x8DE0,
    GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS = 0x8DE1,
    GL_ACTIVE_SUBROUTINES = 0x8DE5,
    GL_ACTIVE_SUBROUTINE_UNIFORMS = 0x8DE6,
    GL_MAX_SUBROUTINES = 0x8DE7,
    GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS = 0x8DE8,
    GL_LOW_FLOAT = 0x8DF0,
    GL_MEDIUM_FLOAT = 0x8DF1,
    GL_HIGH_FLOAT = 0x8DF2,
    GL_LOW_INT = 0x8DF3,
    GL_MEDIUM_INT = 0x8DF4,
    GL_HIGH_INT = 0x8DF5,
    GL_SHADER_BINARY_FORMATS = 0x8DF8,
    GL_NUM_SHADER_BINARY_FORMATS = 0x8DF9,
    GL_SHADER_COMPILER = 0x8DFA,
    GL_MAX_VERTEX_UNIFORM_VECTORS = 0x8DFB,
    GL_MAX_VARYING_VECTORS = 0x8DFC,
    GL_MAX_FRAGMENT_UNIFORM_VECTORS = 0x8DFD,
    GL_QUERY_WAIT = 0x8E13,
    GL_QUERY_NO_WAIT = 0x8E14,
    GL_QUERY_BY_REGION_WAIT = 0x8E15,
    GL_QUERY_BY_REGION_NO_WAIT = 0x8E16,
    GL_QUERY_WAIT_INVERTED = 0x8E17,
    GL_QUERY_NO_WAIT_INVERTED = 0x8E18,
    GL_QUERY_BY_REGION_WAIT_INVERTED = 0x8E19,
    GL_QUERY_BY_REGION_NO_WAIT_INVERTED = 0x8E1A,
    GL_POLYGON_OFFSET_CLAMP = 0x8E1B,
    GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS = 0x8E1E,
    GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS = 0x8E1F,
    GL_TRANSFORM_FEEDBACK = 0x8E22,
    GL_TRANSFORM_FEEDBACK_BUFFER_PAUSED = 0x8E23,
    GL_TRANSFORM_FEEDBACK_PAUSED = 0x8E23,
    GL_TRANSFORM_FEEDBACK_BUFFER_ACTIVE = 0x8E24,
    GL_TRANSFORM_FEEDBACK_ACTIVE = 0x8E24,
    GL_TRANSFORM_FEEDBACK_BINDING = 0x8E25,
    GL_TIMESTAMP = 0x8E28,
    GL_TEXTURE_SWIZZLE_R = 0x8E42,
    GL_TEXTURE_SWIZZLE_G = 0x8E43,
    GL_TEXTURE_SWIZZLE_B = 0x8E44,
    GL_TEXTURE_SWIZZLE_A = 0x8E45,
    GL_TEXTURE_SWIZZLE_RGBA = 0x8E46,
    GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS = 0x8E47,
    GL_ACTIVE_SUBROUTINE_MAX_LENGTH = 0x8E48,
    GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH = 0x8E49,
    GL_NUM_COMPATIBLE_SUBROUTINES = 0x8E4A,
    GL_COMPATIBLE_SUBROUTINES = 0x8E4B,
    GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION = 0x8E4C,
    GL_FIRST_VERTEX_CONVENTION = 0x8E4D,
    GL_LAST_VERTEX_CONVENTION = 0x8E4E,
    GL_PROVOKING_VERTEX = 0x8E4F,
    GL_SAMPLE_POSITION = 0x8E50,
    GL_SAMPLE_MASK = 0x8E51,
    GL_SAMPLE_MASK_VALUE = 0x8E52,
    GL_MAX_SAMPLE_MASK_WORDS = 0x8E59,
    GL_MAX_GEOMETRY_SHADER_INVOCATIONS = 0x8E5A,
    GL_MIN_FRAGMENT_INTERPOLATION_OFFSET = 0x8E5B,
    GL_MAX_FRAGMENT_INTERPOLATION_OFFSET = 0x8E5C,
    GL_FRAGMENT_INTERPOLATION_OFFSET_BITS = 0x8E5D,
    GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET = 0x8E5E,
    GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET = 0x8E5F,
    GL_MAX_TRANSFORM_FEEDBACK_BUFFERS = 0x8E70,
    GL_MAX_VERTEX_STREAMS = 0x8E71,
    GL_PATCH_VERTICES = 0x8E72,
    GL_PATCH_DEFAULT_INNER_LEVEL = 0x8E73,
    GL_PATCH_DEFAULT_OUTER_LEVEL = 0x8E74,
    GL_TESS_CONTROL_OUTPUT_VERTICES = 0x8E75,
    GL_TESS_GEN_MODE = 0x8E76,
    GL_TESS_GEN_SPACING = 0x8E77,
    GL_TESS_GEN_VERTEX_ORDER = 0x8E78,
    GL_TESS_GEN_POINT_MODE = 0x8E79,
    GL_ISOLINES = 0x8E7A,
    GL_FRACTIONAL_ODD = 0x8E7B,
    GL_FRACTIONAL_EVEN = 0x8E7C,
    GL_MAX_PATCH_VERTICES = 0x8E7D,
    GL_MAX_TESS_GEN_LEVEL = 0x8E7E,
    GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS = 0x8E7F,
    GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS = 0x8E80,
    GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS = 0x8E81,
    GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS = 0x8E82,
    GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS = 0x8E83,
    GL_MAX_TESS_PATCH_COMPONENTS = 0x8E84,
    GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS = 0x8E85,
    GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS = 0x8E86,
    GL_TESS_EVALUATION_SHADER = 0x8E87,
    GL_TESS_CONTROL_SHADER = 0x8E88,
    GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS = 0x8E89,
    GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS = 0x8E8A,
    GL_COMPRESSED_RGBA_BPTC_UNORM = 0x8E8C,
    GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM = 0x8E8D,
    GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT = 0x8E8E,
    GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT = 0x8E8F,
    GL_BUFFER_GPU_ADDRESS_NV = 0x8F1D,
    GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV = 0x8F1E,
    GL_ELEMENT_ARRAY_UNIFIED_NV = 0x8F1F,
    GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV = 0x8F20,
    GL_VERTEX_ARRAY_ADDRESS_NV = 0x8F21,
    GL_NORMAL_ARRAY_ADDRESS_NV = 0x8F22,
    GL_COLOR_ARRAY_ADDRESS_NV = 0x8F23,
    GL_INDEX_ARRAY_ADDRESS_NV = 0x8F24,
    GL_TEXTURE_COORD_ARRAY_ADDRESS_NV = 0x8F25,
    GL_EDGE_FLAG_ARRAY_ADDRESS_NV = 0x8F26,
    GL_SECONDARY_COLOR_ARRAY_ADDRESS_NV = 0x8F27,
    GL_FOG_COORD_ARRAY_ADDRESS_NV = 0x8F28,
    GL_ELEMENT_ARRAY_ADDRESS_NV = 0x8F29,
    GL_VERTEX_ATTRIB_ARRAY_LENGTH_NV = 0x8F2A,
    GL_VERTEX_ARRAY_LENGTH_NV = 0x8F2B,
    GL_NORMAL_ARRAY_LENGTH_NV = 0x8F2C,
    GL_COLOR_ARRAY_LENGTH_NV = 0x8F2D,
    GL_INDEX_ARRAY_LENGTH_NV = 0x8F2E,
    GL_TEXTURE_COORD_ARRAY_LENGTH_NV = 0x8F2F,
    GL_EDGE_FLAG_ARRAY_LENGTH_NV = 0x8F30,
    GL_SECONDARY_COLOR_ARRAY_LENGTH_NV = 0x8F31,
    GL_FOG_COORD_ARRAY_LENGTH_NV = 0x8F32,
    GL_ELEMENT_ARRAY_LENGTH_NV = 0x8F33,
    GL_GPU_ADDRESS_NV = 0x8F34,
    GL_MAX_SHADER_BUFFER_ADDRESS_NV = 0x8F35,
    GL_COPY_READ_BUFFER = 0x8F36,
    GL_COPY_READ_BUFFER_BINDING = 0x8F36,
    GL_COPY_WRITE_BUFFER = 0x8F37,
    GL_COPY_WRITE_BUFFER_BINDING = 0x8F37,
    GL_MAX_IMAGE_UNITS = 0x8F38,
    GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS = 0x8F39,
    GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES = 0x8F39,
    GL_IMAGE_BINDING_NAME = 0x8F3A,
    GL_IMAGE_BINDING_LEVEL = 0x8F3B,
    GL_IMAGE_BINDING_LAYERED = 0x8F3C,
    GL_IMAGE_BINDING_LAYER = 0x8F3D,
    GL_IMAGE_BINDING_ACCESS = 0x8F3E,
    GL_DRAW_INDIRECT_BUFFER = 0x8F3F,
    GL_DRAW_INDIRECT_UNIFIED_NV = 0x8F40,
    GL_DRAW_INDIRECT_ADDRESS_NV = 0x8F41,
    GL_DRAW_INDIRECT_LENGTH_NV = 0x8F42,
    GL_DRAW_INDIRECT_BUFFER_BINDING = 0x8F43,
    GL_DOUBLE_MAT2 = 0x8F46,
    GL_DOUBLE_MAT3 = 0x8F47,
    GL_DOUBLE_MAT4 = 0x8F48,
    GL_DOUBLE_MAT2x3 = 0x8F49,
    GL_DOUBLE_MAT2x4 = 0x8F4A,
    GL_DOUBLE_MAT3x2 = 0x8F4B,
    GL_DOUBLE_MAT3x4 = 0x8F4C,
    GL_DOUBLE_MAT4x2 = 0x8F4D,
    GL_DOUBLE_MAT4x3 = 0x8F4E,
    GL_VERTEX_BINDING_BUFFER = 0x8F4F,
    GL_R8_SNORM = 0x8F94,
    GL_RG8_SNORM = 0x8F95,
    GL_RGB8_SNORM = 0x8F96,
    GL_RGBA8_SNORM = 0x8F97,
    GL_R16_SNORM = 0x8F98,
    GL_RG16_SNORM = 0x8F99,
    GL_RGB16_SNORM = 0x8F9A,
    GL_RGBA16_SNORM = 0x8F9B,
    GL_SIGNED_NORMALIZED = 0x8F9C,
    GL_PRIMITIVE_RESTART = 0x8F9D,
    GL_PRIMITIVE_RESTART_INDEX = 0x8F9E,
    GL_DOUBLE_VEC2 = 0x8FFC,
    GL_DOUBLE_VEC3 = 0x8FFD,
    GL_DOUBLE_VEC4 = 0x8FFE,
    GL_TEXTURE_CUBE_MAP_ARRAY = 0x9009,
    GL_TEXTURE_BINDING_CUBE_MAP_ARRAY = 0x900A,
    GL_PROXY_TEXTURE_CUBE_MAP_ARRAY = 0x900B,
    GL_SAMPLER_CUBE_MAP_ARRAY = 0x900C,
    GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW = 0x900D,
    GL_INT_SAMPLER_CUBE_MAP_ARRAY = 0x900E,
    GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY = 0x900F,
    GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX = 0x9047,
    GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX = 0x9048,
    GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX = 0x9049,
    GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX = 0x904A,
    GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX = 0x904B,
    GL_IMAGE_1D = 0x904C,
    GL_IMAGE_2D = 0x904D,
    GL_IMAGE_3D = 0x904E,
    GL_IMAGE_2D_RECT = 0x904F,
    GL_IMAGE_CUBE = 0x9050,
    GL_IMAGE_BUFFER = 0x9051,
    GL_IMAGE_1D_ARRAY = 0x9052,
    GL_IMAGE_2D_ARRAY = 0x9053,
    GL_IMAGE_CUBE_MAP_ARRAY = 0x9054,
    GL_IMAGE_2D_MULTISAMPLE = 0x9055,
    GL_IMAGE_2D_MULTISAMPLE_ARRAY = 0x9056,
    GL_INT_IMAGE_1D = 0x9057,
    GL_INT_IMAGE_2D = 0x9058,
    GL_INT_IMAGE_3D = 0x9059,
    GL_INT_IMAGE_2D_RECT = 0x905A,
    GL_INT_IMAGE_CUBE = 0x905B,
    GL_INT_IMAGE_BUFFER = 0x905C,
    GL_INT_IMAGE_1D_ARRAY = 0x905D,
    GL_INT_IMAGE_2D_ARRAY = 0x905E,
    GL_INT_IMAGE_CUBE_MAP_ARRAY = 0x905F,
    GL_INT_IMAGE_2D_MULTISAMPLE = 0x9060,
    GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY = 0x9061,
    GL_UNSIGNED_INT_IMAGE_1D = 0x9062,
    GL_UNSIGNED_INT_IMAGE_2D = 0x9063,
    GL_UNSIGNED_INT_IMAGE_3D = 0x9064,
    GL_UNSIGNED_INT_IMAGE_2D_RECT = 0x9065,
    GL_UNSIGNED_INT_IMAGE_CUBE = 0x9066,
    GL_UNSIGNED_INT_IMAGE_BUFFER = 0x9067,
    GL_UNSIGNED_INT_IMAGE_1D_ARRAY = 0x9068,
    GL_UNSIGNED_INT_IMAGE_2D_ARRAY = 0x9069,
    GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY = 0x906A,
    GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE = 0x906B,
    GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY = 0x906C,
    GL_MAX_IMAGE_SAMPLES = 0x906D,
    GL_IMAGE_BINDING_FORMAT = 0x906E,
    GL_RGB10_A2UI = 0x906F,
    GL_PATH_FORMAT_SVG_NV = 0x9070,
    GL_PATH_FORMAT_PS_NV = 0x9071,
    GL_STANDARD_FONT_NAME_NV = 0x9072,
    GL_SYSTEM_FONT_NAME_NV = 0x9073,
    GL_FILE_NAME_NV = 0x9074,
    GL_PATH_STROKE_WIDTH_NV = 0x9075,
    GL_PATH_END_CAPS_NV = 0x9076,
    GL_PATH_INITIAL_END_CAP_NV = 0x9077,
    GL_PATH_TERMINAL_END_CAP_NV = 0x9078,
    GL_PATH_JOIN_STYLE_NV = 0x9079,
    GL_PATH_MITER_LIMIT_NV = 0x907A,
    GL_PATH_DASH_CAPS_NV = 0x907B,
    GL_PATH_INITIAL_DASH_CAP_NV = 0x907C,
    GL_PATH_TERMINAL_DASH_CAP_NV = 0x907D,
    GL_PATH_DASH_OFFSET_NV = 0x907E,
    GL_PATH_CLIENT_LENGTH_NV = 0x907F,
    GL_PATH_FILL_MODE_NV = 0x9080,
    GL_PATH_FILL_MASK_NV = 0x9081,
    GL_PATH_FILL_COVER_MODE_NV = 0x9082,
    GL_PATH_STROKE_COVER_MODE_NV = 0x9083,
    GL_PATH_STROKE_MASK_NV = 0x9084,
    GL_COUNT_UP_NV = 0x9088,
    GL_COUNT_DOWN_NV = 0x9089,
    GL_PATH_OBJECT_BOUNDING_BOX_NV = 0x908A,
    GL_CONVEX_HULL_NV = 0x908B,
    GL_BOUNDING_BOX_NV = 0x908D,
    GL_TRANSLATE_X_NV = 0x908E,
    GL_TRANSLATE_Y_NV = 0x908F,
    GL_TRANSLATE_2D_NV = 0x9090,
    GL_TRANSLATE_3D_NV = 0x9091,
    GL_AFFINE_2D_NV = 0x9092,
    GL_AFFINE_3D_NV = 0x9094,
    GL_TRANSPOSE_AFFINE_2D_NV = 0x9096,
    GL_TRANSPOSE_AFFINE_3D_NV = 0x9098,
    GL_UTF8_NV = 0x909A,
    GL_UTF16_NV = 0x909B,
    GL_BOUNDING_BOX_OF_BOUNDING_BOXES_NV = 0x909C,
    GL_PATH_COMMAND_COUNT_NV = 0x909D,
    GL_PATH_COORD_COUNT_NV = 0x909E,
    GL_PATH_DASH_ARRAY_COUNT_NV = 0x909F,
    GL_PATH_COMPUTED_LENGTH_NV = 0x90A0,
    GL_PATH_FILL_BOUNDING_BOX_NV = 0x90A1,
    GL_PATH_STROKE_BOUNDING_BOX_NV = 0x90A2,
    GL_SQUARE_NV = 0x90A3,
    GL_ROUND_NV = 0x90A4,
    GL_TRIANGULAR_NV = 0x90A5,
    GL_BEVEL_NV = 0x90A6,
    GL_MITER_REVERT_NV = 0x90A7,
    GL_MITER_TRUNCATE_NV = 0x90A8,
    GL_SKIP_MISSING_GLYPH_NV = 0x90A9,
    GL_USE_MISSING_GLYPH_NV = 0x90AA,
    GL_PATH_ERROR_POSITION_NV = 0x90AB,
    GL_PATH_FOG_GEN_MODE_NV = 0x90AC,
    GL_ACCUM_ADJACENT_PAIRS_NV = 0x90AD,
    GL_ADJACENT_PAIRS_NV = 0x90AE,
    GL_FIRST_TO_REST_NV = 0x90AF,
    GL_PATH_GEN_MODE_NV = 0x90B0,
    GL_PATH_GEN_COEFF_NV = 0x90B1,
    GL_PATH_GEN_COLOR_FORMAT_NV = 0x90B2,
    GL_PATH_GEN_COMPONENTS_NV = 0x90B3,
    GL_PATH_DASH_OFFSET_RESET_NV = 0x90B4,
    GL_MOVE_TO_RESETS_NV = 0x90B5,
    GL_MOVE_TO_CONTINUES_NV = 0x90B6,
    GL_PATH_STENCIL_FUNC_NV = 0x90B7,
    GL_PATH_STENCIL_REF_NV = 0x90B8,
    GL_PATH_STENCIL_VALUE_MASK_NV = 0x90B9,
    GL_MIN_MAP_BUFFER_ALIGNMENT = 0x90BC,
    GL_PATH_STENCIL_DEPTH_OFFSET_FACTOR_NV = 0x90BD,
    GL_PATH_STENCIL_DEPTH_OFFSET_UNITS_NV = 0x90BE,
    GL_PATH_COVER_DEPTH_FUNC_NV = 0x90BF,
    GL_IMAGE_FORMAT_COMPATIBILITY_TYPE = 0x90C7,
    GL_IMAGE_FORMAT_COMPATIBILITY_BY_SIZE = 0x90C8,
    GL_IMAGE_FORMAT_COMPATIBILITY_BY_CLASS = 0x90C9,
    GL_MAX_VERTEX_IMAGE_UNIFORMS = 0x90CA,
    GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS = 0x90CB,
    GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS = 0x90CC,
    GL_MAX_GEOMETRY_IMAGE_UNIFORMS = 0x90CD,
    GL_MAX_FRAGMENT_IMAGE_UNIFORMS = 0x90CE,
    GL_MAX_COMBINED_IMAGE_UNIFORMS = 0x90CF,
    GL_SHADER_STORAGE_BUFFER = 0x90D2,
    GL_SHADER_STORAGE_BUFFER_BINDING = 0x90D3,
    GL_SHADER_STORAGE_BUFFER_START = 0x90D4,
    GL_SHADER_STORAGE_BUFFER_SIZE = 0x90D5,
    GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS = 0x90D6,
    GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS = 0x90D7,
    GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS = 0x90D8,
    GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS = 0x90D9,
    GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS = 0x90DA,
    GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS = 0x90DB,
    GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS = 0x90DC,
    GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS = 0x90DD,
    GL_MAX_SHADER_STORAGE_BLOCK_SIZE = 0x90DE,
    GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT = 0x90DF,
    GL_DEPTH_STENCIL_TEXTURE_MODE = 0x90EA,
    GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS = 0x90EB,
    GL_MAX_COMPUTE_FIXED_GROUP_INVOCATIONS_ARB = 0x90EB,
    GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER = 0x90EC,
    GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER = 0x90ED,
    GL_DISPATCH_INDIRECT_BUFFER = 0x90EE,
    GL_DISPATCH_INDIRECT_BUFFER_BINDING = 0x90EF,
    GL_TEXTURE_2D_MULTISAMPLE = 0x9100,
    GL_PROXY_TEXTURE_2D_MULTISAMPLE = 0x9101,
    GL_TEXTURE_2D_MULTISAMPLE_ARRAY = 0x9102,
    GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY = 0x9103,
    GL_TEXTURE_BINDING_2D_MULTISAMPLE = 0x9104,
    GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY = 0x9105,
    GL_TEXTURE_SAMPLES = 0x9106,
    GL_TEXTURE_FIXED_SAMPLE_LOCATIONS = 0x9107,
    GL_SAMPLER_2D_MULTISAMPLE = 0x9108,
    GL_INT_SAMPLER_2D_MULTISAMPLE = 0x9109,
    GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE = 0x910A,
    GL_SAMPLER_2D_MULTISAMPLE_ARRAY = 0x910B,
    GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY = 0x910C,
    GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY = 0x910D,
    GL_MAX_COLOR_TEXTURE_SAMPLES = 0x910E,
    GL_MAX_DEPTH_TEXTURE_SAMPLES = 0x910F,
    GL_MAX_INTEGER_SAMPLES = 0x9110,
    GL_MAX_SERVER_WAIT_TIMEOUT = 0x9111,
    GL_OBJECT_TYPE = 0x9112,
    GL_SYNC_CONDITION = 0x9113,
    GL_SYNC_STATUS = 0x9114,
    GL_SYNC_FLAGS = 0x9115,
    GL_SYNC_FENCE = 0x9116,
    GL_SYNC_GPU_COMMANDS_COMPLETE = 0x9117,
    GL_UNSIGNALED = 0x9118,
    GL_SIGNALED = 0x9119,
    GL_ALREADY_SIGNALED = 0x911A,
    GL_TIMEOUT_EXPIRED = 0x911B,
    GL_CONDITION_SATISFIED = 0x911C,
    GL_WAIT_FAILED = 0x911D,
    GL_BUFFER_ACCESS_FLAGS = 0x911F,
    GL_BUFFER_MAP_LENGTH = 0x9120,
    GL_BUFFER_MAP_OFFSET = 0x9121,
    GL_MAX_VERTEX_OUTPUT_COMPONENTS = 0x9122,
    GL_MAX_GEOMETRY_INPUT_COMPONENTS = 0x9123,
    GL_MAX_GEOMETRY_OUTPUT_COMPONENTS = 0x9124,
    GL_MAX_FRAGMENT_INPUT_COMPONENTS = 0x9125,
    GL_CONTEXT_PROFILE_MASK = 0x9126,
    GL_UNPACK_COMPRESSED_BLOCK_WIDTH = 0x9127,
    GL_UNPACK_COMPRESSED_BLOCK_HEIGHT = 0x9128,
    GL_UNPACK_COMPRESSED_BLOCK_DEPTH = 0x9129,
    GL_UNPACK_COMPRESSED_BLOCK_SIZE = 0x912A,
    GL_PACK_COMPRESSED_BLOCK_WIDTH = 0x912B,
    GL_PACK_COMPRESSED_BLOCK_HEIGHT = 0x912C,
    GL_PACK_COMPRESSED_BLOCK_DEPTH = 0x912D,
    GL_PACK_COMPRESSED_BLOCK_SIZE = 0x912E,
    GL_TEXTURE_IMMUTABLE_FORMAT = 0x912F,
    GL_MAX_DEBUG_MESSAGE_LENGTH = 0x9143,
    GL_MAX_DEBUG_MESSAGE_LENGTH_KHR = 0x9143,
    GL_MAX_DEBUG_LOGGED_MESSAGES = 0x9144,
    GL_MAX_DEBUG_LOGGED_MESSAGES_KHR = 0x9144,
    GL_DEBUG_LOGGED_MESSAGES = 0x9145,
    GL_DEBUG_LOGGED_MESSAGES_KHR = 0x9145,
    GL_DEBUG_SEVERITY_HIGH = 0x9146,
    GL_DEBUG_SEVERITY_HIGH_KHR = 0x9146,
    GL_DEBUG_SEVERITY_MEDIUM = 0x9147,
    GL_DEBUG_SEVERITY_MEDIUM_KHR = 0x9147,
    GL_DEBUG_SEVERITY_LOW = 0x9148,
    GL_DEBUG_SEVERITY_LOW_KHR = 0x9148,
    GL_QUERY_BUFFER = 0x9192,
    GL_QUERY_BUFFER_BINDING = 0x9193,
    GL_QUERY_RESULT_NO_WAIT = 0x9194,
    GL_TEXTURE_BUFFER_OFFSET = 0x919D,
    GL_TEXTURE_BUFFER_SIZE = 0x919E,
    GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT = 0x919F,
    GL_COMPUTE_SHADER = 0x91B9,
    GL_MAX_COMPUTE_UNIFORM_BLOCKS = 0x91BB,
    GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS = 0x91BC,
    GL_MAX_COMPUTE_IMAGE_UNIFORMS = 0x91BD,
    GL_MAX_COMPUTE_WORK_GROUP_COUNT = 0x91BE,
    GL_MAX_COMPUTE_WORK_GROUP_SIZE = 0x91BF,
    GL_MAX_COMPUTE_FIXED_GROUP_SIZE_ARB = 0x91BF,
    GL_COMPRESSED_R11_EAC = 0x9270,
    GL_COMPRESSED_SIGNED_R11_EAC = 0x9271,
    GL_COMPRESSED_RG11_EAC = 0x9272,
    GL_COMPRESSED_SIGNED_RG11_EAC = 0x9273,
    GL_COMPRESSED_RGB8_ETC2 = 0x9274,
    GL_COMPRESSED_SRGB8_ETC2 = 0x9275,
    GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 = 0x9276,
    GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 = 0x9277,
    GL_COMPRESSED_RGBA8_ETC2_EAC = 0x9278,
    GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC = 0x9279,
    GL_ATOMIC_COUNTER_BUFFER = 0x92C0,
    GL_ATOMIC_COUNTER_BUFFER_BINDING = 0x92C1,
    GL_ATOMIC_COUNTER_BUFFER_START = 0x92C2,
    GL_ATOMIC_COUNTER_BUFFER_SIZE = 0x92C3,
    GL_ATOMIC_COUNTER_BUFFER_DATA_SIZE = 0x92C4,
    GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS = 0x92C5,
    GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES = 0x92C6,
    GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER = 0x92C7,
    GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER = 0x92C8,
    GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER = 0x92C9,
    GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER = 0x92CA,
    GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER = 0x92CB,
    GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS = 0x92CC,
    GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS = 0x92CD,
    GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS = 0x92CE,
    GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS = 0x92CF,
    GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS = 0x92D0,
    GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS = 0x92D1,
    GL_MAX_VERTEX_ATOMIC_COUNTERS = 0x92D2,
    GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS = 0x92D3,
    GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS = 0x92D4,
    GL_MAX_GEOMETRY_ATOMIC_COUNTERS = 0x92D5,
    GL_MAX_FRAGMENT_ATOMIC_COUNTERS = 0x92D6,
    GL_MAX_COMBINED_ATOMIC_COUNTERS = 0x92D7,
    GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE = 0x92D8,
    GL_ACTIVE_ATOMIC_COUNTER_BUFFERS = 0x92D9,
    GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX = 0x92DA,
    GL_UNSIGNED_INT_ATOMIC_COUNTER = 0x92DB,
    GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS = 0x92DC,
    GL_DEBUG_OUTPUT = 0x92E0,
    GL_DEBUG_OUTPUT_KHR = 0x92E0,
    GL_UNIFORM = 0x92E1,
    GL_UNIFORM_BLOCK = 0x92E2,
    GL_PROGRAM_INPUT = 0x92E3,
    GL_PROGRAM_OUTPUT = 0x92E4,
    GL_BUFFER_VARIABLE = 0x92E5,
    GL_SHADER_STORAGE_BLOCK = 0x92E6,
    GL_IS_PER_PATCH = 0x92E7,
    GL_VERTEX_SUBROUTINE = 0x92E8,
    GL_TESS_CONTROL_SUBROUTINE = 0x92E9,
    GL_TESS_EVALUATION_SUBROUTINE = 0x92EA,
    GL_GEOMETRY_SUBROUTINE = 0x92EB,
    GL_FRAGMENT_SUBROUTINE = 0x92EC,
    GL_COMPUTE_SUBROUTINE = 0x92ED,
    GL_VERTEX_SUBROUTINE_UNIFORM = 0x92EE,
    GL_TESS_CONTROL_SUBROUTINE_UNIFORM = 0x92EF,
    GL_TESS_EVALUATION_SUBROUTINE_UNIFORM = 0x92F0,
    GL_GEOMETRY_SUBROUTINE_UNIFORM = 0x92F1,
    GL_FRAGMENT_SUBROUTINE_UNIFORM = 0x92F2,
    GL_COMPUTE_SUBROUTINE_UNIFORM = 0x92F3,
    GL_TRANSFORM_FEEDBACK_VARYING = 0x92F4,
    GL_ACTIVE_RESOURCES = 0x92F5,
    GL_MAX_NAME_LENGTH = 0x92F6,
    GL_MAX_NUM_ACTIVE_VARIABLES = 0x92F7,
    GL_MAX_NUM_COMPATIBLE_SUBROUTINES = 0x92F8,
    GL_NAME_LENGTH = 0x92F9,
    GL_TYPE = 0x92FA,
    GL_ARRAY_SIZE = 0x92FB,
    GL_OFFSET = 0x92FC,
    GL_BLOCK_INDEX = 0x92FD,
    GL_ARRAY_STRIDE = 0x92FE,
    GL_MATRIX_STRIDE = 0x92FF,
    GL_IS_ROW_MAJOR = 0x9300,
    GL_ATOMIC_COUNTER_BUFFER_INDEX = 0x9301,
    GL_BUFFER_BINDING = 0x9302,
    GL_BUFFER_DATA_SIZE = 0x9303,
    GL_NUM_ACTIVE_VARIABLES = 0x9304,
    GL_ACTIVE_VARIABLES = 0x9305,
    GL_REFERENCED_BY_VERTEX_SHADER = 0x9306,
    GL_REFERENCED_BY_TESS_CONTROL_SHADER = 0x9307,
    GL_REFERENCED_BY_TESS_EVALUATION_SHADER = 0x9308,
    GL_REFERENCED_BY_GEOMETRY_SHADER = 0x9309,
    GL_REFERENCED_BY_FRAGMENT_SHADER = 0x930A,
    GL_REFERENCED_BY_COMPUTE_SHADER = 0x930B,
    GL_TOP_LEVEL_ARRAY_SIZE = 0x930C,
    GL_TOP_LEVEL_ARRAY_STRIDE = 0x930D,
    GL_LOCATION = 0x930E,
    GL_LOCATION_INDEX = 0x930F,
    GL_FRAMEBUFFER_DEFAULT_WIDTH = 0x9310,
    GL_FRAMEBUFFER_DEFAULT_HEIGHT = 0x9311,
    GL_FRAMEBUFFER_DEFAULT_LAYERS = 0x9312,
    GL_FRAMEBUFFER_DEFAULT_SAMPLES = 0x9313,
    GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS = 0x9314,
    GL_MAX_FRAMEBUFFER_WIDTH = 0x9315,
    GL_MAX_FRAMEBUFFER_HEIGHT = 0x9316,
    GL_MAX_FRAMEBUFFER_LAYERS = 0x9317,
    GL_MAX_FRAMEBUFFER_SAMPLES = 0x9318,
    GL_MAX_COMPUTE_VARIABLE_GROUP_INVOCATIONS_ARB = 0x9344,
    GL_MAX_COMPUTE_VARIABLE_GROUP_SIZE_ARB = 0x9345,
    GL_LOCATION_COMPONENT = 0x934A,
    GL_TRANSFORM_FEEDBACK_BUFFER_INDEX = 0x934B,
    GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE = 0x934C,
    GL_CLIP_ORIGIN = 0x935C,
    GL_CLIP_DEPTH_MODE = 0x935D,
    GL_NEGATIVE_ONE_TO_ONE = 0x935E,
    GL_ZERO_TO_ONE = 0x935F,
    GL_CLEAR_TEXTURE = 0x9365,
    GL_FONT_GLYPHS_AVAILABLE_NV = 0x9368,
    GL_FONT_TARGET_UNAVAILABLE_NV = 0x9369,
    GL_FONT_UNAVAILABLE_NV = 0x936A,
    GL_FONT_UNINTELLIGIBLE_NV = 0x936B,
    GL_STANDARD_FONT_FORMAT_NV = 0x936C,
    GL_FRAGMENT_INPUT_NV = 0x936D,
    GL_UNIFORM_BUFFER_UNIFIED_NV = 0x936E,
    GL_UNIFORM_BUFFER_ADDRESS_NV = 0x936F,
    GL_UNIFORM_BUFFER_LENGTH_NV = 0x9370,
    GL_NUM_SAMPLE_COUNTS = 0x9380,
    GL_SHADER_BINARY_FORMAT_SPIR_V = 0x9551,
    GL_SHADER_BINARY_FORMAT_SPIR_V_ARB = 0x9551,
    GL_SPIR_V_BINARY = 0x9552,
    GL_SPIR_V_BINARY_ARB = 0x9552,
    GL_SPIR_V_EXTENSIONS = 0x9553,
    GL_NUM_SPIR_V_EXTENSIONS = 0x9554,
};
    
// Functions
    
inline void (*glActiveShaderProgram)(GLuint pipeline, GLuint program);
inline void (*glActiveTexture)(GLenum texture);
inline void (*glAttachShader)(GLuint program, GLuint shader);
inline void (*glBeginConditionalRender)(GLuint id, GLenum mode);
inline void (*glBeginQuery)(GLenum target, GLuint id);
inline void (*glBeginQueryIndexed)(GLenum target, GLuint index, GLuint id);
inline void (*glBeginTransformFeedback)(GLenum primitiveMode);
inline void (*glBindAttribLocation)(GLuint program, GLuint index, GLchar name);
inline void (*glBindBuffer)(GLenum target, GLuint buffer);
inline void (*glBindBufferBase)(GLenum target, GLuint index, GLuint buffer);
inline void (*glBindBufferRange)(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
inline void (*glBindBuffersBase)(GLenum target, GLuint first, GLsizei count, GLuint buffers);
inline void (*glBindBuffersRange)(GLenum target, GLuint first, GLsizei count, GLuint buffers, GLintptr offsets, GLsizeiptr sizes);
inline void (*glBindFragDataLocation)(GLuint program, GLuint color, GLchar name);
inline void (*glBindFragDataLocationIndexed)(GLuint program, GLuint colorNumber, GLuint index, GLchar name);
inline void (*glBindFramebuffer)(GLenum target, GLuint framebuffer);
inline void (*glBindImageTexture)(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
inline void (*glBindImageTextures)(GLuint first, GLsizei count, GLuint textures);
inline void (*glBindProgramPipeline)(GLuint pipeline);
inline void (*glBindRenderbuffer)(GLenum target, GLuint renderbuffer);
inline void (*glBindSampler)(GLuint unit, GLuint sampler);
inline void (*glBindSamplers)(GLuint first, GLsizei count, GLuint samplers);
inline void (*glBindTexture)(GLenum target, GLuint texture);
inline void (*glBindTextureUnit)(GLuint unit, GLuint texture);
inline void (*glBindTextures)(GLuint first, GLsizei count, GLuint textures);
inline void (*glBindTransformFeedback)(GLenum target, GLuint id);
inline void (*glBindVertexArray)(GLuint array);
inline void (*glBindVertexBuffer)(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
inline void (*glBindVertexBuffers)(GLuint first, GLsizei count, GLuint buffers, GLintptr offsets, GLsizei strides);
inline void (*glBlendColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
inline void (*glBlendEquation)(GLenum mode);
inline void (*glBlendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha);
inline void (*glBlendEquationSeparatei)(GLuint buf, GLenum modeRGB, GLenum modeAlpha);
inline void (*glBlendEquationi)(GLuint buf, GLenum mode);
inline void (*glBlendFunc)(GLenum sfactor, GLenum dfactor);
inline void (*glBlendFuncSeparate)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
inline void (*glBlendFuncSeparatei)(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
inline void (*glBlendFunci)(GLuint buf, GLenum src, GLenum dst);
inline void (*glBlitFramebuffer)(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
inline void (*glBlitNamedFramebuffer)(GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
inline void (*glBufferAddressRangeNV)(GLenum pname, GLuint index, GLuint64EXT address, GLsizeiptr length);
inline void (*glBufferData)(GLenum target, GLsizeiptr size, const void * data, GLenum usage);
inline void (*glBufferStorage)(GLenum target, GLsizeiptr size, const void * data, GLbitfield flags);
inline void (*glBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const void * data);
inline void (*glCallCommandListNV)(GLuint list);
inline GLenum(*glCheckFramebufferStatus)(GLenum target);
inline GLenum(*glCheckNamedFramebufferStatus)(GLuint framebuffer, GLenum target);
inline void (*glClampColor)(GLenum target, GLenum clamp);
inline void (*glClear)(GLbitfield mask);
inline void (*glClearBufferData)(GLenum target, GLenum internalformat, GLenum format, GLenum type, const void * data);
inline void (*glClearBufferSubData)(GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void * data);
inline void (*glClearBufferfi)(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
inline void (*glClearBufferfv)(GLenum buffer, GLint drawbuffer, GLfloat value);
inline void (*glClearBufferiv)(GLenum buffer, GLint drawbuffer, GLint value);
inline void (*glClearBufferuiv)(GLenum buffer, GLint drawbuffer, GLuint value);
inline void (*glClearColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
inline void (*glClearDepth)(GLdouble depth);
inline void (*glClearDepthf)(GLfloat d);
inline void (*glClearNamedBufferData)(GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void * data);
inline void (*glClearNamedBufferSubData)(GLuint buffer, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void * data);
inline void (*glClearNamedFramebufferfi)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
inline void (*glClearNamedFramebufferfv)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, GLfloat value);
inline void (*glClearNamedFramebufferiv)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, GLint value);
inline void (*glClearNamedFramebufferuiv)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, GLuint value);
inline void (*glClearStencil)(GLint s);
inline void (*glClearTexImage)(GLuint texture, GLint level, GLenum format, GLenum type, const void * data);
inline void (*glClearTexSubImage)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void * data);
inline GLenum(*glClientWaitSync)(GLsync sync, GLbitfield flags, GLuint64 timeout);
inline void (*glClipControl)(GLenum origin, GLenum depth);
inline void (*glColorFormatNV)(GLint size, GLenum type, GLsizei stride);
inline void (*glColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
inline void (*glColorMaski)(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
inline void (*glCommandListSegmentsNV)(GLuint list, GLuint segments);
inline void (*glCompileCommandListNV)(GLuint list);
inline void (*glCompileShader)(GLuint shader);
inline void (*glCompressedTexImage1D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void * data);
inline void (*glCompressedTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void * data);
inline void (*glCompressedTexImage3D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void * data);
inline void (*glCompressedTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void * data);
inline void (*glCompressedTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void * data);
inline void (*glCompressedTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void * data);
inline void (*glCompressedTextureSubImage1D)(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void * data);
inline void (*glCompressedTextureSubImage2D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void * data);
inline void (*glCompressedTextureSubImage3D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void * data);
inline void (*glCopyBufferSubData)(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
inline void (*glCopyImageSubData)(GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth);
inline void (*glCopyNamedBufferSubData)(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
inline void (*glCopyPathNV)(GLuint resultPath, GLuint srcPath);
inline void (*glCopyTexImage1D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
inline void (*glCopyTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
inline void (*glCopyTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
inline void (*glCopyTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
inline void (*glCopyTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
inline void (*glCopyTextureSubImage1D)(GLuint texture, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
inline void (*glCopyTextureSubImage2D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
inline void (*glCopyTextureSubImage3D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
inline void (*glCoverFillPathInstancedNV)(GLsizei numPaths, GLenum pathNameType, const void * paths, GLuint pathBase, GLenum coverMode, GLenum transformType, GLfloat transformValues);
inline void (*glCoverFillPathNV)(GLuint path, GLenum coverMode);
inline void (*glCoverStrokePathInstancedNV)(GLsizei numPaths, GLenum pathNameType, const void * paths, GLuint pathBase, GLenum coverMode, GLenum transformType, GLfloat transformValues);
inline void (*glCoverStrokePathNV)(GLuint path, GLenum coverMode);
inline void (*glCreateBuffers)(GLsizei n, GLuint buffers);
inline void (*glCreateCommandListsNV)(GLsizei n, GLuint lists);
inline void (*glCreateFramebuffers)(GLsizei n, GLuint framebuffers);
inline GLuint(*glCreateProgram)();
inline void (*glCreateProgramPipelines)(GLsizei n, GLuint pipelines);
inline void (*glCreateQueries)(GLenum target, GLsizei n, GLuint ids);
inline void (*glCreateRenderbuffers)(GLsizei n, GLuint renderbuffers);
inline void (*glCreateSamplers)(GLsizei n, GLuint samplers);
inline GLuint(*glCreateShader)(GLenum type);
inline GLuint(*glCreateShaderProgramv)(GLenum type, GLsizei count, GLchar strings);
inline void (*glCreateStatesNV)(GLsizei n, GLuint states);
inline void (*glCreateTextures)(GLenum target, GLsizei n, GLuint textures);
inline void (*glCreateTransformFeedbacks)(GLsizei n, GLuint ids);
inline void (*glCreateVertexArrays)(GLsizei n, GLuint arrays);
inline void (*glCullFace)(GLenum mode);
inline void (*glDebugMessageCallback)(GLDEBUGPROC callback, const void * userParam);
inline void (*glDebugMessageCallbackKHR)(GLDEBUGPROCKHR callback, const void * userParam);
inline void (*glDebugMessageControl)(GLenum source, GLenum type, GLenum severity, GLsizei count, GLuint ids, GLboolean enabled);
inline void (*glDebugMessageControlKHR)(GLenum source, GLenum type, GLenum severity, GLsizei count, GLuint ids, GLboolean enabled);
inline void (*glDebugMessageInsert)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar buf);
inline void (*glDebugMessageInsertKHR)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar buf);
inline void (*glDeleteBuffers)(GLsizei n, GLuint buffers);
inline void (*glDeleteCommandListsNV)(GLsizei n, GLuint lists);
inline void (*glDeleteFramebuffers)(GLsizei n, GLuint framebuffers);
inline void (*glDeletePathsNV)(GLuint path, GLsizei range);
inline void (*glDeleteProgram)(GLuint program);
inline void (*glDeleteProgramPipelines)(GLsizei n, GLuint pipelines);
inline void (*glDeleteQueries)(GLsizei n, GLuint ids);
inline void (*glDeleteRenderbuffers)(GLsizei n, GLuint renderbuffers);
inline void (*glDeleteSamplers)(GLsizei count, GLuint samplers);
inline void (*glDeleteShader)(GLuint shader);
inline void (*glDeleteStatesNV)(GLsizei n, GLuint states);
inline void (*glDeleteSync)(GLsync sync);
inline void (*glDeleteTextures)(GLsizei n, GLuint textures);
inline void (*glDeleteTransformFeedbacks)(GLsizei n, GLuint ids);
inline void (*glDeleteVertexArrays)(GLsizei n, GLuint arrays);
inline void (*glDepthFunc)(GLenum func);
inline void (*glDepthMask)(GLboolean flag);
inline void (*glDepthRange)(GLdouble near, GLdouble far);
inline void (*glDepthRangeArrayv)(GLuint first, GLsizei count, GLdouble v);
inline void (*glDepthRangeIndexed)(GLuint index, GLdouble n, GLdouble f);
inline void (*glDepthRangef)(GLfloat n, GLfloat f);
inline void (*glDetachShader)(GLuint program, GLuint shader);
inline void (*glDisable)(GLenum cap);
inline void (*glDisableVertexArrayAttrib)(GLuint vaobj, GLuint index);
inline void (*glDisableVertexAttribArray)(GLuint index);
inline void (*glDisablei)(GLenum target, GLuint index);
inline void (*glDispatchCompute)(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
inline void (*glDispatchComputeGroupSizeARB)(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z, GLuint group_size_x, GLuint group_size_y, GLuint group_size_z);
inline void (*glDispatchComputeIndirect)(GLintptr indirect);
inline void (*glDrawArrays)(GLenum mode, GLint first, GLsizei count);
inline void (*glDrawArraysIndirect)(GLenum mode, const void * indirect);
inline void (*glDrawArraysInstanced)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
inline void (*glDrawArraysInstancedBaseInstance)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount, GLuint baseinstance);
inline void (*glDrawBuffer)(GLenum buf);
inline void (*glDrawBuffers)(GLsizei n, GLenum bufs);
inline void (*glDrawCommandsAddressNV)(GLenum primitiveMode, GLuint64 indirects, GLsizei sizes, GLuint count);
inline void (*glDrawCommandsNV)(GLenum primitiveMode, GLuint buffer, GLintptr indirects, GLsizei sizes, GLuint count);
inline void (*glDrawCommandsStatesAddressNV)(GLuint64 indirects, GLsizei sizes, GLuint states, GLuint fbos, GLuint count);
inline void (*glDrawCommandsStatesNV)(GLuint buffer, GLintptr indirects, GLsizei sizes, GLuint states, GLuint fbos, GLuint count);
inline void (*glDrawElements)(GLenum mode, GLsizei count, GLenum type, const void * indices);
inline void (*glDrawElementsBaseVertex)(GLenum mode, GLsizei count, GLenum type, const void * indices, GLint basevertex);
inline void (*glDrawElementsIndirect)(GLenum mode, GLenum type, const void * indirect);
inline void (*glDrawElementsInstanced)(GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei instancecount);
inline void (*glDrawElementsInstancedBaseInstance)(GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei instancecount, GLuint baseinstance);
inline void (*glDrawElementsInstancedBaseVertex)(GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei instancecount, GLint basevertex);
inline void (*glDrawElementsInstancedBaseVertexBaseInstance)(GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance);
inline void (*glDrawRangeElements)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void * indices);
inline void (*glDrawRangeElementsBaseVertex)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void * indices, GLint basevertex);
inline void (*glDrawTransformFeedback)(GLenum mode, GLuint id);
inline void (*glDrawTransformFeedbackInstanced)(GLenum mode, GLuint id, GLsizei instancecount);
inline void (*glDrawTransformFeedbackStream)(GLenum mode, GLuint id, GLuint stream);
inline void (*glDrawTransformFeedbackStreamInstanced)(GLenum mode, GLuint id, GLuint stream, GLsizei instancecount);
inline void (*glEdgeFlagFormatNV)(GLsizei stride);
inline void (*glEnable)(GLenum cap);
inline void (*glEnableVertexArrayAttrib)(GLuint vaobj, GLuint index);
inline void (*glEnableVertexAttribArray)(GLuint index);
inline void (*glEnablei)(GLenum target, GLuint index);
inline void (*glEndConditionalRender)();
inline void (*glEndQuery)(GLenum target);
inline void (*glEndQueryIndexed)(GLenum target, GLuint index);
inline void (*glEndTransformFeedback)();
inline GLsync(*glFenceSync)(GLenum condition, GLbitfield flags);
inline void (*glFinish)();
inline void (*glFlush)();
inline void (*glFlushMappedBufferRange)(GLenum target, GLintptr offset, GLsizeiptr length);
inline void (*glFlushMappedNamedBufferRange)(GLuint buffer, GLintptr offset, GLsizeiptr length);
inline void (*glFogCoordFormatNV)(GLenum type, GLsizei stride);
inline void (*glFramebufferParameteri)(GLenum target, GLenum pname, GLint param);
inline void (*glFramebufferRenderbuffer)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
inline void (*glFramebufferTexture)(GLenum target, GLenum attachment, GLuint texture, GLint level);
inline void (*glFramebufferTexture1D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
inline void (*glFramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
inline void (*glFramebufferTexture3D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
inline void (*glFramebufferTextureLayer)(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
inline void (*glFrontFace)(GLenum mode);
inline void (*glGenBuffers)(GLsizei n, GLuint buffers);
inline void (*glGenFramebuffers)(GLsizei n, GLuint framebuffers);
inline GLuint(*glGenPathsNV)(GLsizei range);
inline void (*glGenProgramPipelines)(GLsizei n, GLuint pipelines);
inline void (*glGenQueries)(GLsizei n, GLuint ids);
inline void (*glGenRenderbuffers)(GLsizei n, GLuint renderbuffers);
inline void (*glGenSamplers)(GLsizei count, GLuint samplers);
inline void (*glGenTextures)(GLsizei n, GLuint textures);
inline void (*glGenTransformFeedbacks)(GLsizei n, GLuint ids);
inline void (*glGenVertexArrays)(GLsizei n, GLuint arrays);
inline void (*glGenerateMipmap)(GLenum target);
inline void (*glGenerateTextureMipmap)(GLuint texture);
inline void (*glGetActiveAtomicCounterBufferiv)(GLuint program, GLuint bufferIndex, GLenum pname, GLint params);
inline void (*glGetActiveAttrib)(GLuint program, GLuint index, GLsizei bufSize, GLsizei length, GLint size, GLenum type, GLchar name);
inline void (*glGetActiveSubroutineName)(GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei length, GLchar name);
inline void (*glGetActiveSubroutineUniformName)(GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei length, GLchar name);
inline void (*glGetActiveSubroutineUniformiv)(GLuint program, GLenum shadertype, GLuint index, GLenum pname, GLint values);
inline void (*glGetActiveUniform)(GLuint program, GLuint index, GLsizei bufSize, GLsizei length, GLint size, GLenum type, GLchar name);
inline void (*glGetActiveUniformBlockName)(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei length, GLchar uniformBlockName);
inline void (*glGetActiveUniformBlockiv)(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint params);
inline void (*glGetActiveUniformName)(GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei length, GLchar uniformName);
inline void (*glGetActiveUniformsiv)(GLuint program, GLsizei uniformCount, GLuint uniformIndices, GLenum pname, GLint params);
inline void (*glGetAttachedShaders)(GLuint program, GLsizei maxCount, GLsizei count, GLuint shaders);
inline GLint(*glGetAttribLocation)(GLuint program, GLchar name);
inline void (*glGetBooleani_v)(GLenum target, GLuint index, GLboolean data);
inline void (*glGetBooleanv)(GLenum pname, GLboolean data);
inline void (*glGetBufferParameteri64v)(GLenum target, GLenum pname, GLint64 params);
inline void (*glGetBufferParameteriv)(GLenum target, GLenum pname, GLint params);
inline void (*glGetBufferParameterui64vNV)(GLenum target, GLenum pname, GLuint64EXT params);
inline void (*glGetBufferPointerv)(GLenum target, GLenum pname, void ** params);
inline void (*glGetBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, void * data);
inline GLuint(*glGetCommandHeaderNV)(GLenum tokenID, GLuint size);
inline void (*glGetCompressedTexImage)(GLenum target, GLint level, void * img);
inline void (*glGetCompressedTextureImage)(GLuint texture, GLint level, GLsizei bufSize, void * pixels);
inline void (*glGetCompressedTextureSubImage)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei bufSize, void * pixels);
inline GLuint(*glGetDebugMessageLog)(GLuint count, GLsizei bufSize, GLenum sources, GLenum types, GLuint ids, GLenum severities, GLsizei lengths, GLchar messageLog);
inline GLuint(*glGetDebugMessageLogKHR)(GLuint count, GLsizei bufSize, GLenum sources, GLenum types, GLuint ids, GLenum severities, GLsizei lengths, GLchar messageLog);
inline void (*glGetDoublei_v)(GLenum target, GLuint index, GLdouble data);
inline void (*glGetDoublev)(GLenum pname, GLdouble data);
inline GLenum(*glGetError)();
inline void (*glGetFloati_v)(GLenum target, GLuint index, GLfloat data);
inline void (*glGetFloatv)(GLenum pname, GLfloat data);
inline GLint(*glGetFragDataIndex)(GLuint program, GLchar name);
inline GLint(*glGetFragDataLocation)(GLuint program, GLchar name);
inline void (*glGetFramebufferAttachmentParameteriv)(GLenum target, GLenum attachment, GLenum pname, GLint params);
inline void (*glGetFramebufferParameteriv)(GLenum target, GLenum pname, GLint params);
inline GLenum(*glGetGraphicsResetStatus)();
inline GLenum(*glGetGraphicsResetStatusARB)();
inline GLuint64(*glGetImageHandleARB)(GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum format);
inline void (*glGetInteger64i_v)(GLenum target, GLuint index, GLint64 data);
inline void (*glGetInteger64v)(GLenum pname, GLint64 data);
inline void (*glGetIntegeri_v)(GLenum target, GLuint index, GLint data);
inline void (*glGetIntegerui64i_vNV)(GLenum value, GLuint index, GLuint64EXT result);
inline void (*glGetIntegerui64vNV)(GLenum value, GLuint64EXT result);
inline void (*glGetIntegerv)(GLenum pname, GLint data);
inline void (*glGetInternalformati64v)(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint64 params);
inline void (*glGetInternalformativ)(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint params);
inline void (*glGetMultisamplefv)(GLenum pname, GLuint index, GLfloat val);
inline void (*glGetNamedBufferParameteri64v)(GLuint buffer, GLenum pname, GLint64 params);
inline void (*glGetNamedBufferParameteriv)(GLuint buffer, GLenum pname, GLint params);
inline void (*glGetNamedBufferParameterui64vNV)(GLuint buffer, GLenum pname, GLuint64EXT params);
inline void (*glGetNamedBufferPointerv)(GLuint buffer, GLenum pname, void ** params);
inline void (*glGetNamedBufferSubData)(GLuint buffer, GLintptr offset, GLsizeiptr size, void * data);
inline void (*glGetNamedFramebufferAttachmentParameteriv)(GLuint framebuffer, GLenum attachment, GLenum pname, GLint params);
inline void (*glGetNamedFramebufferParameteriv)(GLuint framebuffer, GLenum pname, GLint param);
inline void (*glGetNamedRenderbufferParameteriv)(GLuint renderbuffer, GLenum pname, GLint params);
inline void (*glGetObjectLabel)(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei length, GLchar label);
inline void (*glGetObjectLabelKHR)(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei length, GLchar label);
inline void (*glGetObjectPtrLabel)(const void * ptr, GLsizei bufSize, GLsizei length, GLchar label);
inline void (*glGetObjectPtrLabelKHR)(const void * ptr, GLsizei bufSize, GLsizei length, GLchar label);
inline void (*glGetPathColorGenfvNV)(GLenum color, GLenum pname, GLfloat value);
inline void (*glGetPathColorGenivNV)(GLenum color, GLenum pname, GLint value);
inline void (*glGetPathCommandsNV)(GLuint path, GLubyte commands);
inline void (*glGetPathCoordsNV)(GLuint path, GLfloat coords);
inline void (*glGetPathDashArrayNV)(GLuint path, GLfloat dashArray);
inline GLfloat(*glGetPathLengthNV)(GLuint path, GLsizei startSegment, GLsizei numSegments);
inline void (*glGetPathMetricRangeNV)(GLbitfield metricQueryMask, GLuint firstPathName, GLsizei numPaths, GLsizei stride, GLfloat metrics);
inline void (*glGetPathMetricsNV)(GLbitfield metricQueryMask, GLsizei numPaths, GLenum pathNameType, const void * paths, GLuint pathBase, GLsizei stride, GLfloat metrics);
inline void (*glGetPathParameterfvNV)(GLuint path, GLenum pname, GLfloat value);
inline void (*glGetPathParameterivNV)(GLuint path, GLenum pname, GLint value);
inline void (*glGetPathSpacingNV)(GLenum pathListMode, GLsizei numPaths, GLenum pathNameType, const void * paths, GLuint pathBase, GLfloat advanceScale, GLfloat kerningScale, GLenum transformType, GLfloat returnedSpacing);
inline void (*glGetPathTexGenfvNV)(GLenum texCoordSet, GLenum pname, GLfloat value);
inline void (*glGetPathTexGenivNV)(GLenum texCoordSet, GLenum pname, GLint value);
inline void (*glGetPointerv)(GLenum pname, void ** params);
inline void (*glGetPointervKHR)(GLenum pname, void ** params);
inline void (*glGetProgramBinary)(GLuint program, GLsizei bufSize, GLsizei length, GLenum binaryFormat, void * binary);
inline void (*glGetProgramInfoLog)(GLuint program, GLsizei bufSize, GLsizei length, GLchar infoLog);
inline void (*glGetProgramInterfaceiv)(GLuint program, GLenum programInterface, GLenum pname, GLint params);
inline void (*glGetProgramPipelineInfoLog)(GLuint pipeline, GLsizei bufSize, GLsizei length, GLchar infoLog);
inline void (*glGetProgramPipelineiv)(GLuint pipeline, GLenum pname, GLint params);
inline GLuint(*glGetProgramResourceIndex)(GLuint program, GLenum programInterface, GLchar name);
inline GLint(*glGetProgramResourceLocation)(GLuint program, GLenum programInterface, GLchar name);
inline GLint(*glGetProgramResourceLocationIndex)(GLuint program, GLenum programInterface, GLchar name);
inline void (*glGetProgramResourceName)(GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei length, GLchar name);
inline void (*glGetProgramResourcefvNV)(GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, GLenum props, GLsizei bufSize, GLsizei length, GLfloat params);
inline void (*glGetProgramResourceiv)(GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, GLenum props, GLsizei bufSize, GLsizei length, GLint params);
inline void (*glGetProgramStageiv)(GLuint program, GLenum shadertype, GLenum pname, GLint values);
inline void (*glGetProgramiv)(GLuint program, GLenum pname, GLint params);
inline void (*glGetQueryBufferObjecti64v)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
inline void (*glGetQueryBufferObjectiv)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
inline void (*glGetQueryBufferObjectui64v)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
inline void (*glGetQueryBufferObjectuiv)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
inline void (*glGetQueryIndexediv)(GLenum target, GLuint index, GLenum pname, GLint params);
inline void (*glGetQueryObjecti64v)(GLuint id, GLenum pname, GLint64 params);
inline void (*glGetQueryObjectiv)(GLuint id, GLenum pname, GLint params);
inline void (*glGetQueryObjectui64v)(GLuint id, GLenum pname, GLuint64 params);
inline void (*glGetQueryObjectuiv)(GLuint id, GLenum pname, GLuint params);
inline void (*glGetQueryiv)(GLenum target, GLenum pname, GLint params);
inline void (*glGetRenderbufferParameteriv)(GLenum target, GLenum pname, GLint params);
inline void (*glGetSamplerParameterIiv)(GLuint sampler, GLenum pname, GLint params);
inline void (*glGetSamplerParameterIuiv)(GLuint sampler, GLenum pname, GLuint params);
inline void (*glGetSamplerParameterfv)(GLuint sampler, GLenum pname, GLfloat params);
inline void (*glGetSamplerParameteriv)(GLuint sampler, GLenum pname, GLint params);
inline void (*glGetShaderInfoLog)(GLuint shader, GLsizei bufSize, GLsizei length, GLchar infoLog);
inline void (*glGetShaderPrecisionFormat)(GLenum shadertype, GLenum precisiontype, GLint range, GLint precision);
inline void (*glGetShaderSource)(GLuint shader, GLsizei bufSize, GLsizei length, GLchar source);
inline void (*glGetShaderiv)(GLuint shader, GLenum pname, GLint params);
inline GLushort(*glGetStageIndexNV)(GLenum shadertype);
inline GLubyte(*glGetString)(GLenum name);
inline GLubyte(*glGetStringi)(GLenum name, GLuint index);
inline GLuint(*glGetSubroutineIndex)(GLuint program, GLenum shadertype, GLchar name);
inline GLint(*glGetSubroutineUniformLocation)(GLuint program, GLenum shadertype, GLchar name);
inline void (*glGetSynciv)(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei length, GLint values);
inline void (*glGetTexImage)(GLenum target, GLint level, GLenum format, GLenum type, void * pixels);
inline void (*glGetTexLevelParameterfv)(GLenum target, GLint level, GLenum pname, GLfloat params);
inline void (*glGetTexLevelParameteriv)(GLenum target, GLint level, GLenum pname, GLint params);
inline void (*glGetTexParameterIiv)(GLenum target, GLenum pname, GLint params);
inline void (*glGetTexParameterIuiv)(GLenum target, GLenum pname, GLuint params);
inline void (*glGetTexParameterfv)(GLenum target, GLenum pname, GLfloat params);
inline void (*glGetTexParameteriv)(GLenum target, GLenum pname, GLint params);
inline GLuint64(*glGetTextureHandleARB)(GLuint texture);
inline void (*glGetTextureImage)(GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void * pixels);
inline void (*glGetTextureLevelParameterfv)(GLuint texture, GLint level, GLenum pname, GLfloat params);
inline void (*glGetTextureLevelParameteriv)(GLuint texture, GLint level, GLenum pname, GLint params);
inline void (*glGetTextureParameterIiv)(GLuint texture, GLenum pname, GLint params);
inline void (*glGetTextureParameterIuiv)(GLuint texture, GLenum pname, GLuint params);
inline void (*glGetTextureParameterfv)(GLuint texture, GLenum pname, GLfloat params);
inline void (*glGetTextureParameteriv)(GLuint texture, GLenum pname, GLint params);
inline GLuint64(*glGetTextureSamplerHandleARB)(GLuint texture, GLuint sampler);
inline void (*glGetTextureSubImage)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLsizei bufSize, void * pixels);
inline void (*glGetTransformFeedbackVarying)(GLuint program, GLuint index, GLsizei bufSize, GLsizei length, GLsizei size, GLenum type, GLchar name);
inline void (*glGetTransformFeedbacki64_v)(GLuint xfb, GLenum pname, GLuint index, GLint64 param);
inline void (*glGetTransformFeedbacki_v)(GLuint xfb, GLenum pname, GLuint index, GLint param);
inline void (*glGetTransformFeedbackiv)(GLuint xfb, GLenum pname, GLint param);
inline GLuint(*glGetUniformBlockIndex)(GLuint program, GLchar uniformBlockName);
inline void (*glGetUniformIndices)(GLuint program, GLsizei uniformCount, GLchar uniformNames, GLuint uniformIndices);
inline GLint(*glGetUniformLocation)(GLuint program, GLchar name);
inline void (*glGetUniformSubroutineuiv)(GLenum shadertype, GLint location, GLuint params);
inline void (*glGetUniformdv)(GLuint program, GLint location, GLdouble params);
inline void (*glGetUniformfv)(GLuint program, GLint location, GLfloat params);
inline void (*glGetUniformiv)(GLuint program, GLint location, GLint params);
inline void (*glGetUniformui64vNV)(GLuint program, GLint location, GLuint64EXT params);
inline void (*glGetUniformuiv)(GLuint program, GLint location, GLuint params);
inline void (*glGetVertexArrayIndexed64iv)(GLuint vaobj, GLuint index, GLenum pname, GLint64 param);
inline void (*glGetVertexArrayIndexediv)(GLuint vaobj, GLuint index, GLenum pname, GLint param);
inline void (*glGetVertexArrayiv)(GLuint vaobj, GLenum pname, GLint param);
inline void (*glGetVertexAttribIiv)(GLuint index, GLenum pname, GLint params);
inline void (*glGetVertexAttribIuiv)(GLuint index, GLenum pname, GLuint params);
inline void (*glGetVertexAttribLdv)(GLuint index, GLenum pname, GLdouble params);
inline void (*glGetVertexAttribLui64vARB)(GLuint index, GLenum pname, GLuint64EXT params);
inline void (*glGetVertexAttribPointerv)(GLuint index, GLenum pname, void ** pointer);
inline void (*glGetVertexAttribdv)(GLuint index, GLenum pname, GLdouble params);
inline void (*glGetVertexAttribfv)(GLuint index, GLenum pname, GLfloat params);
inline void (*glGetVertexAttribiv)(GLuint index, GLenum pname, GLint params);
inline void (*glGetnColorTableARB)(GLenum target, GLenum format, GLenum type, GLsizei bufSize, void * table);
inline void (*glGetnCompressedTexImage)(GLenum target, GLint lod, GLsizei bufSize, void * pixels);
inline void (*glGetnCompressedTexImageARB)(GLenum target, GLint lod, GLsizei bufSize, void * img);
inline void (*glGetnConvolutionFilterARB)(GLenum target, GLenum format, GLenum type, GLsizei bufSize, void * image);
inline void (*glGetnHistogramARB)(GLenum target, GLboolean reset, GLenum format, GLenum type, GLsizei bufSize, void * values);
inline void (*glGetnMapdvARB)(GLenum target, GLenum query, GLsizei bufSize, GLdouble v);
inline void (*glGetnMapfvARB)(GLenum target, GLenum query, GLsizei bufSize, GLfloat v);
inline void (*glGetnMapivARB)(GLenum target, GLenum query, GLsizei bufSize, GLint v);
inline void (*glGetnMinmaxARB)(GLenum target, GLboolean reset, GLenum format, GLenum type, GLsizei bufSize, void * values);
inline void (*glGetnPixelMapfvARB)(GLenum map, GLsizei bufSize, GLfloat values);
inline void (*glGetnPixelMapuivARB)(GLenum map, GLsizei bufSize, GLuint values);
inline void (*glGetnPixelMapusvARB)(GLenum map, GLsizei bufSize, GLushort values);
inline void (*glGetnPolygonStippleARB)(GLsizei bufSize, GLubyte pattern);
inline void (*glGetnSeparableFilterARB)(GLenum target, GLenum format, GLenum type, GLsizei rowBufSize, void * row, GLsizei columnBufSize, void * column, void * span);
inline void (*glGetnTexImage)(GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, void * pixels);
inline void (*glGetnTexImageARB)(GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, void * img);
inline void (*glGetnUniformdv)(GLuint program, GLint location, GLsizei bufSize, GLdouble params);
inline void (*glGetnUniformdvARB)(GLuint program, GLint location, GLsizei bufSize, GLdouble params);
inline void (*glGetnUniformfv)(GLuint program, GLint location, GLsizei bufSize, GLfloat params);
inline void (*glGetnUniformfvARB)(GLuint program, GLint location, GLsizei bufSize, GLfloat params);
inline void (*glGetnUniformiv)(GLuint program, GLint location, GLsizei bufSize, GLint params);
inline void (*glGetnUniformivARB)(GLuint program, GLint location, GLsizei bufSize, GLint params);
inline void (*glGetnUniformuiv)(GLuint program, GLint location, GLsizei bufSize, GLuint params);
inline void (*glGetnUniformuivARB)(GLuint program, GLint location, GLsizei bufSize, GLuint params);
inline void (*glHint)(GLenum target, GLenum mode);
inline void (*glIndexFormatNV)(GLenum type, GLsizei stride);
inline void (*glInterpolatePathsNV)(GLuint resultPath, GLuint pathA, GLuint pathB, GLfloat weight);
inline void (*glInvalidateBufferData)(GLuint buffer);
inline void (*glInvalidateBufferSubData)(GLuint buffer, GLintptr offset, GLsizeiptr length);
inline void (*glInvalidateFramebuffer)(GLenum target, GLsizei numAttachments, GLenum attachments);
inline void (*glInvalidateNamedFramebufferData)(GLuint framebuffer, GLsizei numAttachments, GLenum attachments);
inline void (*glInvalidateNamedFramebufferSubData)(GLuint framebuffer, GLsizei numAttachments, GLenum attachments, GLint x, GLint y, GLsizei width, GLsizei height);
inline void (*glInvalidateSubFramebuffer)(GLenum target, GLsizei numAttachments, GLenum attachments, GLint x, GLint y, GLsizei width, GLsizei height);
inline void (*glInvalidateTexImage)(GLuint texture, GLint level);
inline void (*glInvalidateTexSubImage)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth);
inline GLboolean(*glIsBuffer)(GLuint buffer);
inline GLboolean(*glIsBufferResidentNV)(GLenum target);
inline GLboolean(*glIsCommandListNV)(GLuint list);
inline GLboolean(*glIsEnabled)(GLenum cap);
inline GLboolean(*glIsEnabledi)(GLenum target, GLuint index);
inline GLboolean(*glIsFramebuffer)(GLuint framebuffer);
inline GLboolean(*glIsImageHandleResidentARB)(GLuint64 handle);
inline GLboolean(*glIsNamedBufferResidentNV)(GLuint buffer);
inline GLboolean(*glIsPathNV)(GLuint path);
inline GLboolean(*glIsPointInFillPathNV)(GLuint path, GLuint mask, GLfloat x, GLfloat y);
inline GLboolean(*glIsPointInStrokePathNV)(GLuint path, GLfloat x, GLfloat y);
inline GLboolean(*glIsProgram)(GLuint program);
inline GLboolean(*glIsProgramPipeline)(GLuint pipeline);
inline GLboolean(*glIsQuery)(GLuint id);
inline GLboolean(*glIsRenderbuffer)(GLuint renderbuffer);
inline GLboolean(*glIsSampler)(GLuint sampler);
inline GLboolean(*glIsShader)(GLuint shader);
inline GLboolean(*glIsStateNV)(GLuint state);
inline GLboolean(*glIsSync)(GLsync sync);
inline GLboolean(*glIsTexture)(GLuint texture);
inline GLboolean(*glIsTextureHandleResidentARB)(GLuint64 handle);
inline GLboolean(*glIsTransformFeedback)(GLuint id);
inline GLboolean(*glIsVertexArray)(GLuint array);
inline void (*glLineWidth)(GLfloat width);
inline void (*glLinkProgram)(GLuint program);
inline void (*glListDrawCommandsStatesClientNV)(GLuint list, GLuint segment, const void ** indirects, GLsizei sizes, GLuint states, GLuint fbos, GLuint count);
inline void (*glLogicOp)(GLenum opcode);
inline void (*glMakeBufferNonResidentNV)(GLenum target);
inline void (*glMakeBufferResidentNV)(GLenum target, GLenum access);
inline void (*glMakeImageHandleNonResidentARB)(GLuint64 handle);
inline void (*glMakeImageHandleResidentARB)(GLuint64 handle, GLenum access);
inline void (*glMakeNamedBufferNonResidentNV)(GLuint buffer);
inline void (*glMakeNamedBufferResidentNV)(GLuint buffer, GLenum access);
inline void (*glMakeTextureHandleNonResidentARB)(GLuint64 handle);
inline void (*glMakeTextureHandleResidentARB)(GLuint64 handle);
inline void *(*glMapBuffer)(GLenum target, GLenum access);
inline void *(*glMapBufferRange)(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
inline void *(*glMapNamedBuffer)(GLuint buffer, GLenum access);
inline void *(*glMapNamedBufferRange)(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);
inline void (*glMatrixFrustumEXT)(GLenum mode, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
inline void (*glMatrixLoad3x2fNV)(GLenum matrixMode, GLfloat m);
inline void (*glMatrixLoad3x3fNV)(GLenum matrixMode, GLfloat m);
inline void (*glMatrixLoadIdentityEXT)(GLenum mode);
inline void (*glMatrixLoadTranspose3x3fNV)(GLenum matrixMode, GLfloat m);
inline void (*glMatrixLoadTransposedEXT)(GLenum mode, GLdouble m);
inline void (*glMatrixLoadTransposefEXT)(GLenum mode, GLfloat m);
inline void (*glMatrixLoaddEXT)(GLenum mode, GLdouble m);
inline void (*glMatrixLoadfEXT)(GLenum mode, GLfloat m);
inline void (*glMatrixMult3x2fNV)(GLenum matrixMode, GLfloat m);
inline void (*glMatrixMult3x3fNV)(GLenum matrixMode, GLfloat m);
inline void (*glMatrixMultTranspose3x3fNV)(GLenum matrixMode, GLfloat m);
inline void (*glMatrixMultTransposedEXT)(GLenum mode, GLdouble m);
inline void (*glMatrixMultTransposefEXT)(GLenum mode, GLfloat m);
inline void (*glMatrixMultdEXT)(GLenum mode, GLdouble m);
inline void (*glMatrixMultfEXT)(GLenum mode, GLfloat m);
inline void (*glMatrixOrthoEXT)(GLenum mode, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
inline void (*glMatrixPopEXT)(GLenum mode);
inline void (*glMatrixPushEXT)(GLenum mode);
inline void (*glMatrixRotatedEXT)(GLenum mode, GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
inline void (*glMatrixRotatefEXT)(GLenum mode, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
inline void (*glMatrixScaledEXT)(GLenum mode, GLdouble x, GLdouble y, GLdouble z);
inline void (*glMatrixScalefEXT)(GLenum mode, GLfloat x, GLfloat y, GLfloat z);
inline void (*glMatrixTranslatedEXT)(GLenum mode, GLdouble x, GLdouble y, GLdouble z);
inline void (*glMatrixTranslatefEXT)(GLenum mode, GLfloat x, GLfloat y, GLfloat z);
inline void (*glMemoryBarrier)(GLbitfield barriers);
inline void (*glMemoryBarrierByRegion)(GLbitfield barriers);
inline void (*glMinSampleShading)(GLfloat value);
inline void (*glMultiDrawArrays)(GLenum mode, GLint first, GLsizei count, GLsizei drawcount);
inline void (*glMultiDrawArraysIndirect)(GLenum mode, const void * indirect, GLsizei drawcount, GLsizei stride);
inline void (*glMultiDrawArraysIndirectBindlessCountNV)(GLenum mode, const void * indirect, GLsizei drawCount, GLsizei maxDrawCount, GLsizei stride, GLint vertexBufferCount);
inline void (*glMultiDrawArraysIndirectBindlessNV)(GLenum mode, const void * indirect, GLsizei drawCount, GLsizei stride, GLint vertexBufferCount);
inline void (*glMultiDrawArraysIndirectCount)(GLenum mode, const void * indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride);
inline void (*glMultiDrawArraysIndirectCountARB)(GLenum mode, const void * indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride);
inline void (*glMultiDrawElements)(GLenum mode, GLsizei count, GLenum type, const void *const* indices, GLsizei drawcount);
inline void (*glMultiDrawElementsBaseVertex)(GLenum mode, GLsizei count, GLenum type, const void *const* indices, GLsizei drawcount, GLint basevertex);
inline void (*glMultiDrawElementsIndirect)(GLenum mode, GLenum type, const void * indirect, GLsizei drawcount, GLsizei stride);
inline void (*glMultiDrawElementsIndirectBindlessCountNV)(GLenum mode, GLenum type, const void * indirect, GLsizei drawCount, GLsizei maxDrawCount, GLsizei stride, GLint vertexBufferCount);
inline void (*glMultiDrawElementsIndirectBindlessNV)(GLenum mode, GLenum type, const void * indirect, GLsizei drawCount, GLsizei stride, GLint vertexBufferCount);
inline void (*glMultiDrawElementsIndirectCount)(GLenum mode, GLenum type, const void * indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride);
inline void (*glMultiDrawElementsIndirectCountARB)(GLenum mode, GLenum type, const void * indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride);
inline void (*glNamedBufferData)(GLuint buffer, GLsizeiptr size, const void * data, GLenum usage);
inline void (*glNamedBufferStorage)(GLuint buffer, GLsizeiptr size, const void * data, GLbitfield flags);
inline void (*glNamedBufferSubData)(GLuint buffer, GLintptr offset, GLsizeiptr size, const void * data);
inline void (*glNamedFramebufferDrawBuffer)(GLuint framebuffer, GLenum buf);
inline void (*glNamedFramebufferDrawBuffers)(GLuint framebuffer, GLsizei n, GLenum bufs);
inline void (*glNamedFramebufferParameteri)(GLuint framebuffer, GLenum pname, GLint param);
inline void (*glNamedFramebufferReadBuffer)(GLuint framebuffer, GLenum src);
inline void (*glNamedFramebufferRenderbuffer)(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
inline void (*glNamedFramebufferTexture)(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);
inline void (*glNamedFramebufferTextureLayer)(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer);
inline void (*glNamedRenderbufferStorage)(GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height);
inline void (*glNamedRenderbufferStorageMultisample)(GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
inline void (*glNormalFormatNV)(GLenum type, GLsizei stride);
inline void (*glObjectLabel)(GLenum identifier, GLuint name, GLsizei length, GLchar label);
inline void (*glObjectLabelKHR)(GLenum identifier, GLuint name, GLsizei length, GLchar label);
inline void (*glObjectPtrLabel)(const void * ptr, GLsizei length, GLchar label);
inline void (*glObjectPtrLabelKHR)(const void * ptr, GLsizei length, GLchar label);
inline void (*glPatchParameterfv)(GLenum pname, GLfloat values);
inline void (*glPatchParameteri)(GLenum pname, GLint value);
inline void (*glPathColorGenNV)(GLenum color, GLenum genMode, GLenum colorFormat, GLfloat coeffs);
inline void (*glPathCommandsNV)(GLuint path, GLsizei numCommands, GLubyte commands, GLsizei numCoords, GLenum coordType, const void * coords);
inline void (*glPathCoordsNV)(GLuint path, GLsizei numCoords, GLenum coordType, const void * coords);
inline void (*glPathCoverDepthFuncNV)(GLenum func);
inline void (*glPathDashArrayNV)(GLuint path, GLsizei dashCount, GLfloat dashArray);
inline void (*glPathFogGenNV)(GLenum genMode);
inline GLenum(*glPathGlyphIndexArrayNV)(GLuint firstPathName, GLenum fontTarget, const void * fontName, GLbitfield fontStyle, GLuint firstGlyphIndex, GLsizei numGlyphs, GLuint pathParameterTemplate, GLfloat emScale);
inline GLenum(*glPathGlyphIndexRangeNV)(GLenum fontTarget, const void * fontName, GLbitfield fontStyle, GLuint pathParameterTemplate, GLfloat emScale, GLuint baseAndCount);
inline void (*glPathGlyphRangeNV)(GLuint firstPathName, GLenum fontTarget, const void * fontName, GLbitfield fontStyle, GLuint firstGlyph, GLsizei numGlyphs, GLenum handleMissingGlyphs, GLuint pathParameterTemplate, GLfloat emScale);
inline void (*glPathGlyphsNV)(GLuint firstPathName, GLenum fontTarget, const void * fontName, GLbitfield fontStyle, GLsizei numGlyphs, GLenum type, const void * charcodes, GLenum handleMissingGlyphs, GLuint pathParameterTemplate, GLfloat emScale);
inline GLenum(*glPathMemoryGlyphIndexArrayNV)(GLuint firstPathName, GLenum fontTarget, GLsizeiptr fontSize, const void * fontData, GLsizei faceIndex, GLuint firstGlyphIndex, GLsizei numGlyphs, GLuint pathParameterTemplate, GLfloat emScale);
inline void (*glPathParameterfNV)(GLuint path, GLenum pname, GLfloat value);
inline void (*glPathParameterfvNV)(GLuint path, GLenum pname, GLfloat value);
inline void (*glPathParameteriNV)(GLuint path, GLenum pname, GLint value);
inline void (*glPathParameterivNV)(GLuint path, GLenum pname, GLint value);
inline void (*glPathStencilDepthOffsetNV)(GLfloat factor, GLfloat units);
inline void (*glPathStencilFuncNV)(GLenum func, GLint ref, GLuint mask);
inline void (*glPathStringNV)(GLuint path, GLenum format, GLsizei length, const void * pathString);
inline void (*glPathSubCommandsNV)(GLuint path, GLsizei commandStart, GLsizei commandsToDelete, GLsizei numCommands, GLubyte commands, GLsizei numCoords, GLenum coordType, const void * coords);
inline void (*glPathSubCoordsNV)(GLuint path, GLsizei coordStart, GLsizei numCoords, GLenum coordType, const void * coords);
inline void (*glPathTexGenNV)(GLenum texCoordSet, GLenum genMode, GLint components, GLfloat coeffs);
inline void (*glPauseTransformFeedback)();
inline void (*glPixelStoref)(GLenum pname, GLfloat param);
inline void (*glPixelStorei)(GLenum pname, GLint param);
inline GLboolean(*glPointAlongPathNV)(GLuint path, GLsizei startSegment, GLsizei numSegments, GLfloat distance, GLfloat x, GLfloat y, GLfloat tangentX, GLfloat tangentY);
inline void (*glPointParameterf)(GLenum pname, GLfloat param);
inline void (*glPointParameterfv)(GLenum pname, GLfloat params);
inline void (*glPointParameteri)(GLenum pname, GLint param);
inline void (*glPointParameteriv)(GLenum pname, GLint params);
inline void (*glPointSize)(GLfloat size);
inline void (*glPolygonMode)(GLenum face, GLenum mode);
inline void (*glPolygonOffset)(GLfloat factor, GLfloat units);
inline void (*glPolygonOffsetClamp)(GLfloat factor, GLfloat units, GLfloat clamp);
inline void (*glPopDebugGroup)();
inline void (*glPopDebugGroupKHR)();
inline void (*glPrimitiveRestartIndex)(GLuint index);
inline void (*glProgramBinary)(GLuint program, GLenum binaryFormat, const void * binary, GLsizei length);
inline void (*glProgramParameteri)(GLuint program, GLenum pname, GLint value);
inline void (*glProgramPathFragmentInputGenNV)(GLuint program, GLint location, GLenum genMode, GLint components, GLfloat coeffs);
inline void (*glProgramUniform1d)(GLuint program, GLint location, GLdouble v0);
inline void (*glProgramUniform1dv)(GLuint program, GLint location, GLsizei count, GLdouble value);
inline void (*glProgramUniform1f)(GLuint program, GLint location, GLfloat v0);
inline void (*glProgramUniform1fv)(GLuint program, GLint location, GLsizei count, GLfloat value);
inline void (*glProgramUniform1i)(GLuint program, GLint location, GLint v0);
inline void (*glProgramUniform1iv)(GLuint program, GLint location, GLsizei count, GLint value);
inline void (*glProgramUniform1ui)(GLuint program, GLint location, GLuint v0);
inline void (*glProgramUniform1uiv)(GLuint program, GLint location, GLsizei count, GLuint value);
inline void (*glProgramUniform2d)(GLuint program, GLint location, GLdouble v0, GLdouble v1);
inline void (*glProgramUniform2dv)(GLuint program, GLint location, GLsizei count, GLdouble value);
inline void (*glProgramUniform2f)(GLuint program, GLint location, GLfloat v0, GLfloat v1);
inline void (*glProgramUniform2fv)(GLuint program, GLint location, GLsizei count, GLfloat value);
inline void (*glProgramUniform2i)(GLuint program, GLint location, GLint v0, GLint v1);
inline void (*glProgramUniform2iv)(GLuint program, GLint location, GLsizei count, GLint value);
inline void (*glProgramUniform2ui)(GLuint program, GLint location, GLuint v0, GLuint v1);
inline void (*glProgramUniform2uiv)(GLuint program, GLint location, GLsizei count, GLuint value);
inline void (*glProgramUniform3d)(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2);
inline void (*glProgramUniform3dv)(GLuint program, GLint location, GLsizei count, GLdouble value);
inline void (*glProgramUniform3f)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
inline void (*glProgramUniform3fv)(GLuint program, GLint location, GLsizei count, GLfloat value);
inline void (*glProgramUniform3i)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2);
inline void (*glProgramUniform3iv)(GLuint program, GLint location, GLsizei count, GLint value);
inline void (*glProgramUniform3ui)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2);
inline void (*glProgramUniform3uiv)(GLuint program, GLint location, GLsizei count, GLuint value);
inline void (*glProgramUniform4d)(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3);
inline void (*glProgramUniform4dv)(GLuint program, GLint location, GLsizei count, GLdouble value);
inline void (*glProgramUniform4f)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
inline void (*glProgramUniform4fv)(GLuint program, GLint location, GLsizei count, GLfloat value);
inline void (*glProgramUniform4i)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
inline void (*glProgramUniform4iv)(GLuint program, GLint location, GLsizei count, GLint value);
inline void (*glProgramUniform4ui)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
inline void (*glProgramUniform4uiv)(GLuint program, GLint location, GLsizei count, GLuint value);
inline void (*glProgramUniformHandleui64ARB)(GLuint program, GLint location, GLuint64 value);
inline void (*glProgramUniformHandleui64vARB)(GLuint program, GLint location, GLsizei count, GLuint64 values);
inline void (*glProgramUniformMatrix2dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, GLdouble value);
inline void (*glProgramUniformMatrix2fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, GLfloat value);
inline void (*glProgramUniformMatrix2x3dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, GLdouble value);
inline void (*glProgramUniformMatrix2x3fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, GLfloat value);
inline void (*glProgramUniformMatrix2x4dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, GLdouble value);
inline void (*glProgramUniformMatrix2x4fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, GLfloat value);
inline void (*glProgramUniformMatrix3dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, GLdouble value);
inline void (*glProgramUniformMatrix3fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, GLfloat value);
inline void (*glProgramUniformMatrix3x2dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, GLdouble value);
inline void (*glProgramUniformMatrix3x2fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, GLfloat value);
inline void (*glProgramUniformMatrix3x4dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, GLdouble value);
inline void (*glProgramUniformMatrix3x4fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, GLfloat value);
inline void (*glProgramUniformMatrix4dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, GLdouble value);
inline void (*glProgramUniformMatrix4fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, GLfloat value);
inline void (*glProgramUniformMatrix4x2dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, GLdouble value);
inline void (*glProgramUniformMatrix4x2fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, GLfloat value);
inline void (*glProgramUniformMatrix4x3dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, GLdouble value);
inline void (*glProgramUniformMatrix4x3fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, GLfloat value);
inline void (*glProgramUniformui64NV)(GLuint program, GLint location, GLuint64EXT value);
inline void (*glProgramUniformui64vNV)(GLuint program, GLint location, GLsizei count, GLuint64EXT value);
inline void (*glProvokingVertex)(GLenum mode);
inline void (*glPushDebugGroup)(GLenum source, GLuint id, GLsizei length, GLchar message);
inline void (*glPushDebugGroupKHR)(GLenum source, GLuint id, GLsizei length, GLchar message);
inline void (*glQueryCounter)(GLuint id, GLenum target);
inline void (*glReadBuffer)(GLenum src);
inline void (*glReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void * pixels);
inline void (*glReadnPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void * data);
inline void (*glReadnPixelsARB)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void * data);
inline void (*glReleaseShaderCompiler)();
inline void (*glRenderbufferStorage)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
inline void (*glRenderbufferStorageMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
inline void (*glResumeTransformFeedback)();
inline void (*glSampleCoverage)(GLfloat value, GLboolean invert);
inline void (*glSampleMaski)(GLuint maskNumber, GLbitfield mask);
inline void (*glSamplerParameterIiv)(GLuint sampler, GLenum pname, GLint param);
inline void (*glSamplerParameterIuiv)(GLuint sampler, GLenum pname, GLuint param);
inline void (*glSamplerParameterf)(GLuint sampler, GLenum pname, GLfloat param);
inline void (*glSamplerParameterfv)(GLuint sampler, GLenum pname, GLfloat param);
inline void (*glSamplerParameteri)(GLuint sampler, GLenum pname, GLint param);
inline void (*glSamplerParameteriv)(GLuint sampler, GLenum pname, GLint param);
inline void (*glScissor)(GLint x, GLint y, GLsizei width, GLsizei height);
inline void (*glScissorArrayv)(GLuint first, GLsizei count, GLint v);
inline void (*glScissorIndexed)(GLuint index, GLint left, GLint bottom, GLsizei width, GLsizei height);
inline void (*glScissorIndexedv)(GLuint index, GLint v);
inline void (*glSecondaryColorFormatNV)(GLint size, GLenum type, GLsizei stride);
inline void (*glShaderBinary)(GLsizei count, GLuint shaders, GLenum binaryformat, const void * binary, GLsizei length);
inline void (*glShaderSource)(GLuint shader, GLsizei count, GLchar string, GLint length);
inline void (*glShaderStorageBlockBinding)(GLuint program, GLuint storageBlockIndex, GLuint storageBlockBinding);
inline void (*glSpecializeShader)(GLuint shader, GLchar pEntryPoint, GLuint numSpecializationConstants, GLuint pConstantIndex, GLuint pConstantValue);
inline void (*glSpecializeShaderARB)(GLuint shader, GLchar pEntryPoint, GLuint numSpecializationConstants, GLuint pConstantIndex, GLuint pConstantValue);
inline void (*glStateCaptureNV)(GLuint state, GLenum mode);
inline void (*glStencilFillPathInstancedNV)(GLsizei numPaths, GLenum pathNameType, const void * paths, GLuint pathBase, GLenum fillMode, GLuint mask, GLenum transformType, GLfloat transformValues);
inline void (*glStencilFillPathNV)(GLuint path, GLenum fillMode, GLuint mask);
inline void (*glStencilFunc)(GLenum func, GLint ref, GLuint mask);
inline void (*glStencilFuncSeparate)(GLenum face, GLenum func, GLint ref, GLuint mask);
inline void (*glStencilMask)(GLuint mask);
inline void (*glStencilMaskSeparate)(GLenum face, GLuint mask);
inline void (*glStencilOp)(GLenum fail, GLenum zfail, GLenum zpass);
inline void (*glStencilOpSeparate)(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
inline void (*glStencilStrokePathInstancedNV)(GLsizei numPaths, GLenum pathNameType, const void * paths, GLuint pathBase, GLint reference, GLuint mask, GLenum transformType, GLfloat transformValues);
inline void (*glStencilStrokePathNV)(GLuint path, GLint reference, GLuint mask);
inline void (*glStencilThenCoverFillPathInstancedNV)(GLsizei numPaths, GLenum pathNameType, const void * paths, GLuint pathBase, GLenum fillMode, GLuint mask, GLenum coverMode, GLenum transformType, GLfloat transformValues);
inline void (*glStencilThenCoverFillPathNV)(GLuint path, GLenum fillMode, GLuint mask, GLenum coverMode);
inline void (*glStencilThenCoverStrokePathInstancedNV)(GLsizei numPaths, GLenum pathNameType, const void * paths, GLuint pathBase, GLint reference, GLuint mask, GLenum coverMode, GLenum transformType, GLfloat transformValues);
inline void (*glStencilThenCoverStrokePathNV)(GLuint path, GLint reference, GLuint mask, GLenum coverMode);
inline void (*glTexBuffer)(GLenum target, GLenum internalformat, GLuint buffer);
inline void (*glTexBufferRange)(GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);
inline void (*glTexCoordFormatNV)(GLint size, GLenum type, GLsizei stride);
inline void (*glTexImage1D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void * pixels);
inline void (*glTexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void * pixels);
inline void (*glTexImage2DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
inline void (*glTexImage3D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void * pixels);
inline void (*glTexImage3DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
inline void (*glTexParameterIiv)(GLenum target, GLenum pname, GLint params);
inline void (*glTexParameterIuiv)(GLenum target, GLenum pname, GLuint params);
inline void (*glTexParameterf)(GLenum target, GLenum pname, GLfloat param);
inline void (*glTexParameterfv)(GLenum target, GLenum pname, GLfloat params);
inline void (*glTexParameteri)(GLenum target, GLenum pname, GLint param);
inline void (*glTexParameteriv)(GLenum target, GLenum pname, GLint params);
inline void (*glTexStorage1D)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width);
inline void (*glTexStorage2D)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
inline void (*glTexStorage2DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
inline void (*glTexStorage3D)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
inline void (*glTexStorage3DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
inline void (*glTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void * pixels);
inline void (*glTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void * pixels);
inline void (*glTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void * pixels);
inline void (*glTextureBarrier)();
inline void (*glTextureBuffer)(GLuint texture, GLenum internalformat, GLuint buffer);
inline void (*glTextureBufferRange)(GLuint texture, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);
inline void (*glTextureParameterIiv)(GLuint texture, GLenum pname, GLint params);
inline void (*glTextureParameterIuiv)(GLuint texture, GLenum pname, GLuint params);
inline void (*glTextureParameterf)(GLuint texture, GLenum pname, GLfloat param);
inline void (*glTextureParameterfv)(GLuint texture, GLenum pname, GLfloat param);
inline void (*glTextureParameteri)(GLuint texture, GLenum pname, GLint param);
inline void (*glTextureParameteriv)(GLuint texture, GLenum pname, GLint param);
inline void (*glTextureStorage1D)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width);
inline void (*glTextureStorage2D)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
inline void (*glTextureStorage2DMultisample)(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
inline void (*glTextureStorage3D)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
inline void (*glTextureStorage3DMultisample)(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
inline void (*glTextureSubImage1D)(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void * pixels);
inline void (*glTextureSubImage2D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void * pixels);
inline void (*glTextureSubImage3D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void * pixels);
inline void (*glTextureView)(GLuint texture, GLenum target, GLuint origtexture, GLenum internalformat, GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers);
inline void (*glTransformFeedbackBufferBase)(GLuint xfb, GLuint index, GLuint buffer);
inline void (*glTransformFeedbackBufferRange)(GLuint xfb, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
inline void (*glTransformFeedbackVaryings)(GLuint program, GLsizei count, GLchar varyings, GLenum bufferMode);
inline void (*glTransformPathNV)(GLuint resultPath, GLuint srcPath, GLenum transformType, GLfloat transformValues);
inline void (*glUniform1d)(GLint location, GLdouble x);
inline void (*glUniform1dv)(GLint location, GLsizei count, GLdouble value);
inline void (*glUniform1f)(GLint location, GLfloat v0);
inline void (*glUniform1fv)(GLint location, GLsizei count, GLfloat value);
inline void (*glUniform1i)(GLint location, GLint v0);
inline void (*glUniform1iv)(GLint location, GLsizei count, GLint value);
inline void (*glUniform1ui)(GLint location, GLuint v0);
inline void (*glUniform1uiv)(GLint location, GLsizei count, GLuint value);
inline void (*glUniform2d)(GLint location, GLdouble x, GLdouble y);
inline void (*glUniform2dv)(GLint location, GLsizei count, GLdouble value);
inline void (*glUniform2f)(GLint location, GLfloat v0, GLfloat v1);
inline void (*glUniform2fv)(GLint location, GLsizei count, GLfloat value);
inline void (*glUniform2i)(GLint location, GLint v0, GLint v1);
inline void (*glUniform2iv)(GLint location, GLsizei count, GLint value);
inline void (*glUniform2ui)(GLint location, GLuint v0, GLuint v1);
inline void (*glUniform2uiv)(GLint location, GLsizei count, GLuint value);
inline void (*glUniform3d)(GLint location, GLdouble x, GLdouble y, GLdouble z);
inline void (*glUniform3dv)(GLint location, GLsizei count, GLdouble value);
inline void (*glUniform3f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
inline void (*glUniform3fv)(GLint location, GLsizei count, GLfloat value);
inline void (*glUniform3i)(GLint location, GLint v0, GLint v1, GLint v2);
inline void (*glUniform3iv)(GLint location, GLsizei count, GLint value);
inline void (*glUniform3ui)(GLint location, GLuint v0, GLuint v1, GLuint v2);
inline void (*glUniform3uiv)(GLint location, GLsizei count, GLuint value);
inline void (*glUniform4d)(GLint location, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
inline void (*glUniform4dv)(GLint location, GLsizei count, GLdouble value);
inline void (*glUniform4f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
inline void (*glUniform4fv)(GLint location, GLsizei count, GLfloat value);
inline void (*glUniform4i)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
inline void (*glUniform4iv)(GLint location, GLsizei count, GLint value);
inline void (*glUniform4ui)(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
inline void (*glUniform4uiv)(GLint location, GLsizei count, GLuint value);
inline void (*glUniformBlockBinding)(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
inline void (*glUniformHandleui64ARB)(GLint location, GLuint64 value);
inline void (*glUniformHandleui64vARB)(GLint location, GLsizei count, GLuint64 value);
inline void (*glUniformMatrix2dv)(GLint location, GLsizei count, GLboolean transpose, GLdouble value);
inline void (*glUniformMatrix2fv)(GLint location, GLsizei count, GLboolean transpose, GLfloat value);
inline void (*glUniformMatrix2x3dv)(GLint location, GLsizei count, GLboolean transpose, GLdouble value);
inline void (*glUniformMatrix2x3fv)(GLint location, GLsizei count, GLboolean transpose, GLfloat value);
inline void (*glUniformMatrix2x4dv)(GLint location, GLsizei count, GLboolean transpose, GLdouble value);
inline void (*glUniformMatrix2x4fv)(GLint location, GLsizei count, GLboolean transpose, GLfloat value);
inline void (*glUniformMatrix3dv)(GLint location, GLsizei count, GLboolean transpose, GLdouble value);
inline void (*glUniformMatrix3fv)(GLint location, GLsizei count, GLboolean transpose, GLfloat value);
inline void (*glUniformMatrix3x2dv)(GLint location, GLsizei count, GLboolean transpose, GLdouble value);
inline void (*glUniformMatrix3x2fv)(GLint location, GLsizei count, GLboolean transpose, GLfloat value);
inline void (*glUniformMatrix3x4dv)(GLint location, GLsizei count, GLboolean transpose, GLdouble value);
inline void (*glUniformMatrix3x4fv)(GLint location, GLsizei count, GLboolean transpose, GLfloat value);
inline void (*glUniformMatrix4dv)(GLint location, GLsizei count, GLboolean transpose, GLdouble value);
inline void (*glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, GLfloat value);
inline void (*glUniformMatrix4x2dv)(GLint location, GLsizei count, GLboolean transpose, GLdouble value);
inline void (*glUniformMatrix4x2fv)(GLint location, GLsizei count, GLboolean transpose, GLfloat value);
inline void (*glUniformMatrix4x3dv)(GLint location, GLsizei count, GLboolean transpose, GLdouble value);
inline void (*glUniformMatrix4x3fv)(GLint location, GLsizei count, GLboolean transpose, GLfloat value);
inline void (*glUniformSubroutinesuiv)(GLenum shadertype, GLsizei count, GLuint indices);
inline void (*glUniformui64NV)(GLint location, GLuint64EXT value);
inline void (*glUniformui64vNV)(GLint location, GLsizei count, GLuint64EXT value);
inline GLboolean(*glUnmapBuffer)(GLenum target);
inline GLboolean(*glUnmapNamedBuffer)(GLuint buffer);
inline void (*glUseProgram)(GLuint program);
inline void (*glUseProgramStages)(GLuint pipeline, GLbitfield stages, GLuint program);
inline void (*glValidateProgram)(GLuint program);
inline void (*glValidateProgramPipeline)(GLuint pipeline);
inline void (*glVertexArrayAttribBinding)(GLuint vaobj, GLuint attribindex, GLuint bindingindex);
inline void (*glVertexArrayAttribFormat)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
inline void (*glVertexArrayAttribIFormat)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
inline void (*glVertexArrayAttribLFormat)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
inline void (*glVertexArrayBindingDivisor)(GLuint vaobj, GLuint bindingindex, GLuint divisor);
inline void (*glVertexArrayElementBuffer)(GLuint vaobj, GLuint buffer);
inline void (*glVertexArrayVertexBuffer)(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
inline void (*glVertexArrayVertexBuffers)(GLuint vaobj, GLuint first, GLsizei count, GLuint buffers, GLintptr offsets, GLsizei strides);
inline void (*glVertexAttrib1d)(GLuint index, GLdouble x);
inline void (*glVertexAttrib1dv)(GLuint index, GLdouble v);
inline void (*glVertexAttrib1f)(GLuint index, GLfloat x);
inline void (*glVertexAttrib1fv)(GLuint index, GLfloat v);
inline void (*glVertexAttrib1s)(GLuint index, GLshort x);
inline void (*glVertexAttrib1sv)(GLuint index, GLshort v);
inline void (*glVertexAttrib2d)(GLuint index, GLdouble x, GLdouble y);
inline void (*glVertexAttrib2dv)(GLuint index, GLdouble v);
inline void (*glVertexAttrib2f)(GLuint index, GLfloat x, GLfloat y);
inline void (*glVertexAttrib2fv)(GLuint index, GLfloat v);
inline void (*glVertexAttrib2s)(GLuint index, GLshort x, GLshort y);
inline void (*glVertexAttrib2sv)(GLuint index, GLshort v);
inline void (*glVertexAttrib3d)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
inline void (*glVertexAttrib3dv)(GLuint index, GLdouble v);
inline void (*glVertexAttrib3f)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
inline void (*glVertexAttrib3fv)(GLuint index, GLfloat v);
inline void (*glVertexAttrib3s)(GLuint index, GLshort x, GLshort y, GLshort z);
inline void (*glVertexAttrib3sv)(GLuint index, GLshort v);
inline void (*glVertexAttrib4Nbv)(GLuint index, GLbyte v);
inline void (*glVertexAttrib4Niv)(GLuint index, GLint v);
inline void (*glVertexAttrib4Nsv)(GLuint index, GLshort v);
inline void (*glVertexAttrib4Nub)(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
inline void (*glVertexAttrib4Nubv)(GLuint index, GLubyte v);
inline void (*glVertexAttrib4Nuiv)(GLuint index, GLuint v);
inline void (*glVertexAttrib4Nusv)(GLuint index, GLushort v);
inline void (*glVertexAttrib4bv)(GLuint index, GLbyte v);
inline void (*glVertexAttrib4d)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
inline void (*glVertexAttrib4dv)(GLuint index, GLdouble v);
inline void (*glVertexAttrib4f)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
inline void (*glVertexAttrib4fv)(GLuint index, GLfloat v);
inline void (*glVertexAttrib4iv)(GLuint index, GLint v);
inline void (*glVertexAttrib4s)(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
inline void (*glVertexAttrib4sv)(GLuint index, GLshort v);
inline void (*glVertexAttrib4ubv)(GLuint index, GLubyte v);
inline void (*glVertexAttrib4uiv)(GLuint index, GLuint v);
inline void (*glVertexAttrib4usv)(GLuint index, GLushort v);
inline void (*glVertexAttribBinding)(GLuint attribindex, GLuint bindingindex);
inline void (*glVertexAttribDivisor)(GLuint index, GLuint divisor);
inline void (*glVertexAttribFormat)(GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
inline void (*glVertexAttribFormatNV)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride);
inline void (*glVertexAttribI1i)(GLuint index, GLint x);
inline void (*glVertexAttribI1iv)(GLuint index, GLint v);
inline void (*glVertexAttribI1ui)(GLuint index, GLuint x);
inline void (*glVertexAttribI1uiv)(GLuint index, GLuint v);
inline void (*glVertexAttribI2i)(GLuint index, GLint x, GLint y);
inline void (*glVertexAttribI2iv)(GLuint index, GLint v);
inline void (*glVertexAttribI2ui)(GLuint index, GLuint x, GLuint y);
inline void (*glVertexAttribI2uiv)(GLuint index, GLuint v);
inline void (*glVertexAttribI3i)(GLuint index, GLint x, GLint y, GLint z);
inline void (*glVertexAttribI3iv)(GLuint index, GLint v);
inline void (*glVertexAttribI3ui)(GLuint index, GLuint x, GLuint y, GLuint z);
inline void (*glVertexAttribI3uiv)(GLuint index, GLuint v);
inline void (*glVertexAttribI4bv)(GLuint index, GLbyte v);
inline void (*glVertexAttribI4i)(GLuint index, GLint x, GLint y, GLint z, GLint w);
inline void (*glVertexAttribI4iv)(GLuint index, GLint v);
inline void (*glVertexAttribI4sv)(GLuint index, GLshort v);
inline void (*glVertexAttribI4ubv)(GLuint index, GLubyte v);
inline void (*glVertexAttribI4ui)(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
inline void (*glVertexAttribI4uiv)(GLuint index, GLuint v);
inline void (*glVertexAttribI4usv)(GLuint index, GLushort v);
inline void (*glVertexAttribIFormat)(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
inline void (*glVertexAttribIFormatNV)(GLuint index, GLint size, GLenum type, GLsizei stride);
inline void (*glVertexAttribIPointer)(GLuint index, GLint size, GLenum type, GLsizei stride, const void * pointer);
inline void (*glVertexAttribL1d)(GLuint index, GLdouble x);
inline void (*glVertexAttribL1dv)(GLuint index, GLdouble v);
inline void (*glVertexAttribL1ui64ARB)(GLuint index, GLuint64EXT x);
inline void (*glVertexAttribL1ui64vARB)(GLuint index, GLuint64EXT v);
inline void (*glVertexAttribL2d)(GLuint index, GLdouble x, GLdouble y);
inline void (*glVertexAttribL2dv)(GLuint index, GLdouble v);
inline void (*glVertexAttribL3d)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
inline void (*glVertexAttribL3dv)(GLuint index, GLdouble v);
inline void (*glVertexAttribL4d)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
inline void (*glVertexAttribL4dv)(GLuint index, GLdouble v);
inline void (*glVertexAttribLFormat)(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
inline void (*glVertexAttribLPointer)(GLuint index, GLint size, GLenum type, GLsizei stride, const void * pointer);
inline void (*glVertexAttribP1ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
inline void (*glVertexAttribP1uiv)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
inline void (*glVertexAttribP2ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
inline void (*glVertexAttribP2uiv)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
inline void (*glVertexAttribP3ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
inline void (*glVertexAttribP3uiv)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
inline void (*glVertexAttribP4ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
inline void (*glVertexAttribP4uiv)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
inline void (*glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void * pointer);
inline void (*glVertexBindingDivisor)(GLuint bindingindex, GLuint divisor);
inline void (*glVertexFormatNV)(GLint size, GLenum type, GLsizei stride);
inline void (*glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);
inline void (*glViewportArrayv)(GLuint first, GLsizei count, GLfloat v);
inline void (*glViewportIndexedf)(GLuint index, GLfloat x, GLfloat y, GLfloat w, GLfloat h);
inline void (*glViewportIndexedfv)(GLuint index, GLfloat v);
inline void (*glWaitSync)(GLsync sync, GLbitfield flags, GLuint64 timeout);
inline void (*glWeightPathsNV)(GLuint resultPath, GLsizei numPaths, GLuint paths, GLfloat weights);

void* get_func(const char* name)
{
    return nullptr;
}

void load(){
    glActiveShaderProgram = reinterpret_cast<decltype(glActiveShaderProgram)>(get_func("glActiveShaderProgram"));
    glActiveTexture = reinterpret_cast<decltype(glActiveTexture)>(get_func("glActiveTexture"));
    glAttachShader = reinterpret_cast<decltype(glAttachShader)>(get_func("glAttachShader"));
    glBeginConditionalRender = reinterpret_cast<decltype(glBeginConditionalRender)>(get_func("glBeginConditionalRender"));
    glBeginQuery = reinterpret_cast<decltype(glBeginQuery)>(get_func("glBeginQuery"));
    glBeginQueryIndexed = reinterpret_cast<decltype(glBeginQueryIndexed)>(get_func("glBeginQueryIndexed"));
    glBeginTransformFeedback = reinterpret_cast<decltype(glBeginTransformFeedback)>(get_func("glBeginTransformFeedback"));
    glBindAttribLocation = reinterpret_cast<decltype(glBindAttribLocation)>(get_func("glBindAttribLocation"));
    glBindBuffer = reinterpret_cast<decltype(glBindBuffer)>(get_func("glBindBuffer"));
    glBindBufferBase = reinterpret_cast<decltype(glBindBufferBase)>(get_func("glBindBufferBase"));
    glBindBufferRange = reinterpret_cast<decltype(glBindBufferRange)>(get_func("glBindBufferRange"));
    glBindBuffersBase = reinterpret_cast<decltype(glBindBuffersBase)>(get_func("glBindBuffersBase"));
    glBindBuffersRange = reinterpret_cast<decltype(glBindBuffersRange)>(get_func("glBindBuffersRange"));
    glBindFragDataLocation = reinterpret_cast<decltype(glBindFragDataLocation)>(get_func("glBindFragDataLocation"));
    glBindFragDataLocationIndexed = reinterpret_cast<decltype(glBindFragDataLocationIndexed)>(get_func("glBindFragDataLocationIndexed"));
    glBindFramebuffer = reinterpret_cast<decltype(glBindFramebuffer)>(get_func("glBindFramebuffer"));
    glBindImageTexture = reinterpret_cast<decltype(glBindImageTexture)>(get_func("glBindImageTexture"));
    glBindImageTextures = reinterpret_cast<decltype(glBindImageTextures)>(get_func("glBindImageTextures"));
    glBindProgramPipeline = reinterpret_cast<decltype(glBindProgramPipeline)>(get_func("glBindProgramPipeline"));
    glBindRenderbuffer = reinterpret_cast<decltype(glBindRenderbuffer)>(get_func("glBindRenderbuffer"));
    glBindSampler = reinterpret_cast<decltype(glBindSampler)>(get_func("glBindSampler"));
    glBindSamplers = reinterpret_cast<decltype(glBindSamplers)>(get_func("glBindSamplers"));
    glBindTexture = reinterpret_cast<decltype(glBindTexture)>(get_func("glBindTexture"));
    glBindTextureUnit = reinterpret_cast<decltype(glBindTextureUnit)>(get_func("glBindTextureUnit"));
    glBindTextures = reinterpret_cast<decltype(glBindTextures)>(get_func("glBindTextures"));
    glBindTransformFeedback = reinterpret_cast<decltype(glBindTransformFeedback)>(get_func("glBindTransformFeedback"));
    glBindVertexArray = reinterpret_cast<decltype(glBindVertexArray)>(get_func("glBindVertexArray"));
    glBindVertexBuffer = reinterpret_cast<decltype(glBindVertexBuffer)>(get_func("glBindVertexBuffer"));
    glBindVertexBuffers = reinterpret_cast<decltype(glBindVertexBuffers)>(get_func("glBindVertexBuffers"));
    glBlendColor = reinterpret_cast<decltype(glBlendColor)>(get_func("glBlendColor"));
    glBlendEquation = reinterpret_cast<decltype(glBlendEquation)>(get_func("glBlendEquation"));
    glBlendEquationSeparate = reinterpret_cast<decltype(glBlendEquationSeparate)>(get_func("glBlendEquationSeparate"));
    glBlendEquationSeparatei = reinterpret_cast<decltype(glBlendEquationSeparatei)>(get_func("glBlendEquationSeparatei"));
    glBlendEquationi = reinterpret_cast<decltype(glBlendEquationi)>(get_func("glBlendEquationi"));
    glBlendFunc = reinterpret_cast<decltype(glBlendFunc)>(get_func("glBlendFunc"));
    glBlendFuncSeparate = reinterpret_cast<decltype(glBlendFuncSeparate)>(get_func("glBlendFuncSeparate"));
    glBlendFuncSeparatei = reinterpret_cast<decltype(glBlendFuncSeparatei)>(get_func("glBlendFuncSeparatei"));
    glBlendFunci = reinterpret_cast<decltype(glBlendFunci)>(get_func("glBlendFunci"));
    glBlitFramebuffer = reinterpret_cast<decltype(glBlitFramebuffer)>(get_func("glBlitFramebuffer"));
    glBlitNamedFramebuffer = reinterpret_cast<decltype(glBlitNamedFramebuffer)>(get_func("glBlitNamedFramebuffer"));
    glBufferAddressRangeNV = reinterpret_cast<decltype(glBufferAddressRangeNV)>(get_func("glBufferAddressRangeNV"));
    glBufferData = reinterpret_cast<decltype(glBufferData)>(get_func("glBufferData"));
    glBufferStorage = reinterpret_cast<decltype(glBufferStorage)>(get_func("glBufferStorage"));
    glBufferSubData = reinterpret_cast<decltype(glBufferSubData)>(get_func("glBufferSubData"));
    glCallCommandListNV = reinterpret_cast<decltype(glCallCommandListNV)>(get_func("glCallCommandListNV"));
    glCheckFramebufferStatus = reinterpret_cast<decltype(glCheckFramebufferStatus)>(get_func("glCheckFramebufferStatus"));
    glCheckNamedFramebufferStatus = reinterpret_cast<decltype(glCheckNamedFramebufferStatus)>(get_func("glCheckNamedFramebufferStatus"));
    glClampColor = reinterpret_cast<decltype(glClampColor)>(get_func("glClampColor"));
    glClear = reinterpret_cast<decltype(glClear)>(get_func("glClear"));
    glClearBufferData = reinterpret_cast<decltype(glClearBufferData)>(get_func("glClearBufferData"));
    glClearBufferSubData = reinterpret_cast<decltype(glClearBufferSubData)>(get_func("glClearBufferSubData"));
    glClearBufferfi = reinterpret_cast<decltype(glClearBufferfi)>(get_func("glClearBufferfi"));
    glClearBufferfv = reinterpret_cast<decltype(glClearBufferfv)>(get_func("glClearBufferfv"));
    glClearBufferiv = reinterpret_cast<decltype(glClearBufferiv)>(get_func("glClearBufferiv"));
    glClearBufferuiv = reinterpret_cast<decltype(glClearBufferuiv)>(get_func("glClearBufferuiv"));
    glClearColor = reinterpret_cast<decltype(glClearColor)>(get_func("glClearColor"));
    glClearDepth = reinterpret_cast<decltype(glClearDepth)>(get_func("glClearDepth"));
    glClearDepthf = reinterpret_cast<decltype(glClearDepthf)>(get_func("glClearDepthf"));
    glClearNamedBufferData = reinterpret_cast<decltype(glClearNamedBufferData)>(get_func("glClearNamedBufferData"));
    glClearNamedBufferSubData = reinterpret_cast<decltype(glClearNamedBufferSubData)>(get_func("glClearNamedBufferSubData"));
    glClearNamedFramebufferfi = reinterpret_cast<decltype(glClearNamedFramebufferfi)>(get_func("glClearNamedFramebufferfi"));
    glClearNamedFramebufferfv = reinterpret_cast<decltype(glClearNamedFramebufferfv)>(get_func("glClearNamedFramebufferfv"));
    glClearNamedFramebufferiv = reinterpret_cast<decltype(glClearNamedFramebufferiv)>(get_func("glClearNamedFramebufferiv"));
    glClearNamedFramebufferuiv = reinterpret_cast<decltype(glClearNamedFramebufferuiv)>(get_func("glClearNamedFramebufferuiv"));
    glClearStencil = reinterpret_cast<decltype(glClearStencil)>(get_func("glClearStencil"));
    glClearTexImage = reinterpret_cast<decltype(glClearTexImage)>(get_func("glClearTexImage"));
    glClearTexSubImage = reinterpret_cast<decltype(glClearTexSubImage)>(get_func("glClearTexSubImage"));
    glClientWaitSync = reinterpret_cast<decltype(glClientWaitSync)>(get_func("glClientWaitSync"));
    glClipControl = reinterpret_cast<decltype(glClipControl)>(get_func("glClipControl"));
    glColorFormatNV = reinterpret_cast<decltype(glColorFormatNV)>(get_func("glColorFormatNV"));
    glColorMask = reinterpret_cast<decltype(glColorMask)>(get_func("glColorMask"));
    glColorMaski = reinterpret_cast<decltype(glColorMaski)>(get_func("glColorMaski"));
    glCommandListSegmentsNV = reinterpret_cast<decltype(glCommandListSegmentsNV)>(get_func("glCommandListSegmentsNV"));
    glCompileCommandListNV = reinterpret_cast<decltype(glCompileCommandListNV)>(get_func("glCompileCommandListNV"));
    glCompileShader = reinterpret_cast<decltype(glCompileShader)>(get_func("glCompileShader"));
    glCompressedTexImage1D = reinterpret_cast<decltype(glCompressedTexImage1D)>(get_func("glCompressedTexImage1D"));
    glCompressedTexImage2D = reinterpret_cast<decltype(glCompressedTexImage2D)>(get_func("glCompressedTexImage2D"));
    glCompressedTexImage3D = reinterpret_cast<decltype(glCompressedTexImage3D)>(get_func("glCompressedTexImage3D"));
    glCompressedTexSubImage1D = reinterpret_cast<decltype(glCompressedTexSubImage1D)>(get_func("glCompressedTexSubImage1D"));
    glCompressedTexSubImage2D = reinterpret_cast<decltype(glCompressedTexSubImage2D)>(get_func("glCompressedTexSubImage2D"));
    glCompressedTexSubImage3D = reinterpret_cast<decltype(glCompressedTexSubImage3D)>(get_func("glCompressedTexSubImage3D"));
    glCompressedTextureSubImage1D = reinterpret_cast<decltype(glCompressedTextureSubImage1D)>(get_func("glCompressedTextureSubImage1D"));
    glCompressedTextureSubImage2D = reinterpret_cast<decltype(glCompressedTextureSubImage2D)>(get_func("glCompressedTextureSubImage2D"));
    glCompressedTextureSubImage3D = reinterpret_cast<decltype(glCompressedTextureSubImage3D)>(get_func("glCompressedTextureSubImage3D"));
    glCopyBufferSubData = reinterpret_cast<decltype(glCopyBufferSubData)>(get_func("glCopyBufferSubData"));
    glCopyImageSubData = reinterpret_cast<decltype(glCopyImageSubData)>(get_func("glCopyImageSubData"));
    glCopyNamedBufferSubData = reinterpret_cast<decltype(glCopyNamedBufferSubData)>(get_func("glCopyNamedBufferSubData"));
    glCopyPathNV = reinterpret_cast<decltype(glCopyPathNV)>(get_func("glCopyPathNV"));
    glCopyTexImage1D = reinterpret_cast<decltype(glCopyTexImage1D)>(get_func("glCopyTexImage1D"));
    glCopyTexImage2D = reinterpret_cast<decltype(glCopyTexImage2D)>(get_func("glCopyTexImage2D"));
    glCopyTexSubImage1D = reinterpret_cast<decltype(glCopyTexSubImage1D)>(get_func("glCopyTexSubImage1D"));
    glCopyTexSubImage2D = reinterpret_cast<decltype(glCopyTexSubImage2D)>(get_func("glCopyTexSubImage2D"));
    glCopyTexSubImage3D = reinterpret_cast<decltype(glCopyTexSubImage3D)>(get_func("glCopyTexSubImage3D"));
    glCopyTextureSubImage1D = reinterpret_cast<decltype(glCopyTextureSubImage1D)>(get_func("glCopyTextureSubImage1D"));
    glCopyTextureSubImage2D = reinterpret_cast<decltype(glCopyTextureSubImage2D)>(get_func("glCopyTextureSubImage2D"));
    glCopyTextureSubImage3D = reinterpret_cast<decltype(glCopyTextureSubImage3D)>(get_func("glCopyTextureSubImage3D"));
    glCoverFillPathInstancedNV = reinterpret_cast<decltype(glCoverFillPathInstancedNV)>(get_func("glCoverFillPathInstancedNV"));
    glCoverFillPathNV = reinterpret_cast<decltype(glCoverFillPathNV)>(get_func("glCoverFillPathNV"));
    glCoverStrokePathInstancedNV = reinterpret_cast<decltype(glCoverStrokePathInstancedNV)>(get_func("glCoverStrokePathInstancedNV"));
    glCoverStrokePathNV = reinterpret_cast<decltype(glCoverStrokePathNV)>(get_func("glCoverStrokePathNV"));
    glCreateBuffers = reinterpret_cast<decltype(glCreateBuffers)>(get_func("glCreateBuffers"));
    glCreateCommandListsNV = reinterpret_cast<decltype(glCreateCommandListsNV)>(get_func("glCreateCommandListsNV"));
    glCreateFramebuffers = reinterpret_cast<decltype(glCreateFramebuffers)>(get_func("glCreateFramebuffers"));
    glCreateProgram = reinterpret_cast<decltype(glCreateProgram)>(get_func("glCreateProgram"));
    glCreateProgramPipelines = reinterpret_cast<decltype(glCreateProgramPipelines)>(get_func("glCreateProgramPipelines"));
    glCreateQueries = reinterpret_cast<decltype(glCreateQueries)>(get_func("glCreateQueries"));
    glCreateRenderbuffers = reinterpret_cast<decltype(glCreateRenderbuffers)>(get_func("glCreateRenderbuffers"));
    glCreateSamplers = reinterpret_cast<decltype(glCreateSamplers)>(get_func("glCreateSamplers"));
    glCreateShader = reinterpret_cast<decltype(glCreateShader)>(get_func("glCreateShader"));
    glCreateShaderProgramv = reinterpret_cast<decltype(glCreateShaderProgramv)>(get_func("glCreateShaderProgramv"));
    glCreateStatesNV = reinterpret_cast<decltype(glCreateStatesNV)>(get_func("glCreateStatesNV"));
    glCreateTextures = reinterpret_cast<decltype(glCreateTextures)>(get_func("glCreateTextures"));
    glCreateTransformFeedbacks = reinterpret_cast<decltype(glCreateTransformFeedbacks)>(get_func("glCreateTransformFeedbacks"));
    glCreateVertexArrays = reinterpret_cast<decltype(glCreateVertexArrays)>(get_func("glCreateVertexArrays"));
    glCullFace = reinterpret_cast<decltype(glCullFace)>(get_func("glCullFace"));
    glDebugMessageCallback = reinterpret_cast<decltype(glDebugMessageCallback)>(get_func("glDebugMessageCallback"));
    glDebugMessageCallbackKHR = reinterpret_cast<decltype(glDebugMessageCallbackKHR)>(get_func("glDebugMessageCallbackKHR"));
    glDebugMessageControl = reinterpret_cast<decltype(glDebugMessageControl)>(get_func("glDebugMessageControl"));
    glDebugMessageControlKHR = reinterpret_cast<decltype(glDebugMessageControlKHR)>(get_func("glDebugMessageControlKHR"));
    glDebugMessageInsert = reinterpret_cast<decltype(glDebugMessageInsert)>(get_func("glDebugMessageInsert"));
    glDebugMessageInsertKHR = reinterpret_cast<decltype(glDebugMessageInsertKHR)>(get_func("glDebugMessageInsertKHR"));
    glDeleteBuffers = reinterpret_cast<decltype(glDeleteBuffers)>(get_func("glDeleteBuffers"));
    glDeleteCommandListsNV = reinterpret_cast<decltype(glDeleteCommandListsNV)>(get_func("glDeleteCommandListsNV"));
    glDeleteFramebuffers = reinterpret_cast<decltype(glDeleteFramebuffers)>(get_func("glDeleteFramebuffers"));
    glDeletePathsNV = reinterpret_cast<decltype(glDeletePathsNV)>(get_func("glDeletePathsNV"));
    glDeleteProgram = reinterpret_cast<decltype(glDeleteProgram)>(get_func("glDeleteProgram"));
    glDeleteProgramPipelines = reinterpret_cast<decltype(glDeleteProgramPipelines)>(get_func("glDeleteProgramPipelines"));
    glDeleteQueries = reinterpret_cast<decltype(glDeleteQueries)>(get_func("glDeleteQueries"));
    glDeleteRenderbuffers = reinterpret_cast<decltype(glDeleteRenderbuffers)>(get_func("glDeleteRenderbuffers"));
    glDeleteSamplers = reinterpret_cast<decltype(glDeleteSamplers)>(get_func("glDeleteSamplers"));
    glDeleteShader = reinterpret_cast<decltype(glDeleteShader)>(get_func("glDeleteShader"));
    glDeleteStatesNV = reinterpret_cast<decltype(glDeleteStatesNV)>(get_func("glDeleteStatesNV"));
    glDeleteSync = reinterpret_cast<decltype(glDeleteSync)>(get_func("glDeleteSync"));
    glDeleteTextures = reinterpret_cast<decltype(glDeleteTextures)>(get_func("glDeleteTextures"));
    glDeleteTransformFeedbacks = reinterpret_cast<decltype(glDeleteTransformFeedbacks)>(get_func("glDeleteTransformFeedbacks"));
    glDeleteVertexArrays = reinterpret_cast<decltype(glDeleteVertexArrays)>(get_func("glDeleteVertexArrays"));
    glDepthFunc = reinterpret_cast<decltype(glDepthFunc)>(get_func("glDepthFunc"));
    glDepthMask = reinterpret_cast<decltype(glDepthMask)>(get_func("glDepthMask"));
    glDepthRange = reinterpret_cast<decltype(glDepthRange)>(get_func("glDepthRange"));
    glDepthRangeArrayv = reinterpret_cast<decltype(glDepthRangeArrayv)>(get_func("glDepthRangeArrayv"));
    glDepthRangeIndexed = reinterpret_cast<decltype(glDepthRangeIndexed)>(get_func("glDepthRangeIndexed"));
    glDepthRangef = reinterpret_cast<decltype(glDepthRangef)>(get_func("glDepthRangef"));
    glDetachShader = reinterpret_cast<decltype(glDetachShader)>(get_func("glDetachShader"));
    glDisable = reinterpret_cast<decltype(glDisable)>(get_func("glDisable"));
    glDisableVertexArrayAttrib = reinterpret_cast<decltype(glDisableVertexArrayAttrib)>(get_func("glDisableVertexArrayAttrib"));
    glDisableVertexAttribArray = reinterpret_cast<decltype(glDisableVertexAttribArray)>(get_func("glDisableVertexAttribArray"));
    glDisablei = reinterpret_cast<decltype(glDisablei)>(get_func("glDisablei"));
    glDispatchCompute = reinterpret_cast<decltype(glDispatchCompute)>(get_func("glDispatchCompute"));
    glDispatchComputeGroupSizeARB = reinterpret_cast<decltype(glDispatchComputeGroupSizeARB)>(get_func("glDispatchComputeGroupSizeARB"));
    glDispatchComputeIndirect = reinterpret_cast<decltype(glDispatchComputeIndirect)>(get_func("glDispatchComputeIndirect"));
    glDrawArrays = reinterpret_cast<decltype(glDrawArrays)>(get_func("glDrawArrays"));
    glDrawArraysIndirect = reinterpret_cast<decltype(glDrawArraysIndirect)>(get_func("glDrawArraysIndirect"));
    glDrawArraysInstanced = reinterpret_cast<decltype(glDrawArraysInstanced)>(get_func("glDrawArraysInstanced"));
    glDrawArraysInstancedBaseInstance = reinterpret_cast<decltype(glDrawArraysInstancedBaseInstance)>(get_func("glDrawArraysInstancedBaseInstance"));
    glDrawBuffer = reinterpret_cast<decltype(glDrawBuffer)>(get_func("glDrawBuffer"));
    glDrawBuffers = reinterpret_cast<decltype(glDrawBuffers)>(get_func("glDrawBuffers"));
    glDrawCommandsAddressNV = reinterpret_cast<decltype(glDrawCommandsAddressNV)>(get_func("glDrawCommandsAddressNV"));
    glDrawCommandsNV = reinterpret_cast<decltype(glDrawCommandsNV)>(get_func("glDrawCommandsNV"));
    glDrawCommandsStatesAddressNV = reinterpret_cast<decltype(glDrawCommandsStatesAddressNV)>(get_func("glDrawCommandsStatesAddressNV"));
    glDrawCommandsStatesNV = reinterpret_cast<decltype(glDrawCommandsStatesNV)>(get_func("glDrawCommandsStatesNV"));
    glDrawElements = reinterpret_cast<decltype(glDrawElements)>(get_func("glDrawElements"));
    glDrawElementsBaseVertex = reinterpret_cast<decltype(glDrawElementsBaseVertex)>(get_func("glDrawElementsBaseVertex"));
    glDrawElementsIndirect = reinterpret_cast<decltype(glDrawElementsIndirect)>(get_func("glDrawElementsIndirect"));
    glDrawElementsInstanced = reinterpret_cast<decltype(glDrawElementsInstanced)>(get_func("glDrawElementsInstanced"));
    glDrawElementsInstancedBaseInstance = reinterpret_cast<decltype(glDrawElementsInstancedBaseInstance)>(get_func("glDrawElementsInstancedBaseInstance"));
    glDrawElementsInstancedBaseVertex = reinterpret_cast<decltype(glDrawElementsInstancedBaseVertex)>(get_func("glDrawElementsInstancedBaseVertex"));
    glDrawElementsInstancedBaseVertexBaseInstance = reinterpret_cast<decltype(glDrawElementsInstancedBaseVertexBaseInstance)>(get_func("glDrawElementsInstancedBaseVertexBaseInstance"));
    glDrawRangeElements = reinterpret_cast<decltype(glDrawRangeElements)>(get_func("glDrawRangeElements"));
    glDrawRangeElementsBaseVertex = reinterpret_cast<decltype(glDrawRangeElementsBaseVertex)>(get_func("glDrawRangeElementsBaseVertex"));
    glDrawTransformFeedback = reinterpret_cast<decltype(glDrawTransformFeedback)>(get_func("glDrawTransformFeedback"));
    glDrawTransformFeedbackInstanced = reinterpret_cast<decltype(glDrawTransformFeedbackInstanced)>(get_func("glDrawTransformFeedbackInstanced"));
    glDrawTransformFeedbackStream = reinterpret_cast<decltype(glDrawTransformFeedbackStream)>(get_func("glDrawTransformFeedbackStream"));
    glDrawTransformFeedbackStreamInstanced = reinterpret_cast<decltype(glDrawTransformFeedbackStreamInstanced)>(get_func("glDrawTransformFeedbackStreamInstanced"));
    glEdgeFlagFormatNV = reinterpret_cast<decltype(glEdgeFlagFormatNV)>(get_func("glEdgeFlagFormatNV"));
    glEnable = reinterpret_cast<decltype(glEnable)>(get_func("glEnable"));
    glEnableVertexArrayAttrib = reinterpret_cast<decltype(glEnableVertexArrayAttrib)>(get_func("glEnableVertexArrayAttrib"));
    glEnableVertexAttribArray = reinterpret_cast<decltype(glEnableVertexAttribArray)>(get_func("glEnableVertexAttribArray"));
    glEnablei = reinterpret_cast<decltype(glEnablei)>(get_func("glEnablei"));
    glEndConditionalRender = reinterpret_cast<decltype(glEndConditionalRender)>(get_func("glEndConditionalRender"));
    glEndQuery = reinterpret_cast<decltype(glEndQuery)>(get_func("glEndQuery"));
    glEndQueryIndexed = reinterpret_cast<decltype(glEndQueryIndexed)>(get_func("glEndQueryIndexed"));
    glEndTransformFeedback = reinterpret_cast<decltype(glEndTransformFeedback)>(get_func("glEndTransformFeedback"));
    glFenceSync = reinterpret_cast<decltype(glFenceSync)>(get_func("glFenceSync"));
    glFinish = reinterpret_cast<decltype(glFinish)>(get_func("glFinish"));
    glFlush = reinterpret_cast<decltype(glFlush)>(get_func("glFlush"));
    glFlushMappedBufferRange = reinterpret_cast<decltype(glFlushMappedBufferRange)>(get_func("glFlushMappedBufferRange"));
    glFlushMappedNamedBufferRange = reinterpret_cast<decltype(glFlushMappedNamedBufferRange)>(get_func("glFlushMappedNamedBufferRange"));
    glFogCoordFormatNV = reinterpret_cast<decltype(glFogCoordFormatNV)>(get_func("glFogCoordFormatNV"));
    glFramebufferParameteri = reinterpret_cast<decltype(glFramebufferParameteri)>(get_func("glFramebufferParameteri"));
    glFramebufferRenderbuffer = reinterpret_cast<decltype(glFramebufferRenderbuffer)>(get_func("glFramebufferRenderbuffer"));
    glFramebufferTexture = reinterpret_cast<decltype(glFramebufferTexture)>(get_func("glFramebufferTexture"));
    glFramebufferTexture1D = reinterpret_cast<decltype(glFramebufferTexture1D)>(get_func("glFramebufferTexture1D"));
    glFramebufferTexture2D = reinterpret_cast<decltype(glFramebufferTexture2D)>(get_func("glFramebufferTexture2D"));
    glFramebufferTexture3D = reinterpret_cast<decltype(glFramebufferTexture3D)>(get_func("glFramebufferTexture3D"));
    glFramebufferTextureLayer = reinterpret_cast<decltype(glFramebufferTextureLayer)>(get_func("glFramebufferTextureLayer"));
    glFrontFace = reinterpret_cast<decltype(glFrontFace)>(get_func("glFrontFace"));
    glGenBuffers = reinterpret_cast<decltype(glGenBuffers)>(get_func("glGenBuffers"));
    glGenFramebuffers = reinterpret_cast<decltype(glGenFramebuffers)>(get_func("glGenFramebuffers"));
    glGenPathsNV = reinterpret_cast<decltype(glGenPathsNV)>(get_func("glGenPathsNV"));
    glGenProgramPipelines = reinterpret_cast<decltype(glGenProgramPipelines)>(get_func("glGenProgramPipelines"));
    glGenQueries = reinterpret_cast<decltype(glGenQueries)>(get_func("glGenQueries"));
    glGenRenderbuffers = reinterpret_cast<decltype(glGenRenderbuffers)>(get_func("glGenRenderbuffers"));
    glGenSamplers = reinterpret_cast<decltype(glGenSamplers)>(get_func("glGenSamplers"));
    glGenTextures = reinterpret_cast<decltype(glGenTextures)>(get_func("glGenTextures"));
    glGenTransformFeedbacks = reinterpret_cast<decltype(glGenTransformFeedbacks)>(get_func("glGenTransformFeedbacks"));
    glGenVertexArrays = reinterpret_cast<decltype(glGenVertexArrays)>(get_func("glGenVertexArrays"));
    glGenerateMipmap = reinterpret_cast<decltype(glGenerateMipmap)>(get_func("glGenerateMipmap"));
    glGenerateTextureMipmap = reinterpret_cast<decltype(glGenerateTextureMipmap)>(get_func("glGenerateTextureMipmap"));
    glGetActiveAtomicCounterBufferiv = reinterpret_cast<decltype(glGetActiveAtomicCounterBufferiv)>(get_func("glGetActiveAtomicCounterBufferiv"));
    glGetActiveAttrib = reinterpret_cast<decltype(glGetActiveAttrib)>(get_func("glGetActiveAttrib"));
    glGetActiveSubroutineName = reinterpret_cast<decltype(glGetActiveSubroutineName)>(get_func("glGetActiveSubroutineName"));
    glGetActiveSubroutineUniformName = reinterpret_cast<decltype(glGetActiveSubroutineUniformName)>(get_func("glGetActiveSubroutineUniformName"));
    glGetActiveSubroutineUniformiv = reinterpret_cast<decltype(glGetActiveSubroutineUniformiv)>(get_func("glGetActiveSubroutineUniformiv"));
    glGetActiveUniform = reinterpret_cast<decltype(glGetActiveUniform)>(get_func("glGetActiveUniform"));
    glGetActiveUniformBlockName = reinterpret_cast<decltype(glGetActiveUniformBlockName)>(get_func("glGetActiveUniformBlockName"));
    glGetActiveUniformBlockiv = reinterpret_cast<decltype(glGetActiveUniformBlockiv)>(get_func("glGetActiveUniformBlockiv"));
    glGetActiveUniformName = reinterpret_cast<decltype(glGetActiveUniformName)>(get_func("glGetActiveUniformName"));
    glGetActiveUniformsiv = reinterpret_cast<decltype(glGetActiveUniformsiv)>(get_func("glGetActiveUniformsiv"));
    glGetAttachedShaders = reinterpret_cast<decltype(glGetAttachedShaders)>(get_func("glGetAttachedShaders"));
    glGetAttribLocation = reinterpret_cast<decltype(glGetAttribLocation)>(get_func("glGetAttribLocation"));
    glGetBooleani_v = reinterpret_cast<decltype(glGetBooleani_v)>(get_func("glGetBooleani_v"));
    glGetBooleanv = reinterpret_cast<decltype(glGetBooleanv)>(get_func("glGetBooleanv"));
    glGetBufferParameteri64v = reinterpret_cast<decltype(glGetBufferParameteri64v)>(get_func("glGetBufferParameteri64v"));
    glGetBufferParameteriv = reinterpret_cast<decltype(glGetBufferParameteriv)>(get_func("glGetBufferParameteriv"));
    glGetBufferParameterui64vNV = reinterpret_cast<decltype(glGetBufferParameterui64vNV)>(get_func("glGetBufferParameterui64vNV"));
    glGetBufferPointerv = reinterpret_cast<decltype(glGetBufferPointerv)>(get_func("glGetBufferPointerv"));
    glGetBufferSubData = reinterpret_cast<decltype(glGetBufferSubData)>(get_func("glGetBufferSubData"));
    glGetCommandHeaderNV = reinterpret_cast<decltype(glGetCommandHeaderNV)>(get_func("glGetCommandHeaderNV"));
    glGetCompressedTexImage = reinterpret_cast<decltype(glGetCompressedTexImage)>(get_func("glGetCompressedTexImage"));
    glGetCompressedTextureImage = reinterpret_cast<decltype(glGetCompressedTextureImage)>(get_func("glGetCompressedTextureImage"));
    glGetCompressedTextureSubImage = reinterpret_cast<decltype(glGetCompressedTextureSubImage)>(get_func("glGetCompressedTextureSubImage"));
    glGetDebugMessageLog = reinterpret_cast<decltype(glGetDebugMessageLog)>(get_func("glGetDebugMessageLog"));
    glGetDebugMessageLogKHR = reinterpret_cast<decltype(glGetDebugMessageLogKHR)>(get_func("glGetDebugMessageLogKHR"));
    glGetDoublei_v = reinterpret_cast<decltype(glGetDoublei_v)>(get_func("glGetDoublei_v"));
    glGetDoublev = reinterpret_cast<decltype(glGetDoublev)>(get_func("glGetDoublev"));
    glGetError = reinterpret_cast<decltype(glGetError)>(get_func("glGetError"));
    glGetFloati_v = reinterpret_cast<decltype(glGetFloati_v)>(get_func("glGetFloati_v"));
    glGetFloatv = reinterpret_cast<decltype(glGetFloatv)>(get_func("glGetFloatv"));
    glGetFragDataIndex = reinterpret_cast<decltype(glGetFragDataIndex)>(get_func("glGetFragDataIndex"));
    glGetFragDataLocation = reinterpret_cast<decltype(glGetFragDataLocation)>(get_func("glGetFragDataLocation"));
    glGetFramebufferAttachmentParameteriv = reinterpret_cast<decltype(glGetFramebufferAttachmentParameteriv)>(get_func("glGetFramebufferAttachmentParameteriv"));
    glGetFramebufferParameteriv = reinterpret_cast<decltype(glGetFramebufferParameteriv)>(get_func("glGetFramebufferParameteriv"));
    glGetGraphicsResetStatus = reinterpret_cast<decltype(glGetGraphicsResetStatus)>(get_func("glGetGraphicsResetStatus"));
    glGetGraphicsResetStatusARB = reinterpret_cast<decltype(glGetGraphicsResetStatusARB)>(get_func("glGetGraphicsResetStatusARB"));
    glGetImageHandleARB = reinterpret_cast<decltype(glGetImageHandleARB)>(get_func("glGetImageHandleARB"));
    glGetInteger64i_v = reinterpret_cast<decltype(glGetInteger64i_v)>(get_func("glGetInteger64i_v"));
    glGetInteger64v = reinterpret_cast<decltype(glGetInteger64v)>(get_func("glGetInteger64v"));
    glGetIntegeri_v = reinterpret_cast<decltype(glGetIntegeri_v)>(get_func("glGetIntegeri_v"));
    glGetIntegerui64i_vNV = reinterpret_cast<decltype(glGetIntegerui64i_vNV)>(get_func("glGetIntegerui64i_vNV"));
    glGetIntegerui64vNV = reinterpret_cast<decltype(glGetIntegerui64vNV)>(get_func("glGetIntegerui64vNV"));
    glGetIntegerv = reinterpret_cast<decltype(glGetIntegerv)>(get_func("glGetIntegerv"));
    glGetInternalformati64v = reinterpret_cast<decltype(glGetInternalformati64v)>(get_func("glGetInternalformati64v"));
    glGetInternalformativ = reinterpret_cast<decltype(glGetInternalformativ)>(get_func("glGetInternalformativ"));
    glGetMultisamplefv = reinterpret_cast<decltype(glGetMultisamplefv)>(get_func("glGetMultisamplefv"));
    glGetNamedBufferParameteri64v = reinterpret_cast<decltype(glGetNamedBufferParameteri64v)>(get_func("glGetNamedBufferParameteri64v"));
    glGetNamedBufferParameteriv = reinterpret_cast<decltype(glGetNamedBufferParameteriv)>(get_func("glGetNamedBufferParameteriv"));
    glGetNamedBufferParameterui64vNV = reinterpret_cast<decltype(glGetNamedBufferParameterui64vNV)>(get_func("glGetNamedBufferParameterui64vNV"));
    glGetNamedBufferPointerv = reinterpret_cast<decltype(glGetNamedBufferPointerv)>(get_func("glGetNamedBufferPointerv"));
    glGetNamedBufferSubData = reinterpret_cast<decltype(glGetNamedBufferSubData)>(get_func("glGetNamedBufferSubData"));
    glGetNamedFramebufferAttachmentParameteriv = reinterpret_cast<decltype(glGetNamedFramebufferAttachmentParameteriv)>(get_func("glGetNamedFramebufferAttachmentParameteriv"));
    glGetNamedFramebufferParameteriv = reinterpret_cast<decltype(glGetNamedFramebufferParameteriv)>(get_func("glGetNamedFramebufferParameteriv"));
    glGetNamedRenderbufferParameteriv = reinterpret_cast<decltype(glGetNamedRenderbufferParameteriv)>(get_func("glGetNamedRenderbufferParameteriv"));
    glGetObjectLabel = reinterpret_cast<decltype(glGetObjectLabel)>(get_func("glGetObjectLabel"));
    glGetObjectLabelKHR = reinterpret_cast<decltype(glGetObjectLabelKHR)>(get_func("glGetObjectLabelKHR"));
    glGetObjectPtrLabel = reinterpret_cast<decltype(glGetObjectPtrLabel)>(get_func("glGetObjectPtrLabel"));
    glGetObjectPtrLabelKHR = reinterpret_cast<decltype(glGetObjectPtrLabelKHR)>(get_func("glGetObjectPtrLabelKHR"));
    glGetPathColorGenfvNV = reinterpret_cast<decltype(glGetPathColorGenfvNV)>(get_func("glGetPathColorGenfvNV"));
    glGetPathColorGenivNV = reinterpret_cast<decltype(glGetPathColorGenivNV)>(get_func("glGetPathColorGenivNV"));
    glGetPathCommandsNV = reinterpret_cast<decltype(glGetPathCommandsNV)>(get_func("glGetPathCommandsNV"));
    glGetPathCoordsNV = reinterpret_cast<decltype(glGetPathCoordsNV)>(get_func("glGetPathCoordsNV"));
    glGetPathDashArrayNV = reinterpret_cast<decltype(glGetPathDashArrayNV)>(get_func("glGetPathDashArrayNV"));
    glGetPathLengthNV = reinterpret_cast<decltype(glGetPathLengthNV)>(get_func("glGetPathLengthNV"));
    glGetPathMetricRangeNV = reinterpret_cast<decltype(glGetPathMetricRangeNV)>(get_func("glGetPathMetricRangeNV"));
    glGetPathMetricsNV = reinterpret_cast<decltype(glGetPathMetricsNV)>(get_func("glGetPathMetricsNV"));
    glGetPathParameterfvNV = reinterpret_cast<decltype(glGetPathParameterfvNV)>(get_func("glGetPathParameterfvNV"));
    glGetPathParameterivNV = reinterpret_cast<decltype(glGetPathParameterivNV)>(get_func("glGetPathParameterivNV"));
    glGetPathSpacingNV = reinterpret_cast<decltype(glGetPathSpacingNV)>(get_func("glGetPathSpacingNV"));
    glGetPathTexGenfvNV = reinterpret_cast<decltype(glGetPathTexGenfvNV)>(get_func("glGetPathTexGenfvNV"));
    glGetPathTexGenivNV = reinterpret_cast<decltype(glGetPathTexGenivNV)>(get_func("glGetPathTexGenivNV"));
    glGetPointerv = reinterpret_cast<decltype(glGetPointerv)>(get_func("glGetPointerv"));
    glGetPointervKHR = reinterpret_cast<decltype(glGetPointervKHR)>(get_func("glGetPointervKHR"));
    glGetProgramBinary = reinterpret_cast<decltype(glGetProgramBinary)>(get_func("glGetProgramBinary"));
    glGetProgramInfoLog = reinterpret_cast<decltype(glGetProgramInfoLog)>(get_func("glGetProgramInfoLog"));
    glGetProgramInterfaceiv = reinterpret_cast<decltype(glGetProgramInterfaceiv)>(get_func("glGetProgramInterfaceiv"));
    glGetProgramPipelineInfoLog = reinterpret_cast<decltype(glGetProgramPipelineInfoLog)>(get_func("glGetProgramPipelineInfoLog"));
    glGetProgramPipelineiv = reinterpret_cast<decltype(glGetProgramPipelineiv)>(get_func("glGetProgramPipelineiv"));
    glGetProgramResourceIndex = reinterpret_cast<decltype(glGetProgramResourceIndex)>(get_func("glGetProgramResourceIndex"));
    glGetProgramResourceLocation = reinterpret_cast<decltype(glGetProgramResourceLocation)>(get_func("glGetProgramResourceLocation"));
    glGetProgramResourceLocationIndex = reinterpret_cast<decltype(glGetProgramResourceLocationIndex)>(get_func("glGetProgramResourceLocationIndex"));
    glGetProgramResourceName = reinterpret_cast<decltype(glGetProgramResourceName)>(get_func("glGetProgramResourceName"));
    glGetProgramResourcefvNV = reinterpret_cast<decltype(glGetProgramResourcefvNV)>(get_func("glGetProgramResourcefvNV"));
    glGetProgramResourceiv = reinterpret_cast<decltype(glGetProgramResourceiv)>(get_func("glGetProgramResourceiv"));
    glGetProgramStageiv = reinterpret_cast<decltype(glGetProgramStageiv)>(get_func("glGetProgramStageiv"));
    glGetProgramiv = reinterpret_cast<decltype(glGetProgramiv)>(get_func("glGetProgramiv"));
    glGetQueryBufferObjecti64v = reinterpret_cast<decltype(glGetQueryBufferObjecti64v)>(get_func("glGetQueryBufferObjecti64v"));
    glGetQueryBufferObjectiv = reinterpret_cast<decltype(glGetQueryBufferObjectiv)>(get_func("glGetQueryBufferObjectiv"));
    glGetQueryBufferObjectui64v = reinterpret_cast<decltype(glGetQueryBufferObjectui64v)>(get_func("glGetQueryBufferObjectui64v"));
    glGetQueryBufferObjectuiv = reinterpret_cast<decltype(glGetQueryBufferObjectuiv)>(get_func("glGetQueryBufferObjectuiv"));
    glGetQueryIndexediv = reinterpret_cast<decltype(glGetQueryIndexediv)>(get_func("glGetQueryIndexediv"));
    glGetQueryObjecti64v = reinterpret_cast<decltype(glGetQueryObjecti64v)>(get_func("glGetQueryObjecti64v"));
    glGetQueryObjectiv = reinterpret_cast<decltype(glGetQueryObjectiv)>(get_func("glGetQueryObjectiv"));
    glGetQueryObjectui64v = reinterpret_cast<decltype(glGetQueryObjectui64v)>(get_func("glGetQueryObjectui64v"));
    glGetQueryObjectuiv = reinterpret_cast<decltype(glGetQueryObjectuiv)>(get_func("glGetQueryObjectuiv"));
    glGetQueryiv = reinterpret_cast<decltype(glGetQueryiv)>(get_func("glGetQueryiv"));
    glGetRenderbufferParameteriv = reinterpret_cast<decltype(glGetRenderbufferParameteriv)>(get_func("glGetRenderbufferParameteriv"));
    glGetSamplerParameterIiv = reinterpret_cast<decltype(glGetSamplerParameterIiv)>(get_func("glGetSamplerParameterIiv"));
    glGetSamplerParameterIuiv = reinterpret_cast<decltype(glGetSamplerParameterIuiv)>(get_func("glGetSamplerParameterIuiv"));
    glGetSamplerParameterfv = reinterpret_cast<decltype(glGetSamplerParameterfv)>(get_func("glGetSamplerParameterfv"));
    glGetSamplerParameteriv = reinterpret_cast<decltype(glGetSamplerParameteriv)>(get_func("glGetSamplerParameteriv"));
    glGetShaderInfoLog = reinterpret_cast<decltype(glGetShaderInfoLog)>(get_func("glGetShaderInfoLog"));
    glGetShaderPrecisionFormat = reinterpret_cast<decltype(glGetShaderPrecisionFormat)>(get_func("glGetShaderPrecisionFormat"));
    glGetShaderSource = reinterpret_cast<decltype(glGetShaderSource)>(get_func("glGetShaderSource"));
    glGetShaderiv = reinterpret_cast<decltype(glGetShaderiv)>(get_func("glGetShaderiv"));
    glGetStageIndexNV = reinterpret_cast<decltype(glGetStageIndexNV)>(get_func("glGetStageIndexNV"));
    glGetString = reinterpret_cast<decltype(glGetString)>(get_func("glGetString"));
    glGetStringi = reinterpret_cast<decltype(glGetStringi)>(get_func("glGetStringi"));
    glGetSubroutineIndex = reinterpret_cast<decltype(glGetSubroutineIndex)>(get_func("glGetSubroutineIndex"));
    glGetSubroutineUniformLocation = reinterpret_cast<decltype(glGetSubroutineUniformLocation)>(get_func("glGetSubroutineUniformLocation"));
    glGetSynciv = reinterpret_cast<decltype(glGetSynciv)>(get_func("glGetSynciv"));
    glGetTexImage = reinterpret_cast<decltype(glGetTexImage)>(get_func("glGetTexImage"));
    glGetTexLevelParameterfv = reinterpret_cast<decltype(glGetTexLevelParameterfv)>(get_func("glGetTexLevelParameterfv"));
    glGetTexLevelParameteriv = reinterpret_cast<decltype(glGetTexLevelParameteriv)>(get_func("glGetTexLevelParameteriv"));
    glGetTexParameterIiv = reinterpret_cast<decltype(glGetTexParameterIiv)>(get_func("glGetTexParameterIiv"));
    glGetTexParameterIuiv = reinterpret_cast<decltype(glGetTexParameterIuiv)>(get_func("glGetTexParameterIuiv"));
    glGetTexParameterfv = reinterpret_cast<decltype(glGetTexParameterfv)>(get_func("glGetTexParameterfv"));
    glGetTexParameteriv = reinterpret_cast<decltype(glGetTexParameteriv)>(get_func("glGetTexParameteriv"));
    glGetTextureHandleARB = reinterpret_cast<decltype(glGetTextureHandleARB)>(get_func("glGetTextureHandleARB"));
    glGetTextureImage = reinterpret_cast<decltype(glGetTextureImage)>(get_func("glGetTextureImage"));
    glGetTextureLevelParameterfv = reinterpret_cast<decltype(glGetTextureLevelParameterfv)>(get_func("glGetTextureLevelParameterfv"));
    glGetTextureLevelParameteriv = reinterpret_cast<decltype(glGetTextureLevelParameteriv)>(get_func("glGetTextureLevelParameteriv"));
    glGetTextureParameterIiv = reinterpret_cast<decltype(glGetTextureParameterIiv)>(get_func("glGetTextureParameterIiv"));
    glGetTextureParameterIuiv = reinterpret_cast<decltype(glGetTextureParameterIuiv)>(get_func("glGetTextureParameterIuiv"));
    glGetTextureParameterfv = reinterpret_cast<decltype(glGetTextureParameterfv)>(get_func("glGetTextureParameterfv"));
    glGetTextureParameteriv = reinterpret_cast<decltype(glGetTextureParameteriv)>(get_func("glGetTextureParameteriv"));
    glGetTextureSamplerHandleARB = reinterpret_cast<decltype(glGetTextureSamplerHandleARB)>(get_func("glGetTextureSamplerHandleARB"));
    glGetTextureSubImage = reinterpret_cast<decltype(glGetTextureSubImage)>(get_func("glGetTextureSubImage"));
    glGetTransformFeedbackVarying = reinterpret_cast<decltype(glGetTransformFeedbackVarying)>(get_func("glGetTransformFeedbackVarying"));
    glGetTransformFeedbacki64_v = reinterpret_cast<decltype(glGetTransformFeedbacki64_v)>(get_func("glGetTransformFeedbacki64_v"));
    glGetTransformFeedbacki_v = reinterpret_cast<decltype(glGetTransformFeedbacki_v)>(get_func("glGetTransformFeedbacki_v"));
    glGetTransformFeedbackiv = reinterpret_cast<decltype(glGetTransformFeedbackiv)>(get_func("glGetTransformFeedbackiv"));
    glGetUniformBlockIndex = reinterpret_cast<decltype(glGetUniformBlockIndex)>(get_func("glGetUniformBlockIndex"));
    glGetUniformIndices = reinterpret_cast<decltype(glGetUniformIndices)>(get_func("glGetUniformIndices"));
    glGetUniformLocation = reinterpret_cast<decltype(glGetUniformLocation)>(get_func("glGetUniformLocation"));
    glGetUniformSubroutineuiv = reinterpret_cast<decltype(glGetUniformSubroutineuiv)>(get_func("glGetUniformSubroutineuiv"));
    glGetUniformdv = reinterpret_cast<decltype(glGetUniformdv)>(get_func("glGetUniformdv"));
    glGetUniformfv = reinterpret_cast<decltype(glGetUniformfv)>(get_func("glGetUniformfv"));
    glGetUniformiv = reinterpret_cast<decltype(glGetUniformiv)>(get_func("glGetUniformiv"));
    glGetUniformui64vNV = reinterpret_cast<decltype(glGetUniformui64vNV)>(get_func("glGetUniformui64vNV"));
    glGetUniformuiv = reinterpret_cast<decltype(glGetUniformuiv)>(get_func("glGetUniformuiv"));
    glGetVertexArrayIndexed64iv = reinterpret_cast<decltype(glGetVertexArrayIndexed64iv)>(get_func("glGetVertexArrayIndexed64iv"));
    glGetVertexArrayIndexediv = reinterpret_cast<decltype(glGetVertexArrayIndexediv)>(get_func("glGetVertexArrayIndexediv"));
    glGetVertexArrayiv = reinterpret_cast<decltype(glGetVertexArrayiv)>(get_func("glGetVertexArrayiv"));
    glGetVertexAttribIiv = reinterpret_cast<decltype(glGetVertexAttribIiv)>(get_func("glGetVertexAttribIiv"));
    glGetVertexAttribIuiv = reinterpret_cast<decltype(glGetVertexAttribIuiv)>(get_func("glGetVertexAttribIuiv"));
    glGetVertexAttribLdv = reinterpret_cast<decltype(glGetVertexAttribLdv)>(get_func("glGetVertexAttribLdv"));
    glGetVertexAttribLui64vARB = reinterpret_cast<decltype(glGetVertexAttribLui64vARB)>(get_func("glGetVertexAttribLui64vARB"));
    glGetVertexAttribPointerv = reinterpret_cast<decltype(glGetVertexAttribPointerv)>(get_func("glGetVertexAttribPointerv"));
    glGetVertexAttribdv = reinterpret_cast<decltype(glGetVertexAttribdv)>(get_func("glGetVertexAttribdv"));
    glGetVertexAttribfv = reinterpret_cast<decltype(glGetVertexAttribfv)>(get_func("glGetVertexAttribfv"));
    glGetVertexAttribiv = reinterpret_cast<decltype(glGetVertexAttribiv)>(get_func("glGetVertexAttribiv"));
    glGetnColorTableARB = reinterpret_cast<decltype(glGetnColorTableARB)>(get_func("glGetnColorTableARB"));
    glGetnCompressedTexImage = reinterpret_cast<decltype(glGetnCompressedTexImage)>(get_func("glGetnCompressedTexImage"));
    glGetnCompressedTexImageARB = reinterpret_cast<decltype(glGetnCompressedTexImageARB)>(get_func("glGetnCompressedTexImageARB"));
    glGetnConvolutionFilterARB = reinterpret_cast<decltype(glGetnConvolutionFilterARB)>(get_func("glGetnConvolutionFilterARB"));
    glGetnHistogramARB = reinterpret_cast<decltype(glGetnHistogramARB)>(get_func("glGetnHistogramARB"));
    glGetnMapdvARB = reinterpret_cast<decltype(glGetnMapdvARB)>(get_func("glGetnMapdvARB"));
    glGetnMapfvARB = reinterpret_cast<decltype(glGetnMapfvARB)>(get_func("glGetnMapfvARB"));
    glGetnMapivARB = reinterpret_cast<decltype(glGetnMapivARB)>(get_func("glGetnMapivARB"));
    glGetnMinmaxARB = reinterpret_cast<decltype(glGetnMinmaxARB)>(get_func("glGetnMinmaxARB"));
    glGetnPixelMapfvARB = reinterpret_cast<decltype(glGetnPixelMapfvARB)>(get_func("glGetnPixelMapfvARB"));
    glGetnPixelMapuivARB = reinterpret_cast<decltype(glGetnPixelMapuivARB)>(get_func("glGetnPixelMapuivARB"));
    glGetnPixelMapusvARB = reinterpret_cast<decltype(glGetnPixelMapusvARB)>(get_func("glGetnPixelMapusvARB"));
    glGetnPolygonStippleARB = reinterpret_cast<decltype(glGetnPolygonStippleARB)>(get_func("glGetnPolygonStippleARB"));
    glGetnSeparableFilterARB = reinterpret_cast<decltype(glGetnSeparableFilterARB)>(get_func("glGetnSeparableFilterARB"));
    glGetnTexImage = reinterpret_cast<decltype(glGetnTexImage)>(get_func("glGetnTexImage"));
    glGetnTexImageARB = reinterpret_cast<decltype(glGetnTexImageARB)>(get_func("glGetnTexImageARB"));
    glGetnUniformdv = reinterpret_cast<decltype(glGetnUniformdv)>(get_func("glGetnUniformdv"));
    glGetnUniformdvARB = reinterpret_cast<decltype(glGetnUniformdvARB)>(get_func("glGetnUniformdvARB"));
    glGetnUniformfv = reinterpret_cast<decltype(glGetnUniformfv)>(get_func("glGetnUniformfv"));
    glGetnUniformfvARB = reinterpret_cast<decltype(glGetnUniformfvARB)>(get_func("glGetnUniformfvARB"));
    glGetnUniformiv = reinterpret_cast<decltype(glGetnUniformiv)>(get_func("glGetnUniformiv"));
    glGetnUniformivARB = reinterpret_cast<decltype(glGetnUniformivARB)>(get_func("glGetnUniformivARB"));
    glGetnUniformuiv = reinterpret_cast<decltype(glGetnUniformuiv)>(get_func("glGetnUniformuiv"));
    glGetnUniformuivARB = reinterpret_cast<decltype(glGetnUniformuivARB)>(get_func("glGetnUniformuivARB"));
    glHint = reinterpret_cast<decltype(glHint)>(get_func("glHint"));
    glIndexFormatNV = reinterpret_cast<decltype(glIndexFormatNV)>(get_func("glIndexFormatNV"));
    glInterpolatePathsNV = reinterpret_cast<decltype(glInterpolatePathsNV)>(get_func("glInterpolatePathsNV"));
    glInvalidateBufferData = reinterpret_cast<decltype(glInvalidateBufferData)>(get_func("glInvalidateBufferData"));
    glInvalidateBufferSubData = reinterpret_cast<decltype(glInvalidateBufferSubData)>(get_func("glInvalidateBufferSubData"));
    glInvalidateFramebuffer = reinterpret_cast<decltype(glInvalidateFramebuffer)>(get_func("glInvalidateFramebuffer"));
    glInvalidateNamedFramebufferData = reinterpret_cast<decltype(glInvalidateNamedFramebufferData)>(get_func("glInvalidateNamedFramebufferData"));
    glInvalidateNamedFramebufferSubData = reinterpret_cast<decltype(glInvalidateNamedFramebufferSubData)>(get_func("glInvalidateNamedFramebufferSubData"));
    glInvalidateSubFramebuffer = reinterpret_cast<decltype(glInvalidateSubFramebuffer)>(get_func("glInvalidateSubFramebuffer"));
    glInvalidateTexImage = reinterpret_cast<decltype(glInvalidateTexImage)>(get_func("glInvalidateTexImage"));
    glInvalidateTexSubImage = reinterpret_cast<decltype(glInvalidateTexSubImage)>(get_func("glInvalidateTexSubImage"));
    glIsBuffer = reinterpret_cast<decltype(glIsBuffer)>(get_func("glIsBuffer"));
    glIsBufferResidentNV = reinterpret_cast<decltype(glIsBufferResidentNV)>(get_func("glIsBufferResidentNV"));
    glIsCommandListNV = reinterpret_cast<decltype(glIsCommandListNV)>(get_func("glIsCommandListNV"));
    glIsEnabled = reinterpret_cast<decltype(glIsEnabled)>(get_func("glIsEnabled"));
    glIsEnabledi = reinterpret_cast<decltype(glIsEnabledi)>(get_func("glIsEnabledi"));
    glIsFramebuffer = reinterpret_cast<decltype(glIsFramebuffer)>(get_func("glIsFramebuffer"));
    glIsImageHandleResidentARB = reinterpret_cast<decltype(glIsImageHandleResidentARB)>(get_func("glIsImageHandleResidentARB"));
    glIsNamedBufferResidentNV = reinterpret_cast<decltype(glIsNamedBufferResidentNV)>(get_func("glIsNamedBufferResidentNV"));
    glIsPathNV = reinterpret_cast<decltype(glIsPathNV)>(get_func("glIsPathNV"));
    glIsPointInFillPathNV = reinterpret_cast<decltype(glIsPointInFillPathNV)>(get_func("glIsPointInFillPathNV"));
    glIsPointInStrokePathNV = reinterpret_cast<decltype(glIsPointInStrokePathNV)>(get_func("glIsPointInStrokePathNV"));
    glIsProgram = reinterpret_cast<decltype(glIsProgram)>(get_func("glIsProgram"));
    glIsProgramPipeline = reinterpret_cast<decltype(glIsProgramPipeline)>(get_func("glIsProgramPipeline"));
    glIsQuery = reinterpret_cast<decltype(glIsQuery)>(get_func("glIsQuery"));
    glIsRenderbuffer = reinterpret_cast<decltype(glIsRenderbuffer)>(get_func("glIsRenderbuffer"));
    glIsSampler = reinterpret_cast<decltype(glIsSampler)>(get_func("glIsSampler"));
    glIsShader = reinterpret_cast<decltype(glIsShader)>(get_func("glIsShader"));
    glIsStateNV = reinterpret_cast<decltype(glIsStateNV)>(get_func("glIsStateNV"));
    glIsSync = reinterpret_cast<decltype(glIsSync)>(get_func("glIsSync"));
    glIsTexture = reinterpret_cast<decltype(glIsTexture)>(get_func("glIsTexture"));
    glIsTextureHandleResidentARB = reinterpret_cast<decltype(glIsTextureHandleResidentARB)>(get_func("glIsTextureHandleResidentARB"));
    glIsTransformFeedback = reinterpret_cast<decltype(glIsTransformFeedback)>(get_func("glIsTransformFeedback"));
    glIsVertexArray = reinterpret_cast<decltype(glIsVertexArray)>(get_func("glIsVertexArray"));
    glLineWidth = reinterpret_cast<decltype(glLineWidth)>(get_func("glLineWidth"));
    glLinkProgram = reinterpret_cast<decltype(glLinkProgram)>(get_func("glLinkProgram"));
    glListDrawCommandsStatesClientNV = reinterpret_cast<decltype(glListDrawCommandsStatesClientNV)>(get_func("glListDrawCommandsStatesClientNV"));
    glLogicOp = reinterpret_cast<decltype(glLogicOp)>(get_func("glLogicOp"));
    glMakeBufferNonResidentNV = reinterpret_cast<decltype(glMakeBufferNonResidentNV)>(get_func("glMakeBufferNonResidentNV"));
    glMakeBufferResidentNV = reinterpret_cast<decltype(glMakeBufferResidentNV)>(get_func("glMakeBufferResidentNV"));
    glMakeImageHandleNonResidentARB = reinterpret_cast<decltype(glMakeImageHandleNonResidentARB)>(get_func("glMakeImageHandleNonResidentARB"));
    glMakeImageHandleResidentARB = reinterpret_cast<decltype(glMakeImageHandleResidentARB)>(get_func("glMakeImageHandleResidentARB"));
    glMakeNamedBufferNonResidentNV = reinterpret_cast<decltype(glMakeNamedBufferNonResidentNV)>(get_func("glMakeNamedBufferNonResidentNV"));
    glMakeNamedBufferResidentNV = reinterpret_cast<decltype(glMakeNamedBufferResidentNV)>(get_func("glMakeNamedBufferResidentNV"));
    glMakeTextureHandleNonResidentARB = reinterpret_cast<decltype(glMakeTextureHandleNonResidentARB)>(get_func("glMakeTextureHandleNonResidentARB"));
    glMakeTextureHandleResidentARB = reinterpret_cast<decltype(glMakeTextureHandleResidentARB)>(get_func("glMakeTextureHandleResidentARB"));
    glMapBuffer = reinterpret_cast<decltype(glMapBuffer)>(get_func("glMapBuffer"));
    glMapBufferRange = reinterpret_cast<decltype(glMapBufferRange)>(get_func("glMapBufferRange"));
    glMapNamedBuffer = reinterpret_cast<decltype(glMapNamedBuffer)>(get_func("glMapNamedBuffer"));
    glMapNamedBufferRange = reinterpret_cast<decltype(glMapNamedBufferRange)>(get_func("glMapNamedBufferRange"));
    glMatrixFrustumEXT = reinterpret_cast<decltype(glMatrixFrustumEXT)>(get_func("glMatrixFrustumEXT"));
    glMatrixLoad3x2fNV = reinterpret_cast<decltype(glMatrixLoad3x2fNV)>(get_func("glMatrixLoad3x2fNV"));
    glMatrixLoad3x3fNV = reinterpret_cast<decltype(glMatrixLoad3x3fNV)>(get_func("glMatrixLoad3x3fNV"));
    glMatrixLoadIdentityEXT = reinterpret_cast<decltype(glMatrixLoadIdentityEXT)>(get_func("glMatrixLoadIdentityEXT"));
    glMatrixLoadTranspose3x3fNV = reinterpret_cast<decltype(glMatrixLoadTranspose3x3fNV)>(get_func("glMatrixLoadTranspose3x3fNV"));
    glMatrixLoadTransposedEXT = reinterpret_cast<decltype(glMatrixLoadTransposedEXT)>(get_func("glMatrixLoadTransposedEXT"));
    glMatrixLoadTransposefEXT = reinterpret_cast<decltype(glMatrixLoadTransposefEXT)>(get_func("glMatrixLoadTransposefEXT"));
    glMatrixLoaddEXT = reinterpret_cast<decltype(glMatrixLoaddEXT)>(get_func("glMatrixLoaddEXT"));
    glMatrixLoadfEXT = reinterpret_cast<decltype(glMatrixLoadfEXT)>(get_func("glMatrixLoadfEXT"));
    glMatrixMult3x2fNV = reinterpret_cast<decltype(glMatrixMult3x2fNV)>(get_func("glMatrixMult3x2fNV"));
    glMatrixMult3x3fNV = reinterpret_cast<decltype(glMatrixMult3x3fNV)>(get_func("glMatrixMult3x3fNV"));
    glMatrixMultTranspose3x3fNV = reinterpret_cast<decltype(glMatrixMultTranspose3x3fNV)>(get_func("glMatrixMultTranspose3x3fNV"));
    glMatrixMultTransposedEXT = reinterpret_cast<decltype(glMatrixMultTransposedEXT)>(get_func("glMatrixMultTransposedEXT"));
    glMatrixMultTransposefEXT = reinterpret_cast<decltype(glMatrixMultTransposefEXT)>(get_func("glMatrixMultTransposefEXT"));
    glMatrixMultdEXT = reinterpret_cast<decltype(glMatrixMultdEXT)>(get_func("glMatrixMultdEXT"));
    glMatrixMultfEXT = reinterpret_cast<decltype(glMatrixMultfEXT)>(get_func("glMatrixMultfEXT"));
    glMatrixOrthoEXT = reinterpret_cast<decltype(glMatrixOrthoEXT)>(get_func("glMatrixOrthoEXT"));
    glMatrixPopEXT = reinterpret_cast<decltype(glMatrixPopEXT)>(get_func("glMatrixPopEXT"));
    glMatrixPushEXT = reinterpret_cast<decltype(glMatrixPushEXT)>(get_func("glMatrixPushEXT"));
    glMatrixRotatedEXT = reinterpret_cast<decltype(glMatrixRotatedEXT)>(get_func("glMatrixRotatedEXT"));
    glMatrixRotatefEXT = reinterpret_cast<decltype(glMatrixRotatefEXT)>(get_func("glMatrixRotatefEXT"));
    glMatrixScaledEXT = reinterpret_cast<decltype(glMatrixScaledEXT)>(get_func("glMatrixScaledEXT"));
    glMatrixScalefEXT = reinterpret_cast<decltype(glMatrixScalefEXT)>(get_func("glMatrixScalefEXT"));
    glMatrixTranslatedEXT = reinterpret_cast<decltype(glMatrixTranslatedEXT)>(get_func("glMatrixTranslatedEXT"));
    glMatrixTranslatefEXT = reinterpret_cast<decltype(glMatrixTranslatefEXT)>(get_func("glMatrixTranslatefEXT"));
    glMemoryBarrier = reinterpret_cast<decltype(glMemoryBarrier)>(get_func("glMemoryBarrier"));
    glMemoryBarrierByRegion = reinterpret_cast<decltype(glMemoryBarrierByRegion)>(get_func("glMemoryBarrierByRegion"));
    glMinSampleShading = reinterpret_cast<decltype(glMinSampleShading)>(get_func("glMinSampleShading"));
    glMultiDrawArrays = reinterpret_cast<decltype(glMultiDrawArrays)>(get_func("glMultiDrawArrays"));
    glMultiDrawArraysIndirect = reinterpret_cast<decltype(glMultiDrawArraysIndirect)>(get_func("glMultiDrawArraysIndirect"));
    glMultiDrawArraysIndirectBindlessCountNV = reinterpret_cast<decltype(glMultiDrawArraysIndirectBindlessCountNV)>(get_func("glMultiDrawArraysIndirectBindlessCountNV"));
    glMultiDrawArraysIndirectBindlessNV = reinterpret_cast<decltype(glMultiDrawArraysIndirectBindlessNV)>(get_func("glMultiDrawArraysIndirectBindlessNV"));
    glMultiDrawArraysIndirectCount = reinterpret_cast<decltype(glMultiDrawArraysIndirectCount)>(get_func("glMultiDrawArraysIndirectCount"));
    glMultiDrawArraysIndirectCountARB = reinterpret_cast<decltype(glMultiDrawArraysIndirectCountARB)>(get_func("glMultiDrawArraysIndirectCountARB"));
    glMultiDrawElements = reinterpret_cast<decltype(glMultiDrawElements)>(get_func("glMultiDrawElements"));
    glMultiDrawElementsBaseVertex = reinterpret_cast<decltype(glMultiDrawElementsBaseVertex)>(get_func("glMultiDrawElementsBaseVertex"));
    glMultiDrawElementsIndirect = reinterpret_cast<decltype(glMultiDrawElementsIndirect)>(get_func("glMultiDrawElementsIndirect"));
    glMultiDrawElementsIndirectBindlessCountNV = reinterpret_cast<decltype(glMultiDrawElementsIndirectBindlessCountNV)>(get_func("glMultiDrawElementsIndirectBindlessCountNV"));
    glMultiDrawElementsIndirectBindlessNV = reinterpret_cast<decltype(glMultiDrawElementsIndirectBindlessNV)>(get_func("glMultiDrawElementsIndirectBindlessNV"));
    glMultiDrawElementsIndirectCount = reinterpret_cast<decltype(glMultiDrawElementsIndirectCount)>(get_func("glMultiDrawElementsIndirectCount"));
    glMultiDrawElementsIndirectCountARB = reinterpret_cast<decltype(glMultiDrawElementsIndirectCountARB)>(get_func("glMultiDrawElementsIndirectCountARB"));
    glNamedBufferData = reinterpret_cast<decltype(glNamedBufferData)>(get_func("glNamedBufferData"));
    glNamedBufferStorage = reinterpret_cast<decltype(glNamedBufferStorage)>(get_func("glNamedBufferStorage"));
    glNamedBufferSubData = reinterpret_cast<decltype(glNamedBufferSubData)>(get_func("glNamedBufferSubData"));
    glNamedFramebufferDrawBuffer = reinterpret_cast<decltype(glNamedFramebufferDrawBuffer)>(get_func("glNamedFramebufferDrawBuffer"));
    glNamedFramebufferDrawBuffers = reinterpret_cast<decltype(glNamedFramebufferDrawBuffers)>(get_func("glNamedFramebufferDrawBuffers"));
    glNamedFramebufferParameteri = reinterpret_cast<decltype(glNamedFramebufferParameteri)>(get_func("glNamedFramebufferParameteri"));
    glNamedFramebufferReadBuffer = reinterpret_cast<decltype(glNamedFramebufferReadBuffer)>(get_func("glNamedFramebufferReadBuffer"));
    glNamedFramebufferRenderbuffer = reinterpret_cast<decltype(glNamedFramebufferRenderbuffer)>(get_func("glNamedFramebufferRenderbuffer"));
    glNamedFramebufferTexture = reinterpret_cast<decltype(glNamedFramebufferTexture)>(get_func("glNamedFramebufferTexture"));
    glNamedFramebufferTextureLayer = reinterpret_cast<decltype(glNamedFramebufferTextureLayer)>(get_func("glNamedFramebufferTextureLayer"));
    glNamedRenderbufferStorage = reinterpret_cast<decltype(glNamedRenderbufferStorage)>(get_func("glNamedRenderbufferStorage"));
    glNamedRenderbufferStorageMultisample = reinterpret_cast<decltype(glNamedRenderbufferStorageMultisample)>(get_func("glNamedRenderbufferStorageMultisample"));
    glNormalFormatNV = reinterpret_cast<decltype(glNormalFormatNV)>(get_func("glNormalFormatNV"));
    glObjectLabel = reinterpret_cast<decltype(glObjectLabel)>(get_func("glObjectLabel"));
    glObjectLabelKHR = reinterpret_cast<decltype(glObjectLabelKHR)>(get_func("glObjectLabelKHR"));
    glObjectPtrLabel = reinterpret_cast<decltype(glObjectPtrLabel)>(get_func("glObjectPtrLabel"));
    glObjectPtrLabelKHR = reinterpret_cast<decltype(glObjectPtrLabelKHR)>(get_func("glObjectPtrLabelKHR"));
    glPatchParameterfv = reinterpret_cast<decltype(glPatchParameterfv)>(get_func("glPatchParameterfv"));
    glPatchParameteri = reinterpret_cast<decltype(glPatchParameteri)>(get_func("glPatchParameteri"));
    glPathColorGenNV = reinterpret_cast<decltype(glPathColorGenNV)>(get_func("glPathColorGenNV"));
    glPathCommandsNV = reinterpret_cast<decltype(glPathCommandsNV)>(get_func("glPathCommandsNV"));
    glPathCoordsNV = reinterpret_cast<decltype(glPathCoordsNV)>(get_func("glPathCoordsNV"));
    glPathCoverDepthFuncNV = reinterpret_cast<decltype(glPathCoverDepthFuncNV)>(get_func("glPathCoverDepthFuncNV"));
    glPathDashArrayNV = reinterpret_cast<decltype(glPathDashArrayNV)>(get_func("glPathDashArrayNV"));
    glPathFogGenNV = reinterpret_cast<decltype(glPathFogGenNV)>(get_func("glPathFogGenNV"));
    glPathGlyphIndexArrayNV = reinterpret_cast<decltype(glPathGlyphIndexArrayNV)>(get_func("glPathGlyphIndexArrayNV"));
    glPathGlyphIndexRangeNV = reinterpret_cast<decltype(glPathGlyphIndexRangeNV)>(get_func("glPathGlyphIndexRangeNV"));
    glPathGlyphRangeNV = reinterpret_cast<decltype(glPathGlyphRangeNV)>(get_func("glPathGlyphRangeNV"));
    glPathGlyphsNV = reinterpret_cast<decltype(glPathGlyphsNV)>(get_func("glPathGlyphsNV"));
    glPathMemoryGlyphIndexArrayNV = reinterpret_cast<decltype(glPathMemoryGlyphIndexArrayNV)>(get_func("glPathMemoryGlyphIndexArrayNV"));
    glPathParameterfNV = reinterpret_cast<decltype(glPathParameterfNV)>(get_func("glPathParameterfNV"));
    glPathParameterfvNV = reinterpret_cast<decltype(glPathParameterfvNV)>(get_func("glPathParameterfvNV"));
    glPathParameteriNV = reinterpret_cast<decltype(glPathParameteriNV)>(get_func("glPathParameteriNV"));
    glPathParameterivNV = reinterpret_cast<decltype(glPathParameterivNV)>(get_func("glPathParameterivNV"));
    glPathStencilDepthOffsetNV = reinterpret_cast<decltype(glPathStencilDepthOffsetNV)>(get_func("glPathStencilDepthOffsetNV"));
    glPathStencilFuncNV = reinterpret_cast<decltype(glPathStencilFuncNV)>(get_func("glPathStencilFuncNV"));
    glPathStringNV = reinterpret_cast<decltype(glPathStringNV)>(get_func("glPathStringNV"));
    glPathSubCommandsNV = reinterpret_cast<decltype(glPathSubCommandsNV)>(get_func("glPathSubCommandsNV"));
    glPathSubCoordsNV = reinterpret_cast<decltype(glPathSubCoordsNV)>(get_func("glPathSubCoordsNV"));
    glPathTexGenNV = reinterpret_cast<decltype(glPathTexGenNV)>(get_func("glPathTexGenNV"));
    glPauseTransformFeedback = reinterpret_cast<decltype(glPauseTransformFeedback)>(get_func("glPauseTransformFeedback"));
    glPixelStoref = reinterpret_cast<decltype(glPixelStoref)>(get_func("glPixelStoref"));
    glPixelStorei = reinterpret_cast<decltype(glPixelStorei)>(get_func("glPixelStorei"));
    glPointAlongPathNV = reinterpret_cast<decltype(glPointAlongPathNV)>(get_func("glPointAlongPathNV"));
    glPointParameterf = reinterpret_cast<decltype(glPointParameterf)>(get_func("glPointParameterf"));
    glPointParameterfv = reinterpret_cast<decltype(glPointParameterfv)>(get_func("glPointParameterfv"));
    glPointParameteri = reinterpret_cast<decltype(glPointParameteri)>(get_func("glPointParameteri"));
    glPointParameteriv = reinterpret_cast<decltype(glPointParameteriv)>(get_func("glPointParameteriv"));
    glPointSize = reinterpret_cast<decltype(glPointSize)>(get_func("glPointSize"));
    glPolygonMode = reinterpret_cast<decltype(glPolygonMode)>(get_func("glPolygonMode"));
    glPolygonOffset = reinterpret_cast<decltype(glPolygonOffset)>(get_func("glPolygonOffset"));
    glPolygonOffsetClamp = reinterpret_cast<decltype(glPolygonOffsetClamp)>(get_func("glPolygonOffsetClamp"));
    glPopDebugGroup = reinterpret_cast<decltype(glPopDebugGroup)>(get_func("glPopDebugGroup"));
    glPopDebugGroupKHR = reinterpret_cast<decltype(glPopDebugGroupKHR)>(get_func("glPopDebugGroupKHR"));
    glPrimitiveRestartIndex = reinterpret_cast<decltype(glPrimitiveRestartIndex)>(get_func("glPrimitiveRestartIndex"));
    glProgramBinary = reinterpret_cast<decltype(glProgramBinary)>(get_func("glProgramBinary"));
    glProgramParameteri = reinterpret_cast<decltype(glProgramParameteri)>(get_func("glProgramParameteri"));
    glProgramPathFragmentInputGenNV = reinterpret_cast<decltype(glProgramPathFragmentInputGenNV)>(get_func("glProgramPathFragmentInputGenNV"));
    glProgramUniform1d = reinterpret_cast<decltype(glProgramUniform1d)>(get_func("glProgramUniform1d"));
    glProgramUniform1dv = reinterpret_cast<decltype(glProgramUniform1dv)>(get_func("glProgramUniform1dv"));
    glProgramUniform1f = reinterpret_cast<decltype(glProgramUniform1f)>(get_func("glProgramUniform1f"));
    glProgramUniform1fv = reinterpret_cast<decltype(glProgramUniform1fv)>(get_func("glProgramUniform1fv"));
    glProgramUniform1i = reinterpret_cast<decltype(glProgramUniform1i)>(get_func("glProgramUniform1i"));
    glProgramUniform1iv = reinterpret_cast<decltype(glProgramUniform1iv)>(get_func("glProgramUniform1iv"));
    glProgramUniform1ui = reinterpret_cast<decltype(glProgramUniform1ui)>(get_func("glProgramUniform1ui"));
    glProgramUniform1uiv = reinterpret_cast<decltype(glProgramUniform1uiv)>(get_func("glProgramUniform1uiv"));
    glProgramUniform2d = reinterpret_cast<decltype(glProgramUniform2d)>(get_func("glProgramUniform2d"));
    glProgramUniform2dv = reinterpret_cast<decltype(glProgramUniform2dv)>(get_func("glProgramUniform2dv"));
    glProgramUniform2f = reinterpret_cast<decltype(glProgramUniform2f)>(get_func("glProgramUniform2f"));
    glProgramUniform2fv = reinterpret_cast<decltype(glProgramUniform2fv)>(get_func("glProgramUniform2fv"));
    glProgramUniform2i = reinterpret_cast<decltype(glProgramUniform2i)>(get_func("glProgramUniform2i"));
    glProgramUniform2iv = reinterpret_cast<decltype(glProgramUniform2iv)>(get_func("glProgramUniform2iv"));
    glProgramUniform2ui = reinterpret_cast<decltype(glProgramUniform2ui)>(get_func("glProgramUniform2ui"));
    glProgramUniform2uiv = reinterpret_cast<decltype(glProgramUniform2uiv)>(get_func("glProgramUniform2uiv"));
    glProgramUniform3d = reinterpret_cast<decltype(glProgramUniform3d)>(get_func("glProgramUniform3d"));
    glProgramUniform3dv = reinterpret_cast<decltype(glProgramUniform3dv)>(get_func("glProgramUniform3dv"));
    glProgramUniform3f = reinterpret_cast<decltype(glProgramUniform3f)>(get_func("glProgramUniform3f"));
    glProgramUniform3fv = reinterpret_cast<decltype(glProgramUniform3fv)>(get_func("glProgramUniform3fv"));
    glProgramUniform3i = reinterpret_cast<decltype(glProgramUniform3i)>(get_func("glProgramUniform3i"));
    glProgramUniform3iv = reinterpret_cast<decltype(glProgramUniform3iv)>(get_func("glProgramUniform3iv"));
    glProgramUniform3ui = reinterpret_cast<decltype(glProgramUniform3ui)>(get_func("glProgramUniform3ui"));
    glProgramUniform3uiv = reinterpret_cast<decltype(glProgramUniform3uiv)>(get_func("glProgramUniform3uiv"));
    glProgramUniform4d = reinterpret_cast<decltype(glProgramUniform4d)>(get_func("glProgramUniform4d"));
    glProgramUniform4dv = reinterpret_cast<decltype(glProgramUniform4dv)>(get_func("glProgramUniform4dv"));
    glProgramUniform4f = reinterpret_cast<decltype(glProgramUniform4f)>(get_func("glProgramUniform4f"));
    glProgramUniform4fv = reinterpret_cast<decltype(glProgramUniform4fv)>(get_func("glProgramUniform4fv"));
    glProgramUniform4i = reinterpret_cast<decltype(glProgramUniform4i)>(get_func("glProgramUniform4i"));
    glProgramUniform4iv = reinterpret_cast<decltype(glProgramUniform4iv)>(get_func("glProgramUniform4iv"));
    glProgramUniform4ui = reinterpret_cast<decltype(glProgramUniform4ui)>(get_func("glProgramUniform4ui"));
    glProgramUniform4uiv = reinterpret_cast<decltype(glProgramUniform4uiv)>(get_func("glProgramUniform4uiv"));
    glProgramUniformHandleui64ARB = reinterpret_cast<decltype(glProgramUniformHandleui64ARB)>(get_func("glProgramUniformHandleui64ARB"));
    glProgramUniformHandleui64vARB = reinterpret_cast<decltype(glProgramUniformHandleui64vARB)>(get_func("glProgramUniformHandleui64vARB"));
    glProgramUniformMatrix2dv = reinterpret_cast<decltype(glProgramUniformMatrix2dv)>(get_func("glProgramUniformMatrix2dv"));
    glProgramUniformMatrix2fv = reinterpret_cast<decltype(glProgramUniformMatrix2fv)>(get_func("glProgramUniformMatrix2fv"));
    glProgramUniformMatrix2x3dv = reinterpret_cast<decltype(glProgramUniformMatrix2x3dv)>(get_func("glProgramUniformMatrix2x3dv"));
    glProgramUniformMatrix2x3fv = reinterpret_cast<decltype(glProgramUniformMatrix2x3fv)>(get_func("glProgramUniformMatrix2x3fv"));
    glProgramUniformMatrix2x4dv = reinterpret_cast<decltype(glProgramUniformMatrix2x4dv)>(get_func("glProgramUniformMatrix2x4dv"));
    glProgramUniformMatrix2x4fv = reinterpret_cast<decltype(glProgramUniformMatrix2x4fv)>(get_func("glProgramUniformMatrix2x4fv"));
    glProgramUniformMatrix3dv = reinterpret_cast<decltype(glProgramUniformMatrix3dv)>(get_func("glProgramUniformMatrix3dv"));
    glProgramUniformMatrix3fv = reinterpret_cast<decltype(glProgramUniformMatrix3fv)>(get_func("glProgramUniformMatrix3fv"));
    glProgramUniformMatrix3x2dv = reinterpret_cast<decltype(glProgramUniformMatrix3x2dv)>(get_func("glProgramUniformMatrix3x2dv"));
    glProgramUniformMatrix3x2fv = reinterpret_cast<decltype(glProgramUniformMatrix3x2fv)>(get_func("glProgramUniformMatrix3x2fv"));
    glProgramUniformMatrix3x4dv = reinterpret_cast<decltype(glProgramUniformMatrix3x4dv)>(get_func("glProgramUniformMatrix3x4dv"));
    glProgramUniformMatrix3x4fv = reinterpret_cast<decltype(glProgramUniformMatrix3x4fv)>(get_func("glProgramUniformMatrix3x4fv"));
    glProgramUniformMatrix4dv = reinterpret_cast<decltype(glProgramUniformMatrix4dv)>(get_func("glProgramUniformMatrix4dv"));
    glProgramUniformMatrix4fv = reinterpret_cast<decltype(glProgramUniformMatrix4fv)>(get_func("glProgramUniformMatrix4fv"));
    glProgramUniformMatrix4x2dv = reinterpret_cast<decltype(glProgramUniformMatrix4x2dv)>(get_func("glProgramUniformMatrix4x2dv"));
    glProgramUniformMatrix4x2fv = reinterpret_cast<decltype(glProgramUniformMatrix4x2fv)>(get_func("glProgramUniformMatrix4x2fv"));
    glProgramUniformMatrix4x3dv = reinterpret_cast<decltype(glProgramUniformMatrix4x3dv)>(get_func("glProgramUniformMatrix4x3dv"));
    glProgramUniformMatrix4x3fv = reinterpret_cast<decltype(glProgramUniformMatrix4x3fv)>(get_func("glProgramUniformMatrix4x3fv"));
    glProgramUniformui64NV = reinterpret_cast<decltype(glProgramUniformui64NV)>(get_func("glProgramUniformui64NV"));
    glProgramUniformui64vNV = reinterpret_cast<decltype(glProgramUniformui64vNV)>(get_func("glProgramUniformui64vNV"));
    glProvokingVertex = reinterpret_cast<decltype(glProvokingVertex)>(get_func("glProvokingVertex"));
    glPushDebugGroup = reinterpret_cast<decltype(glPushDebugGroup)>(get_func("glPushDebugGroup"));
    glPushDebugGroupKHR = reinterpret_cast<decltype(glPushDebugGroupKHR)>(get_func("glPushDebugGroupKHR"));
    glQueryCounter = reinterpret_cast<decltype(glQueryCounter)>(get_func("glQueryCounter"));
    glReadBuffer = reinterpret_cast<decltype(glReadBuffer)>(get_func("glReadBuffer"));
    glReadPixels = reinterpret_cast<decltype(glReadPixels)>(get_func("glReadPixels"));
    glReadnPixels = reinterpret_cast<decltype(glReadnPixels)>(get_func("glReadnPixels"));
    glReadnPixelsARB = reinterpret_cast<decltype(glReadnPixelsARB)>(get_func("glReadnPixelsARB"));
    glReleaseShaderCompiler = reinterpret_cast<decltype(glReleaseShaderCompiler)>(get_func("glReleaseShaderCompiler"));
    glRenderbufferStorage = reinterpret_cast<decltype(glRenderbufferStorage)>(get_func("glRenderbufferStorage"));
    glRenderbufferStorageMultisample = reinterpret_cast<decltype(glRenderbufferStorageMultisample)>(get_func("glRenderbufferStorageMultisample"));
    glResumeTransformFeedback = reinterpret_cast<decltype(glResumeTransformFeedback)>(get_func("glResumeTransformFeedback"));
    glSampleCoverage = reinterpret_cast<decltype(glSampleCoverage)>(get_func("glSampleCoverage"));
    glSampleMaski = reinterpret_cast<decltype(glSampleMaski)>(get_func("glSampleMaski"));
    glSamplerParameterIiv = reinterpret_cast<decltype(glSamplerParameterIiv)>(get_func("glSamplerParameterIiv"));
    glSamplerParameterIuiv = reinterpret_cast<decltype(glSamplerParameterIuiv)>(get_func("glSamplerParameterIuiv"));
    glSamplerParameterf = reinterpret_cast<decltype(glSamplerParameterf)>(get_func("glSamplerParameterf"));
    glSamplerParameterfv = reinterpret_cast<decltype(glSamplerParameterfv)>(get_func("glSamplerParameterfv"));
    glSamplerParameteri = reinterpret_cast<decltype(glSamplerParameteri)>(get_func("glSamplerParameteri"));
    glSamplerParameteriv = reinterpret_cast<decltype(glSamplerParameteriv)>(get_func("glSamplerParameteriv"));
    glScissor = reinterpret_cast<decltype(glScissor)>(get_func("glScissor"));
    glScissorArrayv = reinterpret_cast<decltype(glScissorArrayv)>(get_func("glScissorArrayv"));
    glScissorIndexed = reinterpret_cast<decltype(glScissorIndexed)>(get_func("glScissorIndexed"));
    glScissorIndexedv = reinterpret_cast<decltype(glScissorIndexedv)>(get_func("glScissorIndexedv"));
    glSecondaryColorFormatNV = reinterpret_cast<decltype(glSecondaryColorFormatNV)>(get_func("glSecondaryColorFormatNV"));
    glShaderBinary = reinterpret_cast<decltype(glShaderBinary)>(get_func("glShaderBinary"));
    glShaderSource = reinterpret_cast<decltype(glShaderSource)>(get_func("glShaderSource"));
    glShaderStorageBlockBinding = reinterpret_cast<decltype(glShaderStorageBlockBinding)>(get_func("glShaderStorageBlockBinding"));
    glSpecializeShader = reinterpret_cast<decltype(glSpecializeShader)>(get_func("glSpecializeShader"));
    glSpecializeShaderARB = reinterpret_cast<decltype(glSpecializeShaderARB)>(get_func("glSpecializeShaderARB"));
    glStateCaptureNV = reinterpret_cast<decltype(glStateCaptureNV)>(get_func("glStateCaptureNV"));
    glStencilFillPathInstancedNV = reinterpret_cast<decltype(glStencilFillPathInstancedNV)>(get_func("glStencilFillPathInstancedNV"));
    glStencilFillPathNV = reinterpret_cast<decltype(glStencilFillPathNV)>(get_func("glStencilFillPathNV"));
    glStencilFunc = reinterpret_cast<decltype(glStencilFunc)>(get_func("glStencilFunc"));
    glStencilFuncSeparate = reinterpret_cast<decltype(glStencilFuncSeparate)>(get_func("glStencilFuncSeparate"));
    glStencilMask = reinterpret_cast<decltype(glStencilMask)>(get_func("glStencilMask"));
    glStencilMaskSeparate = reinterpret_cast<decltype(glStencilMaskSeparate)>(get_func("glStencilMaskSeparate"));
    glStencilOp = reinterpret_cast<decltype(glStencilOp)>(get_func("glStencilOp"));
    glStencilOpSeparate = reinterpret_cast<decltype(glStencilOpSeparate)>(get_func("glStencilOpSeparate"));
    glStencilStrokePathInstancedNV = reinterpret_cast<decltype(glStencilStrokePathInstancedNV)>(get_func("glStencilStrokePathInstancedNV"));
    glStencilStrokePathNV = reinterpret_cast<decltype(glStencilStrokePathNV)>(get_func("glStencilStrokePathNV"));
    glStencilThenCoverFillPathInstancedNV = reinterpret_cast<decltype(glStencilThenCoverFillPathInstancedNV)>(get_func("glStencilThenCoverFillPathInstancedNV"));
    glStencilThenCoverFillPathNV = reinterpret_cast<decltype(glStencilThenCoverFillPathNV)>(get_func("glStencilThenCoverFillPathNV"));
    glStencilThenCoverStrokePathInstancedNV = reinterpret_cast<decltype(glStencilThenCoverStrokePathInstancedNV)>(get_func("glStencilThenCoverStrokePathInstancedNV"));
    glStencilThenCoverStrokePathNV = reinterpret_cast<decltype(glStencilThenCoverStrokePathNV)>(get_func("glStencilThenCoverStrokePathNV"));
    glTexBuffer = reinterpret_cast<decltype(glTexBuffer)>(get_func("glTexBuffer"));
    glTexBufferRange = reinterpret_cast<decltype(glTexBufferRange)>(get_func("glTexBufferRange"));
    glTexCoordFormatNV = reinterpret_cast<decltype(glTexCoordFormatNV)>(get_func("glTexCoordFormatNV"));
    glTexImage1D = reinterpret_cast<decltype(glTexImage1D)>(get_func("glTexImage1D"));
    glTexImage2D = reinterpret_cast<decltype(glTexImage2D)>(get_func("glTexImage2D"));
    glTexImage2DMultisample = reinterpret_cast<decltype(glTexImage2DMultisample)>(get_func("glTexImage2DMultisample"));
    glTexImage3D = reinterpret_cast<decltype(glTexImage3D)>(get_func("glTexImage3D"));
    glTexImage3DMultisample = reinterpret_cast<decltype(glTexImage3DMultisample)>(get_func("glTexImage3DMultisample"));
    glTexParameterIiv = reinterpret_cast<decltype(glTexParameterIiv)>(get_func("glTexParameterIiv"));
    glTexParameterIuiv = reinterpret_cast<decltype(glTexParameterIuiv)>(get_func("glTexParameterIuiv"));
    glTexParameterf = reinterpret_cast<decltype(glTexParameterf)>(get_func("glTexParameterf"));
    glTexParameterfv = reinterpret_cast<decltype(glTexParameterfv)>(get_func("glTexParameterfv"));
    glTexParameteri = reinterpret_cast<decltype(glTexParameteri)>(get_func("glTexParameteri"));
    glTexParameteriv = reinterpret_cast<decltype(glTexParameteriv)>(get_func("glTexParameteriv"));
    glTexStorage1D = reinterpret_cast<decltype(glTexStorage1D)>(get_func("glTexStorage1D"));
    glTexStorage2D = reinterpret_cast<decltype(glTexStorage2D)>(get_func("glTexStorage2D"));
    glTexStorage2DMultisample = reinterpret_cast<decltype(glTexStorage2DMultisample)>(get_func("glTexStorage2DMultisample"));
    glTexStorage3D = reinterpret_cast<decltype(glTexStorage3D)>(get_func("glTexStorage3D"));
    glTexStorage3DMultisample = reinterpret_cast<decltype(glTexStorage3DMultisample)>(get_func("glTexStorage3DMultisample"));
    glTexSubImage1D = reinterpret_cast<decltype(glTexSubImage1D)>(get_func("glTexSubImage1D"));
    glTexSubImage2D = reinterpret_cast<decltype(glTexSubImage2D)>(get_func("glTexSubImage2D"));
    glTexSubImage3D = reinterpret_cast<decltype(glTexSubImage3D)>(get_func("glTexSubImage3D"));
    glTextureBarrier = reinterpret_cast<decltype(glTextureBarrier)>(get_func("glTextureBarrier"));
    glTextureBuffer = reinterpret_cast<decltype(glTextureBuffer)>(get_func("glTextureBuffer"));
    glTextureBufferRange = reinterpret_cast<decltype(glTextureBufferRange)>(get_func("glTextureBufferRange"));
    glTextureParameterIiv = reinterpret_cast<decltype(glTextureParameterIiv)>(get_func("glTextureParameterIiv"));
    glTextureParameterIuiv = reinterpret_cast<decltype(glTextureParameterIuiv)>(get_func("glTextureParameterIuiv"));
    glTextureParameterf = reinterpret_cast<decltype(glTextureParameterf)>(get_func("glTextureParameterf"));
    glTextureParameterfv = reinterpret_cast<decltype(glTextureParameterfv)>(get_func("glTextureParameterfv"));
    glTextureParameteri = reinterpret_cast<decltype(glTextureParameteri)>(get_func("glTextureParameteri"));
    glTextureParameteriv = reinterpret_cast<decltype(glTextureParameteriv)>(get_func("glTextureParameteriv"));
    glTextureStorage1D = reinterpret_cast<decltype(glTextureStorage1D)>(get_func("glTextureStorage1D"));
    glTextureStorage2D = reinterpret_cast<decltype(glTextureStorage2D)>(get_func("glTextureStorage2D"));
    glTextureStorage2DMultisample = reinterpret_cast<decltype(glTextureStorage2DMultisample)>(get_func("glTextureStorage2DMultisample"));
    glTextureStorage3D = reinterpret_cast<decltype(glTextureStorage3D)>(get_func("glTextureStorage3D"));
    glTextureStorage3DMultisample = reinterpret_cast<decltype(glTextureStorage3DMultisample)>(get_func("glTextureStorage3DMultisample"));
    glTextureSubImage1D = reinterpret_cast<decltype(glTextureSubImage1D)>(get_func("glTextureSubImage1D"));
    glTextureSubImage2D = reinterpret_cast<decltype(glTextureSubImage2D)>(get_func("glTextureSubImage2D"));
    glTextureSubImage3D = reinterpret_cast<decltype(glTextureSubImage3D)>(get_func("glTextureSubImage3D"));
    glTextureView = reinterpret_cast<decltype(glTextureView)>(get_func("glTextureView"));
    glTransformFeedbackBufferBase = reinterpret_cast<decltype(glTransformFeedbackBufferBase)>(get_func("glTransformFeedbackBufferBase"));
    glTransformFeedbackBufferRange = reinterpret_cast<decltype(glTransformFeedbackBufferRange)>(get_func("glTransformFeedbackBufferRange"));
    glTransformFeedbackVaryings = reinterpret_cast<decltype(glTransformFeedbackVaryings)>(get_func("glTransformFeedbackVaryings"));
    glTransformPathNV = reinterpret_cast<decltype(glTransformPathNV)>(get_func("glTransformPathNV"));
    glUniform1d = reinterpret_cast<decltype(glUniform1d)>(get_func("glUniform1d"));
    glUniform1dv = reinterpret_cast<decltype(glUniform1dv)>(get_func("glUniform1dv"));
    glUniform1f = reinterpret_cast<decltype(glUniform1f)>(get_func("glUniform1f"));
    glUniform1fv = reinterpret_cast<decltype(glUniform1fv)>(get_func("glUniform1fv"));
    glUniform1i = reinterpret_cast<decltype(glUniform1i)>(get_func("glUniform1i"));
    glUniform1iv = reinterpret_cast<decltype(glUniform1iv)>(get_func("glUniform1iv"));
    glUniform1ui = reinterpret_cast<decltype(glUniform1ui)>(get_func("glUniform1ui"));
    glUniform1uiv = reinterpret_cast<decltype(glUniform1uiv)>(get_func("glUniform1uiv"));
    glUniform2d = reinterpret_cast<decltype(glUniform2d)>(get_func("glUniform2d"));
    glUniform2dv = reinterpret_cast<decltype(glUniform2dv)>(get_func("glUniform2dv"));
    glUniform2f = reinterpret_cast<decltype(glUniform2f)>(get_func("glUniform2f"));
    glUniform2fv = reinterpret_cast<decltype(glUniform2fv)>(get_func("glUniform2fv"));
    glUniform2i = reinterpret_cast<decltype(glUniform2i)>(get_func("glUniform2i"));
    glUniform2iv = reinterpret_cast<decltype(glUniform2iv)>(get_func("glUniform2iv"));
    glUniform2ui = reinterpret_cast<decltype(glUniform2ui)>(get_func("glUniform2ui"));
    glUniform2uiv = reinterpret_cast<decltype(glUniform2uiv)>(get_func("glUniform2uiv"));
    glUniform3d = reinterpret_cast<decltype(glUniform3d)>(get_func("glUniform3d"));
    glUniform3dv = reinterpret_cast<decltype(glUniform3dv)>(get_func("glUniform3dv"));
    glUniform3f = reinterpret_cast<decltype(glUniform3f)>(get_func("glUniform3f"));
    glUniform3fv = reinterpret_cast<decltype(glUniform3fv)>(get_func("glUniform3fv"));
    glUniform3i = reinterpret_cast<decltype(glUniform3i)>(get_func("glUniform3i"));
    glUniform3iv = reinterpret_cast<decltype(glUniform3iv)>(get_func("glUniform3iv"));
    glUniform3ui = reinterpret_cast<decltype(glUniform3ui)>(get_func("glUniform3ui"));
    glUniform3uiv = reinterpret_cast<decltype(glUniform3uiv)>(get_func("glUniform3uiv"));
    glUniform4d = reinterpret_cast<decltype(glUniform4d)>(get_func("glUniform4d"));
    glUniform4dv = reinterpret_cast<decltype(glUniform4dv)>(get_func("glUniform4dv"));
    glUniform4f = reinterpret_cast<decltype(glUniform4f)>(get_func("glUniform4f"));
    glUniform4fv = reinterpret_cast<decltype(glUniform4fv)>(get_func("glUniform4fv"));
    glUniform4i = reinterpret_cast<decltype(glUniform4i)>(get_func("glUniform4i"));
    glUniform4iv = reinterpret_cast<decltype(glUniform4iv)>(get_func("glUniform4iv"));
    glUniform4ui = reinterpret_cast<decltype(glUniform4ui)>(get_func("glUniform4ui"));
    glUniform4uiv = reinterpret_cast<decltype(glUniform4uiv)>(get_func("glUniform4uiv"));
    glUniformBlockBinding = reinterpret_cast<decltype(glUniformBlockBinding)>(get_func("glUniformBlockBinding"));
    glUniformHandleui64ARB = reinterpret_cast<decltype(glUniformHandleui64ARB)>(get_func("glUniformHandleui64ARB"));
    glUniformHandleui64vARB = reinterpret_cast<decltype(glUniformHandleui64vARB)>(get_func("glUniformHandleui64vARB"));
    glUniformMatrix2dv = reinterpret_cast<decltype(glUniformMatrix2dv)>(get_func("glUniformMatrix2dv"));
    glUniformMatrix2fv = reinterpret_cast<decltype(glUniformMatrix2fv)>(get_func("glUniformMatrix2fv"));
    glUniformMatrix2x3dv = reinterpret_cast<decltype(glUniformMatrix2x3dv)>(get_func("glUniformMatrix2x3dv"));
    glUniformMatrix2x3fv = reinterpret_cast<decltype(glUniformMatrix2x3fv)>(get_func("glUniformMatrix2x3fv"));
    glUniformMatrix2x4dv = reinterpret_cast<decltype(glUniformMatrix2x4dv)>(get_func("glUniformMatrix2x4dv"));
    glUniformMatrix2x4fv = reinterpret_cast<decltype(glUniformMatrix2x4fv)>(get_func("glUniformMatrix2x4fv"));
    glUniformMatrix3dv = reinterpret_cast<decltype(glUniformMatrix3dv)>(get_func("glUniformMatrix3dv"));
    glUniformMatrix3fv = reinterpret_cast<decltype(glUniformMatrix3fv)>(get_func("glUniformMatrix3fv"));
    glUniformMatrix3x2dv = reinterpret_cast<decltype(glUniformMatrix3x2dv)>(get_func("glUniformMatrix3x2dv"));
    glUniformMatrix3x2fv = reinterpret_cast<decltype(glUniformMatrix3x2fv)>(get_func("glUniformMatrix3x2fv"));
    glUniformMatrix3x4dv = reinterpret_cast<decltype(glUniformMatrix3x4dv)>(get_func("glUniformMatrix3x4dv"));
    glUniformMatrix3x4fv = reinterpret_cast<decltype(glUniformMatrix3x4fv)>(get_func("glUniformMatrix3x4fv"));
    glUniformMatrix4dv = reinterpret_cast<decltype(glUniformMatrix4dv)>(get_func("glUniformMatrix4dv"));
    glUniformMatrix4fv = reinterpret_cast<decltype(glUniformMatrix4fv)>(get_func("glUniformMatrix4fv"));
    glUniformMatrix4x2dv = reinterpret_cast<decltype(glUniformMatrix4x2dv)>(get_func("glUniformMatrix4x2dv"));
    glUniformMatrix4x2fv = reinterpret_cast<decltype(glUniformMatrix4x2fv)>(get_func("glUniformMatrix4x2fv"));
    glUniformMatrix4x3dv = reinterpret_cast<decltype(glUniformMatrix4x3dv)>(get_func("glUniformMatrix4x3dv"));
    glUniformMatrix4x3fv = reinterpret_cast<decltype(glUniformMatrix4x3fv)>(get_func("glUniformMatrix4x3fv"));
    glUniformSubroutinesuiv = reinterpret_cast<decltype(glUniformSubroutinesuiv)>(get_func("glUniformSubroutinesuiv"));
    glUniformui64NV = reinterpret_cast<decltype(glUniformui64NV)>(get_func("glUniformui64NV"));
    glUniformui64vNV = reinterpret_cast<decltype(glUniformui64vNV)>(get_func("glUniformui64vNV"));
    glUnmapBuffer = reinterpret_cast<decltype(glUnmapBuffer)>(get_func("glUnmapBuffer"));
    glUnmapNamedBuffer = reinterpret_cast<decltype(glUnmapNamedBuffer)>(get_func("glUnmapNamedBuffer"));
    glUseProgram = reinterpret_cast<decltype(glUseProgram)>(get_func("glUseProgram"));
    glUseProgramStages = reinterpret_cast<decltype(glUseProgramStages)>(get_func("glUseProgramStages"));
    glValidateProgram = reinterpret_cast<decltype(glValidateProgram)>(get_func("glValidateProgram"));
    glValidateProgramPipeline = reinterpret_cast<decltype(glValidateProgramPipeline)>(get_func("glValidateProgramPipeline"));
    glVertexArrayAttribBinding = reinterpret_cast<decltype(glVertexArrayAttribBinding)>(get_func("glVertexArrayAttribBinding"));
    glVertexArrayAttribFormat = reinterpret_cast<decltype(glVertexArrayAttribFormat)>(get_func("glVertexArrayAttribFormat"));
    glVertexArrayAttribIFormat = reinterpret_cast<decltype(glVertexArrayAttribIFormat)>(get_func("glVertexArrayAttribIFormat"));
    glVertexArrayAttribLFormat = reinterpret_cast<decltype(glVertexArrayAttribLFormat)>(get_func("glVertexArrayAttribLFormat"));
    glVertexArrayBindingDivisor = reinterpret_cast<decltype(glVertexArrayBindingDivisor)>(get_func("glVertexArrayBindingDivisor"));
    glVertexArrayElementBuffer = reinterpret_cast<decltype(glVertexArrayElementBuffer)>(get_func("glVertexArrayElementBuffer"));
    glVertexArrayVertexBuffer = reinterpret_cast<decltype(glVertexArrayVertexBuffer)>(get_func("glVertexArrayVertexBuffer"));
    glVertexArrayVertexBuffers = reinterpret_cast<decltype(glVertexArrayVertexBuffers)>(get_func("glVertexArrayVertexBuffers"));
    glVertexAttrib1d = reinterpret_cast<decltype(glVertexAttrib1d)>(get_func("glVertexAttrib1d"));
    glVertexAttrib1dv = reinterpret_cast<decltype(glVertexAttrib1dv)>(get_func("glVertexAttrib1dv"));
    glVertexAttrib1f = reinterpret_cast<decltype(glVertexAttrib1f)>(get_func("glVertexAttrib1f"));
    glVertexAttrib1fv = reinterpret_cast<decltype(glVertexAttrib1fv)>(get_func("glVertexAttrib1fv"));
    glVertexAttrib1s = reinterpret_cast<decltype(glVertexAttrib1s)>(get_func("glVertexAttrib1s"));
    glVertexAttrib1sv = reinterpret_cast<decltype(glVertexAttrib1sv)>(get_func("glVertexAttrib1sv"));
    glVertexAttrib2d = reinterpret_cast<decltype(glVertexAttrib2d)>(get_func("glVertexAttrib2d"));
    glVertexAttrib2dv = reinterpret_cast<decltype(glVertexAttrib2dv)>(get_func("glVertexAttrib2dv"));
    glVertexAttrib2f = reinterpret_cast<decltype(glVertexAttrib2f)>(get_func("glVertexAttrib2f"));
    glVertexAttrib2fv = reinterpret_cast<decltype(glVertexAttrib2fv)>(get_func("glVertexAttrib2fv"));
    glVertexAttrib2s = reinterpret_cast<decltype(glVertexAttrib2s)>(get_func("glVertexAttrib2s"));
    glVertexAttrib2sv = reinterpret_cast<decltype(glVertexAttrib2sv)>(get_func("glVertexAttrib2sv"));
    glVertexAttrib3d = reinterpret_cast<decltype(glVertexAttrib3d)>(get_func("glVertexAttrib3d"));
    glVertexAttrib3dv = reinterpret_cast<decltype(glVertexAttrib3dv)>(get_func("glVertexAttrib3dv"));
    glVertexAttrib3f = reinterpret_cast<decltype(glVertexAttrib3f)>(get_func("glVertexAttrib3f"));
    glVertexAttrib3fv = reinterpret_cast<decltype(glVertexAttrib3fv)>(get_func("glVertexAttrib3fv"));
    glVertexAttrib3s = reinterpret_cast<decltype(glVertexAttrib3s)>(get_func("glVertexAttrib3s"));
    glVertexAttrib3sv = reinterpret_cast<decltype(glVertexAttrib3sv)>(get_func("glVertexAttrib3sv"));
    glVertexAttrib4Nbv = reinterpret_cast<decltype(glVertexAttrib4Nbv)>(get_func("glVertexAttrib4Nbv"));
    glVertexAttrib4Niv = reinterpret_cast<decltype(glVertexAttrib4Niv)>(get_func("glVertexAttrib4Niv"));
    glVertexAttrib4Nsv = reinterpret_cast<decltype(glVertexAttrib4Nsv)>(get_func("glVertexAttrib4Nsv"));
    glVertexAttrib4Nub = reinterpret_cast<decltype(glVertexAttrib4Nub)>(get_func("glVertexAttrib4Nub"));
    glVertexAttrib4Nubv = reinterpret_cast<decltype(glVertexAttrib4Nubv)>(get_func("glVertexAttrib4Nubv"));
    glVertexAttrib4Nuiv = reinterpret_cast<decltype(glVertexAttrib4Nuiv)>(get_func("glVertexAttrib4Nuiv"));
    glVertexAttrib4Nusv = reinterpret_cast<decltype(glVertexAttrib4Nusv)>(get_func("glVertexAttrib4Nusv"));
    glVertexAttrib4bv = reinterpret_cast<decltype(glVertexAttrib4bv)>(get_func("glVertexAttrib4bv"));
    glVertexAttrib4d = reinterpret_cast<decltype(glVertexAttrib4d)>(get_func("glVertexAttrib4d"));
    glVertexAttrib4dv = reinterpret_cast<decltype(glVertexAttrib4dv)>(get_func("glVertexAttrib4dv"));
    glVertexAttrib4f = reinterpret_cast<decltype(glVertexAttrib4f)>(get_func("glVertexAttrib4f"));
    glVertexAttrib4fv = reinterpret_cast<decltype(glVertexAttrib4fv)>(get_func("glVertexAttrib4fv"));
    glVertexAttrib4iv = reinterpret_cast<decltype(glVertexAttrib4iv)>(get_func("glVertexAttrib4iv"));
    glVertexAttrib4s = reinterpret_cast<decltype(glVertexAttrib4s)>(get_func("glVertexAttrib4s"));
    glVertexAttrib4sv = reinterpret_cast<decltype(glVertexAttrib4sv)>(get_func("glVertexAttrib4sv"));
    glVertexAttrib4ubv = reinterpret_cast<decltype(glVertexAttrib4ubv)>(get_func("glVertexAttrib4ubv"));
    glVertexAttrib4uiv = reinterpret_cast<decltype(glVertexAttrib4uiv)>(get_func("glVertexAttrib4uiv"));
    glVertexAttrib4usv = reinterpret_cast<decltype(glVertexAttrib4usv)>(get_func("glVertexAttrib4usv"));
    glVertexAttribBinding = reinterpret_cast<decltype(glVertexAttribBinding)>(get_func("glVertexAttribBinding"));
    glVertexAttribDivisor = reinterpret_cast<decltype(glVertexAttribDivisor)>(get_func("glVertexAttribDivisor"));
    glVertexAttribFormat = reinterpret_cast<decltype(glVertexAttribFormat)>(get_func("glVertexAttribFormat"));
    glVertexAttribFormatNV = reinterpret_cast<decltype(glVertexAttribFormatNV)>(get_func("glVertexAttribFormatNV"));
    glVertexAttribI1i = reinterpret_cast<decltype(glVertexAttribI1i)>(get_func("glVertexAttribI1i"));
    glVertexAttribI1iv = reinterpret_cast<decltype(glVertexAttribI1iv)>(get_func("glVertexAttribI1iv"));
    glVertexAttribI1ui = reinterpret_cast<decltype(glVertexAttribI1ui)>(get_func("glVertexAttribI1ui"));
    glVertexAttribI1uiv = reinterpret_cast<decltype(glVertexAttribI1uiv)>(get_func("glVertexAttribI1uiv"));
    glVertexAttribI2i = reinterpret_cast<decltype(glVertexAttribI2i)>(get_func("glVertexAttribI2i"));
    glVertexAttribI2iv = reinterpret_cast<decltype(glVertexAttribI2iv)>(get_func("glVertexAttribI2iv"));
    glVertexAttribI2ui = reinterpret_cast<decltype(glVertexAttribI2ui)>(get_func("glVertexAttribI2ui"));
    glVertexAttribI2uiv = reinterpret_cast<decltype(glVertexAttribI2uiv)>(get_func("glVertexAttribI2uiv"));
    glVertexAttribI3i = reinterpret_cast<decltype(glVertexAttribI3i)>(get_func("glVertexAttribI3i"));
    glVertexAttribI3iv = reinterpret_cast<decltype(glVertexAttribI3iv)>(get_func("glVertexAttribI3iv"));
    glVertexAttribI3ui = reinterpret_cast<decltype(glVertexAttribI3ui)>(get_func("glVertexAttribI3ui"));
    glVertexAttribI3uiv = reinterpret_cast<decltype(glVertexAttribI3uiv)>(get_func("glVertexAttribI3uiv"));
    glVertexAttribI4bv = reinterpret_cast<decltype(glVertexAttribI4bv)>(get_func("glVertexAttribI4bv"));
    glVertexAttribI4i = reinterpret_cast<decltype(glVertexAttribI4i)>(get_func("glVertexAttribI4i"));
    glVertexAttribI4iv = reinterpret_cast<decltype(glVertexAttribI4iv)>(get_func("glVertexAttribI4iv"));
    glVertexAttribI4sv = reinterpret_cast<decltype(glVertexAttribI4sv)>(get_func("glVertexAttribI4sv"));
    glVertexAttribI4ubv = reinterpret_cast<decltype(glVertexAttribI4ubv)>(get_func("glVertexAttribI4ubv"));
    glVertexAttribI4ui = reinterpret_cast<decltype(glVertexAttribI4ui)>(get_func("glVertexAttribI4ui"));
    glVertexAttribI4uiv = reinterpret_cast<decltype(glVertexAttribI4uiv)>(get_func("glVertexAttribI4uiv"));
    glVertexAttribI4usv = reinterpret_cast<decltype(glVertexAttribI4usv)>(get_func("glVertexAttribI4usv"));
    glVertexAttribIFormat = reinterpret_cast<decltype(glVertexAttribIFormat)>(get_func("glVertexAttribIFormat"));
    glVertexAttribIFormatNV = reinterpret_cast<decltype(glVertexAttribIFormatNV)>(get_func("glVertexAttribIFormatNV"));
    glVertexAttribIPointer = reinterpret_cast<decltype(glVertexAttribIPointer)>(get_func("glVertexAttribIPointer"));
    glVertexAttribL1d = reinterpret_cast<decltype(glVertexAttribL1d)>(get_func("glVertexAttribL1d"));
    glVertexAttribL1dv = reinterpret_cast<decltype(glVertexAttribL1dv)>(get_func("glVertexAttribL1dv"));
    glVertexAttribL1ui64ARB = reinterpret_cast<decltype(glVertexAttribL1ui64ARB)>(get_func("glVertexAttribL1ui64ARB"));
    glVertexAttribL1ui64vARB = reinterpret_cast<decltype(glVertexAttribL1ui64vARB)>(get_func("glVertexAttribL1ui64vARB"));
    glVertexAttribL2d = reinterpret_cast<decltype(glVertexAttribL2d)>(get_func("glVertexAttribL2d"));
    glVertexAttribL2dv = reinterpret_cast<decltype(glVertexAttribL2dv)>(get_func("glVertexAttribL2dv"));
    glVertexAttribL3d = reinterpret_cast<decltype(glVertexAttribL3d)>(get_func("glVertexAttribL3d"));
    glVertexAttribL3dv = reinterpret_cast<decltype(glVertexAttribL3dv)>(get_func("glVertexAttribL3dv"));
    glVertexAttribL4d = reinterpret_cast<decltype(glVertexAttribL4d)>(get_func("glVertexAttribL4d"));
    glVertexAttribL4dv = reinterpret_cast<decltype(glVertexAttribL4dv)>(get_func("glVertexAttribL4dv"));
    glVertexAttribLFormat = reinterpret_cast<decltype(glVertexAttribLFormat)>(get_func("glVertexAttribLFormat"));
    glVertexAttribLPointer = reinterpret_cast<decltype(glVertexAttribLPointer)>(get_func("glVertexAttribLPointer"));
    glVertexAttribP1ui = reinterpret_cast<decltype(glVertexAttribP1ui)>(get_func("glVertexAttribP1ui"));
    glVertexAttribP1uiv = reinterpret_cast<decltype(glVertexAttribP1uiv)>(get_func("glVertexAttribP1uiv"));
    glVertexAttribP2ui = reinterpret_cast<decltype(glVertexAttribP2ui)>(get_func("glVertexAttribP2ui"));
    glVertexAttribP2uiv = reinterpret_cast<decltype(glVertexAttribP2uiv)>(get_func("glVertexAttribP2uiv"));
    glVertexAttribP3ui = reinterpret_cast<decltype(glVertexAttribP3ui)>(get_func("glVertexAttribP3ui"));
    glVertexAttribP3uiv = reinterpret_cast<decltype(glVertexAttribP3uiv)>(get_func("glVertexAttribP3uiv"));
    glVertexAttribP4ui = reinterpret_cast<decltype(glVertexAttribP4ui)>(get_func("glVertexAttribP4ui"));
    glVertexAttribP4uiv = reinterpret_cast<decltype(glVertexAttribP4uiv)>(get_func("glVertexAttribP4uiv"));
    glVertexAttribPointer = reinterpret_cast<decltype(glVertexAttribPointer)>(get_func("glVertexAttribPointer"));
    glVertexBindingDivisor = reinterpret_cast<decltype(glVertexBindingDivisor)>(get_func("glVertexBindingDivisor"));
    glVertexFormatNV = reinterpret_cast<decltype(glVertexFormatNV)>(get_func("glVertexFormatNV"));
    glViewport = reinterpret_cast<decltype(glViewport)>(get_func("glViewport"));
    glViewportArrayv = reinterpret_cast<decltype(glViewportArrayv)>(get_func("glViewportArrayv"));
    glViewportIndexedf = reinterpret_cast<decltype(glViewportIndexedf)>(get_func("glViewportIndexedf"));
    glViewportIndexedfv = reinterpret_cast<decltype(glViewportIndexedfv)>(get_func("glViewportIndexedfv"));
    glWaitSync = reinterpret_cast<decltype(glWaitSync)>(get_func("glWaitSync"));
    glWeightPathsNV = reinterpret_cast<decltype(glWeightPathsNV)>(get_func("glWeightPathsNV"));
}
