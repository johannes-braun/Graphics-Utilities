#ifndef __glad_gl_h_
#define __glad_gl_h_

// http://glad2.dav1d.de/#profile=gl%3Dcore&profile=gles1%3Dcommon&api=egl%3Dnone&api=gl%3D4.5&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&api=glx%3Dnone&api=wgl%3Dnone&extensions=GL_ARB_bindless_texture&extensions=GL_ARB_compute_variable_group_size&extensions=GL_ARB_gpu_shader_int64&extensions=GL_ARB_vertex_attrib_binding&extensions=GL_EXT_texture_filter_anisotropic&extensions=GL_KHR_debug&extensions=GL_NVX_gpu_memory_info&extensions=GL_NV_bindless_multi_draw_indirect&extensions=GL_NV_bindless_multi_draw_indirect_count&extensions=GL_NV_command_list&extensions=GL_NV_path_rendering&extensions=GL_NV_path_rendering_shared_edge&extensions=GL_NV_shader_buffer_load&extensions=GL_NV_shader_buffer_store&extensions=GL_NV_uniform_buffer_unified_memory&extensions=GL_NV_vertex_buffer_unified_memory&option=ALIAS&option=HEADER_ONLY&option=LOADER&language=c

#ifdef __gl_h_
    #error OpenGL header already included (API: gl), remove previous include!
#endif
#define __gl_h_


#define GLAD_GL
#define GLAD_OPTION_GL_LOADER
#define GLAD_OPTION_GL_ALIAS
#define GLAD_OPTION_GL_HEADER_ONLY

#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#endif
#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif
#ifdef __cplusplus
extern "C" {
#endif

#ifndef GLAPI
# if defined(GLAD_GLAPI_EXPORT)
#  if defined(_WIN32) || defined(__CYGWIN__)
#   if defined(GLAD_GLAPI_EXPORT_BUILD)
#    if defined(__GNUC__)
#     define GLAPI __attribute__ ((dllexport)) extern
#    else
#     define GLAPI __declspec(dllexport) extern
#    endif
#   else
#    if defined(__GNUC__)
#     define GLAPI __attribute__ ((dllimport)) extern
#    else
#     define GLAPI __declspec(dllimport) extern
#    endif
#   endif
#  elif defined(__GNUC__) && defined(GLAD_GLAPI_EXPORT_BUILD)
#   define GLAPI __attribute__ ((visibility ("default"))) extern
#  else
#   define GLAPI extern
#  endif
# else
#  define GLAPI extern
# endif
#endif

#define GL_VERSION_1_0 1
GLAPI int GLAD_GL_VERSION_1_0;
#define GL_VERSION_1_1 1
GLAPI int GLAD_GL_VERSION_1_1;
#define GL_VERSION_1_2 1
GLAPI int GLAD_GL_VERSION_1_2;
#define GL_VERSION_1_3 1
GLAPI int GLAD_GL_VERSION_1_3;
#define GL_VERSION_1_4 1
GLAPI int GLAD_GL_VERSION_1_4;
#define GL_VERSION_1_5 1
GLAPI int GLAD_GL_VERSION_1_5;
#define GL_VERSION_2_0 1
GLAPI int GLAD_GL_VERSION_2_0;
#define GL_VERSION_2_1 1
GLAPI int GLAD_GL_VERSION_2_1;
#define GL_VERSION_3_0 1
GLAPI int GLAD_GL_VERSION_3_0;
#define GL_VERSION_3_1 1
GLAPI int GLAD_GL_VERSION_3_1;
#define GL_VERSION_3_2 1
GLAPI int GLAD_GL_VERSION_3_2;
#define GL_VERSION_3_3 1
GLAPI int GLAD_GL_VERSION_3_3;
#define GL_VERSION_4_0 1
GLAPI int GLAD_GL_VERSION_4_0;
#define GL_VERSION_4_1 1
GLAPI int GLAD_GL_VERSION_4_1;
#define GL_VERSION_4_2 1
GLAPI int GLAD_GL_VERSION_4_2;
#define GL_VERSION_4_3 1
GLAPI int GLAD_GL_VERSION_4_3;
#define GL_VERSION_4_4 1
GLAPI int GLAD_GL_VERSION_4_4;
#define GL_VERSION_4_5 1
GLAPI int GLAD_GL_VERSION_4_5;
#define GL_NV_transform_feedback2 1
GLAPI int GLAD_GL_NV_transform_feedback2;
#define GL_ARB_texture_compression 1
GLAPI int GLAD_GL_ARB_texture_compression;
#define GL_NV_point_sprite 1
GLAPI int GLAD_GL_NV_point_sprite;
#define GL_ARB_get_texture_sub_image 1
GLAPI int GLAD_GL_ARB_get_texture_sub_image;
#define GL_AMD_multi_draw_indirect 1
GLAPI int GLAD_GL_AMD_multi_draw_indirect;
#define GL_EXT_blend_color 1
GLAPI int GLAD_GL_EXT_blend_color;
#define GL_ARB_texture_storage_multisample 1
GLAPI int GLAD_GL_ARB_texture_storage_multisample;
#define GL_ARB_clip_control 1
GLAPI int GLAD_GL_ARB_clip_control;
#define GL_ARB_instanced_arrays 1
GLAPI int GLAD_GL_ARB_instanced_arrays;
#define GL_EXT_vertex_attrib_64bit 1
GLAPI int GLAD_GL_EXT_vertex_attrib_64bit;
#define GL_ARB_vertex_attrib_binding 1
GLAPI int GLAD_GL_ARB_vertex_attrib_binding;
#define GL_ARB_framebuffer_no_attachments 1
GLAPI int GLAD_GL_ARB_framebuffer_no_attachments;
#define GL_ARB_texture_view 1
GLAPI int GLAD_GL_ARB_texture_view;
#define GL_ATI_separate_stencil 1
GLAPI int GLAD_GL_ATI_separate_stencil;
#define GL_EXT_texture_buffer_object 1
GLAPI int GLAD_GL_EXT_texture_buffer_object;
#define GL_NV_shader_buffer_store 1
GLAPI int GLAD_GL_NV_shader_buffer_store;
#define GL_ARB_shader_subroutine 1
GLAPI int GLAD_GL_ARB_shader_subroutine;
#define GL_EXT_direct_state_access 1
GLAPI int GLAD_GL_EXT_direct_state_access;
#define GL_ARB_program_interface_query 1
GLAPI int GLAD_GL_ARB_program_interface_query;
#define GL_ARB_copy_image 1
GLAPI int GLAD_GL_ARB_copy_image;
#define GL_EXT_texture_object 1
GLAPI int GLAD_GL_EXT_texture_object;
#define GL_NVX_conditional_render 1
GLAPI int GLAD_GL_NVX_conditional_render;
#define GL_ARB_copy_buffer 1
GLAPI int GLAD_GL_ARB_copy_buffer;
#define GL_EXT_blend_func_separate 1
GLAPI int GLAD_GL_EXT_blend_func_separate;
#define GL_ARB_buffer_storage 1
GLAPI int GLAD_GL_ARB_buffer_storage;
#define GL_ARB_occlusion_query 1
GLAPI int GLAD_GL_ARB_occlusion_query;
#define GL_ARB_multi_bind 1
GLAPI int GLAD_GL_ARB_multi_bind;
#define GL_KHR_robustness 1
GLAPI int GLAD_GL_KHR_robustness;
#define GL_ARB_provoking_vertex 1
GLAPI int GLAD_GL_ARB_provoking_vertex;
#define GL_EXT_gpu_shader4 1
GLAPI int GLAD_GL_EXT_gpu_shader4;
#define GL_NV_geometry_program4 1
GLAPI int GLAD_GL_NV_geometry_program4;
#define GL_NV_path_rendering_shared_edge 1
GLAPI int GLAD_GL_NV_path_rendering_shared_edge;
#define GL_NV_uniform_buffer_unified_memory 1
GLAPI int GLAD_GL_NV_uniform_buffer_unified_memory;
#define GL_ARB_internalformat_query2 1
GLAPI int GLAD_GL_ARB_internalformat_query2;
#define GL_NV_shader_buffer_load 1
GLAPI int GLAD_GL_NV_shader_buffer_load;
#define GL_EXT_vertex_array 1
GLAPI int GLAD_GL_EXT_vertex_array;
#define GL_NV_vertex_buffer_unified_memory 1
GLAPI int GLAD_GL_NV_vertex_buffer_unified_memory;
#define GL_EXT_blend_equation_separate 1
GLAPI int GLAD_GL_EXT_blend_equation_separate;
#define GL_EXT_provoking_vertex 1
GLAPI int GLAD_GL_EXT_provoking_vertex;
#define GL_ARB_point_parameters 1
GLAPI int GLAD_GL_ARB_point_parameters;
#define GL_ARB_clear_texture 1
GLAPI int GLAD_GL_ARB_clear_texture;
#define GL_ARB_shader_image_load_store 1
GLAPI int GLAD_GL_ARB_shader_image_load_store;
#define GL_EXT_multi_draw_arrays 1
GLAPI int GLAD_GL_EXT_multi_draw_arrays;
#define GL_ARB_shader_atomic_counters 1
GLAPI int GLAD_GL_ARB_shader_atomic_counters;
#define GL_ARB_texture_barrier 1
GLAPI int GLAD_GL_ARB_texture_barrier;
#define GL_NV_bindless_multi_draw_indirect 1
GLAPI int GLAD_GL_NV_bindless_multi_draw_indirect;
#define GL_EXT_transform_feedback 1
GLAPI int GLAD_GL_EXT_transform_feedback;
#define GL_ARB_transform_feedback2 1
GLAPI int GLAD_GL_ARB_transform_feedback2;
#define GL_ARB_transform_feedback3 1
GLAPI int GLAD_GL_ARB_transform_feedback3;
#define GL_ARB_vertex_program 1
GLAPI int GLAD_GL_ARB_vertex_program;
#define GL_ARB_robustness 1
GLAPI int GLAD_GL_ARB_robustness;
#define GL_ARB_timer_query 1
GLAPI int GLAD_GL_ARB_timer_query;
#define GL_ARB_vertex_array_object 1
GLAPI int GLAD_GL_ARB_vertex_array_object;
#define GL_ARB_vertex_buffer_object 1
GLAPI int GLAD_GL_ARB_vertex_buffer_object;
#define GL_ARB_map_buffer_range 1
GLAPI int GLAD_GL_ARB_map_buffer_range;
#define GL_ARB_geometry_shader4 1
GLAPI int GLAD_GL_ARB_geometry_shader4;
#define GL_ARB_texture_buffer_object 1
GLAPI int GLAD_GL_ARB_texture_buffer_object;
#define GL_EXT_timer_query 1
GLAPI int GLAD_GL_EXT_timer_query;
#define GL_KHR_debug 1
GLAPI int GLAD_GL_KHR_debug;
#define GL_ARB_get_program_binary 1
GLAPI int GLAD_GL_ARB_get_program_binary;
#define GL_ARB_transform_feedback_instanced 1
GLAPI int GLAD_GL_ARB_transform_feedback_instanced;
#define GL_SGIS_point_parameters 1
GLAPI int GLAD_GL_SGIS_point_parameters;
#define GL_EXT_geometry_shader4 1
GLAPI int GLAD_GL_EXT_geometry_shader4;
#define GL_EXT_framebuffer_object 1
GLAPI int GLAD_GL_EXT_framebuffer_object;
#define GL_ARB_draw_indirect 1
GLAPI int GLAD_GL_ARB_draw_indirect;
#define GL_NV_bindless_multi_draw_indirect_count 1
GLAPI int GLAD_GL_NV_bindless_multi_draw_indirect_count;
#define GL_ARB_ES2_compatibility 1
GLAPI int GLAD_GL_ARB_ES2_compatibility;
#define GL_NV_command_list 1
GLAPI int GLAD_GL_NV_command_list;
#define GL_ARB_draw_buffers 1
GLAPI int GLAD_GL_ARB_draw_buffers;
#define GL_ARB_multi_draw_indirect 1
GLAPI int GLAD_GL_ARB_multi_draw_indirect;
#define GL_ARB_debug_output 1
GLAPI int GLAD_GL_ARB_debug_output;
#define GL_AMD_draw_buffers_blend 1
GLAPI int GLAD_GL_AMD_draw_buffers_blend;
#define GL_OES_single_precision 1
GLAPI int GLAD_GL_OES_single_precision;
#define GL_ARB_texture_buffer_range 1
GLAPI int GLAD_GL_ARB_texture_buffer_range;
#define GL_NV_explicit_multisample 1
GLAPI int GLAD_GL_NV_explicit_multisample;
#define GL_EXT_texture_array 1
GLAPI int GLAD_GL_EXT_texture_array;
#define GL_EXT_point_parameters 1
GLAPI int GLAD_GL_EXT_point_parameters;
#define GL_APPLE_vertex_array_object 1
GLAPI int GLAD_GL_APPLE_vertex_array_object;
#define GL_ARB_vertex_type_2_10_10_10_rev 1
GLAPI int GLAD_GL_ARB_vertex_type_2_10_10_10_rev;
#define GL_ARB_vertex_shader 1
GLAPI int GLAD_GL_ARB_vertex_shader;
#define GL_APPLE_flush_buffer_range 1
GLAPI int GLAD_GL_APPLE_flush_buffer_range;
#define GL_ARB_draw_instanced 1
GLAPI int GLAD_GL_ARB_draw_instanced;
#define GL_ARB_shader_objects 1
GLAPI int GLAD_GL_ARB_shader_objects;
#define GL_ARB_tessellation_shader 1
GLAPI int GLAD_GL_ARB_tessellation_shader;
#define GL_EXT_draw_buffers2 1
GLAPI int GLAD_GL_EXT_draw_buffers2;
#define GL_ARB_vertex_attrib_64bit 1
GLAPI int GLAD_GL_ARB_vertex_attrib_64bit;
#define GL_ARB_imaging 1
GLAPI int GLAD_GL_ARB_imaging;
#define GL_INGR_blend_func_separate 1
GLAPI int GLAD_GL_INGR_blend_func_separate;
#define GL_NV_path_rendering 1
GLAPI int GLAD_GL_NV_path_rendering;
#define GL_ARB_framebuffer_object 1
GLAPI int GLAD_GL_ARB_framebuffer_object;
#define GL_ARB_draw_buffers_blend 1
GLAPI int GLAD_GL_ARB_draw_buffers_blend;
#define GL_ARB_direct_state_access 1
GLAPI int GLAD_GL_ARB_direct_state_access;
#define GL_ARB_gpu_shader_int64 1
GLAPI int GLAD_GL_ARB_gpu_shader_int64;
#define GL_ARB_texture_storage 1
GLAPI int GLAD_GL_ARB_texture_storage;
#define GL_NV_vertex_program 1
GLAPI int GLAD_GL_NV_vertex_program;
#define GL_ARB_color_buffer_float 1
GLAPI int GLAD_GL_ARB_color_buffer_float;
#define GL_ARB_clear_buffer_object 1
GLAPI int GLAD_GL_ARB_clear_buffer_object;
#define GL_ARB_multisample 1
GLAPI int GLAD_GL_ARB_multisample;
#define GL_NV_conditional_render 1
GLAPI int GLAD_GL_NV_conditional_render;
#define GL_EXT_texture_filter_anisotropic 1
GLAPI int GLAD_GL_EXT_texture_filter_anisotropic;
#define GL_ARB_shader_storage_buffer_object 1
GLAPI int GLAD_GL_ARB_shader_storage_buffer_object;
#define GL_ARB_sample_shading 1
GLAPI int GLAD_GL_ARB_sample_shading;
#define GL_EXT_blend_minmax 1
GLAPI int GLAD_GL_EXT_blend_minmax;
#define GL_EXT_framebuffer_blit 1
GLAPI int GLAD_GL_EXT_framebuffer_blit;
#define GL_ARB_gpu_shader_fp64 1
GLAPI int GLAD_GL_ARB_gpu_shader_fp64;
#define GL_NV_vertex_program4 1
GLAPI int GLAD_GL_NV_vertex_program4;
#define GL_ARB_base_instance 1
GLAPI int GLAD_GL_ARB_base_instance;
#define GL_ARB_compute_shader 1
GLAPI int GLAD_GL_ARB_compute_shader;
#define GL_ARB_blend_func_extended 1
GLAPI int GLAD_GL_ARB_blend_func_extended;
#define GL_ARB_sync 1
GLAPI int GLAD_GL_ARB_sync;
#define GL_EXT_subtexture 1
GLAPI int GLAD_GL_EXT_subtexture;
#define GL_ARB_ES3_1_compatibility 1
GLAPI int GLAD_GL_ARB_ES3_1_compatibility;
#define GL_ARB_bindless_texture 1
GLAPI int GLAD_GL_ARB_bindless_texture;
#define GL_ARB_internalformat_query 1
GLAPI int GLAD_GL_ARB_internalformat_query;
#define GL_EXT_texture_integer 1
GLAPI int GLAD_GL_EXT_texture_integer;
#define GL_ARB_compute_variable_group_size 1
GLAPI int GLAD_GL_ARB_compute_variable_group_size;
#define GL_ARB_texture_multisample 1
GLAPI int GLAD_GL_ARB_texture_multisample;
#define GL_AMD_gpu_shader_int64 1
GLAPI int GLAD_GL_AMD_gpu_shader_int64;
#define GL_ARB_invalidate_subdata 1
GLAPI int GLAD_GL_ARB_invalidate_subdata;
#define GL_EXT_framebuffer_multisample 1
GLAPI int GLAD_GL_EXT_framebuffer_multisample;
#define GL_EXT_shader_image_load_store 1
GLAPI int GLAD_GL_EXT_shader_image_load_store;
#define GL_EXT_texture3D 1
GLAPI int GLAD_GL_EXT_texture3D;
#define GL_ARB_multitexture 1
GLAPI int GLAD_GL_ARB_multitexture;
#define GL_EXT_draw_instanced 1
GLAPI int GLAD_GL_EXT_draw_instanced;
#define GL_ARB_draw_elements_base_vertex 1
GLAPI int GLAD_GL_ARB_draw_elements_base_vertex;
#define GL_ARB_viewport_array 1
GLAPI int GLAD_GL_ARB_viewport_array;
#define GL_ARB_separate_shader_objects 1
GLAPI int GLAD_GL_ARB_separate_shader_objects;
#define GL_ATI_draw_buffers 1
GLAPI int GLAD_GL_ATI_draw_buffers;
#define GL_NV_transform_feedback 1
GLAPI int GLAD_GL_NV_transform_feedback;
#define GL_NVX_gpu_memory_info 1
GLAPI int GLAD_GL_NVX_gpu_memory_info;
#define GL_ARB_sampler_objects 1
GLAPI int GLAD_GL_ARB_sampler_objects;
#define GL_EXT_copy_texture 1
GLAPI int GLAD_GL_EXT_copy_texture;
#define GL_EXT_draw_range_elements 1
GLAPI int GLAD_GL_EXT_draw_range_elements;
#define GL_ARB_uniform_buffer_object 1
GLAPI int GLAD_GL_ARB_uniform_buffer_object;


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
#if defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ > 1060)
typedef long GLintptr;
#else
typedef ptrdiff_t GLintptr;
#endif
#if defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ > 1060)
typedef long GLsizeiptr;
#else
typedef ptrdiff_t GLsizeiptr;
#endif
typedef int64_t GLint64;
typedef uint64_t GLuint64;
#if defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ > 1060)
typedef long GLintptrARB;
#else
typedef ptrdiff_t GLintptrARB;
#endif
#if defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ > 1060)
typedef long GLsizeiptrARB;
#else
typedef ptrdiff_t GLsizeiptrARB;
#endif
typedef int64_t GLint64EXT;
typedef uint64_t GLuint64EXT;
typedef struct __GLsync *GLsync;
struct _cl_context;
struct _cl_event;
typedef void (APIENTRY *GLDEBUGPROC)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef void (APIENTRY *GLDEBUGPROCARB)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef void (APIENTRY *GLDEBUGPROCKHR)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef void (APIENTRY *GLDEBUGPROCAMD)(GLuint id,GLenum category,GLenum severity,GLsizei length,const GLchar *message,void *userParam);
typedef unsigned short GLhalfNV;
typedef GLintptr GLvdpauSurfaceNV;

#define GL_REFERENCED_BY_COMPUTE_SHADER 0x930B
#define GL_PROVOKING_VERTEX_EXT 0x8E4F
#define GL_UNSIGNED_INT_IMAGE_2D_ARRAY_EXT 0x9069
#define GL_MATRIX28_ARB 0x88DC
#define GL_PATH_MAX_MODELVIEW_STACK_DEPTH_NV 0x0D36
#define GL_TOP_LEVEL_ARRAY_SIZE 0x930C
#define GL_DITHER 0x0BD0
#define GL_SKIP_COMPONENTS2_NV -5
#define GL_SHADER_IMAGE_ACCESS_BARRIER_BIT_EXT 0x00000020
#define GL_IMAGE_2D_ARRAY_EXT 0x9053
#define GL_INT_IMAGE_2D_MULTISAMPLE 0x9060
#define GL_COORD_REPLACE_NV 0x8862
#define GL_ATOMIC_COUNTER_BUFFER_BINDING 0x92C1
#define GL_INT_2_10_10_10_REV 0x8D9F
#define GL_PROXY_TEXTURE_2D_MULTISAMPLE 0x9101
#define GL_TEXTURE_MAX_LOD 0x813B
#define GL_PATH_ERROR_POSITION_NV 0x90AB
#define GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS 0x8E82
#define GL_RGBA_INTEGER_EXT 0x8D99
#define GL_DISPATCH_INDIRECT_BUFFER 0x90EE
#define GL_SAMPLER_2D_RECT 0x8B63
#define GL_RGB9_E5 0x8C3D
#define GL_TEXTURE_COMPRESSED 0x86A1
#define GL_IMAGE_1D_EXT 0x904C
#define GL_TRANSPOSE_PROGRAM_MATRIX_EXT 0x8E2E
#define GL_RGBA32UI 0x8D70
#define GL_TEXTURE_MIN_LOD 0x813A
#define GL_MAX_GEOMETRY_VARYING_COMPONENTS_ARB 0x8DDD
#define GL_IMAGE_CLASS_4_X_32 0x82B9
#define GL_FONT_Y_MIN_BOUNDS_BIT_NV 0x00020000
#define GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT 0x8E8E
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#define GL_GLYPH_VERTICAL_BEARING_X_BIT_NV 0x20
#define GL_BLEND_EQUATION 0x8009
#define GL_BYTE 0x1400
#define GL_IMAGE_FORMAT_COMPATIBILITY_BY_SIZE 0x90C8
#define GL_TEXTURE21_ARB 0x84D5
#define GL_TIMEOUT_IGNORED 0xFFFFFFFFFFFFFFFF
#define GL_PATH_COORD_COUNT_NV 0x909E
#define GL_SAMPLER_1D_ARRAY_EXT 0x8DC0
#define GL_MAX_FRAMEBUFFER_LAYERS 0x9317
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB 0x8243
#define GL_TEXTURE_BINDING_BUFFER 0x8C2C
#define GL_BLUE_INTEGER_EXT 0x8D96
#define GL_BUFFER_USAGE 0x8765
#define GL_PROGRAM_TARGET_NV 0x8646
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB 0x86A2
#define GL_TRANSFORM_FEEDBACK_BUFFER_BINDING 0x8C8F
#define GL_RENDERBUFFER_RED_SIZE 0x8D50
#define GL_ATOMIC_COUNTER_BUFFER_DATA_SIZE 0x92C4
#define GL_COMPARE_REF_DEPTH_TO_TEXTURE_EXT 0x884E
#define GL_COLOR_ATTACHMENT28 0x8CFC
#define GL_COLOR_ATTACHMENT29 0x8CFD
#define GL_DEBUG_SEVERITY_NOTIFICATION 0x826B
#define GL_COLOR_ATTACHMENT24 0x8CF8
#define GL_COLOR_ATTACHMENT25 0x8CF9
#define GL_COLOR_ATTACHMENT26 0x8CFA
#define GL_COLOR_ATTACHMENT27 0x8CFB
#define GL_COLOR_ATTACHMENT20 0x8CF4
#define GL_COLOR_ATTACHMENT21 0x8CF5
#define GL_COLOR_ATTACHMENT22 0x8CF6
#define GL_COLOR_ATTACHMENT23 0x8CF7
#define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS 0x92D4
#define GL_TRANSFORM_FEEDBACK_BUFFER 0x8C8E
#define GL_TRANSLATE_Y_NV 0x908F
#define GL_STENCIL_CLEAR_VALUE 0x0B91
#define GL_MATRIX19_ARB 0x88D3
#define GL_RENDERBUFFER_INTERNAL_FORMAT_EXT 0x8D44
#define GL_SAMPLER_2D_ARB 0x8B5E
#define GL_PROVOKING_VERTEX 0x8E4F
#define GL_TEXTURE_COORD_ARRAY_LENGTH_NV 0x8F2F
#define GL_SIGNED_NORMALIZED 0x8F9C
#define GL_MAP_PERSISTENT_BIT 0x0040
#define GL_FUNC_ADD 0x8006
#define GL_LINE_STRIP_ADJACENCY 0x000B
#define GL_UNSIGNED_INT_VEC3_EXT 0x8DC7
#define GL_POINT_SIZE 0x0B11
#define GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE 0x8217
#define GL_SHADER_IMAGE_STORE 0x82A5
#define GL_TEXTURE_COMPARE_FUNC 0x884D
#define GL_TIME_ELAPSED_EXT 0x88BF
#define GL_BLEND_DST_ALPHA_EXT 0x80CA
#define GL_SHADER_OBJECT_ARB 0x8B48
#define GL_RGB16 0x8054
#define GL_VIEW_CLASS_S3TC_DXT1_RGBA 0x82CD
#define GL_SAMPLE_SHADING_ARB 0x8C36
#define GL_POLYGON_OFFSET_FILL 0x8037
#define GL_FRONT_FACE_COMMAND_NV 0x0012
#define GL_FIRST_VERTEX_CONVENTION 0x8E4D
#define GL_COUNT_DOWN_NV 0x9089
#define GL_DOUBLEBUFFER 0x0C32
#define GL_SKIP_COMPONENTS4_NV -3
#define GL_FRAGMENT_SHADER_BIT 0x00000002
#define GL_DOUBLE_MAT4 0x8F48
#define GL_MAP2_VERTEX_ATTRIB4_4_NV 0x8674
#define GL_DOUBLE_MAT2 0x8F46
#define GL_IMAGE_BINDING_ACCESS_EXT 0x8F3E
#define GL_VIEW_COMPATIBILITY_CLASS 0x82B6
#define GL_COPY_WRITE_BUFFER_BINDING 0x8F37
#define GL_TEXTURE_GREEN_TYPE 0x8C11
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C
#define GL_MIN_SAMPLE_SHADING_VALUE 0x8C37
#define GL_UNSIGNED_INT_VEC4_EXT 0x8DC8
#define GL_SCISSOR_COMMAND_NV 0x0011
#define GL_UNSIGNED_SHORT_1_5_5_5_REV 0x8366
#define GL_MATRIX1_ARB 0x88C1
#define GL_READ_WRITE_ARB 0x88BA
#define GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB 0x8645
#define GL_CONTEXT_CORE_PROFILE_BIT 0x00000001
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS 0x8C8B
#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT 0x8B8B
#define GL_TEXTURE_DEPTH 0x8071
#define GL_RECT_NV 0xF6
#define GL_DRAW_BUFFER6 0x882B
#define GL_DRAW_BUFFER7 0x882C
#define GL_DRAW_BUFFER4 0x8829
#define GL_DRAW_BUFFER5 0x882A
#define GL_DRAW_BUFFER2 0x8827
#define GL_DRAW_BUFFER3 0x8828
#define GL_DRAW_BUFFER0 0x8825
#define GL_DRAW_BUFFER1 0x8826
#define GL_TEXTURE27_ARB 0x84DB
#define GL_COPY 0x1503
#define GL_DRAW_BUFFER8 0x882D
#define GL_DRAW_BUFFER9 0x882E
#define GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB 0x88B6
#define GL_SMOOTH_LINE_WIDTH_RANGE 0x0B22
#define GL_TEXTURE_CUBE_MAP_SEAMLESS 0x884F
#define GL_IMAGE_PIXEL_FORMAT 0x82A9
#define GL_TEXTURE_RECTANGLE 0x84F5
#define GL_PROXY_TEXTURE_2D_ARRAY_EXT 0x8C1B
#define GL_FILL 0x1B02
#define GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT 0x00004000
#define GL_SRC_COLOR 0x0300
#define GL_SAMPLER_BINDING 0x8919
#define GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES 0x92C6
#define GL_SAMPLE_BUFFERS 0x80A8
#define GL_RELATIVE_SMOOTH_CUBIC_CURVE_TO_NV 0x11
#define GL_RGBA_INTEGER 0x8D99
#define GL_TEXTURE_IMAGE_TYPE 0x8290
#define GL_COLOR_ATTACHMENT11_EXT 0x8CEB
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH 0x8B8A
#define GL_EXTENSIONS 0x1F03
#define GL_UPPER_LEFT 0x8CA2
#define GL_MAP1_VERTEX_ATTRIB2_4_NV 0x8662
#define GL_COLOR_ARRAY_LENGTH_NV 0x8F2D
#define GL_MAX_DEPTH 0x8280
#define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS_EXT 0x8DE1
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_STENCIL_BACK_PASS_DEPTH_FAIL 0x8802
#define GL_INT_SAMPLER_CUBE_MAP_ARRAY 0x900E
#define GL_UNIFORM_BUFFER 0x8A11
#define GL_SAMPLE_MASK 0x8E51
#define GL_MULTISAMPLE_ARB 0x809D
#define GL_CONTEXT_RELEASE_BEHAVIOR 0x82FB
#define GL_DEPTH_COMPONENT24 0x81A6
#define GL_QUERY_BY_REGION_WAIT_INVERTED 0x8E19
#define GL_VERTEX_ATTRIB_ARRAY_INTEGER 0x88FD
#define GL_STATIC_DRAW_ARB 0x88E4
#define GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE 0x8216
#define GL_R32I 0x8235
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT 0x8DA8
#define GL_R32F 0x822E
#define GL_OR_REVERSE 0x150B
#define GL_MAX_VARYING_COMPONENTS 0x8B4B
#define GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS 0x92D1
#define GL_DEPTH_COMPONENTS 0x8284
#define GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_WRITE 0x82AE
#define GL_MATRIX20_ARB 0x88D4
#define GL_FLOAT_MAT2 0x8B5A
#define GL_FLOAT_MAT3 0x8B5B
#define GL_DEPTH 0x1801
#define GL_FLOAT_MAT4 0x8B5C
#define GL_MAX_CULL_DISTANCES 0x82F9
#define GL_MAX_FRAMEBUFFER_WIDTH 0x9315
#define GL_OBJECT_LINK_STATUS_ARB 0x8B82
#define GL_COLOR_ATTACHMENT8_EXT 0x8CE8
#define GL_ALL_BARRIER_BITS 0xFFFFFFFF
#define GL_MAX_TESS_PATCH_COMPONENTS 0x8E84
#define GL_MAX_PROGRAM_PARAMETERS_ARB 0x88A9
#define GL_RGBA8I 0x8D8E
#define GL_PROGRAM_PARAMETERS_ARB 0x88A8
#define GL_SRC1_ALPHA 0x8589
#define GL_ELEMENT_ARRAY_UNIFIED_NV 0x8F1F
#define GL_BOUNDING_BOX_OF_BOUNDING_BOXES_NV 0x909C
#define GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES 0x82FA
#define GL_TRANSFORM_FEEDBACK_NV 0x8E22
#define GL_INT_IMAGE_CUBE_EXT 0x905B
#define GL_TEXTURE_BUFFER_FORMAT_EXT 0x8C2E
#define GL_RASTERIZER_DISCARD_EXT 0x8C89
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER 0x92C8
#define GL_DEBUG_SEVERITY_HIGH 0x9146
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER 0x92C9
#define GL_TEXTURE_BINDING_3D 0x806A
#define GL_SHADER_STORAGE_BUFFER_BINDING 0x90D3
#define GL_RELATIVE_LINE_TO_NV 0x05
#define GL_PACK_IMAGE_HEIGHT_EXT 0x806C
#define GL_MAX_UNIFORM_BLOCK_SIZE 0x8A30
#define GL_OBJECT_TYPE_ARB 0x8B4E
#define GL_TEXTURE 0x1702
#define GL_DOUBLE_MAT4x2_EXT 0x8F4D
#define GL_MAX_CLIP_DISTANCES 0x0D32
#define GL_UNSIGNED_INT64_VEC4_ARB 0x8FF7
#define GL_IMAGE_2D_MULTISAMPLE_ARRAY 0x9056
#define GL_CLIP_DISTANCE_NV 0x8C7A
#define GL_PROGRAM_INPUT 0x92E3
#define GL_ACTIVE_UNIFORMS 0x8B86
#define GL_MAX_TEXTURE_IMAGE_UNITS 0x8872
#define GL_UNSIGNED_INT_ATOMIC_COUNTER 0x92DB
#define GL_SAMPLES_ARB 0x80A9
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS 0x86A2
#define GL_NORMAL_ARRAY_BUFFER_BINDING_ARB 0x8897
#define GL_TRANSFORM_FEEDBACK_BUFFER_ACTIVE 0x8E24
#define GL_RASTERIZER_DISCARD_NV 0x8C89
#define GL_TEXTURE_MAX_LEVEL 0x813D
#define GL_MAX_IMAGE_UNITS 0x8F38
#define GL_PATH_COMPUTED_LENGTH_NV 0x90A0
#define GL_ROUNDED_RECT2_NV 0xEA
#define GL_CLAMP_VERTEX_COLOR_ARB 0x891A
#define GL_MAX_FRAMEBUFFER_SAMPLES 0x9318
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
#define GL_PROGRAM_ATTRIBS_ARB 0x88AC
#define GL_INT_IMAGE_BUFFER 0x905C
#define GL_DOUBLE_MAT2x4_EXT 0x8F4A
#define GL_BUFFER_VARIABLE 0x92E5
#define GL_TRANSFORM_FEEDBACK_BUFFER_INDEX 0x934B
#define GL_LINE 0x1B01
#define GL_SHARED_EDGE_NV 0xC0
#define GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS 0x8E1E
#define GL_PATH_DASH_ARRAY_COUNT_NV 0x909F
#define GL_DRAW_BUFFER4_ATI 0x8829
#define GL_RG32F 0x8230
#define GL_MATRIX3_NV 0x8633
#define GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE 0x906B
#define GL_PATH_DASH_CAPS_NV 0x907B
#define GL_RGBA32UI_EXT 0x8D70
#define GL_BLUE_INTEGER 0x8D96
#define GL_MAX_PROGRAM_TEXEL_OFFSET_EXT 0x8905
#define GL_DYNAMIC_COPY 0x88EA
#define GL_GEOMETRY_PROGRAM_NV 0x8C26
#define GL_SYNC_FLAGS 0x9115
#define GL_SAMPLE_MASK_VALUE_NV 0x8E52
#define GL_PATCH_DEFAULT_OUTER_LEVEL 0x8E74
#define GL_DOUBLE_MAT2x3_EXT 0x8F49
#define GL_MAX_TEXTURE_IMAGE_UNITS_ARB 0x8872
#define GL_PATH_FILL_BOUNDING_BOX_NV 0x90A1
#define GL_UNSIGNED_BYTE_2_3_3_REV 0x8362
#define GL_TEXTURE_BINDING_2D_ARRAY_EXT 0x8C1D
#define GL_BOOL_VEC2_ARB 0x8B57
#define GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY 0x9061
#define GL_DEBUG_LOGGED_MESSAGES_ARB 0x9145
#define GL_QUERY_BUFFER 0x9192
#define GL_BUFFER_FLUSHING_UNMAP_APPLE 0x8A13
#define GL_FONT_MAX_ADVANCE_HEIGHT_BIT_NV 0x02000000
#define GL_BUFFER_MAPPED_ARB 0x88BC
#define GL_RENDERBUFFER_WIDTH 0x8D42
#define GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER 0x8A45
#define GL_SAMPLE_ALPHA_TO_COVERAGE 0x809E
#define GL_BUFFER_MAP_POINTER_ARB 0x88BD
#define GL_MOVE_TO_CONTINUES_NV 0x90B6
#define GL_DRAW_INDIRECT_BUFFER_BINDING 0x8F43
#define GL_IMAGE_2D_MULTISAMPLE 0x9055
#define GL_QUERY_BY_REGION_NO_WAIT_INVERTED 0x8E1A
#define GL_INT_IMAGE_2D 0x9058
#define GL_FRAMEBUFFER_ATTACHMENT_LAYERED 0x8DA7
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING_ARB 0x8C2D
#define GL_INVALID_OPERATION 0x0502
#define GL_CLAMP_READ_COLOR 0x891C
#define GL_RED_INTEGER 0x8D94
#define GL_COLOR_ATTACHMENT5 0x8CE5
#define GL_COLOR_ATTACHMENT4 0x8CE4
#define GL_MAX_VARYING_FLOATS_ARB 0x8B4B
#define GL_COLOR_ATTACHMENT6 0x8CE6
#define GL_COLOR_ATTACHMENT1 0x8CE1
#define GL_COLOR_ATTACHMENT3 0x8CE3
#define GL_COLOR_ATTACHMENT2 0x8CE2
#define GL_COLOR_ATTACHMENT9 0x8CE9
#define GL_COLOR_ATTACHMENT8 0x8CE8
#define GL_COMPRESSED_RG11_EAC 0x9272
#define GL_PACK_IMAGE_HEIGHT 0x806C
#define GL_MATRIX5_NV 0x8635
#define GL_DEPTH_WRITEMASK 0x0B72
#define GL_PATCH_VERTICES 0x8E72
#define GL_DEBUG_TYPE_PERFORMANCE 0x8250
#define GL_CULL_FACE_MODE 0x0B45
#define GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS 0x91BC
#define GL_MIN_MAP_BUFFER_ALIGNMENT 0x90BC
#define GL_MATRIX30_ARB 0x88DE
#define GL_MOVE_TO_RESETS_NV 0x90B5
#define GL_MAX_PROGRAM_MATRICES_ARB 0x862F
#define GL_FLOAT_MAT3_ARB 0x8B5B
#define GL_ROUNDED_RECT8_NV 0xEE
#define GL_TRANSPOSE_AFFINE_3D_NV 0x9098
#define GL_POINT_SIZE_RANGE 0x0B12
#define GL_DEBUG_SOURCE_APPLICATION_ARB 0x824A
#define GL_DRAW_BUFFER12_ARB 0x8831
#define GL_SAMPLE_POSITION_NV 0x8E50
#define GL_MIN_PROGRAM_TEXEL_OFFSET_EXT 0x8904
#define GL_MAX_WIDTH 0x827E
#define GL_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910B
#define GL_AUTO_GENERATE_MIPMAP 0x8295
#define GL_SHADER_TYPE 0x8B4F
#define GL_MAP_COHERENT_BIT 0x0080
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE 0x8D56
#define GL_FRAMEBUFFER_EXT 0x8D40
#define GL_UNSIGNED_INT16_NV 0x8FF0
#define GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER 0x84F0
#define GL_MAX_VERTEX_STREAMS 0x8E71
#define GL_RG16UI 0x823A
#define GL_RGBA8 0x8058
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE 0x9104
#define GL_MAP2_VERTEX_ATTRIB1_4_NV 0x8671
#define GL_PROGRAM_FORMAT_ARB 0x8876
#define GL_INT_IMAGE_2D_EXT 0x9058
#define GL_STENCIL_BACK_REF 0x8CA3
#define GL_TEXTURE9_ARB 0x84C9
#define GL_COLOR_SUM_ARB 0x8458
#define GL_LOCATION_COMPONENT 0x934A
#define GL_INT_IMAGE_BUFFER_EXT 0x905C
#define GL_TEXTURE0_ARB 0x84C0
#define GL_STEREO 0x0C33
#define GL_MATRIX21_ARB 0x88D5
#define GL_STATIC_COPY_ARB 0x88E6
#define GL_TEXTURE_DEPTH_TYPE 0x8C16
#define GL_QUADRATIC_CURVE_TO_NV 0x0A
#define GL_UNSIGNED_INT_SAMPLER_CUBE_EXT 0x8DD4
#define GL_SAMPLER_3D_ARB 0x8B5F
#define GL_MEDIUM_INT 0x8DF4
#define GL_TEXTURE_CUBE_MAP 0x8513
#define GL_MAX_ELEMENTS_INDICES_EXT 0x80E9
#define GL_TRIANGLE_STRIP_ADJACENCY 0x000D
#define GL_MAX_IMAGE_SAMPLES 0x906D
#define GL_TEXTURE_BUFFER 0x8C2A
#define GL_PACK_COMPRESSED_BLOCK_WIDTH 0x912B
#define GL_COLOR_ATTACHMENT10_EXT 0x8CEA
#define GL_POINTS 0x0000
#define GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES 0x8F39
#define GL_RENDERBUFFER_BLUE_SIZE 0x8D52
#define GL_UNIFORM_NAME_LENGTH 0x8A39
#define GL_FRAMEBUFFER_DEFAULT_LAYERS 0x9312
#define GL_PATH_TERMINAL_END_CAP_NV 0x9078
#define GL_IMAGE_CLASS_11_11_10 0x82C2
#define GL_SCISSOR_BOX 0x0C10
#define GL_LINE_WIDTH_GRANULARITY 0x0B23
#define GL_UNSIGNED_INT_SAMPLER_RENDERBUFFER_NV 0x8E58
#define GL_VERTEX_ARRAY_BINDING_APPLE 0x85B5
#define GL_MAX_3D_TEXTURE_SIZE 0x8073
#define GL_INTERNALFORMAT_DEPTH_SIZE 0x8275
#define GL_DONT_CARE 0x1100
#define GL_MATRIX22_ARB 0x88D6
#define GL_MAX_EXT 0x8008
#define GL_RENDERBUFFER_STENCIL_SIZE 0x8D55
#define GL_PATH_STENCIL_DEPTH_OFFSET_FACTOR_NV 0x90BD
#define GL_UNIFORM_BLOCK_BINDING 0x8A3F
#define GL_NEXT_BUFFER_NV -2
#define GL_LINE_WIDTH 0x0B21
#define GL_FILE_NAME_NV 0x9074
#define GL_DYNAMIC_READ_ARB 0x88E9
#define GL_LEQUAL 0x0203
#define GL_SAMPLE_COVERAGE_INVERT_ARB 0x80AB
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT 0x8CDA
#define GL_DISTANCE_ATTENUATION_SGIS 0x8129
#define GL_MAX_SAMPLE_MASK_WORDS_NV 0x8E59
#define GL_ONE_MINUS_SRC1_ALPHA 0x88FB
#define GL_SYSTEM_FONT_NAME_NV 0x9073
#define GL_PROGRAM_INSTRUCTIONS_ARB 0x88A0
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT 0x8CD7
#define GL_VERTEX_ARRAY_SIZE_EXT 0x807A
#define GL_VIEW_CLASS_8_BITS 0x82CB
#define GL_COLOR_ATTACHMENT15_EXT 0x8CEF
#define GL_CURRENT_VERTEX_ATTRIB 0x8626
#define GL_MAP1_VERTEX_ATTRIB0_4_NV 0x8660
#define GL_MATRIX10_ARB 0x88CA
#define GL_LINES_ADJACENCY 0x000A
#define GL_ITALIC_BIT_NV 0x02
#define GL_IMPLEMENTATION_COLOR_READ_TYPE 0x8B9A
#define GL_IMAGE_3D_EXT 0x904E
#define GL_UNSIGNED_SHORT_4_4_4_4_REV 0x8365
#define GL_UNPACK_COMPRESSED_BLOCK_SIZE 0x912A
#define GL_INT_SAMPLER_2D_EXT 0x8DCA
#define GL_FLOAT16_VEC2_NV 0x8FF9
#define GL_PRIMITIVE_ID_NV 0x8C7C
#define GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING_ARB 0x889D
#define GL_STATIC_READ 0x88E5
#define GL_LUMINANCE8I_EXT 0x8D92
#define GL_SAMPLER_1D_ARB 0x8B5D
#define GL_IMAGE_BINDING_LEVEL_EXT 0x8F3B
#define GL_INT_IMAGE_CUBE_MAP_ARRAY 0x905F
#define GL_BOOL_VEC3_ARB 0x8B58
#define GL_MATRIX25_ARB 0x88D9
#define GL_VIEWPORT_BOUNDS_RANGE 0x825D
#define GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION 0x8E4C
#define GL_DEBUG_CALLBACK_USER_PARAM 0x8245
#define GL_MAX_TRACK_MATRIX_STACK_DEPTH_NV 0x862E
#define GL_PIXEL_UNPACK_BUFFER_BINDING 0x88EF
#define GL_PROGRAM_ADDRESS_REGISTERS_ARB 0x88B0
#define GL_TRANSFORM_FEEDBACK_VARYINGS 0x8C83
#define GL_ELEMENT_ARRAY_LENGTH_NV 0x8F33
#define GL_TRANSPOSE_NV 0x862C
#define GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS 0x8E81
#define GL_MAX_GEOMETRY_SHADER_INVOCATIONS 0x8E5A
#define GL_ALPHA_INTEGER_EXT 0x8D97
#define GL_COMPRESSED_SIGNED_RG_RGTC2 0x8DBE
#define GL_INT_VEC2_ARB 0x8B53
#define GL_STENCIL_BACK_WRITEMASK 0x8CA5
#define GL_SET 0x150F
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506
#define GL_RGB12 0x8053
#define GL_RGB_INTEGER_EXT 0x8D98
#define GL_TEXTURE_VIEW_MIN_LAYER 0x82DD
#define GL_MAX_TRANSFORM_FEEDBACK_BUFFERS 0x8E70
#define GL_RGB10 0x8052
#define GL_MAX_ARRAY_TEXTURE_LAYERS 0x88FF
#define GL_MAX_COMBINED_IMAGE_UNIFORMS 0x90CF
#define GL_IMAGE_BINDING_NAME 0x8F3A
#define GL_MAX_SERVER_WAIT_TIMEOUT 0x9111
#define GL_UNSIGNED_INT_2_10_10_10_REV 0x8368
#define GL_NOR 0x1508
#define GL_TEXTURE_UPDATE_BARRIER_BIT 0x00000100
#define GL_INT_VEC4_ARB 0x8B55
#define GL_PACK_ALIGNMENT 0x0D05
#define GL_DRAW_BUFFER6_ATI 0x882B
#define GL_IMAGE_CLASS_2_X_16 0x82BD
#define GL_UNPACK_LSB_FIRST 0x0CF1
#define GL_PATH_STENCIL_FUNC_NV 0x90B7
#define GL_ALREADY_SIGNALED 0x911A
#define GL_TRIANGLES_ADJACENCY_ARB 0x000C
#define GL_ACTIVE_TEXTURE 0x84E0
#define GL_LOCATION 0x930E
#define GL_MATRIX17_ARB 0x88D1
#define GL_PATH_PROJECTION_STACK_DEPTH_NV 0x0BA4
#define GL_COMPRESSED_SIGNED_R11_EAC 0x9271
#define GL_UNSIGNED_INT_SAMPLER_3D 0x8DD3
#define GL_INT_VEC4 0x8B55
#define GL_INT_VEC3 0x8B54
#define GL_ELEMENT_ARRAY_ADDRESS_NV 0x8F29
#define GL_STENCIL_FAIL 0x0B94
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_CLEAR_BUFFER 0x82B4
#define GL_VIEW_CLASS_BPTC_UNORM 0x82D2
#define GL_PACK_SKIP_IMAGES_EXT 0x806B
#define GL_VERTEX_SUBROUTINE_UNIFORM 0x92EE
#define GL_FRAMEBUFFER_UNSUPPORTED 0x8CDD
#define GL_LUMINANCE_ALPHA32I_EXT 0x8D87
#define GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB 0x88A3
#define GL_SHADER_STORAGE_BARRIER_BIT 0x00002000
#define GL_PROGRAM_LENGTH_ARB 0x8627
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE 0x851C
#define GL_CONSTANT_COLOR_EXT 0x8001
#define GL_STENCIL_BACK_VALUE_MASK 0x8CA4
#define GL_MAP2_VERTEX_ATTRIB13_4_NV 0x867D
#define GL_VERTEX_ARRAY_STRIDE_EXT 0x807C
#define GL_TEXTURE_BLUE_SIZE 0x805E
#define GL_FLOAT16_NV 0x8FF8
#define GL_TEXTURE24_ARB 0x84D8
#define GL_POINT_SIZE_MIN_SGIS 0x8126
#define GL_VERTEX_ATTRIB_ARRAY2_NV 0x8652
#define GL_VIEWPORT_COMMAND_NV 0x0010
#define GL_DRAW_BUFFER10 0x882F
#define GL_DRAW_BUFFER11 0x8830
#define GL_DRAW_BUFFER12 0x8831
#define GL_DRAW_BUFFER13 0x8832
#define GL_UNSIGNED_INT_10_10_10_2 0x8036
#define GL_DRAW_BUFFER15 0x8834
#define GL_INT_IMAGE_3D 0x9059
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE_ARB 0x8624
#define GL_INTERNALFORMAT_BLUE_SIZE 0x8273
#define GL_ATTRIBUTE_ADDRESS_COMMAND_NV 0x0009
#define GL_CONSTANT_ALPHA 0x8003
#define GL_GLYPH_HORIZONTAL_BEARING_Y_BIT_NV 0x08
#define GL_R16I 0x8233
#define GL_DOUBLE_MAT3 0x8F47
#define GL_MAX_COMPUTE_UNIFORM_BLOCKS 0x91BB
#define GL_DEPTH_COMPONENT 0x1902
#define GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER 0x8A46
#define GL_MAX_HEIGHT 0x827F
#define GL_TEXTURE_WRAP_R_EXT 0x8072
#define GL_ACTIVE_VARYING_MAX_LENGTH_NV 0x8C82
#define GL_MAX_IMAGE_SAMPLES_EXT 0x906D
#define GL_MATRIX9_ARB 0x88C9
#define GL_MAX_COMPUTE_WORK_GROUP_SIZE 0x91BF
#define GL_SMALL_CW_ARC_TO_NV 0x14
#define GL_TEXTURE_3D_EXT 0x806F
#define GL_IMAGE_CLASS_1_X_32 0x82BB
#define GL_STENCIL_PASS_DEPTH_PASS 0x0B96
#define GL_OBJECT_INFO_LOG_LENGTH_ARB 0x8B84
#define GL_VERTEX_ATTRIB_ARRAY4_NV 0x8654
#define GL_MAX_RENDERBUFFER_SIZE 0x84E8
#define GL_INCR_WRAP 0x8507
#define GL_RENDERBUFFER_ALPHA_SIZE 0x8D53
#define GL_TEXTURE_2D_BINDING_EXT 0x8069
#define GL_SQUARE_NV 0x90A3
#define GL_VERTEX_BINDING_OFFSET 0x82D7
#define GL_DECR_WRAP 0x8508
#define GL_MAP2_VERTEX_ATTRIB6_4_NV 0x8676
#define GL_BOLD_BIT_NV 0x01
#define GL_HIGH_INT 0x8DF5
#define GL_SAMPLE_POSITION 0x8E50
#define GL_MAX_FRAGMENT_ATOMIC_COUNTERS 0x92D6
#define GL_GUILTY_CONTEXT_RESET_ARB 0x8253
#define GL_INCR 0x1E02
#define GL_TEXTURE_BORDER_COLOR 0x1004
#define GL_UNPACK_SKIP_ROWS 0x0CF3
#define GL_UNSIGNED_INT_IMAGE_3D 0x9064
#define GL_SHADER_IMAGE_LOAD 0x82A4
#define GL_INTERNALFORMAT_BLUE_TYPE 0x827A
#define GL_PROGRAM_RESIDENT_NV 0x8647
#define GL_DEPTH_CLAMP 0x864F
#define GL_LUMINANCE_ALPHA_INTEGER_EXT 0x8D9D
#define GL_BLEND_DST_ALPHA 0x80CA
#define GL_INT_SAMPLER_CUBE 0x8DCC
#define GL_POINT_SPRITE_R_MODE_NV 0x8863
#define GL_CURRENT_QUERY 0x8865
#define GL_DRAW_BUFFER3_ATI 0x8828
#define GL_COMPRESSED_RGBA_BPTC_UNORM 0x8E8C
#define GL_VERTEX_SHADER 0x8B31
#define GL_VIEW_CLASS_24_BITS 0x82C9
#define GL_RGB16UI 0x8D77
#define GL_COPY_INVERTED 0x150C
#define GL_MAX_GEOMETRY_INPUT_COMPONENTS 0x9123
#define GL_LOWER_LEFT 0x8CA1
#define GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN_EXT 0x8C88
#define GL_INDEX_ARRAY_EXT 0x8077
#define GL_DEBUG_SOURCE_API 0x8246
#define GL_BLEND_COLOR_COMMAND_NV 0x000B
#define GL_TEXTURE_BINDING_1D 0x8068
#define GL_MATRIX27_ARB 0x88DB
#define GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB 0x88B3
#define GL_TEXTURE_SWIZZLE_R 0x8E42
#define GL_PIXEL_UNPACK_BUFFER 0x88EC
#define GL_POINT_FADE_THRESHOLD_SIZE_ARB 0x8128
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING 0x8C2D
#define GL_TEXTURE_VIEW_MIN_LEVEL 0x82DB
#define GL_TEXTURE_1D_BINDING_EXT 0x8068
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_UTF16_NV 0x909B
#define GL_MATRIX7_ARB 0x88C7
#define GL_DEBUG_TYPE_ERROR 0x824C
#define GL_VERTEX_SHADER_BIT 0x00000001
#define GL_ZERO 0
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_TESS_CONTROL_SHADER_BIT 0x00000008
#define GL_BUFFER_MAP_LENGTH 0x9120
#define GL_TRANSFORM_FEEDBACK_ATTRIBS_NV 0x8C7E
#define GL_READ_ONLY 0x88B8
#define GL_MAX_FRAGMENT_IMAGE_UNIFORMS 0x90CE
#define GL_FIXED 0x140C
#define GL_COLOR_ATTACHMENT6_EXT 0x8CE6
#define GL_TEXTURE_BUFFER_SIZE 0x919E
#define GL_TEXTURE_COORD_ARRAY_EXT 0x8078
#define GL_COMPRESSED_RED 0x8225
#define GL_BGR 0x80E0
#define GL_DEBUG_TYPE_MARKER 0x8268
#define GL_RGBA16_SNORM 0x8F9B
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED_ARB 0x886A
#define GL_MAX_VIEWPORT_DIMS 0x0D3A
#define GL_LOSE_CONTEXT_ON_RESET_ARB 0x8252
#define GL_DEBUG_OUTPUT 0x92E0
#define GL_CONTEXT_FLAG_DEBUG_BIT 0x00000002
#define GL_MATRIX31_ARB 0x88DF
#define GL_ACTIVE_ATOMIC_COUNTER_BUFFERS 0x92D9
#define GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT 0x8DE0
#define GL_ELEMENT_ARRAY_BARRIER_BIT 0x00000002
#define GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB 0x88AB
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
#define GL_COMPILE_STATUS 0x8B81
#define GL_LOGIC_OP_MODE 0x0BF0
#define GL_VIEW_CLASS_S3TC_DXT5_RGBA 0x82CF
#define GL_INTERNALFORMAT_STENCIL_TYPE 0x827D
#define GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB 0x8B87
#define GL_UNSIGNED_INT_24_8 0x84FA
#define GL_UNSIGNED_BYTE 0x1401
#define GL_INT_SAMPLER_BUFFER_EXT 0x8DD0
#define GL_R16 0x822A
#define GL_DRAW_BUFFER0_ATI 0x8825
#define GL_PIXEL_PACK_BUFFER 0x88EB
#define GL_PACK_LSB_FIRST 0x0D01
#define GL_ATOMIC_COUNTER_BUFFER_SIZE 0x92C3
#define GL_OFFSET 0x92FC
#define GL_TRANSFORM_FEEDBACK_BUFFER_SIZE 0x8C85
#define GL_DRAW_BUFFER 0x0C01
#define GL_FONT_UNDERLINE_THICKNESS_BIT_NV 0x08000000
#define GL_STENCIL_INDEX1 0x8D46
#define GL_VERTEX_ATTRIB_ARRAY6_NV 0x8656
#define GL_PATH_COMMAND_COUNT_NV 0x909D
#define GL_STENCIL_INDEX4 0x8D47
#define GL_WRITE_ONLY_ARB 0x88B9
#define GL_STENCIL_INDEX8 0x8D48
#define GL_RGB8UI 0x8D7D
#define GL_STENCIL_INDEX16 0x8D49
#define GL_INT_IMAGE_2D_RECT 0x905A
#define GL_VERTEX_PROGRAM_POINT_SIZE_NV 0x8642
#define GL_TEXTURE26_ARB 0x84DA
#define GL_MATRIX6_NV 0x8636
#define GL_MAX_COMPUTE_UNIFORM_COMPONENTS 0x8263
#define GL_VERTEX_ATTRIB_ARRAY_SIZE_ARB 0x8623
#define GL_BLEND_DST_RGB 0x80C8
#define GL_RED_INTEGER_EXT 0x8D94
#define GL_VERTEX_ATTRIB_ARRAY_INTEGER_EXT 0x88FD
#define GL_CLEAR 0x1500
#define GL_TEXTURE22_ARB 0x84D6
#define GL_COLOR_ATTACHMENT3_EXT 0x8CE3
#define GL_PIXEL_PACK_BUFFER_BINDING 0x88ED
#define GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT_ARB 0x00000004
#define GL_INT_IMAGE_1D_ARRAY_EXT 0x905D
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D
#define GL_POINT_SIZE_MAX_ARB 0x8127
#define GL_MITER_REVERT_NV 0x90A7
#define GL_FLOAT16_VEC3_NV 0x8FFA
#define GL_IMAGE_BINDING_FORMAT_EXT 0x906E
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING_EXT 0x8C2D
#define GL_MAP2_VERTEX_ATTRIB14_4_NV 0x867E
#define GL_TRANSLATE_2D_NV 0x9090
#define GL_VIEW_CLASS_16_BITS 0x82CA
#define GL_NUM_ACTIVE_VARIABLES 0x9304
#define GL_DRAW_ARRAYS_COMMAND_NV 0x0003
#define GL_SAMPLER_RENDERBUFFER_NV 0x8E56
#define GL_BUFFER_BINDING 0x9302
#define GL_VERTEX_ATTRIB_ARRAY_SIZE 0x8623
#define GL_INT8_NV 0x8FE0
#define GL_MAX_GEOMETRY_IMAGE_UNIFORMS 0x90CD
#define GL_TEXTURE28_ARB 0x84DC
#define GL_PROGRAM_ERROR_STRING_ARB 0x8874
#define GL_TRANSFORM_FEEDBACK_PAUSED 0x8E23
#define GL_INVERSE_NV 0x862B
#define GL_PATH_DASH_OFFSET_NV 0x907E
#define GL_TEXTURE_VIEW_NUM_LEVELS 0x82DC
#define GL_FLOAT_VEC2 0x8B50
#define GL_FLOAT_VEC3 0x8B51
#define GL_DRAW_INDIRECT_LENGTH_NV 0x8F42
#define GL_FLOAT_VEC4 0x8B52
#define GL_VERTEX_PROGRAM_ARB 0x8620
#define GL_TEXTURE_COMPARE_MODE 0x884C
#define GL_PATH_GEN_MODE_NV 0x90B0
#define GL_BUFFER_MAP_OFFSET 0x9121
#define GL_ATTRIB_ARRAY_SIZE_NV 0x8623
#define GL_BUFFER_SIZE 0x8764
#define GL_MAP1_VERTEX_ATTRIB9_4_NV 0x8669
#define GL_PATH_PROJECTION_MATRIX_NV 0x0BA7
#define GL_UNSIGNED_INT_SAMPLER_BUFFER 0x8DD8
#define GL_UNIFORM_BUFFER_START 0x8A29
#define GL_CLAMP_READ_COLOR_ARB 0x891C
#define GL_TRANSFORM_FEEDBACK_BUFFER_SIZE_NV 0x8C85
#define GL_SHADER_BINARY_FORMATS 0x8DF8
#define GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS 0x90DB
#define GL_SMOOTH_CUBIC_CURVE_TO_NV 0x10
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT 0x8CD6
#define GL_UNSIGNED_INT_SAMPLER_2D 0x8DD2
#define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_EXT 0x8DDF
#define GL_TRANSFORM_FEEDBACK_RECORD_NV 0x8C86
#define GL_MAX_VARYING_FLOATS 0x8B4B
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT 0x8CDB
#define GL_UNSIGNED_INT_IMAGE_3D_EXT 0x9064
#define GL_PATH_STROKE_COVER_MODE_NV 0x9083
#define GL_DOUBLE 0x140A
#define GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER 0x90EC
#define GL_STENCIL_BACK_FUNC 0x8800
#define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS_ARB 0x8DE1
#define GL_INT_VEC2 0x8B53
#define GL_VIEW_CLASS_RGTC1_RED 0x82D0
#define GL_FRAMEBUFFER_BLEND 0x828B
#define GL_SAMPLER_2D_ARRAY_EXT 0x8DC1
#define GL_DEBUG_SOURCE_THIRD_PARTY 0x8249
#define GL_MAX_TEXTURE_UNITS_ARB 0x84E2
#define GL_GEOMETRY_OUTPUT_TYPE_EXT 0x8DDC
#define GL_NOOP 0x1505
#define GL_POINT_DISTANCE_ATTENUATION_ARB 0x8129
#define GL_CONTEXT_FLAGS 0x821E
#define GL_PROGRAM_LENGTH_NV 0x8627
#define GL_INTERNALFORMAT_STENCIL_SIZE 0x8276
#define GL_BLEND_DST_RGB_EXT 0x80C8
#define GL_MATRIX24_ARB 0x88D8
#define GL_ALL_SHADER_BITS 0xFFFFFFFF
#define GL_DRAW_BUFFER14_ARB 0x8833
#define GL_QUERY_WAIT 0x8E13
#define GL_PROGRAM_OBJECT_ARB 0x8B40
#define GL_STENCIL_RENDERABLE 0x8288
#define GL_ARRAY_STRIDE 0x92FE
#define GL_ALIASED_LINE_WIDTH_RANGE 0x846E
#define GL_RGBA2 0x8055
#define GL_VERTEX_ATTRIB_ARRAY_LENGTH_NV 0x8F2A
#define GL_COMPRESSED_SIGNED_RG11_EAC 0x9273
#define GL_DEBUG_TYPE_POP_GROUP 0x826A
#define GL_BGRA 0x80E1
#define GL_UNSIGNED_INT_SAMPLER_2D_EXT 0x8DD2
#define GL_RENDERBUFFER_WIDTH_EXT 0x8D42
#define GL_EDGE_FLAG_ARRAY_ADDRESS_NV 0x8F26
#define GL_CURRENT_MATRIX_ARB 0x8641
#define GL_STREAM_READ_ARB 0x88E1
#define GL_UNIFORM 0x92E1
#define GL_POINT 0x1B00
#define GL_ALPHA16UI_EXT 0x8D78
#define GL_RESET_NOTIFICATION_STRATEGY_ARB 0x8256
#define GL_RENDERBUFFER_HEIGHT_EXT 0x8D43
#define GL_RGB16I_EXT 0x8D89
#define GL_SMOOTH_LINE_WIDTH_GRANULARITY 0x0B23
#define GL_PROGRAM_BINARY_FORMATS 0x87FF
#define GL_BUFFER_UPDATE_BARRIER_BIT_EXT 0x00000200
#define GL_SRGB 0x8C40
#define GL_ONE_MINUS_CONSTANT_COLOR 0x8002
#define GL_UNSIGNED_INT_8_8_8_8 0x8035
#define GL_INT_IMAGE_CUBE_MAP_ARRAY_EXT 0x905F
#define GL_MAX_TESS_CONTROL_INPUT_COMPONENTS 0x886C
#define GL_UNSIGNED_INT_SAMPLER_2D_RECT 0x8DD5
#define GL_TEXTURE_IMMUTABLE_LEVELS 0x82DF
#define GL_ALPHA_REF_COMMAND_NV 0x000F
#define GL_AFFINE_2D_NV 0x9092
#define GL_LUMINANCE_ALPHA16UI_EXT 0x8D7B
#define GL_RELATIVE_QUADRATIC_CURVE_TO_NV 0x0B
#define GL_VIEW_CLASS_BPTC_FLOAT 0x82D3
#define GL_FOG_COORD_ARRAY_LENGTH_NV 0x8F32
#define GL_VERTEX_BINDING_BUFFER 0x8F4F
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME 0x8CD1
#define GL_BLEND_SRC_ALPHA 0x80CB
#define GL_UNSIGNED_INT64_VEC2_NV 0x8FF5
#define GL_AND_REVERSE 0x1502
#define GL_FRAGMENT_SUBROUTINE 0x92EC
#define GL_QUERY_RESULT_AVAILABLE 0x8867
#define GL_MAX_DEBUG_MESSAGE_LENGTH 0x9143
#define GL_PROGRAM_STRING_ARB 0x8628
#define GL_MATRIX1_NV 0x8631
#define GL_RGBA8_SNORM 0x8F97
#define GL_INT_SAMPLER_3D 0x8DCB
#define GL_DYNAMIC_STORAGE_BIT 0x0100
#define GL_OBJECT_ACTIVE_ATTRIBUTES_ARB 0x8B89
#define GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS_EXT 0x8F39
#define GL_AND 0x1501
#define GL_VERTEX_ATTRIB_ARRAY0_NV 0x8650
#define GL_LINE_STRIP_ADJACENCY_EXT 0x000B
#define GL_INT_SAMPLER_1D_ARRAY_EXT 0x8DCE
#define GL_STATIC_COPY 0x88E6
#define GL_FOG_COORD_ARRAY_ADDRESS_NV 0x8F28
#define GL_EDGE_FLAG_ARRAY_POINTER_EXT 0x8093
#define GL_DISPATCH_INDIRECT_BUFFER_BINDING 0x90EF
#define GL_VIEWPORT_SUBPIXEL_BITS 0x825C
#define GL_SYNC_GPU_COMMANDS_COMPLETE 0x9117
#define GL_SMOOTH_POINT_SIZE_RANGE 0x0B12
#define GL_UNIFORM_BARRIER_BIT_EXT 0x00000004
#define GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY 0x906C
#define GL_MAX_GEOMETRY_OUTPUT_VERTICES 0x8DE0
#define GL_MATRIX15_ARB 0x88CF
#define GL_UNSIGNED_INT_SAMPLER_2D_RECT_EXT 0x8DD5
#define GL_MAX_NAME_LENGTH 0x92F6
#define GL_DRAW_INDIRECT_BUFFER 0x8F3F
#define GL_IMPLEMENTATION_COLOR_READ_FORMAT 0x8B9B
#define GL_MAX_PROGRAM_ENV_PARAMETERS_ARB 0x88B5
#define GL_TEXTURE_BINDING_1D_ARRAY_EXT 0x8C1C
#define GL_TEXTURE_RENDERBUFFER_DATA_STORE_BINDING_NV 0x8E54
#define GL_COMPRESSED_TEXTURE_FORMATS 0x86A3
#define GL_DEBUG_OUTPUT_SYNCHRONOUS 0x8242
#define GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY 0x900F
#define GL_DEBUG_SEVERITY_HIGH_ARB 0x9146
#define GL_DRAW_FRAMEBUFFER_BINDING_EXT 0x8CA6
#define GL_TERMINATE_SEQUENCE_COMMAND_NV 0x0000
#define GL_PRIMITIVES_GENERATED_EXT 0x8C87
#define GL_TESS_CONTROL_OUTPUT_VERTICES 0x8E75
#define GL_RENDERBUFFER_STENCIL_SIZE_EXT 0x8D55
#define GL_CCW 0x0901
#define GL_SAMPLER_2D_RECT_SHADOW 0x8B64
#define GL_DRAW_ARRAYS_INSTANCED_COMMAND_NV 0x0007
#define GL_TEXTURE30 0x84DE
#define GL_TEXTURE31 0x84DF
#define GL_LUMINANCE_ALPHA32UI_EXT 0x8D75
#define GL_UNSIGNED_INT_SAMPLER_1D 0x8DD1
#define GL_POLYGON_OFFSET_COMMAND_NV 0x000E
#define GL_R8_SNORM 0x8F94
#define GL_BACK_LEFT 0x0402
#define GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS 0x90CB
#define GL_BUFFER_MAP_POINTER 0x88BD
#define GL_MAP1_VERTEX_ATTRIB11_4_NV 0x866B
#define GL_STENCIL_ATTACHMENT_EXT 0x8D20
#define GL_SAMPLER_1D_ARRAY 0x8DC0
#define GL_STENCIL_REF 0x0B97
#define GL_LUMINANCE_ALPHA8UI_EXT 0x8D81
#define GL_DRAW_ELEMENTS_STRIP_COMMAND_NV 0x0004
#define GL_ACTIVE_SUBROUTINE_UNIFORMS 0x8DE6
#define GL_DOUBLE_MAT4x2 0x8F4D
#define GL_BLEND_EQUATION_RGB_EXT 0x8009
#define GL_INDEX_ARRAY_COUNT_EXT 0x8087
#define GL_QUERY_BUFFER_BARRIER_BIT 0x00008000
#define GL_UNSIGNED_INT8_NV 0x8FEC
#define GL_OR_INVERTED 0x150D
#define GL_SRGB_READ 0x8297
#define GL_GLYPH_HEIGHT_BIT_NV 0x02
#define GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT 0x8E8F
#define GL_WEIGHT_ARRAY_BUFFER_BINDING_ARB 0x889E
#define GL_PROXY_TEXTURE_1D_ARRAY 0x8C19
#define GL_RELATIVE_MOVE_TO_NV 0x03
#define GL_INTERLEAVED_ATTRIBS_EXT 0x8C8C
#define GL_TEXTURE29_ARB 0x84DD
#define GL_DEBUG_SOURCE_SHADER_COMPILER 0x8248
#define GL_COMPUTE_WORK_GROUP_SIZE 0x8267
#define GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS 0x92C5
#define GL_FULL_SUPPORT 0x82B7
#define GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS 0x8E47
#define GL_POINT_SPRITE_NV 0x8861
#define GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9277
#define GL_PROGRAM_SEPARABLE 0x8258
#define GL_MAX_VERTEX_UNIFORM_VECTORS 0x8DFB
#define GL_PATH_MODELVIEW_NV 0x1700
#define GL_INT64_NV 0x140E
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_ELEMENT_ARRAY_BARRIER_BIT_EXT 0x00000002
#define GL_CAVEAT_SUPPORT 0x82B8
#define GL_PRIMITIVE_RESTART_FIXED_INDEX 0x8D69
#define GL_PROGRAM_POINT_SIZE_ARB 0x8642
#define GL_STREAM_READ 0x88E1
#define GL_SAMPLER_CUBE 0x8B60
#define GL_OBJECT_ACTIVE_UNIFORMS_ARB 0x8B86
#define GL_GLYPH_WIDTH_BIT_NV 0x01
#define GL_FLOAT_32_UNSIGNED_INT_24_8_REV 0x8DAD
#define GL_LEFT 0x0406
#define GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB 0x88AF
#define GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER 0x84F1
#define GL_PROXY_TEXTURE_3D_EXT 0x8070
#define GL_MAX_VERTEX_ATTRIB_BINDINGS 0x82DA
#define GL_MATRIX4_ARB 0x88C4
#define GL_PROGRAM_NATIVE_ATTRIBS_ARB 0x88AE
#define GL_COMPUTE_SHADER_BIT 0x00000020
#define GL_MAX_TEXTURE_SIZE 0x0D33
#define GL_ARRAY_BUFFER 0x8892
#define GL_MAX_GEOMETRY_VARYING_COMPONENTS_EXT 0x8DDD
#define GL_PATH_CLIENT_LENGTH_NV 0x907F
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE 0x86A0
#define GL_UNSIGNED_INT_SAMPLER_2D_ARRAY_EXT 0x8DD7
#define GL_TEXTURE_1D 0x0DE0
#define GL_VERTEX_BINDING_DIVISOR 0x82D6
#define GL_BLEND_SRC_RGB 0x80C9
#define GL_MATRIX14_ARB 0x88CE
#define GL_ALPHA8I_EXT 0x8D90
#define GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS 0x8A31
#define GL_TRIANGLE_STRIP_ADJACENCY_EXT 0x000D
#define GL_CURRENT_MATRIX_STACK_DEPTH_NV 0x8640
#define GL_SAMPLE_ALPHA_TO_ONE 0x809F
#define GL_PROGRAM_POINT_SIZE_EXT 0x8642
#define GL_NUM_SHADER_BINARY_FORMATS 0x8DF9
#define GL_CLAMP_FRAGMENT_COLOR_ARB 0x891B
#define GL_ATTRIB_ARRAY_STRIDE_NV 0x8624
#define GL_RELATIVE_HORIZONTAL_LINE_TO_NV 0x07
#define GL_UNPACK_IMAGE_HEIGHT_EXT 0x806E
#define GL_TRANSFORM_FEEDBACK_BUFFER_ACTIVE_NV 0x8E24
#define GL_LARGE_CCW_ARC_TO_NV 0x16
#define GL_DRAW_BUFFER13_ARB 0x8832
#define GL_INT_SAMPLER_2D 0x8DCA
#define GL_SAMPLER_2D_MULTISAMPLE 0x9108
#define GL_IMAGE_CLASS_1_X_16 0x82BE
#define GL_DOUBLE_MAT3x2 0x8F4B
#define GL_MAP1_VERTEX_ATTRIB6_4_NV 0x8666
#define GL_RG32I 0x823B
#define GL_DRAW_BUFFER3_ARB 0x8828
#define GL_DOUBLE_MAT3x4 0x8F4C
#define GL_MAX_GEOMETRY_UNIFORM_BLOCKS 0x8A2C
#define GL_REFERENCED_BY_TESS_CONTROL_SHADER 0x9307
#define GL_UNIFORM_OFFSET 0x8A3B
#define GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_EXT 0x906B
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_ARB 0x8DA9
#define GL_DRAW_BUFFER10_ARB 0x882F
#define GL_POINT_FADE_THRESHOLD_SIZE_EXT 0x8128
#define GL_DOUBLE_MAT4_EXT 0x8F48
#define GL_VERTEX_ATTRIB_ARRAY11_NV 0x865B
#define GL_RGBA32I_EXT 0x8D82
#define GL_NONE 0
#define GL_HALF_FLOAT 0x140B
#define GL_PRIMITIVES_GENERATED_NV 0x8C87
#define GL_INNOCENT_CONTEXT_RESET 0x8254
#define GL_PATH_GEN_COMPONENTS_NV 0x90B3
#define GL_NAND 0x150E
#define GL_UNIFORM_BLOCK_DATA_SIZE 0x8A40
#define GL_POINT_FADE_THRESHOLD_SIZE_SGIS 0x8128
#define GL_UNIFORM_IS_ROW_MAJOR 0x8A3E
#define GL_TEXTURE_IMAGE_FORMAT 0x828F
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER_EXT 0x8CD4
#define GL_INTENSITY8I_EXT 0x8D91
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED 0x8622
#define GL_TRANSFORM_FEEDBACK_BUFFER_BINDING_NV 0x8C8F
#define GL_ALPHA 0x1906
#define GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC 0x9279
#define GL_MIN_FRAGMENT_INTERPOLATION_OFFSET 0x8E5B
#define GL_COLOR_WRITEMASK 0x0C23
#define GL_MATRIX23_ARB 0x88D7
#define GL_UNSIGNED_INT_IMAGE_1D 0x9062
#define GL_MAP2_VERTEX_ATTRIB10_4_NV 0x867A
#define GL_PROGRAM_NATIVE_PARAMETERS_ARB 0x88AA
#define GL_TEXTURE_WRAP_R 0x8072
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_RGBA8UI_EXT 0x8D7C
#define GL_ACTIVE_RESOURCES 0x92F5
#define GL_DST_ALPHA 0x0304
#define GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS_NV 0x8C8A
#define GL_PROGRAM_BINARY_LENGTH 0x8741
#define GL_INT_SAMPLER_2D_ARRAY 0x8DCF
#define GL_MAX_NUM_ACTIVE_VARIABLES 0x92F7
#define GL_SRC1_COLOR 0x88F9
#define GL_COLOR_ARRAY_STRIDE_EXT 0x8083
#define GL_TEXTURE12_ARB 0x84CC
#define GL_UNSIGNED_NORMALIZED 0x8C17
#define GL_QUERY_RESULT_ARB 0x8866
#define GL_PATH_STROKE_MASK_NV 0x9084
#define GL_SAMPLE_MASK_NV 0x8E51
#define GL_RELATIVE_ARC_TO_NV 0xFF
#define GL_PACK_SWAP_BYTES 0x0D00
#define GL_EQUAL 0x0202
#define GL_TEXTURE_SWIZZLE_G 0x8E43
#define GL_MIN_SAMPLE_SHADING_VALUE_ARB 0x8C37
#define GL_TEXTURE_BINDING_1D_ARRAY 0x8C1C
#define GL_TEXTURE_SWIZZLE_B 0x8E44
#define GL_TEXTURE_SWIZZLE_A 0x8E45
#define GL_LOW_FLOAT 0x8DF0
#define GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY_EXT 0x906C
#define GL_QUERY_RESULT_NO_WAIT 0x9194
#define GL_POINT_FADE_THRESHOLD_SIZE 0x8128
#define GL_INT_SAMPLER_BUFFER 0x8DD0
#define GL_INT_SAMPLER_1D_EXT 0x8DC9
#define GL_MAX_GEOMETRY_OUTPUT_VERTICES_ARB 0x8DE0
#define GL_CURRENT_MATRIX_STACK_DEPTH_ARB 0x8640
#define GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW 0x900D
#define GL_LINE_STRIP 0x0003
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR 0x824E
#define GL_PACK_ROW_LENGTH 0x0D02
#define GL_BUFFER 0x82E0
#define GL_CONTEXT_LOST 0x0507
#define GL_DRAW_BUFFER0_ARB 0x8825
#define GL_FONT_HEIGHT_BIT_NV 0x00800000
#define GL_COLOR 0x1800
#define GL_MOVE_TO_NV 0x02
#define GL_COMPUTE_SUBROUTINE_UNIFORM 0x92F3
#define GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT 0x919F
#define GL_DYNAMIC_READ 0x88E9
#define GL_RELATIVE_CONIC_CURVE_TO_NV 0x1B
#define GL_INDEX_ARRAY_LENGTH_NV 0x8F2E
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT 0x8CD9
#define GL_DEPTH_STENCIL 0x84F9
#define GL_SYNC_CONDITION 0x9113
#define GL_MATRIX16_ARB 0x88D0
#define GL_ACTIVE_UNIFORM_MAX_LENGTH 0x8B87
#define GL_TEXTURE1_ARB 0x84C1
#define GL_MAP_INVALIDATE_RANGE_BIT 0x0004
#define GL_TEXTURE23 0x84D7
#define GL_TEXTURE22 0x84D6
#define GL_TEXTURE21 0x84D5
#define GL_TEXTURE20 0x84D4
#define GL_TEXTURE27 0x84DB
#define GL_TEXTURE26 0x84DA
#define GL_TEXTURE25 0x84D9
#define GL_TEXTURE24 0x84D8
#define GL_DOUBLE_MAT2_EXT 0x8F46
#define GL_TEXTURE29 0x84DD
#define GL_TEXTURE28 0x84DC
#define GL_REFERENCED_BY_GEOMETRY_SHADER 0x9309
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 0x8CDC
#define GL_TRIANGLES_ADJACENCY 0x000C
#define GL_IMAGE_2D_ARRAY 0x9053
#define GL_INT_VEC3_ARB 0x8B54
#define GL_REPLACE 0x1E01
#define GL_READ_BUFFER 0x0C02
#define GL_PACK_SKIP_PIXELS 0x0D04
#define GL_RG8I 0x8237
#define GL_PATH_FILL_MASK_NV 0x9081
#define GL_BLEND_EQUATION_EXT 0x8009
#define GL_TESS_CONTROL_SHADER 0x8E88
#define GL_DEBUG_SOURCE_APPLICATION 0x824A
#define GL_FIXED_ONLY_ARB 0x891D
#define GL_GEQUAL 0x0206
#define GL_COLOR_ATTACHMENT5_EXT 0x8CE5
#define GL_RGBA8UI 0x8D7C
#define GL_R16F 0x822D
#define GL_INTENSITY32UI_EXT 0x8D73
#define GL_TRANSFORM_FEEDBACK 0x8E22
#define GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION_EXT 0x8E4C
#define GL_TEXTURE_2D_ARRAY_EXT 0x8C1A
#define GL_EDGE_FLAG_ARRAY_EXT 0x8079
#define GL_COMPRESSED_RED_RGTC1 0x8DBB
#define GL_FRAMEBUFFER_RENDERABLE_LAYERED 0x828A
#define GL_ARRAY_BUFFER_BINDING 0x8894
#define GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX 0x9048
#define GL_TEXTURE_2D 0x0DE1
#define GL_DOUBLE_MAT4x3_EXT 0x8F4E
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_BUFFER_STORAGE_FLAGS 0x8220
#define GL_MAX_COMPUTE_IMAGE_UNIFORMS 0x91BD
#define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS 0x8DE1
#define GL_EDGE_FLAG_ARRAY_BUFFER_BINDING_ARB 0x889B
#define GL_UNPACK_ROW_LENGTH 0x0CF2
#define GL_OBJECT_ATTACHED_OBJECTS_ARB 0x8B85
#define GL_CURRENT_PROGRAM 0x8B8D
#define GL_PROGRAM_PARAMETER_NV 0x8644
#define GL_VERTEX_ATTRIB_ARRAY7_NV 0x8657
#define GL_STREAM_DRAW 0x88E0
#define GL_FRAMEBUFFER_DEFAULT_SAMPLES 0x9313
#define GL_MAX_UNIFORM_BUFFER_BINDINGS 0x8A2F
#define GL_BUFFER_UPDATE_BARRIER_BIT 0x00000200
#define GL_SIGNALED 0x9119
#define GL_BOOL_VEC4_ARB 0x8B59
#define GL_UNSIGNED_INT_IMAGE_BUFFER_EXT 0x9067
#define GL_UNSIGNED_INT16_VEC4_NV 0x8FF3
#define GL_RELATIVE_CUBIC_CURVE_TO_NV 0x0D
#define GL_MAX_PROGRAM_ATTRIBS_ARB 0x88AD
#define GL_RGB32I_EXT 0x8D83
#define GL_COMPUTE_SUBROUTINE 0x92ED
#define GL_BUFFER_MAPPED 0x88BC
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB 0x824D
#define GL_PATH_INITIAL_DASH_CAP_NV 0x907C
#define GL_STENCIL_BACK_FAIL 0x8801
#define GL_POLYGON_OFFSET_FACTOR 0x8038
#define GL_LOCATION_INDEX 0x930F
#define GL_INTENSITY8UI_EXT 0x8D7F
#define GL_SYNC_FLUSH_COMMANDS_BIT 0x00000001
#define GL_DRAW_BUFFER14_ATI 0x8833
#define GL_SHADER_IMAGE_ACCESS_BARRIER_BIT 0x00000020
#define GL_MAX_FRAMEBUFFER_HEIGHT 0x9316
#define GL_PIXEL_BUFFER_BARRIER_BIT 0x00000080
#define GL_UNSIGNED_INT16_VEC2_NV 0x8FF1
#define GL_RGBA16I_EXT 0x8D88
#define GL_PATH_STROKE_BOUNDING_BOX_NV 0x90A2
#define GL_RENDERBUFFER 0x8D41
#define GL_UNIFORM_MATRIX_STRIDE 0x8A3D
#define GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_WRITE 0x82AF
#define GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS 0x92D3
#define GL_MAX_DEPTH_TEXTURE_SAMPLES 0x910F
#define GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN_NV 0x8C88
#define GL_QUERY_BY_REGION_WAIT 0x8E15
#define GL_TESS_GEN_VERTEX_ORDER 0x8E78
#define GL_QUERY_RESULT_AVAILABLE_ARB 0x8867
#define GL_TEXTURE18_ARB 0x84D2
#define GL_STATIC_READ_ARB 0x88E5
#define GL_UNSIGNED_BYTE_3_3_2 0x8032
#define GL_PATH_FILL_COVER_MODE_NV 0x9082
#define GL_INT_IMAGE_3D_EXT 0x9059
#define GL_VERTEX_ATTRIB_ARRAY1_NV 0x8651
#define GL_COMPATIBLE_SUBROUTINES 0x8E4B
#define GL_SAMPLER_CUBE_SHADOW 0x8DC5
#define GL_LAST_VERTEX_CONVENTION_EXT 0x8E4E
#define GL_STENCIL_INDEX4_EXT 0x8D47
#define GL_ONE_MINUS_CONSTANT_ALPHA_EXT 0x8004
#define GL_CURRENT_MATRIX_NV 0x8641
#define GL_INDEX_ARRAY_STRIDE_EXT 0x8086
#define GL_FONT_NUM_GLYPH_INDICES_BIT_NV 0x20000000
#define GL_MAX_GEOMETRY_ATOMIC_COUNTERS 0x92D5
#define GL_DEBUG_TYPE_PERFORMANCE_ARB 0x8250
#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR_ARB 0x88FE
#define GL_AFFINE_3D_NV 0x9094
#define GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS 0x8A32
#define GL_TRUE 1
#define GL_MAX_VERTEX_ATTRIBS 0x8869
#define GL_LINE_WIDTH_RANGE 0x0B22
#define GL_VERTEX_ATTRIB_ARRAY15_NV 0x865F
#define GL_PATH_DASH_OFFSET_RESET_NV 0x90B4
#define GL_VERTEX_ATTRIB_ARRAY3_NV 0x8653
#define GL_RELATIVE_ROUNDED_RECT4_NV 0xED
#define GL_LESS 0x0201
#define GL_VERTEX_ATTRIB_ARRAY13_NV 0x865D
#define GL_FRAMEBUFFER_UNDEFINED 0x8219
#define GL_TEXTURE_STENCIL_SIZE 0x88F1
#define GL_FONT_X_MIN_BOUNDS_BIT_NV 0x00010000
#define GL_RELATIVE_RECT_NV 0xF7
#define GL_FRAMEBUFFER 0x8D40
#define GL_GEOMETRY_TEXTURE 0x829E
#define GL_MATRIX13_ARB 0x88CD
#define GL_MAX_PROGRAM_INSTRUCTIONS_ARB 0x88A1
#define GL_INT_SAMPLER_1D 0x8DC9
#define GL_TEXTURE14_ARB 0x84CE
#define GL_TRANSFORM_FEEDBACK_VARYING 0x92F4
#define GL_UNIFORM_BUFFER_UNIFIED_NV 0x936E
#define GL_INTERNALFORMAT_RED_SIZE 0x8271
#define GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT_EXT 0x00000001
#define GL_PACK_COMPRESSED_BLOCK_HEIGHT 0x912C
#define GL_MAX_DRAW_BUFFERS_ARB 0x8824
#define GL_DEBUG_SEVERITY_LOW_ARB 0x9148
#define GL_MAX_COLOR_ATTACHMENTS_EXT 0x8CDF
#define GL_STENCIL_INDEX16_EXT 0x8D49
#define GL_REPEAT 0x2901
#define GL_INT64_VEC4_NV 0x8FEB
#define GL_MAP2_VERTEX_ATTRIB11_4_NV 0x867B
#define GL_UNSIGNED_INT64_VEC3_ARB 0x8FF6
#define GL_DEBUG_SOURCE_OTHER_ARB 0x824B
#define GL_MAP1_VERTEX_ATTRIB8_4_NV 0x8668
#define GL_VALIDATE_STATUS 0x8B83
#define GL_RG16 0x822C
#define GL_UNPACK_SKIP_IMAGES 0x806D
#define GL_UNSIGNED_INT_IMAGE_2D_RECT_EXT 0x9065
#define GL_TEXTURE_2D_MULTISAMPLE 0x9100
#define GL_SAMPLER_1D_ARRAY_SHADOW 0x8DC3
#define GL_UNSIGNED_INT_SAMPLER_1D_EXT 0x8DD1
#define GL_ATOMIC_COUNTER_BARRIER_BIT_EXT 0x00001000
#define GL_RGB16UI_EXT 0x8D77
#define GL_BUFFER_USAGE_ARB 0x8765
#define GL_BLEND_EQUATION_ALPHA 0x883D
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS 0x8DA8
#define GL_ONE_MINUS_CONSTANT_COLOR_EXT 0x8002
#define GL_ATOMIC_COUNTER_BUFFER_INDEX 0x9301
#define GL_SHADER_GLOBAL_ACCESS_BARRIER_BIT_NV 0x00000010
#define GL_RELATIVE_SMOOTH_QUADRATIC_CURVE_TO_NV 0x0F
#define GL_NORMAL_ARRAY_POINTER_EXT 0x808F
#define GL_UNPACK_COMPRESSED_BLOCK_WIDTH 0x9127
#define GL_IMAGE_CUBE_EXT 0x9050
#define GL_READ_FRAMEBUFFER_BINDING_EXT 0x8CAA
#define GL_FLOAT_MAT2_ARB 0x8B5A
#define GL_UNSIGNED_INT_SAMPLER_3D_EXT 0x8DD3
#define GL_QUERY_BY_REGION_NO_WAIT 0x8E16
#define GL_MAP1_VERTEX_ATTRIB12_4_NV 0x866C
#define GL_LINES 0x0001
#define GL_TEXTURE18 0x84D2
#define GL_TEXTURE19 0x84D3
#define GL_TEXTURE16 0x84D0
#define GL_TEXTURE17 0x84D1
#define GL_NORMAL_ARRAY_TYPE_EXT 0x807E
#define GL_TEXTURE15 0x84CF
#define GL_TEXTURE12 0x84CC
#define GL_TEXTURE13 0x84CD
#define GL_TEXTURE10 0x84CA
#define GL_TEXTURE11 0x84CB
#define GL_POLYGON_MODE 0x0B40
#define GL_RGB 0x1907
#define GL_MAP2_VERTEX_ATTRIB7_4_NV 0x8677
#define GL_TRANSFORM_FEEDBACK_BUFFER_START 0x8C84
#define GL_MAX_PROGRAM_TEXEL_OFFSET 0x8905
#define GL_RGBA32F 0x8814
#define GL_RGBA32I 0x8D82
#define GL_TRANSFORM_FEEDBACK_BUFFER_EXT 0x8C8E
#define GL_VERTEX_ATTRIB_ARRAY_TYPE 0x8625
#define GL_NORMAL_ARRAY_LENGTH_NV 0x8F2C
#define GL_TESS_CONTROL_SUBROUTINE_UNIFORM 0x92EF
#define GL_STENCIL_WRITEMASK 0x0B98
#define GL_RG8 0x822B
#define GL_STENCIL_INDEX8_EXT 0x8D48
#define GL_UNSIGNED_INT_IMAGE_2D_ARRAY 0x9069
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE 0x8CD3
#define GL_PRIMITIVE_RESTART 0x8F9D
#define GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX 0x92DA
#define GL_VERTEX_ATTRIB_ARRAY_INTEGER_NV 0x88FD
#define GL_VERTEX_ARRAY_LENGTH_NV 0x8F2B
#define GL_DEBUG_SOURCE_OTHER 0x824B
#define GL_TEXTURE_BINDING_RENDERBUFFER_NV 0x8E53
#define GL_MATRIX4_NV 0x8634
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS 0x8B4C
#define GL_VIEW_CLASS_64_BITS 0x82C6
#define GL_SRC_ALPHA 0x0302
#define GL_TEXTURE_3D 0x806F
#define GL_BUFFER_DATA_SIZE 0x9303
#define GL_DRAW_BUFFER5_ARB 0x882A
#define GL_BLEND_SRC_RGB_EXT 0x80C9
#define GL_QUERY_WAIT_NV 0x8E13
#define GL_TEXTURE_VIEW 0x82B5
#define GL_QUERY_NO_WAIT_INVERTED 0x8E18
#define GL_VERTEX_ATTRIB_ARRAY12_NV 0x865C
#define GL_UNSIGNED_INT8_VEC4_NV 0x8FEF
#define GL_MIN_EXT 0x8007
#define GL_DRAW_FRAMEBUFFER_BINDING 0x8CA6
#define GL_SAMPLE_SHADING 0x8C36
#define GL_CLIP_DISTANCE3 0x3003
#define GL_RG32UI 0x823C
#define GL_GEOMETRY_OUTPUT_TYPE_ARB 0x8DDC
#define GL_TEXTURE_INTERNAL_FORMAT 0x1003
#define GL_INTERLEAVED_ATTRIBS_NV 0x8C8C
#define GL_QUERY_TARGET 0x82EA
#define GL_VERTEX_ATTRIB_ARRAY10_NV 0x865A
#define GL_MATRIX11_ARB 0x88CB
#define GL_MAX_PATCH_VERTICES 0x8E7D
#define GL_CLIP_ORIGIN 0x935C
#define GL_NICEST 0x1102
#define GL_VERTEX_PROGRAM_BINDING_NV 0x864A
#define GL_DRAW_BUFFER6_ARB 0x882B
#define GL_UNIFORM_BARRIER_BIT 0x00000004
#define GL_VIEW_CLASS_RGTC2_RG 0x82D1
#define GL_OBJECT_TYPE 0x9112
#define GL_NUM_PROGRAM_BINARY_FORMATS 0x87FE
#define GL_PACK_SKIP_IMAGES 0x806B
#define GL_DEPTH_ATTACHMENT_EXT 0x8D00
#define GL_DRAW_BUFFER13_ATI 0x8832
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB 0x8622
#define GL_FLOAT_MAT2x4 0x8B66
#define GL_GUILTY_CONTEXT_RESET 0x8253
#define GL_FRAGMENT_SUBROUTINE_UNIFORM 0x92F2
#define GL_TRANSFORM_FEEDBACK_ACTIVE 0x8E24
#define GL_TEXTURE_BINDING_BUFFER_ARB 0x8C2C
#define GL_PRIMITIVES_GENERATED 0x8C87
#define GL_NAME_LENGTH 0x92F9
#define GL_TRIANGLES 0x0004
#define GL_BACK_PRIMARY_COLOR_NV 0x8C77
#define GL_PROGRAM_ERROR_POSITION_ARB 0x864B
#define GL_INVALID_INDEX 0xFFFFFFFF
#define GL_DOUBLE_MAT3x2_EXT 0x8F4B
#define GL_SCISSOR_TEST 0x0C11
#define GL_COLOR_ATTACHMENT12_EXT 0x8CEC
#define GL_CLIP_DEPTH_MODE 0x935D
#define GL_INTERNALFORMAT_GREEN_SIZE 0x8272
#define GL_INTENSITY16UI_EXT 0x8D79
#define GL_TRANSFORM_FEEDBACK_BARRIER_BIT 0x00000800
#define GL_IMAGE_2D_RECT 0x904F
#define GL_SMALL_CCW_ARC_TO_NV 0x12
#define GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH 0x8E49
#define GL_SMOOTH_QUADRATIC_CURVE_TO_NV 0x0E
#define GL_TEXTURE_LOD_BIAS 0x8501
#define GL_INDEX_ARRAY_POINTER_EXT 0x8091
#define GL_MAX_DEBUG_GROUP_STACK_DEPTH 0x826C
#define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS_EXT 0x8C29
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 0x8CDB
#define GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE 0x8214
#define GL_DUP_FIRST_CUBIC_CURVE_TO_NV 0xF2
#define GL_SECONDARY_COLOR_ARRAY_LENGTH_NV 0x8F31
#define GL_TEXTURE_1D_ARRAY_EXT 0x8C18
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_ARRAY_SIZE 0x92FB
#define GL_QUERY_RESULT 0x8866
#define GL_PROGRAM_NATIVE_TEMPORARIES_ARB 0x88A6
#define GL_TEXTURE_SAMPLES 0x9106
#define GL_R16_SNORM 0x8F98
#define GL_DOUBLE_VEC2_EXT 0x8FFC
#define GL_FLOAT 0x1406
#define GL_TEXTURE10_ARB 0x84CA
#define GL_COLOR_ATTACHMENT1_EXT 0x8CE1
#define GL_IMAGE_2D 0x904D
#define GL_INT_IMAGE_2D_ARRAY_EXT 0x905E
#define GL_RGB16_SNORM 0x8F9A
#define GL_COPY_READ_BUFFER 0x8F36
#define GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE 0x934C
#define GL_MAX_VERTEX_UNIFORM_BLOCKS 0x8A2B
#define GL_DRAW_BUFFER15_ARB 0x8834
#define GL_DOUBLE_MAT2x3 0x8F49
#define GL_CONIC_CURVE_TO_NV 0x1A
#define GL_COLOR_ARRAY_ADDRESS_NV 0x8F23
#define GL_MAP1_VERTEX_ATTRIB5_4_NV 0x8665
#define GL_PROXY_TEXTURE_3D 0x8070
#define GL_TEXTURE_BINDING_CUBE_MAP 0x8514
#define GL_POINT_SIZE_MIN_EXT 0x8126
#define GL_ELEMENT_ARRAY_BUFFER_BINDING 0x8895
#define GL_INT64_VEC3_NV 0x8FEA
#define GL_MAX_COMPUTE_ATOMIC_COUNTERS 0x8265
#define GL_UNSIGNED_INT_IMAGE_1D_EXT 0x9062
#define GL_DRAW_BUFFER2_ARB 0x8827
#define GL_MAX_GEOMETRY_OUTPUT_COMPONENTS 0x9124
#define GL_RELATIVE_ROUNDED_RECT2_NV 0xEB
#define GL_RENDERBUFFER_SAMPLES 0x8CAB
#define GL_DRAW_INDIRECT_ADDRESS_NV 0x8F41
#define GL_SECONDARY_COLOR_ARRAY_ADDRESS_NV 0x8F27
#define GL_RG 0x8227
#define GL_INT_SAMPLER_3D_EXT 0x8DCB
#define GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS 0x8E1F
#define GL_PACK_COMPRESSED_BLOCK_DEPTH 0x912D
#define GL_MAX_PROGRAM_OUTPUT_VERTICES_NV 0x8C27
#define GL_MAX_UNIFORM_LOCATIONS 0x826E
#define GL_GREEN_INTEGER 0x8D95
#define GL_TEXTURE_DEPTH_SIZE 0x884A
#define GL_TESS_CONTROL_TEXTURE 0x829C
#define GL_FLOAT_MAT3x4 0x8B68
#define GL_DRAW_BUFFER11_ATI 0x8830
#define GL_QUERY_BY_REGION_NO_WAIT_NV 0x8E16
#define GL_TEXTURE_UPDATE_BARRIER_BIT_EXT 0x00000100
#define GL_COMPRESSED_RGBA 0x84EE
#define GL_MATRIX6_ARB 0x88C6
#define GL_QUERY_BUFFER_BINDING 0x9193
#define GL_COMPUTE_TEXTURE 0x82A0
#define GL_COLOR_ARRAY_COUNT_EXT 0x8084
#define GL_MAX_COLOR_ATTACHMENTS 0x8CDF
#define GL_QUERY_NO_WAIT 0x8E14
#define GL_VIEW_CLASS_48_BITS 0x82C7
#define GL_GEOMETRY_SHADER_EXT 0x8DD9
#define GL_UNSIGNED_INT64_VEC2_ARB 0x8FF5
#define GL_UNPACK_SWAP_BYTES 0x0CF0
#define GL_COLOR_ARRAY_SIZE_EXT 0x8081
#define GL_EDGE_FLAG_ARRAY_COUNT_EXT 0x808D
#define GL_ZERO_TO_ONE 0x935F
#define GL_TRANSFORM_FEEDBACK_BUFFER_MODE 0x8C7F
#define GL_INT_IMAGE_2D_MULTISAMPLE_EXT 0x9060
#define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS 0x8DDF
#define GL_BEVEL_NV 0x90A6
#define GL_MAX_VERTEX_ATTRIB_STRIDE 0x82E5
#define GL_IMAGE_BINDING_LEVEL 0x8F3B
#define GL_R8 0x8229
#define GL_TRANSFORM_FEEDBACK_BUFFER_START_EXT 0x8C84
#define GL_BOUNDING_BOX_NV 0x908D
#define GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5F
#define GL_RGB_INTEGER 0x8D98
#define GL_INT_SAMPLER_2D_RECT_EXT 0x8DCD
#define GL_STENCIL 0x1802
#define GL_VIEW_CLASS_128_BITS 0x82C4
#define GL_INVALID_FRAMEBUFFER_OPERATION_EXT 0x0506
#define GL_MAP1_VERTEX_ATTRIB7_4_NV 0x8667
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT 0x8D56
#define GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS_EXT 0x8C8A
#define GL_CLOSE_PATH_NV 0x00
#define GL_PROGRAM_MATRIX_STACK_DEPTH_EXT 0x8E2F
#define GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH 0x82FC
#define GL_SYNC_STATUS 0x9114
#define GL_MAX_IMAGE_UNITS_EXT 0x8F38
#define GL_RGBA16I 0x8D88
#define GL_STREAM_DRAW_ARB 0x88E0
#define GL_ROUND_NV 0x90A4
#define GL_VIEWPORT 0x0BA2
#define GL_MAX_FRAGMENT_INTERPOLATION_OFFSET 0x8E5C
#define GL_UNPACK_COMPRESSED_BLOCK_HEIGHT 0x9128
#define GL_TEXTURE_BUFFER_BINDING 0x8C2A
#define GL_RGB8I 0x8D8F
#define GL_ATTRIB_ARRAY_TYPE_NV 0x8625
#define GL_DRAW_BUFFER7_ARB 0x882C
#define GL_VERTEX_ARRAY_COUNT_EXT 0x807D
#define GL_BLEND_SRC 0x0BE1
#define GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET 0x82D9
#define GL_UNSIGNED_INT_IMAGE_2D_EXT 0x9063
#define GL_GLYPH_HORIZONTAL_BEARING_ADVANCE_BIT_NV 0x10
#define GL_GREEN_INTEGER_EXT 0x8D95
#define GL_INT_SAMPLER_2D_RECT 0x8DCD
#define GL_SHADER_STORAGE_BLOCK 0x92E6
#define GL_TEXTURE4_ARB 0x84C4
#define GL_DEPTH24_STENCIL8 0x88F0
#define GL_FONT_X_MAX_BOUNDS_BIT_NV 0x00040000
#define GL_SAMPLE_BUFFERS_ARB 0x80A8
#define GL_UNSIGNED_INT_SAMPLER_1D_ARRAY_EXT 0x8DD6
#define GL_TEXTURE_COMPRESSED_ARB 0x86A1
#define GL_UNSIGNED_INT8_VEC3_NV 0x8FEE
#define GL_FUNC_ADD_EXT 0x8006
#define GL_GEOMETRY_SHADER_INVOCATIONS 0x887F
#define GL_TEXTURE16_ARB 0x84D0
#define GL_SAMPLER_2D_RECT_SHADOW_ARB 0x8B64
#define GL_RGBA16F 0x881A
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT 0x8CD1
#define GL_ANY_SAMPLES_PASSED 0x8C2F
#define GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE 0x8215
#define GL_UNPACK_ALIGNMENT 0x0CF5
#define GL_COMPRESSED_LUMINANCE_ARB 0x84EA
#define GL_MAP_WRITE_BIT 0x0002
#define GL_FONT_UNINTELLIGIBLE_NV 0x936B
#define GL_ALL_BARRIER_BITS_EXT 0xFFFFFFFF
#define GL_VIEW_CLASS_S3TC_DXT1_RGB 0x82CC
#define GL_STENCIL_PASS_DEPTH_FAIL 0x0B95
#define GL_VERTEX_ATTRIB_ARRAY14_NV 0x865E
#define GL_BUFFER_SIZE_ARB 0x8764
#define GL_LUMINANCE16I_EXT 0x8D8C
#define GL_MAP1_VERTEX_ATTRIB14_4_NV 0x866E
#define GL_TRIANGLES_ADJACENCY_EXT 0x000C
#define GL_VIEW_CLASS_32_BITS 0x82C8
#define GL_IMAGE_BINDING_ACCESS 0x8F3E
#define GL_BGRA_INTEGER_EXT 0x8D9B
#define GL_TESS_GEN_SPACING 0x8E77
#define GL_LINEAR 0x2601
#define GL_INT_IMAGE_2D_RECT_EXT 0x905A
#define GL_UNSIGNED_INT_SAMPLER_CUBE 0x8DD4
#define GL_TEXTURE31_ARB 0x84DF
#define GL_STENCIL_INDEX 0x1901
#define GL_PROGRAM_TEMPORARIES_ARB 0x88A4
#define GL_ATOMIC_COUNTER_BUFFER_START 0x92C2
#define GL_SEPARATE_ATTRIBS_NV 0x8C8D
#define GL_GREATER 0x0204
#define GL_BLOCK_INDEX 0x92FD
#define GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS 0x90D9
#define GL_TESS_EVALUATION_SHADER_BIT 0x00000010
#define GL_FRONT_FACE 0x0B46
#define GL_TEXTURE_DEPTH_EXT 0x8071
#define GL_SHADER 0x82E1
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE 0x8624
#define GL_UNSIGNED_INT16_VEC3_NV 0x8FF2
#define GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE 0x8212
#define GL_UNIFORM_BUFFER_LENGTH_NV 0x9370
#define GL_VERTEX_TEXTURE 0x829B
#define GL_VERTEX_ARRAY_EXT 0x8074
#define GL_IS_ROW_MAJOR 0x9300
#define GL_INT64_ARB 0x140E
#define GL_SAMPLER_1D_SHADOW_ARB 0x8B61
#define GL_MATRIX0_NV 0x8630
#define GL_RENDERBUFFER_INTERNAL_FORMAT 0x8D44
#define GL_TESS_EVALUATION_SHADER 0x8E87
#define GL_CLEAR_TEXTURE 0x9365
#define GL_NOP_COMMAND_NV 0x0001
#define GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS 0x8E7F
#define GL_COLOR_ARRAY_TYPE_EXT 0x8082
#define GL_BLEND 0x0BE2
#define GL_R16UI 0x8234
#define GL_UNSIGNED_SHORT 0x1403
#define GL_MIN 0x8007
#define GL_COMPRESSED_SRGB_ALPHA 0x8C49
#define GL_LUMINANCE_ALPHA8I_EXT 0x8D93
#define GL_ONE_MINUS_SRC_COLOR 0x0301
#define GL_DOUBLE_MAT3x4_EXT 0x8F4C
#define GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY_EXT 0x9061
#define GL_DRAW_BUFFER1_ATI 0x8826
#define GL_PATCHES 0x000E
#define GL_TIMESTAMP 0x8E28
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER 0x92CB
#define GL_IMAGE_2D_EXT 0x904D
#define GL_ACTIVE_VARYINGS_NV 0x8C81
#define GL_VERTEX_PROGRAM_POINT_SIZE 0x8642
#define GL_INT64_VEC2_ARB 0x8FE9
#define GL_MAP2_VERTEX_ATTRIB2_4_NV 0x8672
#define GL_SRGB_ALPHA 0x8C42
#define GL_DRAW_BUFFER9_ARB 0x882E
#define GL_PACK_SKIP_ROWS 0x0D03
#define GL_MAX_DRAW_BUFFERS_ATI 0x8824
#define GL_INTERNALFORMAT_SHARED_SIZE 0x8277
#define GL_RESET_NOTIFICATION_STRATEGY 0x8256
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_PATH_FORMAT_SVG_NV 0x9070
#define GL_GEOMETRY_SHADER 0x8DD9
#define GL_UNSIGNED_INT_IMAGE_CUBE 0x9066
#define GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS 0x886D
#define GL_TEXTURE_COORD_ARRAY_TYPE_EXT 0x8089
#define GL_TRANSFORM_FEEDBACK_VARYINGS_NV 0x8C83
#define GL_BLEND_COLOR 0x8005
#define GL_TEXTURE_COORD_ARRAY_STRIDE_EXT 0x808A
#define GL_SAMPLER_2D_ARRAY_SHADOW_EXT 0x8DC4
#define GL_ONE_MINUS_CONSTANT_ALPHA 0x8004
#define GL_TRANSLATE_3D_NV 0x9091
#define GL_FRACTIONAL_EVEN 0x8E7C
#define GL_DRAW_BUFFER14 0x8833
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER 0x90ED
#define GL_FIRST_VERTEX_CONVENTION_EXT 0x8E4D
#define GL_TEXTURE3_ARB 0x84C3
#define GL_ADJACENT_PAIRS_NV 0x90AE
#define GL_TEXTURE_FETCH_BARRIER_BIT_EXT 0x00000008
#define GL_CLAMP_TO_BORDER 0x812D
#define GL_SAMPLES_PASSED 0x8914
#define GL_MATRIX26_ARB 0x88DA
#define GL_TEXTURE_FIXED_SAMPLE_LOCATIONS 0x9107
#define GL_SEPARATE_ATTRIBS 0x8C8D
#define GL_IMAGE_3D 0x904E
#define GL_MAP2_VERTEX_ATTRIB0_4_NV 0x8670
#define GL_ATTRIB_ARRAY_POINTER_NV 0x8645
#define GL_MAP1_VERTEX_ATTRIB1_4_NV 0x8661
#define GL_GLYPH_VERTICAL_BEARING_ADVANCE_BIT_NV 0x80
#define GL_TEXTURE_HEIGHT 0x1001
#define GL_HIGH_FLOAT 0x8DF2
#define GL_INT16_VEC3_NV 0x8FE6
#define GL_FONT_UNAVAILABLE_NV 0x936A
#define GL_MAX_VERTEX_VARYING_COMPONENTS_ARB 0x8DDE
#define GL_RGBA_FLOAT_MODE_ARB 0x8820
#define GL_RED 0x1903
#define GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY 0x906A
#define GL_POLYGON_OFFSET_LINE 0x2A02
#define GL_FUNC_REVERSE_SUBTRACT 0x800B
#define GL_NO_RESET_NOTIFICATION_ARB 0x8261
#define GL_GREEN 0x1904
#define GL_TESS_GEN_POINT_MODE 0x8E79
#define GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM 0x8E8D
#define GL_BACK_SECONDARY_COLOR_NV 0x8C78
#define GL_STENCIL_BACK_PASS_DEPTH_PASS 0x8803
#define GL_VERTEX_ATTRIB_ARRAY5_NV 0x8655
#define GL_SAMPLER_2D_RECT_ARB 0x8B63
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS 0x8A42
#define GL_COLOR_ARRAY_POINTER_EXT 0x8090
#define GL_TRANSPOSE_AFFINE_2D_NV 0x9096
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE 0x910A
#define GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV 0x8F20
#define GL_DEBUG_SEVERITY_LOW 0x9148
#define GL_MATRIX29_ARB 0x88DD
#define GL_COUNT_UP_NV 0x9088
#define GL_UNSIGNED_INT_8_8_8_8_REV 0x8367
#define GL_OBJECT_SHADER_SOURCE_LENGTH_ARB 0x8B88
#define GL_IMAGE_BINDING_FORMAT 0x906E
#define GL_NEGATIVE_ONE_TO_ONE 0x935E
#define GL_UNPACK_IMAGE_HEIGHT 0x806E
#define GL_MAX_COMBINED_UNIFORM_BLOCKS 0x8A2E
#define GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING_ARB 0x889C
#define GL_INT16_VEC4_NV 0x8FE7
#define GL_COLOR_CLEAR_VALUE 0x0C22
#define GL_DST_COLOR 0x0306
#define GL_MAX_SAMPLES_EXT 0x8D57
#define GL_DRAW_BUFFER5_ATI 0x882A
#define GL_UNSIGNED_INT 0x1405
#define GL_SAMPLE_MASK_VALUE 0x8E52
#define GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS 0x90D7
#define GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS 0x92DC
#define GL_FLOAT_MAT3x2 0x8B67
#define GL_TRANSPOSE_CURRENT_MATRIX_ARB 0x88B7
#define GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT 0x82B2
#define GL_DOUBLE_MAT3_EXT 0x8F47
#define GL_PIXEL_BUFFER_BARRIER_BIT_EXT 0x00000080
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES 0x8A43
#define GL_LINE_STRIP_ADJACENCY_ARB 0x000B
#define GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT 0x8A34
#define GL_RG16_SNORM 0x8F99
#define GL_RENDERBUFFER_BINDING 0x8CA7
#define GL_FRAMEBUFFER_ATTACHMENT_LAYERED_EXT 0x8DA7
#define GL_COMPRESSED_RGB 0x84ED
#define GL_MULTISAMPLE_BIT_ARB 0x20000000
#define GL_RG_INTEGER 0x8228
#define GL_MAX_VERTEX_ATOMIC_COUNTERS 0x92D2
#define GL_IMAGE_FORMAT_COMPATIBILITY_TYPE 0x90C7
#define GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS 0x92D0
#define GL_RENDERBUFFER_HEIGHT 0x8D43
#define GL_INTERLEAVED_ATTRIBS 0x8C8C
#define GL_TEXTURE_ALPHA_TYPE 0x8C13
#define GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB 0x8895
#define GL_BLUE 0x1905
#define GL_COLOR_ATTACHMENT7 0x8CE7
#define GL_HORIZONTAL_LINE_TO_NV 0x06
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS 0x8C80
#define GL_MAX_COMPUTE_SHARED_MEMORY_SIZE 0x8262
#define GL_COLOR_ATTACHMENT7_EXT 0x8CE7
#define GL_TEXTURE_BLUE_TYPE 0x8C12
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH_EXT 0x8C76
#define GL_STREAM_COPY 0x88E2
#define GL_INT64_VEC2_NV 0x8FE9
#define GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB 0x88A2
#define GL_TEXTURE_FETCH_BARRIER_BIT 0x00000008
#define GL_FIXED_ONLY 0x891D
#define GL_VERTEX_ATTRIB_ARRAY_LONG 0x874E
#define GL_MATRIX18_ARB 0x88D2
#define GL_SAMPLER_BUFFER_EXT 0x8DC2
#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY 0x9102
#define GL_DRAW_FRAMEBUFFER_EXT 0x8CA9
#define GL_RGB8UI_EXT 0x8D7D
#define GL_TRANSFORM_FEEDBACK_BUFFER_MODE_EXT 0x8C7F
#define GL_READ_PIXELS_TYPE 0x828E
#define GL_SAMPLER_CUBE_SHADOW_EXT 0x8DC5
#define GL_ARC_TO_NV 0xFE
#define GL_INT8_VEC4_NV 0x8FE3
#define GL_IMAGE_FORMAT_COMPATIBILITY_BY_CLASS 0x90C9
#define GL_SHADER_IMAGE_ATOMIC 0x82A6
#define GL_PATH_TRANSPOSE_PROJECTION_MATRIX_NV 0x84E4
#define GL_INT_SAMPLER_2D_ARRAY_EXT 0x8DCF
#define GL_MAP1_VERTEX_ATTRIB15_4_NV 0x866F
#define GL_OBJECT_COMPILE_STATUS_ARB 0x8B81
#define GL_TEXTURE11_ARB 0x84CB
#define GL_FASTEST 0x1101
#define GL_TEXTURE_RED_TYPE 0x8C10
#define GL_BUFFER_ACCESS_ARB 0x88BB
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM 0x8247
#define GL_GET_TEXTURE_IMAGE_TYPE 0x8292
#define GL_DRAW_BUFFER8_ATI 0x882D
#define GL_INT16_VEC2_NV 0x8FE5
#define GL_PRIMITIVE_RESTART_INDEX 0x8F9E
#define GL_DEBUG_SOURCE_API_ARB 0x8246
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A
#define GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS 0x8DE8
#define GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS 0x8266
#define GL_SAMPLER_CUBE_ARB 0x8B60
#define GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB 0x88B1
#define GL_TEXTURE15_ARB 0x84CF
#define GL_INT_SAMPLER_RENDERBUFFER_NV 0x8E57
#define GL_COMPRESSED_RG_RGTC2 0x8DBD
#define GL_COMPRESSED_SIGNED_RED_RGTC1 0x8DBC
#define GL_TEXTURE_CUBE_MAP_ARRAY 0x9009
#define GL_MAX_SHADER_BUFFER_ADDRESS_NV 0x8F35
#define GL_COMPRESSED_RGB8_ETC2 0x9274
#define GL_TEXTURE_WIDTH 0x1000
#define GL_RELATIVE_SMALL_CCW_ARC_TO_NV 0x13
#define GL_UNIFORM_SIZE 0x8A38
#define GL_FONT_Y_MAX_BOUNDS_BIT_NV 0x00080000
#define GL_READ_ONLY_ARB 0x88B8
#define GL_FLOAT_MAT4x2 0x8B69
#define GL_SHADER_SOURCE_LENGTH 0x8B88
#define GL_DRAW_BUFFER4_ARB 0x8829
#define GL_TEXTURE2_ARB 0x84C2
#define GL_COMPRESSED_RG 0x8226
#define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS_ARB 0x8C29
#define GL_GEOMETRY_VERTICES_OUT_EXT 0x8DDA
#define GL_GLYPH_HAS_KERNING_BIT_NV 0x100
#define GL_TEXTURE_TARGET 0x1006
#define GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS 0x92CF
#define GL_UNKNOWN_CONTEXT_RESET_ARB 0x8255
#define GL_OUT_OF_MEMORY 0x0505
#define GL_TRANSFORM_FEEDBACK_BUFFER_START_NV 0x8C84
#define GL_TEXTURE30_ARB 0x84DE
#define GL_MAX_TEXTURE_BUFFER_SIZE 0x8C2B
#define GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS 0x90D8
#define GL_OBJECT_DELETE_STATUS_ARB 0x8B80
#define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_ARB 0x8DDF
#define GL_COMPRESSED_LUMINANCE_ALPHA_ARB 0x84EB
#define GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS 0x8C8A
#define GL_GEOMETRY_OUTPUT_TYPE 0x8918
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT 0x8CDC
#define GL_BOOL 0x8B56
#define GL_MAX_TEXTURE_LOD_BIAS 0x84FD
#define GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS 0x8F39
#define GL_RENDERBUFFER_SAMPLES_EXT 0x8CAB
#define GL_TEXTURE_BUFFER_EXT 0x8C2A
#define GL_LINES_ADJACENCY_EXT 0x000A
#define GL_VERTEX_ATTRIB_ARRAY_TYPE_ARB 0x8625
#define GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER 0x8A44
#define GL_DRAW_BUFFER2_ATI 0x8827
#define GL_MAP2_VERTEX_ATTRIB12_4_NV 0x867C
#define GL_TRANSFORM_FEEDBACK_VARYINGS_EXT 0x8C83
#define GL_INT_SAMPLER_2D_MULTISAMPLE 0x9109
#define GL_STENCIL_TEST 0x0B90
#define GL_COLOR_COMPONENTS 0x8283
#define GL_MAX_COMPUTE_WORK_GROUP_COUNT 0x91BE
#define GL_R11F_G11F_B10F 0x8C3A
#define GL_UNSIGNED_INT8_VEC2_NV 0x8FED
#define GL_INTERNALFORMAT_GREEN_TYPE 0x8279
#define GL_INVERT 0x150A
#define GL_PATH_TERMINAL_DASH_CAP_NV 0x907D
#define GL_TRANSFORM_FEEDBACK_BARRIER_BIT_EXT 0x00000800
#define GL_TRIANGLE_FAN 0x0006
#define GL_FILTER 0x829A
#define GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS 0x8E86
#define GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS 0x90EB
#define GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS 0x8E80
#define GL_COMPRESSED_RGBA8_ETC2_EAC 0x9278
#define GL_UNIFORM_BUFFER_SIZE 0x8A2A
#define GL_INT 0x1404
#define GL_TOP_LEVEL_ARRAY_STRIDE 0x930D
#define GL_TEXTURE17_ARB 0x84D1
#define GL_MAP2_VERTEX_ATTRIB5_4_NV 0x8675
#define GL_TRACK_MATRIX_TRANSFORM_NV 0x8649
#define GL_MAX_SUBROUTINES 0x8DE7
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT 0x8CD0
#define GL_RELATIVE_ROUNDED_RECT8_NV 0xEF
#define GL_VERTEX_ARRAY_TYPE_EXT 0x807B
#define GL_CLIP_DISTANCE0 0x3000
#define GL_ACTIVE_UNIFORM_BLOCKS 0x8A36
#define GL_GEOMETRY_SHADER_ARB 0x8DD9
#define GL_UNIFORM_ADDRESS_COMMAND_NV 0x000A
#define GL_POINT_SIZE_MIN_ARB 0x8126
#define GL_RGB16I 0x8D89
#define GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX 0x904B
#define GL_RGB16F 0x881B
#define GL_ONE_MINUS_SRC1_COLOR 0x88FA
#define GL_PATH_FILL_MODE_NV 0x9080
#define GL_MANUAL_GENERATE_MIPMAP 0x8294
#define GL_IMAGE_BINDING_LAYER 0x8F3D
#define GL_MINOR_VERSION 0x821C
#define GL_LAST_VERTEX_CONVENTION 0x8E4E
#define GL_COLOR_ATTACHMENT4_EXT 0x8CE4
#define GL_EDGE_FLAG_ARRAY_STRIDE_EXT 0x808C
#define GL_ALWAYS 0x0207
#define GL_TEXTURE_BUFFER_FORMAT_ARB 0x8C2E
#define GL_STENCIL_REF_COMMAND_NV 0x000C
#define GL_MATRIX8_ARB 0x88C8
#define GL_CONDITION_SATISFIED 0x911C
#define GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT 0x00000001
#define GL_GENERIC_ATTRIB_NV 0x8C7D
#define GL_BGR_INTEGER_EXT 0x8D9A
#define GL_TRANSLATE_X_NV 0x908E
#define GL_MATRIX12_ARB 0x88CC
#define GL_RGB565 0x8D62
#define GL_TEXTURE_1D_ARRAY 0x8C18
#define GL_READ_FRAMEBUFFER_BINDING 0x8CAA
#define GL_VERTEX_SHADER_ARB 0x8B31
#define GL_INT8_VEC3_NV 0x8FE2
#define GL_PATH_STROKE_WIDTH_NV 0x9075
#define GL_DEPTH_COMPONENT32F 0x8CAC
#define GL_USE_MISSING_GLYPH_NV 0x90AA
#define GL_UNIFORM_ARRAY_STRIDE 0x8A3C
#define GL_IMAGE_2D_MULTISAMPLE_ARRAY_EXT 0x9056
#define GL_POINT_SIZE_MAX_SGIS 0x8127
#define GL_VERTEX_PROGRAM_NV 0x8620
#define GL_RGBA4 0x8056
#define GL_POINT_SPRITE_COORD_ORIGIN 0x8CA0
#define GL_MAX_VERTEX_ATTRIBS_ARB 0x8869
#define GL_LUMINANCE_ALPHA16I_EXT 0x8D8D
#define GL_INFO_LOG_LENGTH 0x8B84
#define GL_ACCUM_ADJACENT_PAIRS_NV 0x90AD
#define GL_DRAW_BUFFER10_ATI 0x882F
#define GL_SHADER_STORAGE_BUFFER 0x90D2
#define GL_POLYGON_OFFSET_UNITS 0x2A00
#define GL_SRGB8 0x8C41
#define GL_TEXTURE13_ARB 0x84CD
#define GL_INT_IMAGE_2D_ARRAY 0x905E
#define GL_READ_PIXELS_FORMAT 0x828D
#define GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_TEST 0x82AD
#define GL_DRAW_ELEMENTS_COMMAND_NV 0x0002
#define GL_POINT_SIZE_GRANULARITY 0x0B13
#define GL_SRGB_DECODE_ARB 0x8299
#define GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9276
#define GL_GEOMETRY_SHADER_BIT 0x00000004
#define GL_MAP_READ_BIT 0x0001
#define GL_ACTIVE_ATTRIBUTES 0x8B89
#define GL_TESS_CONTROL_SUBROUTINE 0x92E9
#define GL_VERTEX_ARRAY_ADDRESS_NV 0x8F21
#define GL_TEXTURE20_ARB 0x84D4
#define GL_DEBUG_TYPE_OTHER_ARB 0x8251
#define GL_DEBUG_TYPE_ERROR_ARB 0x824C
#define GL_COLOR_ATTACHMENT30 0x8CFE
#define GL_INTENSITY16I_EXT 0x8D8B
#define GL_MAP1_VERTEX_ATTRIB3_4_NV 0x8663
#define GL_DISTANCE_ATTENUATION_EXT 0x8129
#define GL_MAX_ELEMENTS_VERTICES_EXT 0x80E8
#define GL_LINE_TO_NV 0x04
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS_NV 0x8C80
#define GL_POLYGON_SMOOTH 0x0B41
#define GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB 0x8B4A
#define GL_PROGRAM_FORMAT_ASCII_ARB 0x8875
#define GL_FRAMEBUFFER_UNSUPPORTED_EXT 0x8CDD
#define GL_LINE_WIDTH_COMMAND_NV 0x000D
#define GL_RGBA12 0x805A
#define GL_PATH_INITIAL_END_CAP_NV 0x9077
#define GL_RGBA16 0x805B
#define GL_RELATIVE_LARGE_CCW_ARC_TO_NV 0x17
#define GL_LUMINANCE32I_EXT 0x8D86
#define GL_RGB5_A1 0x8057
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_TEXTURE_ALPHA_SIZE 0x805F
#define GL_QUERY_COUNTER_BITS 0x8864
#define GL_CIRCULAR_CW_ARC_TO_NV 0xFA
#define GL_VERTEX_SUBROUTINE 0x92E8
#define GL_MATRIX_STRIDE 0x92FF
#define GL_FONT_HAS_KERNING_BIT_NV 0x10000000
#define GL_VIEW_CLASS_96_BITS 0x82C5
#define GL_ATTACHED_SHADERS 0x8B85
#define GL_FRAMEBUFFER_COMPLETE_EXT 0x8CD5
#define GL_LAYER_NV 0x8DAA
#define GL_MAX_SAMPLES 0x8D57
#define GL_DEBUG_CALLBACK_FUNCTION_ARB 0x8244
#define GL_RGB10_A2 0x8059
#define GL_ACTIVE_SUBROUTINE_MAX_LENGTH 0x8E48
#define GL_PATH_TRANSPOSE_MODELVIEW_MATRIX_NV 0x84E3
#define GL_MAX_VERTEX_IMAGE_UNIFORMS 0x90CA
#define GL_READ_FRAMEBUFFER_EXT 0x8CA8
#define GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS 0x8E8A
#define GL_MAP1_VERTEX_ATTRIB13_4_NV 0x866D
#define GL_MATRIX7_NV 0x8637
#define GL_SAMPLE_ALPHA_TO_ONE_ARB 0x809F
#define GL_MAX_FRAGMENT_INPUT_COMPONENTS 0x9125
#define GL_PATH_MAX_PROJECTION_STACK_DEPTH_NV 0x0D38
#define GL_FONT_UNDERLINE_POSITION_BIT_NV 0x04000000
#define GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB 0x8242
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB 0x8B4D
#define GL_SMOOTH_POINT_SIZE_GRANULARITY 0x0B13
#define GL_VIEWPORT_INDEX_PROVOKING_VERTEX 0x825F
#define GL_EDGE_FLAG_ARRAY_LENGTH_NV 0x8F30
#define GL_SAMPLE_COVERAGE_INVERT 0x80AB
#define GL_IMAGE_BINDING_LAYER_EXT 0x8F3D
#define GL_NOTEQUAL 0x0205
#define GL_PROGRAM_OUTPUT 0x92E4
#define GL_COMPRESSED_ALPHA_ARB 0x84E9
#define GL_INT_SAMPLER_1D_ARRAY 0x8DCE
#define GL_TEXTURE_RED_SIZE 0x805C
#define GL_STENCIL_COMPONENTS 0x8285
#define GL_FRAMEBUFFER_RENDERABLE 0x8289
#define GL_CURRENT_ATTRIB_NV 0x8626
#define GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX 0x904A
#define GL_COLOR_ATTACHMENT14_EXT 0x8CEE
#define GL_CURRENT_QUERY_ARB 0x8865
#define GL_TEXTURE_BINDING_CUBE_MAP_ARRAY 0x900A
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS_NV 0x8C8B
#define GL_IMAGE_TEXEL_SIZE 0x82A7
#define GL_TEXTURE_SHARED_SIZE 0x8C3F
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_GEOMETRY_SUBROUTINE_UNIFORM 0x92F1
#define GL_RENDERBUFFER_DEPTH_SIZE 0x8D54
#define GL_PACK_COMPRESSED_BLOCK_SIZE 0x912E
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT 0x8CD3
#define GL_TESS_EVALUATION_SUBROUTINE_UNIFORM 0x92F0
#define GL_DOUBLE_VEC4_EXT 0x8FFE
#define GL_TEXTURE_COORD_ARRAY_POINTER_EXT 0x8092
#define GL_FONT_GLYPHS_AVAILABLE_NV 0x9368
#define GL_SHORT 0x1402
#define GL_CW 0x0900
#define GL_STENCIL_BACK_FUNC_ATI 0x8800
#define GL_MAX_VERTEX_UNIFORM_COMPONENTS 0x8B4A
#define GL_IMAGE_1D 0x904C
#define GL_MAX_PROGRAM_TEMPORARIES_ARB 0x88A5
#define GL_RGB8I_EXT 0x8D8F
#define GL_IMAGE_1D_ARRAY_EXT 0x9052
#define GL_ACTIVE_VARIABLES 0x9305
#define GL_UNSIGNED_INT_10F_11F_11F_REV 0x8C3B
#define GL_STATIC_DRAW 0x88E4
#define GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV 0x8F1E
#define GL_VERTEX_STATE_PROGRAM_NV 0x8621
#define GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE 0x8213
#define GL_NUM_COMPATIBLE_SUBROUTINES 0x8E4A
#define GL_INDEX_ARRAY_TYPE_EXT 0x8085
#define GL_SAMPLER_3D 0x8B5F
#define GL_MAX_INTEGER_SAMPLES 0x9110
#define GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5E
#define GL_TEXTURE_GREEN_SIZE 0x805D
#define GL_ELEMENT_ADDRESS_COMMAND_NV 0x0008
#define GL_TEXTURE8_ARB 0x84C8
#define GL_TEXTURE_COORD_ARRAY_ADDRESS_NV 0x8F25
#define GL_TEXTURE_BINDING_2D_ARRAY 0x8C1D
#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR 0x88FE
#define GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS 0x92CC
#define GL_TEXTURE_SHADOW 0x82A1
#define GL_SAMPLE_COVERAGE_ARB 0x80A0
#define GL_CONTEXT_ROBUST_ACCESS 0x90F3
#define GL_UNDEFINED_VERTEX 0x8260
#define GL_PATH_MODELVIEW_MATRIX_NV 0x0BA6
#define GL_SAMPLER 0x82E6
#define GL_SAMPLES 0x80A9
#define GL_FRAMEBUFFER_BINDING_EXT 0x8CA6
#define GL_VERTEX_PROGRAM_POINT_SIZE_ARB 0x8642
#define GL_UNSIGNED_INT_VEC2 0x8DC6
#define GL_UNSIGNED_INT_VEC3 0x8DC7
#define GL_UNSIGNED_INT_VEC4 0x8DC8
#define GL_UNSIGNED_SHORT_5_6_5 0x8363
#define GL_COLOR_ARRAY_EXT 0x8076
#define GL_RGBA16UI_EXT 0x8D76
#define GL_COMPRESSED_SRGB8_ETC2 0x9275
#define GL_BUFFER_SERIALIZED_MODIFY_APPLE 0x8A12
#define GL_MAX_VARYING_COMPONENTS_EXT 0x8B4B
#define GL_LUMINANCE_INTEGER_EXT 0x8D9C
#define GL_TEXTURE_BUFFER_OFFSET 0x919D
#define GL_DEPTH_RENDERABLE 0x8287
#define GL_RGB32UI 0x8D71
#define GL_GET_TEXTURE_IMAGE_FORMAT 0x8291
#define GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB 0x88B4
#define GL_PROGRAM_ERROR_POSITION_NV 0x864B
#define GL_IMAGE_COMPATIBILITY_CLASS 0x82A8
#define GL_PROXY_TEXTURE_RECTANGLE 0x84F7
#define GL_XOR 0x1506
#define GL_STENCIL_BACK_FAIL_ATI 0x8801
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT 0x8CD4
#define GL_TRANSFORM_FEEDBACK_BUFFER_MODE_NV 0x8C7F
#define GL_INT64_VEC3_ARB 0x8FEA
#define GL_COMPRESSED_R11_EAC 0x9270
#define GL_MAX_NUM_COMPATIBLE_SUBROUTINES 0x92F8
#define GL_CONSTANT_COLOR 0x8001
#define GL_RG8UI 0x8238
#define GL_TEXTURE8 0x84C8
#define GL_TEXTURE9 0x84C9
#define GL_DEBUG_CALLBACK_USER_PARAM_ARB 0x8245
#define GL_TEXTURE4 0x84C4
#define GL_TEXTURE5 0x84C5
#define GL_TEXTURE6 0x84C6
#define GL_TEXTURE7 0x84C7
#define GL_TEXTURE0 0x84C0
#define GL_CONTEXT_PROFILE_MASK 0x9126
#define GL_TEXTURE2 0x84C2
#define GL_TEXTURE3 0x84C3
#define GL_FLOAT_VEC2_ARB 0x8B50
#define GL_BOOL_VEC4 0x8B59
#define GL_RENDERBUFFER_BINDING_EXT 0x8CA7
#define GL_BOOL_VEC3 0x8B58
#define GL_BOOL_VEC2 0x8B57
#define GL_OBJECT_VALIDATE_STATUS_ARB 0x8B83
#define GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS 0x8E85
#define GL_IMAGE_2D_MULTISAMPLE_EXT 0x9055
#define GL_ONE 1
#define GL_BLEND_EQUATION_ALPHA_EXT 0x883D
#define GL_SRC_ALPHA_SATURATE 0x0308
#define GL_TRIANGLE_STRIP 0x0005
#define GL_INT64_VEC4_ARB 0x8FEB
#define GL_PROGRAM_BINARY_RETRIEVABLE_HINT 0x8257
#define GL_FRONT_LEFT 0x0400
#define GL_RELATIVE_SMALL_CW_ARC_TO_NV 0x15
#define GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS 0x90DA
#define GL_NUM_SAMPLE_COUNTS 0x9380
#define GL_IS_PER_PATCH 0x92E7
#define GL_FRAGMENT_TEXTURE 0x829F
#define GL_VERTEX_ARRAY_BINDING 0x85B5
#define GL_TIMEOUT_EXPIRED 0x911B
#define GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910C
#define GL_VERTEX_PROGRAM_TWO_SIDE_NV 0x8643
#define GL_VERTEX_ARRAY_BUFFER_BINDING_ARB 0x8896
#define GL_STENCIL_FUNC 0x0B92
#define GL_TEXTURE14 0x84CE
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB 0x86A0
#define GL_DEPTH_STENCIL_TEXTURE_MODE 0x90EA
#define GL_DRAW_BUFFER8_ARB 0x882D
#define GL_TEXTURE_VIEW_NUM_LAYERS 0x82DE
#define GL_BACK 0x0405
#define GL_LUMINANCE8UI_EXT 0x8D80
#define GL_TEXTURE_IMMUTABLE_FORMAT 0x912F
#define GL_ATOMIC_COUNTER_BUFFER 0x92C0
#define GL_CLIP_DISTANCE1 0x3001
#define GL_CLIP_DISTANCE2 0x3002
#define GL_CLIP_DISTANCE5 0x3005
#define GL_CLIP_DISTANCE4 0x3004
#define GL_CLIP_DISTANCE7 0x3007
#define GL_CLIP_DISTANCE6 0x3006
#define GL_FRONT_AND_BACK 0x0408
#define GL_MAX_COMBINED_DIMENSIONS 0x8282
#define GL_TEXTURE_COORD_ARRAY_SIZE_EXT 0x8088
#define GL_PATH_MITER_LIMIT_NV 0x907A
#define GL_SYNC_FENCE 0x9116
#define GL_IMAGE_CLASS_2_X_8 0x82C0
#define GL_RGB8_SNORM 0x8F96
#define GL_DEBUG_LOGGED_MESSAGES 0x9145
#define GL_SHADER_STORAGE_BUFFER_SIZE 0x90D5
#define GL_DOUBLE_VEC4 0x8FFE
#define GL_UNSIGNED_SHORT_4_4_4_4 0x8033
#define GL_DOUBLE_VEC2 0x8FFC
#define GL_DOUBLE_VEC3 0x8FFD
#define GL_MAP1_VERTEX_ATTRIB10_4_NV 0x866A
#define GL_MAX_TEXTURE_COORDS_ARB 0x8871
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB 0x8B4C
#define GL_RGBA16UI 0x8D76
#define GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED 0x8221
#define GL_SAMPLER_2D_SHADOW_ARB 0x8B62
#define GL_UNSIGNED_INT64_ARB 0x140F
#define GL_VERTEX_ATTRIB_ARRAY8_NV 0x8658
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
#define GL_MAX_DRAW_BUFFERS 0x8824
#define GL_ONE_MINUS_DST_ALPHA 0x0305
#define GL_IMAGE_CUBE_MAP_ARRAY 0x9054
#define GL_R32UI 0x8236
#define GL_RENDERBUFFER_DEPTH_SIZE_EXT 0x8D54
#define GL_QUERY_WAIT_INVERTED 0x8E17
#define GL_PATCH_DEFAULT_INNER_LEVEL 0x8E73
#define GL_TRANSFORM_FEEDBACK_BINDING_NV 0x8E25
#define GL_COLOR_ATTACHMENT9_EXT 0x8CE9
#define GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB 0x862E
#define GL_FRAMEBUFFER_BARRIER_BIT 0x00000400
#define GL_MAJOR_VERSION 0x821B
#define GL_VERSION 0x1F02
#define GL_LUMINANCE32UI_EXT 0x8D74
#define GL_BGR_INTEGER 0x8D9A
#define GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS 0x8264
#define GL_MAP_UNSYNCHRONIZED_BIT 0x0020
#define GL_LAYER_PROVOKING_VERTEX 0x825E
#define GL_UNIFORM_BUFFER_BINDING 0x8A28
#define GL_UNIFORM_TYPE 0x8A37
#define GL_RENDERBUFFER_RED_SIZE_EXT 0x8D50
#define GL_COMMAND_BARRIER_BIT_EXT 0x00000040
#define GL_COLOR_ATTACHMENT13_EXT 0x8CED
#define GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS 0x90DC
#define GL_FRONT_RIGHT 0x0401
#define GL_MAP_INVALIDATE_BUFFER_BIT 0x0008
#define GL_FRACTIONAL_ODD 0x8E7B
#define GL_DEPTH_TEST 0x0B71
#define GL_TRACK_MATRIX_NV 0x8648
#define GL_VERTEX_ATTRIB_ARRAY_POINTER 0x8645
#define GL_IMAGE_BINDING_LAYERED_EXT 0x8F3C
#define GL_MULTISAMPLE 0x809D
#define GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN 0x8C88
#define GL_INT8_VEC2_NV 0x8FE1
#define GL_FUNC_SUBTRACT 0x800A
#define GL_MAX_TESS_GEN_LEVEL 0x8E7E
#define GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH 0x8A35
#define GL_TEXTURE_COMPRESSED_BLOCK_SIZE 0x82B3
#define GL_MAX_COMPUTE_FIXED_GROUP_INVOCATIONS_ARB 0x90EB
#define GL_MAX_ELEMENT_INDEX 0x8D6B
#define GL_COLOR_ATTACHMENT15 0x8CEF
#define GL_COLOR_ATTACHMENT14 0x8CEE
#define GL_COLOR_ATTACHMENT17 0x8CF1
#define GL_COLOR_ATTACHMENT16 0x8CF0
#define GL_COLOR_ATTACHMENT11 0x8CEB
#define GL_COLOR_ATTACHMENT10 0x8CEA
#define GL_COLOR_ATTACHMENT13 0x8CED
#define GL_COLOR_ATTACHMENT12 0x8CEC
#define GL_NEAREST 0x2600
#define GL_COLOR_ATTACHMENT19 0x8CF3
#define GL_COLOR_ATTACHMENT18 0x8CF2
#define GL_MAX_RENDERBUFFER_SIZE_EXT 0x84E8
#define GL_UNIFORM_BLOCK 0x92E2
#define GL_MAX_DUAL_SOURCE_DRAW_BUFFERS 0x88FC
#define GL_RESTART_PATH_NV 0xF0
#define GL_SAMPLES_PASSED_ARB 0x8914
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
#define GL_SAMPLER_1D_ARRAY_SHADOW_EXT 0x8DC3
#define GL_RASTERIZER_DISCARD 0x8C89
#define GL_INTERNALFORMAT_PREFERRED 0x8270
#define GL_RENDERBUFFER_EXT 0x8D41
#define GL_DYNAMIC_DRAW_ARB 0x88E8
#define GL_DEPTH_CLEAR_VALUE 0x0B73
#define GL_UNSIGNED_INT_SAMPLER_BUFFER_EXT 0x8DD8
#define GL_GEOMETRY_INPUT_TYPE 0x8917
#define GL_TRANSFORM_FEEDBACK_BUFFER_PAUSED 0x8E23
#define GL_COMPUTE_SHADER 0x91B9
#define GL_UNSIGNED_INT_5_9_9_9_REV 0x8C3E
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE 0x8CD0
#define GL_QUERY 0x82E3
#define GL_RELATIVE_ROUNDED_RECT_NV 0xE9
#define GL_BLEND_COLOR_EXT 0x8005
#define GL_MIRRORED_REPEAT 0x8370
#define GL_TESS_EVALUATION_SUBROUTINE 0x92EA
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
#define GL_MAX_VARYING_VECTORS 0x8DFC
#define GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX 0x9047
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
#define GL_READ_PIXELS 0x828C
#define GL_ONE_MINUS_DST_COLOR 0x0307
#define GL_BLEND_EQUATION_RGB 0x8009
#define GL_GEOMETRY_VERTICES_OUT_ARB 0x8DDA
#define GL_GLYPH_VERTICAL_BEARING_Y_BIT_NV 0x40
#define GL_TEXTURE19_ARB 0x84D3
#define GL_INNOCENT_CONTEXT_RESET_ARB 0x8254
#define GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT 0x90DF
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_INTERNALFORMAT_SUPPORTED 0x826F
#define GL_FRAMEBUFFER_SRGB 0x8DB9
#define GL_INDEX_ARRAY_BUFFER_BINDING_ARB 0x8899
#define GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS 0x9314
#define GL_IMAGE_BUFFER 0x9051
#define GL_FONT_UNITS_PER_EM_BIT_NV 0x00100000
#define GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS 0x90D6
#define GL_IMAGE_BINDING_LAYERED 0x8F3C
#define GL_DUP_LAST_CUBIC_CURVE_TO_NV 0xF4
#define GL_R8I 0x8231
#define GL_UNSIGNED_INT_IMAGE_2D 0x9063
#define GL_NORMAL_ARRAY_EXT 0x8075
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
#define GL_LINK_STATUS 0x8B82
#define GL_ROUNDED_RECT_NV 0xE8
#define GL_UNPACK_SKIP_IMAGES_EXT 0x806D
#define GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_TEST 0x82AC
#define GL_FLOAT_VEC3_ARB 0x8B51
#define GL_FONT_DESCENDER_BIT_NV 0x00400000
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS_EXT 0x8C8B
#define GL_FRAMEBUFFER_DEFAULT_HEIGHT 0x9311
#define GL_GEOMETRY_SUBROUTINE 0x92EB
#define GL_MAP_FLUSH_EXPLICIT_BIT 0x0010
#define GL_RGB4 0x804F
#define GL_RGBA8I_EXT 0x8D8E
#define GL_MATRIX3_ARB 0x88C3
#define GL_CONSTANT_ALPHA_EXT 0x8003
#define GL_AND_INVERTED 0x1504
#define GL_DRAW_BUFFER12_ATI 0x8831
#define GL_NORMAL_ARRAY_ADDRESS_NV 0x8F22
#define GL_INTENSITY32I_EXT 0x8D85
#define GL_VERTEX_ATTRIB_BINDING 0x82D4
#define GL_GEOMETRY_INPUT_TYPE_ARB 0x8DDB
#define GL_POLYGON_OFFSET_POINT 0x2A01
#define GL_TEXTURE_GATHER 0x82A2
#define GL_FRAMEBUFFER_DEFAULT_WIDTH 0x9310
#define GL_RIGHT 0x0407
#define GL_IMAGE_CLASS_4_X_8 0x82BF
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB 0x8247
#define GL_ALPHA8UI_EXT 0x8D7E
#define GL_DEPTH_COMPONENT16 0x81A5
#define GL_MAX_SAMPLE_MASK_WORDS 0x8E59
#define GL_UNSIGNALED 0x9118
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#define GL_RGB32I 0x8D83
#define GL_SAMPLER_BUFFER 0x8DC2
#define GL_BGRA_INTEGER 0x8D9B
#define GL_FALSE 0
#define GL_MAP2_VERTEX_ATTRIB9_4_NV 0x8679
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_PROGRAM_BINDING_ARB 0x8677
#define GL_UNSIGNED_INT_IMAGE_1D_ARRAY 0x9068
#define GL_PROGRAM_POINT_SIZE 0x8642
#define GL_UNPACK_COMPRESSED_BLOCK_DEPTH 0x9129
#define GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING 0x8210
#define GL_INDEX_ARRAY_ADDRESS_NV 0x8F24
#define GL_UNSIGNED_INT_VEC2_EXT 0x8DC6
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB 0x824E
#define GL_ARRAY_BUFFER_BINDING_ARB 0x8894
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER 0x92CA
#define GL_DYNAMIC_COPY_ARB 0x88EA
#define GL_TEXTURE5_ARB 0x84C5
#define GL_FLOAT_MAT4_ARB 0x8B5C
#define GL_DOUBLE_VEC3_EXT 0x8FFD
#define GL_PROGRAM_PIPELINE_BINDING 0x825A
#define GL_MAX_RECTANGLE_TEXTURE_SIZE 0x84F8
#define GL_PATH_STENCIL_DEPTH_OFFSET_UNITS_NV 0x90BE
#define GL_COLOR_ATTACHMENT0_EXT 0x8CE0
#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS 0x8B49
#define GL_UNIFORM_BLOCK_NAME_LENGTH 0x8A41
#define GL_FRAMEBUFFER_DEFAULT 0x8218
#define GL_ATOMIC_COUNTER_BARRIER_BIT 0x00001000
#define GL_TEXTURE_BINDING_RECTANGLE 0x84F6
#define GL_LINE_SMOOTH_HINT 0x0C52
#define GL_FRONT 0x0404
#define GL_MAX_COMPUTE_VARIABLE_GROUP_INVOCATIONS_ARB 0x9344
#define GL_MAX_FRAGMENT_UNIFORM_VECTORS 0x8DFD
#define GL_FIRST_TO_REST_NV 0x90AF
#define GL_MAX_DEBUG_LOGGED_MESSAGES_ARB 0x9144
#define GL_NUM_EXTENSIONS 0x821D
#define GL_TRANSFORM_FEEDBACK_BUFFER_PAUSED_NV 0x8E23
#define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS 0x92CE
#define GL_FRAMEBUFFER_BINDING 0x8CA6
#define GL_REFERENCED_BY_VERTEX_SHADER 0x9306
#define GL_RGB32F 0x8815
#define GL_COLOR_RENDERABLE 0x8286
#define GL_BUFFER_GPU_ADDRESS_NV 0x8F1D
#define GL_UNSIGNED_INT_SAMPLER_1D_ARRAY 0x8DD6
#define GL_DEPTH_FUNC 0x0B74
#define GL_UNSIGNED_INT_SAMPLER_2D_ARRAY 0x8DD7
#define GL_INVALID_ENUM 0x0500
#define GL_MAX_VERTEX_VARYING_COMPONENTS_EXT 0x8DDE
#define GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS 0x92CD
#define GL_STANDARD_FONT_NAME_NV 0x9072
#define GL_UNSIGNED_INT_IMAGE_CUBE_EXT 0x9066
#define GL_MATRIX5_ARB 0x88C5
#define GL_SAMPLE_ALPHA_TO_COVERAGE_ARB 0x809E
#define GL_TESS_EVALUATION_TEXTURE 0x829D
#define GL_SAMPLE_COVERAGE_VALUE_ARB 0x80AA
#define GL_WAIT_FAILED 0x911D
#define GL_INT16_NV 0x8FE4
#define GL_IMAGE_CLASS_2_X_32 0x82BA
#define GL_COMPARE_REF_TO_TEXTURE 0x884E
#define GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE 0x8211
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_STANDARD_FONT_FORMAT_NV 0x936C
#define GL_INT_IMAGE_1D 0x9057
#define GL_MAP2_VERTEX_ATTRIB3_4_NV 0x8673
#define GL_RENDERBUFFER_GREEN_SIZE 0x8D51
#define GL_DEBUG_SOURCE_THIRD_PARTY_ARB 0x8249
#define GL_TEXTURE_COMPRESSION_HINT_ARB 0x84EF
#define GL_OBJECT_SUBTYPE_ARB 0x8B4F
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER 0x8CD4
#define GL_CIRCULAR_TANGENT_ARC_TO_NV 0xFC
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH 0x8243
#define GL_PATH_OBJECT_BOUNDING_BOX_NV 0x908A
#define GL_FONT_MAX_ADVANCE_WIDTH_BIT_NV 0x01000000
#define GL_FLOAT_MAT4x3 0x8B6A
#define GL_SKIP_COMPONENTS3_NV -4
#define GL_IMAGE_1D_ARRAY 0x9052
#define GL_FONT_ASCENDER_BIT_NV 0x00200000
#define GL_RELATIVE_VERTICAL_LINE_TO_NV 0x09
#define GL_PROGRAM 0x82E2
#define GL_LARGE_CW_ARC_TO_NV 0x18
#define GL_RG8_SNORM 0x8F95
#define GL_PATH_PROJECTION_NV 0x1701
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_DEBUG_SEVERITY_MEDIUM_ARB 0x9147
#define GL_VERTEX_PROGRAM_TWO_SIDE_ARB 0x8643
#define GL_DEBUG_TYPE_PORTABILITY_ARB 0x824F
#define GL_FLOAT16_VEC4_NV 0x8FFB
#define GL_UNSIGNED_INT64_VEC3_NV 0x8FF6
#define GL_STENCIL_INDEX1_EXT 0x8D46
#define GL_LOW_INT 0x8DF3
#define GL_LUMINANCE16UI_EXT 0x8D7A
#define GL_DOUBLE_MAT2x4 0x8F4A
#define GL_UNPACK_SKIP_PIXELS 0x0CF4
#define GL_GEOMETRY_INPUT_TYPE_EXT 0x8DDB
#define GL_MAX_VERTEX_OUTPUT_COMPONENTS 0x9122
#define GL_KEEP 0x1E00
#define GL_NO_RESET_NOTIFICATION 0x8261
#define GL_DECR 0x1E03
#define GL_PROXY_TEXTURE_2D 0x8064
#define GL_PROGRAM_STRING_NV 0x8628
#define GL_MAP2_VERTEX_ATTRIB8_4_NV 0x8678
#define GL_OR 0x1507
#define GL_IMAGE_PIXEL_TYPE 0x82AA
#define GL_COLOR_ARRAY_BUFFER_BINDING_ARB 0x8898
#define GL_MAX_COLOR_TEXTURE_SAMPLES 0x910E
#define GL_ACTIVE_SUBROUTINES 0x8DE5
#define GL_TRANSFORM_FEEDBACK_BUFFER_NV 0x8C8E
#define GL_LINE_LOOP 0x0002
#define GL_TEXTURE6_ARB 0x84C6
#define GL_BACK_RIGHT 0x0403
#define GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY_EXT 0x906A
#define GL_SUBPIXEL_BITS 0x0D50
#define GL_COLOR_ENCODING 0x8296
#define GL_INT_SAMPLER_CUBE_EXT 0x8DCC
#define GL_VERTEX_ATTRIB_ARRAY9_NV 0x8659
#define GL_R3_G3_B2 0x2A10
#define GL_PATH_END_CAPS_NV 0x9076
#define GL_CIRCULAR_CCW_ARC_TO_NV 0xF8
#define GL_IMAGE_CLASS_10_10_10_2 0x82C3
#define GL_TEXTURE1 0x84C1
#define GL_PATH_FORMAT_PS_NV 0x9071
#define GL_FRAMEBUFFER_BARRIER_BIT_EXT 0x00000400
#define GL_TIME_ELAPSED 0x88BF
#define GL_SKIP_COMPONENTS1_NV -6
#define GL_UNSIGNED_SHORT_5_6_5_REV 0x8364
#define GL_COMPRESSED_INTENSITY_ARB 0x84EC
#define GL_IMAGE_2D_RECT_EXT 0x904F
#define GL_BUFFER_ACCESS 0x88BB
#define GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT 0x00000004
#define GL_NORMAL_ARRAY_COUNT_EXT 0x8080
#define GL_SHADER_COMPILER 0x8DFA
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT 0x8CD2
#define GL_TEXTURE_BUFFER_ARB 0x8C2A
#define GL_MAP1_VERTEX_ATTRIB4_4_NV 0x8664
#define GL_DRAW_ARRAYS_STRIP_COMMAND_NV 0x0005
#define GL_TEXTURE_RESIDENT_EXT 0x8067
#define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING_ARB 0x889A
#define GL_MAP2_VERTEX_ATTRIB15_4_NV 0x867F
#define GL_COMMAND_BARRIER_BIT 0x00000040
#define GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS 0x90DD
#define GL_TEXTURE7_ARB 0x84C7
#define GL_SKIP_MISSING_GLYPH_NV 0x90A9
#define GL_UNIFORM_BUFFER_ADDRESS_NV 0x936F
#define GL_TRIANGULAR_NV 0x90A5
#define GL_POLYGON_SMOOTH_HINT 0x0C53
#define GL_DEBUG_SEVERITY_MEDIUM 0x9147
#define GL_MAX_COMPUTE_VARIABLE_GROUP_SIZE_ARB 0x9345
#define GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS 0x8E83
#define GL_MEDIUM_FLOAT 0x8DF1
#define GL_MATRIX2_ARB 0x88C2
#define GL_IMAGE_CLASS_1_X_8 0x82C1
#define GL_INT_IMAGE_CUBE 0x905B
#define GL_MATRIX0_ARB 0x88C0
#define GL_TEXTURE_PRIORITY_EXT 0x8066
#define GL_DEPTH_RANGE 0x0B70
#define GL_MAX_LABEL_LENGTH 0x82E8
#define GL_DEBUG_TYPE_PORTABILITY 0x824F
#define GL_DEBUG_GROUP_STACK_DEPTH 0x826D
#define GL_UNSIGNED_INT_IMAGE_BUFFER 0x9067
#define GL_MAX_TRACK_MATRICES_NV 0x862F
#define GL_R8UI 0x8232
#define GL_INVERSE_TRANSPOSE_NV 0x862D
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS_EXT 0x8C80
#define GL_DELETE_STATUS 0x8B80
#define GL_BOOL_ARB 0x8B56
#define GL_PROGRAM_MATRIX_EXT 0x8E2D
#define GL_SAMPLER_2D_ARRAY_SHADOW 0x8DC4
#define GL_DOUBLE_MAT4x3 0x8F4E
#define GL_IMAGE_BINDING_NAME_EXT 0x8F3A
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING 0x889F
#define GL_MAX_SHADER_STORAGE_BLOCK_SIZE 0x90DE
#define GL_COPY_WRITE_BUFFER 0x8F37
#define GL_NORMAL_ARRAY_STRIDE_EXT 0x807F
#define GL_IMAGE_CLASS_4_X_16 0x82BC
#define GL_SRGB8_ALPHA8 0x8C43
#define GL_BUFFER_IMMUTABLE_STORAGE 0x821F
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_ARB 0x8DA8
#define GL_FLOAT_MAT2x3 0x8B65
#define GL_REFERENCED_BY_TESS_EVALUATION_SHADER 0x9308
#define GL_DEBUG_CALLBACK_FUNCTION 0x8244
#define GL_DEBUG_TYPE_PUSH_GROUP 0x8269
#define GL_SAMPLER_2D_ARRAY 0x8DC1
#define GL_RENDERER 0x1F01
#define GL_RENDERBUFFER_BLUE_SIZE_EXT 0x8D52
#define GL_QUERY_NO_WAIT_NV 0x8E14
#define GL_PATH_MODELVIEW_STACK_DEPTH_NV 0x0BA3
#define GL_PROGRAM_PIPELINE 0x82E4
#define GL_INTERNALFORMAT_ALPHA_TYPE 0x827B
#define GL_QUERY_COUNTER_BITS_ARB 0x8864
#define GL_COLOR_LOGIC_OP 0x0BF2
#define GL_MAX_TEXTURE_BUFFER_SIZE_ARB 0x8C2B
#define GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH 0x8C76
#define GL_UNIFORM_BLOCK_INDEX 0x8A3A
#define GL_VIEW_CLASS_S3TC_DXT3_RGBA 0x82CE
#define GL_COPY_READ_BUFFER_BINDING 0x8F36
#define GL_IMAGE_CUBE 0x9050
#define GL_STENCIL_BACK_PASS_DEPTH_PASS_ATI 0x8803
#define GL_BLEND_SRC_ALPHA_EXT 0x80CB
#define GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE 0x92D8
#define GL_COMPRESSED_TEXTURE_FORMATS_ARB 0x86A3
#define GL_ARRAY_BUFFER_ARB 0x8892
#define GL_VERTEX_BINDING_STRIDE 0x82D8
#define GL_INTERNALFORMAT_DEPTH_TYPE 0x827C
#define GL_CUBIC_CURVE_TO_NV 0x0C
#define GL_NUM_SHADING_LANGUAGE_VERSIONS 0x82E9
#define GL_RGBA_INTEGER_MODE_EXT 0x8D9E
#define GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB 0x88A7
#define GL_CONVEX_HULL_NV 0x908B
#define GL_INTERNALFORMAT_RED_TYPE 0x8278
#define GL_TRANSFORM_FEEDBACK_BINDING 0x8E25
#define GL_RENDERBUFFER_ALPHA_SIZE_EXT 0x8D53
#define GL_TEXTURE_BINDING_BUFFER_EXT 0x8C2C
#define GL_MODELVIEW_PROJECTION_NV 0x8629
#define GL_COMPRESSED_SRGB 0x8C48
#define GL_DRAW_INDIRECT_UNIFIED_NV 0x8F40
#define GL_DRAW_BUFFER15_ATI 0x8834
#define GL_EQUIV 0x1509
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB 0x889F
#define GL_RENDERBUFFER_GREEN_SIZE_EXT 0x8D51
#define GL_INTERNALFORMAT_ALPHA_SIZE 0x8274
#define GL_TEXTURE23_ARB 0x84D7
#define GL_ALPHA32UI_EXT 0x8D72
#define GL_INT_IMAGE_1D_EXT 0x9057
#define GL_OBJECT_ACTIVE_ATTRIBUTE_MAX_LENGTH_ARB 0x8B8A
#define GL_MAX_DEBUG_MESSAGE_LENGTH_ARB 0x9143
#define GL_MAX_COMPUTE_FIXED_GROUP_SIZE_ARB 0x91BF
#define GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB 0x88B2
#define GL_PATH_JOIN_STYLE_NV 0x9079
#define GL_TEXTURE_RENDERBUFFER_NV 0x8E55
#define GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT 0x00000001
#define GL_VERTEX_ATTRIB_RELATIVE_OFFSET 0x82D5
#define GL_FRAGMENT_INTERPOLATION_OFFSET_BITS 0x8E5D
#define GL_TEXTURE_COORD_ARRAY_COUNT_EXT 0x808B
#define GL_MAX_ARRAY_TEXTURE_LAYERS_EXT 0x88FF
#define GL_FRAMEBUFFER_ATTACHMENT_LAYERED_ARB 0x8DA7
#define GL_STENCIL_BACK_PASS_DEPTH_FAIL_ATI 0x8802
#define GL_INT_IMAGE_1D_ARRAY 0x905D
#define GL_IDENTITY_NV 0x862A
#define GL_TRANSFORM_FEEDBACK_BUFFER_SIZE_EXT 0x8C85
#define GL_STENCIL_BUFFER_BIT 0x00000400
#define GL_ALPHA16I_EXT 0x8D8A
#define GL_UNKNOWN_CONTEXT_RESET 0x8255
#define GL_MAX_TEXTURE_BUFFER_SIZE_EXT 0x8C2B
#define GL_GPU_ADDRESS_NV 0x8F34
#define GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS 0x8A33
#define GL_UNSIGNED_INT_IMAGE_2D_RECT 0x9065
#define GL_UNSIGNED_INT64_NV 0x140F
#define GL_COLOR_ATTACHMENT31 0x8CFF
#define GL_RGB32UI_EXT 0x8D71
#define GL_SRGB_WRITE 0x8298
#define GL_SAMPLER_CUBE_MAP_ARRAY 0x900C
#define GL_POINT_SIZE_MAX_EXT 0x8127
#define GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS 0x8E89
#define GL_MAX_VIEWPORTS 0x825B
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910D
#define GL_READ_WRITE 0x88BA
#define GL_GLYPH_HORIZONTAL_BEARING_X_BIT_NV 0x04
#define GL_MITER_TRUNCATE_NV 0x90A8
#define GL_MAX_FRAGMENT_UNIFORM_BLOCKS 0x8A2D
#define GL_FONT_TARGET_UNAVAILABLE_NV 0x9369
#define GL_DRAW_BUFFER9_ATI 0x882E
#define GL_COMPRESSED_RGB_ARB 0x84ED
#define GL_PROXY_TEXTURE_1D 0x8063
#define GL_DEBUG_TYPE_OTHER 0x8251
#define GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX 0x9049
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED 0x886A
#define GL_UNSIGNED_INT_IMAGE_1D_ARRAY_EXT 0x9068
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_EXT 0x8DA9
#define GL_TEXTURE_COMPRESSION_HINT 0x84EF
#define GL_DRAW_ELEMENTS_INSTANCED_COMMAND_NV 0x0006
#define GL_TRIANGLE_STRIP_ADJACENCY_ARB 0x000D
#define GL_MAX_3D_TEXTURE_SIZE_EXT 0x8073
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_LAYERS 0x8281
#define GL_MAX_COMBINED_ATOMIC_COUNTERS 0x92D7
#define GL_VERTEX_ARRAY_POINTER_EXT 0x808E
#define GL_PROXY_TEXTURE_1D_ARRAY_EXT 0x8C19
#define GL_MAX_PROGRAM_TOTAL_OUTPUT_COMPONENTS_NV 0x8C28
#define GL_FLOAT_VEC4_ARB 0x8B52
#define GL_PATH_STENCIL_REF_NV 0x90B8
#define GL_CURRENT_VERTEX_ATTRIB_ARB 0x8626
#define GL_MATRIX2_NV 0x8632
#define GL_UNSIGNED_SHORT_5_5_5_1 0x8034
#define GL_DRAW_BUFFER7_ATI 0x882C
#define GL_UTF8_NV 0x909A
#define GL_DRAW_BUFFER11_ARB 0x8830
#define GL_TEXTURE_BASE_LEVEL 0x813C
#define GL_MAX_ELEMENTS_INDICES 0x80E9
#define GL_NO_ERROR 0
#define GL_LOSE_CONTEXT_ON_RESET 0x8252
#define GL_VERTICAL_LINE_TO_NV 0x08
#define GL_GEOMETRY_VERTICES_OUT 0x8916
#define GL_RGB8 0x8051
#define GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS 0x90CC
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_CLIENT_ACTIVE_TEXTURE_ARB 0x84E1
#define GL_CLIENT_STORAGE_BIT 0x0200
#define GL_TYPE 0x92FA
#define GL_RGB5 0x8050
#define GL_CULL_FACE 0x0B44
#define GL_VERTEX_ID_NV 0x8C7B
#define GL_PATH_COVER_DEPTH_FUNC_NV 0x90BF
#define GL_SAMPLE_COVERAGE_VALUE 0x80AA
#define GL_PROXY_TEXTURE_CUBE_MAP 0x851B
#define GL_SAMPLER_1D 0x8B5D
#define GL_COLOR_ATTACHMENT2_EXT 0x8CE2
#define GL_MAX_ELEMENTS_VERTICES 0x80E8
#define GL_NEVER 0x0200
#define GL_INVALID_VALUE 0x0501
#define GL_STENCIL_VALUE_MASK 0x0B93
#define GL_BLEND_DST 0x0BE0
#define GL_DEBUG_SOURCE_SHADER_COMPILER_ARB 0x8248
#define GL_PATH_STENCIL_VALUE_MASK_NV 0x90B9
#define GL_TEXTURE_COMPRESSED_BLOCK_WIDTH 0x82B1
#define GL_STREAM_COPY_ARB 0x88E2
#define GL_TEXTURE25_ARB 0x84D9
#define GL_COMPRESSED_RGBA_ARB 0x84EE
#define GL_TEXTURE_SWIZZLE_RGBA 0x8E46
#define GL_SHADER_STORAGE_BUFFER_START 0x90D4
#define GL_DEPTH_COMPONENT32 0x81A7
#define GL_RGBA 0x1908
#define GL_READ_FRAMEBUFFER 0x8CA8
#define GL_MIN_PROGRAM_TEXEL_OFFSET 0x8904
#define GL_VENDOR 0x1F00
#define GL_ELEMENT_ARRAY_BUFFER_ARB 0x8893
#define GL_MIRROR_CLAMP_TO_EDGE 0x8743
#define GL_TEXTURE_2D_ARRAY 0x8C1A
#define GL_TEXTURE_BINDING_2D 0x8069
#define GL_REFERENCED_BY_FRAGMENT_SHADER 0x930A
#define GL_ISOLINES 0x8E7A
#define GL_TRANSFORM_FEEDBACK_BUFFER_BINDING_EXT 0x8C8F
#define GL_ACTIVE_PROGRAM 0x8259
#define GL_BUFFER_ACCESS_FLAGS 0x911F
#define GL_IMAGE_CUBE_MAP_ARRAY_EXT 0x9054
#define GL_TESS_GEN_MODE 0x8E76
#define GL_DRAW_BUFFER1_ARB 0x8826
#define GL_UNSIGNED_INT64_VEC4_NV 0x8FF7
#define GL_RELATIVE_LARGE_CW_ARC_TO_NV 0x19
#define GL_FRAGMENT_INPUT_NV 0x936D
#define GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY 0x9103
#define GL_ACTIVE_TEXTURE_ARB 0x84E0
#define GL_MAX 0x8008
#define GL_PROXY_TEXTURE_2D_ARRAY 0x8C1B
#define GL_LINE_SMOOTH 0x0B20
#define GL_PROXY_TEXTURE_CUBE_MAP_ARRAY 0x900B
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_IMAGE_BUFFER_EXT 0x9051
#define GL_SEPARATE_ATTRIBS_EXT 0x8C8D
#define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS 0x8C29
#define GL_QUERY_BY_REGION_WAIT_NV 0x8E15
#define GL_SAMPLER_1D_SHADOW 0x8B61
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY 0x9105
#define GL_SAMPLER_2D_SHADOW 0x8B62
#define GL_DEPTH32F_STENCIL8 0x8CAD
#define GL_RG16F 0x822F
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL 0x8CD2
#define GL_LINES_ADJACENCY_ARB 0x000A
#define GL_RG16I 0x8239
#define GL_WRITE_ONLY 0x88B9
#define GL_TEXTURE_COORD_NV 0x8C79
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER 0x92C7
#define GL_ALPHA32I_EXT 0x8D84
#define GL_STENCIL_ATTACHMENT 0x8D20
#define GL_SAMPLER_2D 0x8B5E
#define GL_PATH_GEN_COEFF_NV 0x90B1
#define GL_ROUNDED_RECT4_NV 0xEC
#define GL_SAMPLE_COVERAGE 0x80A0
#define GL_ANY_SAMPLES_PASSED_CONSERVATIVE 0x8D6A
#define GL_TEXTURE_3D_BINDING_EXT 0x806A
#define GL_TEXTURE_GATHER_SHADOW 0x82A3
#define GL_DRAW_FRAMEBUFFER 0x8CA9
#define GL_RGB10_A2UI 0x906F
#define GL_MAX_DEBUG_LOGGED_MESSAGES 0x9144
#define GL_MIPMAP 0x8293

typedef void (APIENTRYP PFNGLMULTIDRAWARRAYSINDIRECTBINDLESSCOUNTNVPROC)(GLenum mode, const void *indirect, GLsizei drawCount, GLsizei maxDrawCount, GLsizei stride, GLint vertexBufferCount);
typedef void (APIENTRYP PFNGLCOPYTEXIMAGE1DPROC)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1I64ARBPROC)(GLuint program, GLint location, GLint64 x);
typedef void (APIENTRYP PFNGLSTENCILMASKSEPARATEPROC)(GLenum face, GLuint mask);
typedef void (APIENTRYP PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)(GLenum target, GLenum attachment, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC)(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
typedef void (APIENTRYP PFNGLGETINTEGERINDEXEDVEXTPROC)(GLenum target, GLuint index, GLint *data);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERFVPROC)(GLuint texture, GLenum pname, const GLfloat *param);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4UI64ARBPROC)(GLuint program, GLint location, GLuint64 x, GLuint64 y, GLuint64 z, GLuint64 w);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2SARBPROC)(GLenum target, GLshort s, GLshort t);
typedef void (APIENTRYP PFNGLPROGRAMENVPARAMETER4FARBPROC)(GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRYP PFNGLCOPYTEXTUREIMAGE1DEXTPROC)(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLTEXTURESTORAGE3DEXTPROC)(GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
typedef void (APIENTRYP PFNGLUNIFORM4UIEXTPROC)(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3UI64ARBPROC)(GLuint program, GLint location, GLuint64 x, GLuint64 y, GLuint64 z);
typedef void (APIENTRYP PFNGLVERTEXARRAYBINDVERTEXBUFFEREXTPROC)(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
typedef void (APIENTRYP PFNGLVERTEXARRAYELEMENTBUFFERPROC)(GLuint vaobj, GLuint buffer);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI3UIVEXTPROC)(GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC)(GLuint xfb, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLPATHGLYPHSNVPROC)(GLuint firstPathName, GLenum fontTarget, const void *fontName, GLbitfield fontStyle, GLsizei numGlyphs, GLenum type, const void *charcodes, GLenum handleMissingGlyphs, GLuint pathParameterTemplate, GLfloat emScale);
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLuint baseinstance);
typedef void (APIENTRYP PFNGLGETTEXTUREPARAMETERIVEXTPROC)(GLuint texture, GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP4UIPROC)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
typedef GLboolean (APIENTRYP PFNGLISBUFFERPROC)(GLuint buffer);
typedef void (APIENTRYP PFNGLGETMULTISAMPLEFVPROC)(GLenum pname, GLuint index, GLfloat *val);
typedef void (APIENTRYP PFNGLCOMPRESSEDMULTITEXSUBIMAGE3DEXTPROC)(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *bits);
typedef void (APIENTRYP PFNGLMINSAMPLESHADINGPROC)(GLfloat value);
typedef void (APIENTRYP PFNGLCOVERSTROKEPATHNVPROC)(GLuint path, GLenum coverMode);
typedef void (APIENTRYP PFNGLDEBUGMESSAGECONTROLPROC)(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4IEXTPROC)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2SVARBPROC)(GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4UBVPROC)(GLuint index, const GLubyte *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUBVARBPROC)(GLuint index, const GLubyte *v);
typedef void (APIENTRYP PFNGLMATRIXPUSHEXTPROC)(GLenum mode);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2FEXTPROC)(GLuint program, GLint location, GLfloat v0, GLfloat v1);
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSINDIRECTBINDLESSNVPROC)(GLenum mode, GLenum type, const void *indirect, GLsizei drawCount, GLsizei stride, GLint vertexBufferCount);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBLDVEXTPROC)(GLuint index, GLenum pname, GLdouble *params);
typedef void (APIENTRYP PFNGLGETVERTEXARRAYINDEXEDIVPROC)(GLuint vaobj, GLuint index, GLenum pname, GLint *param);
typedef void (APIENTRYP PFNGLRESUMETRANSFORMFEEDBACKPROC)();
typedef void (APIENTRYP PFNGLGETMULTITEXPARAMETERIVEXTPROC)(GLenum texunit, GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI2IPROC)(GLuint index, GLint x, GLint y);
typedef void (APIENTRYP PFNGLBINDPROGRAMNVPROC)(GLenum target, GLuint id);
typedef void (APIENTRYP PFNGLLOADPROGRAMNVPROC)(GLenum target, GLuint id, GLsizei len, const GLubyte *program);
typedef GLenum (APIENTRYP PFNGLGETGRAPHICSRESETSTATUSPROC)();
typedef void (APIENTRYP PFNGLVERTEXATTRIB1FVPROC)(GLuint index, const GLfloat *v);
typedef GLboolean (APIENTRYP PFNGLISENABLEDPROC)(GLenum cap);
typedef void (APIENTRYP PFNGLSTENCILOPPROC)(GLenum fail, GLenum zfail, GLenum zpass);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLVERTEXARRAYATTRIBIFORMATPROC)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
typedef void (APIENTRYP PFNGLGENFRAMEBUFFERSPROC)(GLsizei n, GLuint *framebuffers);
typedef void (APIENTRYP PFNGLTEXSUBIMAGE3DEXTPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLGETATTACHEDSHADERSPROC)(GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders);
typedef void (APIENTRYP PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint *arrays);
typedef void (APIENTRYP PFNGLVIEWPORTARRAYVPROC)(GLuint first, GLsizei count, const GLfloat *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3DVARBPROC)(GLenum target, const GLdouble *v);
typedef void (APIENTRYP PFNGLUNIFORM3UI64ARBPROC)(GLint location, GLuint64 x, GLuint64 y, GLuint64 z);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2FVNVPROC)(GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLGETNAMEDBUFFERSUBDATAEXTPROC)(GLuint buffer, GLintptr offset, GLsizeiptr size, void *data);
typedef void (APIENTRYP PFNGLMEMORYBARRIERBYREGIONPROC)(GLbitfield barriers);
typedef void (APIENTRYP PFNGLMATRIXROTATEDEXTPROC)(GLenum mode, GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
typedef GLboolean (APIENTRYP PFNGLISPROGRAMNVPROC)(GLuint id);
typedef void (APIENTRYP PFNGLUNIFORM2DVPROC)(GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLMULTITEXPARAMETERIEXTPROC)(GLenum texunit, GLenum target, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLGETPROGRAMSTRINGNVPROC)(GLuint id, GLenum pname, GLubyte *program);
typedef void (APIENTRYP PFNGLGETUNIFORMFVPROC)(GLuint program, GLint location, GLfloat *params);
typedef void (APIENTRYP PFNGLGETUNIFORMUIVPROC)(GLuint program, GLint location, GLuint *params);
typedef void (APIENTRYP PFNGLGETNUNIFORMFVARBPROC)(GLuint program, GLint location, GLsizei bufSize, GLfloat *params);
typedef void (APIENTRYP PFNGLVERTEXARRAYBINDINGDIVISORPROC)(GLuint vaobj, GLuint bindingindex, GLuint divisor);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1DVEXTPROC)(GLuint program, GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4IARBPROC)(GLenum target, GLint s, GLint t, GLint r, GLint q);
typedef void (APIENTRYP PFNGLGETRENDERBUFFERPARAMETERIVPROC)(GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3FEXTPROC)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRYP PFNGLPROGRAMPATHFRAGMENTINPUTGENNVPROC)(GLuint program, GLint location, GLenum genMode, GLint components, const GLfloat *coeffs);
typedef GLsync (APIENTRYP PFNGLFENCESYNCPROC)(GLenum condition, GLbitfield flags);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIIVEXTPROC)(GLuint index, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLBINDRENDERBUFFEREXTPROC)(GLenum target, GLuint renderbuffer);
typedef void (APIENTRYP PFNGLFRAMEBUFFERDRAWBUFFERSEXTPROC)(GLuint framebuffer, GLsizei n, const GLenum *bufs);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS4FVNVPROC)(GLuint index, GLsizei count, const GLfloat *v);
typedef void (APIENTRYP PFNGLPAUSETRANSFORMFEEDBACKNVPROC)();
typedef void (APIENTRYP PFNGLGETQUERYOBJECTI64VEXTPROC)(GLuint id, GLenum pname, GLint64 *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4UIEXTPROC)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
typedef void (APIENTRYP PFNGLVALIDATEPROGRAMPIPELINEPROC)(GLuint pipeline);
typedef void (APIENTRYP PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC)(GLsizei numPaths, GLenum pathNameType, const void *paths, GLuint pathBase, GLint reference, GLuint mask, GLenum transformType, const GLfloat *transformValues);
typedef void (APIENTRYP PFNGLGENQUERIESARBPROC)(GLsizei n, GLuint *ids);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2I64NVPROC)(GLuint program, GLint location, GLint64EXT x, GLint64EXT y);
typedef void (APIENTRYP PFNGLGENSAMPLERSPROC)(GLsizei count, GLuint *samplers);
typedef void (APIENTRYP PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC)(GLenum mode, GLuint id, GLsizei instancecount);
typedef void (APIENTRYP PFNGLSTENCILTHENCOVERSTROKEPATHNVPROC)(GLuint path, GLint reference, GLuint mask, GLenum coverMode);
typedef void (APIENTRYP PFNGLDELETEVERTEXARRAYSAPPLEPROC)(GLsizei n, const GLuint *arrays);
typedef void (APIENTRYP PFNGLGETNAMEDBUFFERPARAMETERUI64VNVPROC)(GLuint buffer, GLenum pname, GLuint64EXT *params);
typedef void (APIENTRYP PFNGLDRAWTRANSFORMFEEDBACKPROC)(GLenum mode, GLuint id);
typedef void (APIENTRYP PFNGLGETTEXPARAMETERIUIVPROC)(GLenum target, GLenum pname, GLuint *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NBVPROC)(GLuint index, const GLbyte *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL4DEXTPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRYP PFNGLUNIFORMUI64VNVPROC)(GLint location, GLsizei count, const GLuint64EXT *value);
typedef GLboolean (APIENTRYP PFNGLISSYNCPROC)(GLsync sync);
typedef void (APIENTRYP PFNGLGETOBJECTPTRLABELPROC)(const void *ptr, GLsizei bufSize, GLsizei *length, GLchar *label);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERIPROC)(GLuint texture, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERFPROC)(GLuint texture, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2X3DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLFRAMEBUFFERDRAWBUFFEREXTPROC)(GLuint framebuffer, GLenum mode);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL3DPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXTUREIMAGE1DEXTPROC)(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void *bits);
typedef void (APIENTRYP PFNGLMATRIXLOADTRANSPOSE3X3FNVPROC)(GLenum matrixMode, const GLfloat *m);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1UIPROC)(GLuint program, GLint location, GLuint v0);
typedef void (APIENTRYP PFNGLCOPYTEXIMAGE2DPROC)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE3DEXTPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEEXTPROC)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLPATHPARAMETERIVNVPROC)(GLuint path, GLenum pname, const GLint *value);
typedef void (APIENTRYP PFNGLUNIFORM4UIVPROC)(GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLSCISSORARRAYVPROC)(GLuint first, GLsizei count, const GLint *v);
typedef void (APIENTRYP PFNGLPOINTPARAMETERFVARBPROC)(GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLINVALIDATESUBFRAMEBUFFERPROC)(GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void * (APIENTRYP PFNGLMAPBUFFERPROC)(GLenum target, GLenum access);
typedef void (APIENTRYP PFNGLVERTEXATTRIBLFORMATPROC)(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
typedef void (APIENTRYP PFNGLDELETESYNCPROC)(GLsync sync);
typedef void (APIENTRYP PFNGLGETPROGRAMPARAMETERFVNVPROC)(GLenum target, GLuint index, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLVERTEXARRAYVERTEXATTRIBLOFFSETEXTPROC)(GLuint vaobj, GLuint buffer, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr offset);
typedef void (APIENTRYP PFNGLUNIFORM3IVPROC)(GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4IVPROC)(GLuint program, GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1DVARBPROC)(GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLTEXTURESUBIMAGE2DEXTPROC)(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
typedef GLboolean (APIENTRYP PFNGLISVERTEXARRAYAPPLEPROC)(GLuint array);
typedef void (APIENTRYP PFNGLUSEPROGRAMPROC)(GLuint program);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4FVARBPROC)(GLenum target, const GLfloat *v);
typedef void (APIENTRYP PFNGLUNIFORM3FVARBPROC)(GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint *buffers);
typedef void (APIENTRYP PFNGLMULTITEXIMAGE1DEXTPROC)(GLenum texunit, GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLDELETEOBJECTARBPROC)(GLhandleARB obj);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3DARBPROC)(GLenum target, GLdouble s, GLdouble t, GLdouble r);
typedef void (APIENTRYP PFNGLSCISSORPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERTEXTUREEXTPROC)(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERIEXTPROC)(GLuint texture, GLenum target, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2DVARBPROC)(GLenum target, const GLdouble *v);
typedef void (APIENTRYP PFNGLDELETESHADERPROC)(GLuint shader);
typedef void (APIENTRYP PFNGLCOPYTEXTURESUBIMAGE1DPROC)(GLuint texture, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERIUIVPROC)(GLuint texture, GLenum pname, const GLuint *params);
typedef void (APIENTRYP PFNGLTRANSFORMFEEDBACKATTRIBSNVPROC)(GLsizei count, const GLint *attribs, GLenum bufferMode);
typedef void (APIENTRYP PFNGLUNIFORM2UIPROC)(GLint location, GLuint v0, GLuint v1);
typedef void (APIENTRYP PFNGLVERTEXATTRIBIPOINTEREXTPROC)(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI3IPROC)(GLuint index, GLint x, GLint y, GLint z);
typedef void (APIENTRYP PFNGLVERTEXATTRIBFORMATPROC)(GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
typedef void (APIENTRYP PFNGLGETPROGRAMIVARBPROC)(GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4USVPROC)(GLuint index, const GLushort *v);
typedef void (APIENTRYP PFNGLTEXPARAMETERFPROC)(GLenum target, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLVERTEXATTRIBBINDINGPROC)(GLuint attribindex, GLuint bindingindex);
typedef void (APIENTRYP PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLGETSHADERSOURCEPROC)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source);
typedef void (APIENTRYP PFNGLTEXTUREBUFFEREXTPROC)(GLuint texture, GLenum target, GLenum internalformat, GLuint buffer);
typedef void (APIENTRYP PFNGLGETMULTITEXIMAGEEXTPROC)(GLenum texunit, GLenum target, GLint level, GLenum format, GLenum type, void *pixels);
typedef void (APIENTRYP PFNGLGETPOINTERINDEXEDVEXTPROC)(GLenum target, GLuint index, void **data);
typedef void (APIENTRYP PFNGLBUFFERPARAMETERIAPPLEPROC)(GLenum target, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3FVPROC)(GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP1UIVPROC)(GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
typedef void (APIENTRYP PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void (APIENTRYP PFNGLGETOBJECTLABELPROC)(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei *length, GLchar *label);
typedef void (APIENTRYP PFNGLVERTEXPOINTEREXTPROC)(GLint size, GLenum type, GLsizei stride, GLsizei count, const void *pointer);
typedef void (APIENTRYP PFNGLDEBUGMESSAGECALLBACKARBPROC)(GLDEBUGPROCARB callback, const void *userParam);
typedef const GLubyte * (APIENTRYP PFNGLGETSTRINGPROC)(GLenum name);
typedef void (APIENTRYP PFNGLGETPATHPARAMETERFVNVPROC)(GLuint path, GLenum pname, GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3IPROC)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2);
typedef void (APIENTRYP PFNGLENDQUERYPROC)(GLenum target);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4IVEXTPROC)(GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLFRAMEBUFFERPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI2UIPROC)(GLuint index, GLuint x, GLuint y);
typedef void (APIENTRYP PFNGLUNIFORM2IARBPROC)(GLint location, GLint v0, GLint v1);
typedef void (APIENTRYP PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint *textures);
typedef void (APIENTRYP PFNGLCREATEVERTEXARRAYSPROC)(GLsizei n, GLuint *arrays);
typedef void (APIENTRYP PFNGLBEGINCONDITIONALRENDERPROC)(GLuint id, GLenum mode);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3FARBPROC)(GLenum target, GLfloat s, GLfloat t, GLfloat r);
typedef void (APIENTRYP PFNGLGETQUERYOBJECTUIVARBPROC)(GLuint id, GLenum pname, GLuint *params);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIPROC)(GLuint sampler, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERFPROC)(GLuint sampler, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLUNIFORM1FPROC)(GLint location, GLfloat v0);
typedef void (APIENTRYP PFNGLUNIFORM1DPROC)(GLint location, GLdouble x);
typedef void (APIENTRYP PFNGLCLEARCOLORIIEXTPROC)(GLint red, GLint green, GLint blue, GLint alpha);
typedef void (APIENTRYP PFNGLGETCOMPRESSEDTEXIMAGEPROC)(GLenum target, GLint level, void *img);
typedef void (APIENTRYP PFNGLMINSAMPLESHADINGARBPROC)(GLfloat value);
typedef void (APIENTRYP PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTUREEXTPROC)(GLenum target, GLenum attachment, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLCULLFACEPROC)(GLenum mode);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4IPROC)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
typedef void (APIENTRYP PFNGLUNIFORMHANDLEUI64ARBPROC)(GLint location, GLuint64 value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4FPROC)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (APIENTRYP PFNGLVIEWPORTINDEXEDFPROC)(GLuint index, GLfloat x, GLfloat y, GLfloat w, GLfloat h);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4DPROC)(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3);
typedef GLboolean (APIENTRYP PFNGLPOINTALONGPATHNVPROC)(GLuint path, GLsizei startSegment, GLsizei numSegments, GLfloat distance, GLfloat *x, GLfloat *y, GLfloat *tangentX, GLfloat *tangentY);
typedef void (APIENTRYP PFNGLENDQUERYARBPROC)(GLenum target);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4UIVARBPROC)(GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLRESUMETRANSFORMFEEDBACKNVPROC)();
typedef void (APIENTRYP PFNGLQUERYCOUNTERPROC)(GLuint id, GLenum target);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3FARBPROC)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRYP PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC)(GLenum mode, GLuint id, GLuint stream, GLsizei instancecount);
typedef void (APIENTRYP PFNGLGETBUFFERPARAMETERIVPROC)(GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERTEXTURE3DEXTPROC)(GLuint framebuffer, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
typedef void (APIENTRYP PFNGLTEXPARAMETERIUIVPROC)(GLenum target, GLenum pname, const GLuint *params);
typedef void (APIENTRYP PFNGLGETCOMPRESSEDTEXIMAGEARBPROC)(GLenum target, GLint level, void *img);
typedef void (APIENTRYP PFNGLDELETEPATHSNVPROC)(GLuint path, GLsizei range);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4UBVEXTPROC)(GLuint index, const GLubyte *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1DNVPROC)(GLuint index, GLdouble x);
typedef void (APIENTRYP PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIEXTPROC)(GLuint program, GLenum target, GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1IEXTPROC)(GLuint program, GLint location, GLint v0);
typedef void (APIENTRYP PFNGLDELETEQUERIESARBPROC)(GLsizei n, const GLuint *ids);
typedef void (APIENTRYP PFNGLTEXTURESTORAGE3DPROC)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
typedef GLboolean (APIENTRYP PFNGLISTRANSFORMFEEDBACKPROC)(GLuint id);
typedef GLboolean (APIENTRYP PFNGLISPROGRAMPIPELINEPROC)(GLuint pipeline);
typedef void (APIENTRYP PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC)(GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei *length, GLchar *name);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4SNVPROC)(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
typedef void (APIENTRYP PFNGLPROGRAMLOCALPARAMETER4DVARBPROC)(GLenum target, GLuint index, const GLdouble *params);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERFVEXTPROC)(GLuint texture, GLenum target, GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLGETNUNIFORMFVPROC)(GLuint program, GLint location, GLsizei bufSize, GLfloat *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL2DVPROC)(GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLTEXRENDERBUFFERNVPROC)(GLenum target, GLuint renderbuffer);
typedef void (APIENTRYP PFNGLBINDVERTEXARRAYPROC)(GLuint array);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL2DPROC)(GLuint index, GLdouble x, GLdouble y);
typedef void (APIENTRYP PFNGLSTENCILFUNCPROC)(GLenum func, GLint ref, GLuint mask);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBDVNVPROC)(GLuint index, GLenum pname, GLdouble *params);
typedef void (APIENTRYP PFNGLGETQUERYBUFFEROBJECTIVPROC)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
typedef void (APIENTRYP PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)(GLenum target, GLenum attachment, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETPROGRAMPIPELINEIVPROC)(GLuint pipeline, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL4DPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRYP PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXTPROC)(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *bits);
typedef void (APIENTRYP PFNGLCLEARBUFFERSUBDATAPROC)(GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4IPROC)(GLuint index, GLint x, GLint y, GLint z, GLint w);
typedef void (APIENTRYP PFNGLGETINTEGERUI64I_VNVPROC)(GLenum value, GLuint index, GLuint64EXT *result);
typedef void (APIENTRYP PFNGLBINDATTRIBLOCATIONPROC)(GLuint program, GLuint index, const GLchar *name);
typedef void (APIENTRYP PFNGLBLENDEQUATIONSEPARATEPROC)(GLenum modeRGB, GLenum modeAlpha);
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDARBPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei primcount);
typedef void (APIENTRYP PFNGLBEGINQUERYARBPROC)(GLenum target, GLuint id);
typedef GLuint (APIENTRYP PFNGLGETSUBROUTINEINDEXPROC)(GLuint program, GLenum shadertype, const GLchar *name);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2SVPROC)(GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLPATHPARAMETERFNVPROC)(GLuint path, GLenum pname, GLfloat value);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3FVARBPROC)(GLenum target, const GLfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1FVARBPROC)(GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLNAMEDPROGRAMLOCALPARAMETER4FEXTPROC)(GLuint program, GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL1DEXTPROC)(GLuint index, GLdouble x);
typedef void (APIENTRYP PFNGLGETNUNIFORMDVARBPROC)(GLuint program, GLint location, GLsizei bufSize, GLdouble *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1UIVEXTPROC)(GLuint program, GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLVERTEXARRAYVERTEXATTRIBLFORMATEXTPROC)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
typedef void (APIENTRYP PFNGLBINDPROGRAMPIPELINEPROC)(GLuint pipeline);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4FVPROC)(GLuint program, GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLGETBOOLEANVPROC)(GLenum pname, GLboolean *data);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1DPROC)(GLuint index, GLdouble x);
typedef void (APIENTRYP PFNGLMULTIDRAWARRAYSINDIRECTAMDPROC)(GLenum mode, const void *indirect, GLsizei primcount, GLsizei stride);
typedef void (APIENTRYP PFNGLGENTEXTURESPROC)(GLsizei n, GLuint *textures);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4UIVEXTPROC)(GLuint program, GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLGETTEXTURELEVELPARAMETERIVPROC)(GLuint texture, GLint level, GLenum pname, GLint *params);
typedef const GLubyte * (APIENTRYP PFNGLGETSTRINGIPROC)(GLenum name, GLuint index);
typedef void (APIENTRYP PFNGLPOINTPARAMETERFVPROC)(GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2FVARBPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORM2FVPROC)(GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLFOGCOORDFORMATNVPROC)(GLenum type, GLsizei stride);
typedef void (APIENTRYP PFNGLGETMULTITEXGENFVEXTPROC)(GLenum texunit, GLenum coord, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLMATRIXFRUSTUMEXTPROC)(GLenum mode, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
typedef void (APIENTRYP PFNGLDISPATCHCOMPUTEINDIRECTPROC)(GLintptr indirect);
typedef void (APIENTRYP PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC)(GLuint framebuffer, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLPROGRAMENVPARAMETER4DARBPROC)(GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRYP PFNGLBINDBUFFERRANGEPROC)(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLGETUNIFORMI64VNVPROC)(GLuint program, GLint location, GLint64EXT *params);
typedef void (APIENTRYP PFNGLGETUNIFORMDVPROC)(GLuint program, GLint location, GLdouble *params);
typedef void (APIENTRYP PFNGLGETMULTITEXLEVELPARAMETERFVEXTPROC)(GLenum texunit, GLenum target, GLint level, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3FVEXTPROC)(GLuint program, GLint location, GLsizei count, const GLfloat *value);
typedef GLboolean (APIENTRYP PFNGLISVERTEXARRAYPROC)(GLuint array);
typedef void (APIENTRYP PFNGLGETVERTEXARRAYPOINTERVEXTPROC)(GLuint vaobj, GLenum pname, void **param);
typedef void (APIENTRYP PFNGLPOINTPARAMETERFSGISPROC)(GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLUNIFORM1FARBPROC)(GLint location, GLfloat v0);
typedef GLenum (APIENTRYP PFNGLCLIENTWAITSYNCPROC)(GLsync sync, GLbitfield flags, GLuint64 timeout);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS1FVNVPROC)(GLuint index, GLsizei count, const GLfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUSVARBPROC)(GLuint index, const GLushort *v);
typedef void (APIENTRYP PFNGLBLENDFUNCIARBPROC)(GLuint buf, GLenum src, GLenum dst);
typedef void (APIENTRYP PFNGLINVALIDATETEXIMAGEPROC)(GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLCOPYIMAGESUBDATAPROC)(GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth);
typedef void (APIENTRYP PFNGLGETUNIFORMSUBROUTINEUIVPROC)(GLenum shadertype, GLint location, GLuint *params);
typedef void (APIENTRYP PFNGLBINDVERTEXBUFFERPROC)(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1IARBPROC)(GLenum target, GLint s);
typedef void (APIENTRYP PFNGLDEBUGMESSAGEINSERTPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf);
typedef GLboolean (APIENTRYP PFNGLISSAMPLERPROC)(GLuint sampler);
typedef void (APIENTRYP PFNGLMULTITEXGENIVEXTPROC)(GLenum texunit, GLenum coord, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERTEXTURE2DEXTPROC)(GLuint framebuffer, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE1DPROC)(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
typedef GLenum (APIENTRYP PFNGLCHECKFRAMEBUFFERSTATUSPROC)(GLenum target);
typedef void (APIENTRYP PFNGLBINDIMAGETEXTUREPROC)(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
typedef GLboolean (APIENTRYP PFNGLISPATHNVPROC)(GLuint path);
typedef void (APIENTRYP PFNGLDRAWRANGEELEMENTSPROC)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices);
typedef void (APIENTRYP PFNGLMULTITEXCOORDPOINTEREXTPROC)(GLenum texunit, GLint size, GLenum type, GLsizei stride, const void *pointer);
typedef void (APIENTRYP PFNGLBINDBUFFERBASEPROC)(GLenum target, GLuint index, GLuint buffer);
typedef void (APIENTRYP PFNGLMULTIDRAWARRAYSPROC)(GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount);
typedef void (APIENTRYP PFNGLNAMEDRENDERBUFFERSTORAGEEXTPROC)(GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL1DPROC)(GLuint index, GLdouble x);
typedef void (APIENTRYP PFNGLMULTITEXSUBIMAGE1DEXTPROC)(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI3UIVPROC)(GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLBUFFERDATAARBPROC)(GLenum target, GLsizeiptrARB size, const void *data, GLenum usage);
typedef void (APIENTRYP PFNGLVERTEXATTRIBIFORMATPROC)(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
typedef void (APIENTRYP PFNGLCREATEFRAMEBUFFERSPROC)(GLsizei n, GLuint *framebuffers);
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMNAMEPROC)(GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformName);
typedef void (APIENTRYP PFNGLUNIFORM2UIVEXTPROC)(GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4UIPROC)(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
typedef void (APIENTRYP PFNGLBEGINQUERYPROC)(GLenum target, GLuint id);
typedef void (APIENTRYP PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2X4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLGETMULTITEXPARAMETERFVEXTPROC)(GLenum texunit, GLenum target, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLDEPTHRANGEINDEXEDPROC)(GLuint index, GLdouble n, GLdouble f);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIUIVEXTPROC)(GLuint index, GLenum pname, GLuint *params);
typedef void (APIENTRYP PFNGLMATRIXSCALEFEXTPROC)(GLenum mode, GLfloat x, GLfloat y, GLfloat z);
typedef GLenum (APIENTRYP PFNGLGETERRORPROC)();
typedef void (APIENTRYP PFNGLGETUNIFORMIVARBPROC)(GLhandleARB programObj, GLint location, GLint *params);
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMARBPROC)(GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI3IVEXTPROC)(GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLGETTEXLEVELPARAMETERFVPROC)(GLenum target, GLint level, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI1IVEXTPROC)(GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3SVNVPROC)(GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC)(GLuint buffer, GLintptr offset, GLsizeiptr length);
typedef void (APIENTRYP PFNGLGETNUNIFORMUI64VARBPROC)(GLuint program, GLint location, GLsizei bufSize, GLuint64 *params);
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMSIVPROC)(GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI1UIEXTPROC)(GLuint index, GLuint x);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3I64VARBPROC)(GLuint program, GLint location, GLsizei count, const GLint64 *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4UIPROC)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE2DEXTPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLSTENCILMASKPROC)(GLuint mask);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1DVPROC)(GLuint program, GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLNAMEDBUFFERDATAPROC)(GLuint buffer, GLsizeiptr size, const void *data, GLenum usage);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3FVARBPROC)(GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLCLEARNAMEDBUFFERSUBDATAPROC)(GLuint buffer, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data);
typedef void * (APIENTRYP PFNGLMAPNAMEDBUFFERRANGEPROC)(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);
typedef void (APIENTRYP PFNGLUNIFORM3IARBPROC)(GLint location, GLint v0, GLint v1, GLint v2);
typedef GLhandleARB (APIENTRYP PFNGLCREATEPROGRAMOBJECTARBPROC)();
typedef void (APIENTRYP PFNGLMULTITEXCOORD1DVARBPROC)(GLenum target, const GLdouble *v);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3DVEXTPROC)(GLuint program, GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLMULTITEXIMAGE2DEXTPROC)(GLenum texunit, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1I64NVPROC)(GLuint program, GLint location, GLint64EXT x);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1FVARBPROC)(GLenum target, const GLfloat *v);
typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEINGRPROC)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2FARBPROC)(GLuint index, GLfloat x, GLfloat y);
typedef void (APIENTRYP PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei bufSize, void *pixels);
typedef GLenum (APIENTRYP PFNGLPATHGLYPHINDEXARRAYNVPROC)(GLuint firstPathName, GLenum fontTarget, const void *fontName, GLbitfield fontStyle, GLuint firstGlyphIndex, GLsizei numGlyphs, GLuint pathParameterTemplate, GLfloat emScale);
typedef void (APIENTRYP PFNGLINDEXPOINTEREXTPROC)(GLenum type, GLsizei stride, GLsizei count, const void *pointer);
typedef void (APIENTRYP PFNGLTEXTUREIMAGE2DEXTPROC)(GLuint texture, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLACTIVESHADERPROGRAMPROC)(GLuint pipeline, GLuint program);
typedef void (APIENTRYP PFNGLPATCHPARAMETERFVPROC)(GLenum pname, const GLfloat *values);
typedef GLboolean (APIENTRYP PFNGLISFRAMEBUFFEREXTPROC)(GLuint framebuffer);
typedef void (APIENTRYP PFNGLTEXTURESTORAGE2DPROC)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1UI64ARBPROC)(GLuint program, GLint location, GLuint64 x);
typedef void (APIENTRYP PFNGLTEXBUFFERRANGEPROC)(GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLTEXTURESUBIMAGE3DEXTPROC)(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLCLIPCONTROLPROC)(GLenum origin, GLenum depth);
typedef void (APIENTRYP PFNGLUNIFORM2I64NVPROC)(GLint location, GLint64EXT x, GLint64EXT y);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI3UIPROC)(GLuint index, GLuint x, GLuint y, GLuint z);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NIVARBPROC)(GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLBLENDEQUATIONSEPARATEINDEXEDAMDPROC)(GLuint buf, GLenum modeRGB, GLenum modeAlpha);
typedef void (APIENTRYP PFNGLVERTEXARRAYVERTEXBUFFERSPROC)(GLuint vaobj, GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizei *strides);
typedef void (APIENTRYP PFNGLUNIFORM1UI64NVPROC)(GLint location, GLuint64EXT x);
typedef void (APIENTRYP PFNGLPROGRAMPARAMETERIPROC)(GLuint program, GLenum pname, GLint value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2I64VNVPROC)(GLuint program, GLint location, GLsizei count, const GLint64EXT *value);
typedef GLboolean (APIENTRYP PFNGLISSTATENVPROC)(GLuint state);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL2DEXTPROC)(GLuint index, GLdouble x, GLdouble y);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X2DVEXTPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLENABLEVERTEXARRAYATTRIBEXTPROC)(GLuint vaobj, GLuint index);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2UIVEXTPROC)(GLuint program, GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLGETDOUBLEINDEXEDVEXTPROC)(GLenum target, GLuint index, GLdouble *data);
typedef GLint (APIENTRYP PFNGLGETVARYINGLOCATIONNVPROC)(GLuint program, const GLchar *name);
typedef void (APIENTRYP PFNGLBINDFRAMEBUFFEREXTPROC)(GLenum target, GLuint framebuffer);
typedef GLuint (APIENTRYP PFNGLCREATESHADERPROC)(GLenum type);
typedef GLuint (APIENTRYP PFNGLGENPATHSNVPROC)(GLsizei range);
typedef void (APIENTRYP PFNGLGENRENDERBUFFERSPROC)(GLsizei n, GLuint *renderbuffers);
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4UBVARBPROC)(GLuint index, const GLubyte *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS1SVNVPROC)(GLuint index, GLsizei count, const GLshort *v);
typedef void (APIENTRYP PFNGLBINDBUFFERBASEEXTPROC)(GLenum target, GLuint index, GLuint buffer);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE1DPROC)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEPROC)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
typedef void (APIENTRYP PFNGLPOINTPARAMETERFARBPROC)(GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLPOINTSIZEPROC)(GLfloat size);
typedef void (APIENTRYP PFNGLBINDTEXTUREUNITPROC)(GLuint unit, GLuint texture);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4I64VARBPROC)(GLuint program, GLint location, GLsizei count, const GLint64 *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUIVPROC)(GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLGETUNIFORMFVARBPROC)(GLhandleARB programObj, GLint location, GLfloat *params);
typedef void (APIENTRYP PFNGLPROGRAMLOCALPARAMETER4FARBPROC)(GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRYP PFNGLUNIFORM3IPROC)(GLint location, GLint v0, GLint v1, GLint v2);
typedef void (APIENTRYP PFNGLBLENDEQUATIONSEPARATEIPROC)(GLuint buf, GLenum modeRGB, GLenum modeAlpha);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERIVEXTPROC)(GLuint texture, GLenum target, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLUNIFORM3DPROC)(GLint location, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRYP PFNGLUNIFORM3FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS4DVNVPROC)(GLuint index, GLsizei count, const GLdouble *v);
typedef void (APIENTRYP PFNGLUNIFORM3UI64VARBPROC)(GLint location, GLsizei count, const GLuint64 *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1UI64VARBPROC)(GLuint program, GLint location, GLsizei count, const GLuint64 *value);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERFEXTPROC)(GLuint texture, GLenum target, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4SARBPROC)(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
typedef GLint (APIENTRYP PFNGLGETFRAGDATAINDEXPROC)(GLuint program, const GLchar *name);
typedef void (APIENTRYP PFNGLORTHOFOESPROC)(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f);
typedef void (APIENTRYP PFNGLWEIGHTPATHSNVPROC)(GLuint resultPath, GLsizei numPaths, const GLuint *paths, const GLfloat *weights);
typedef void (APIENTRYP PFNGLCOPYTEXIMAGE1DEXTPROC)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXTPROC)(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *bits);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL3DVEXTPROC)(GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei n, const GLuint *framebuffers);
typedef void (APIENTRYP PFNGLDRAWARRAYSPROC)(GLenum mode, GLint first, GLsizei count);
typedef void (APIENTRYP PFNGLGETNTEXIMAGEARBPROC)(GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, void *img);
typedef void (APIENTRYP PFNGLCLEARPROC)(GLbitfield mask);
typedef void (APIENTRYP PFNGLUNIFORM1I64ARBPROC)(GLint location, GLint64 x);
typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEIARBPROC)(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
typedef void (APIENTRYP PFNGLCREATEQUERIESPROC)(GLenum target, GLsizei n, GLuint *ids);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3IEXTPROC)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUBPROC)(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
typedef void (APIENTRYP PFNGLDEPTHRANGEARRAYVPROC)(GLuint first, GLsizei count, const GLdouble *v);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIIVPROC)(GLuint sampler, GLenum pname, const GLint *param);
typedef void (APIENTRYP PFNGLTEXIMAGE1DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels);
typedef GLboolean (APIENTRYP PFNGLISRENDERBUFFEREXTPROC)(GLuint renderbuffer);
typedef void (APIENTRYP PFNGLDRAWCOMMANDSNVPROC)(GLenum primitiveMode, GLuint buffer, const GLintptr *indirects, const GLsizei *sizes, GLuint count);
typedef void (APIENTRYP PFNGLUNIFORM3IVARBPROC)(GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLMATRIXMULTTRANSPOSEDEXTPROC)(GLenum mode, const GLdouble *m);
typedef void (APIENTRYP PFNGLREQUESTRESIDENTPROGRAMSNVPROC)(GLsizei n, const GLuint *programs);
typedef void (APIENTRYP PFNGLMATRIXMULT3X3FNVPROC)(GLenum matrixMode, const GLfloat *m);
typedef void (APIENTRYP PFNGLPATHSUBCOMMANDSNVPROC)(GLuint path, GLsizei commandStart, GLsizei commandsToDelete, GLsizei numCommands, const GLubyte *commands, GLsizei numCoords, GLenum coordType, const void *coords);
typedef GLboolean (APIENTRYP PFNGLISRENDERBUFFERPROC)(GLuint renderbuffer);
typedef void (APIENTRYP PFNGLDRAWBUFFERSARBPROC)(GLsizei n, const GLenum *bufs);
typedef void (APIENTRYP PFNGLFRUSTUMFOESPROC)(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f);
typedef void (APIENTRYP PFNGLCREATESAMPLERSPROC)(GLsizei n, GLuint *samplers);
typedef void (APIENTRYP PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (APIENTRYP PFNGLTEXCOORDFORMATNVPROC)(GLint size, GLenum type, GLsizei stride);
typedef void (APIENTRYP PFNGLBLITNAMEDFRAMEBUFFERPROC)(GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
typedef void (APIENTRYP PFNGLMATRIXLOADTRANSPOSEFEXTPROC)(GLenum mode, const GLfloat *m);
typedef void (APIENTRYP PFNGLMULTITEXGENFEXTPROC)(GLenum texunit, GLenum coord, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLSHADERSTORAGEBLOCKBINDINGPROC)(GLuint program, GLuint storageBlockIndex, GLuint storageBlockBinding);
typedef void (APIENTRYP PFNGLSTENCILOPSEPARATEPROC)(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4UBVPROC)(GLuint index, const GLubyte *v);
typedef void (APIENTRYP PFNGLVERTEXARRAYVERTEXATTRIBFORMATEXTPROC)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
typedef void (APIENTRYP PFNGLTEXIMAGE3DEXTPROC)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLTEXTURESTORAGE1DPROC)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width);
typedef void (APIENTRYP PFNGLGETPROGRAMINTERFACEIVPROC)(GLuint program, GLenum programInterface, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLTEXCOORDPOINTEREXTPROC)(GLint size, GLenum type, GLsizei stride, GLsizei count, const void *pointer);
typedef void (APIENTRYP PFNGLMULTITEXGENIEXTPROC)(GLenum texunit, GLenum coord, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2FNVPROC)(GLuint index, GLfloat x, GLfloat y);
typedef void (APIENTRYP PFNGLNAMEDPROGRAMSTRINGEXTPROC)(GLuint program, GLenum target, GLenum format, GLsizei len, const void *string);
typedef void (APIENTRYP PFNGLGETNUNIFORMDVPROC)(GLuint program, GLint location, GLsizei bufSize, GLdouble *params);
typedef void (APIENTRYP PFNGLGETTEXTURESUBIMAGEPROC)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLsizei bufSize, void *pixels);
typedef void (APIENTRYP PFNGLVIEWPORTINDEXEDFVPROC)(GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4SVARBPROC)(GLenum target, const GLshort *v);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBPOINTERVARBPROC)(GLuint index, GLenum pname, void **pointer);
typedef void (APIENTRYP PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEEXTPROC)(GLuint buffer, GLintptr offset, GLsizeiptr length);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIIVPROC)(GLuint index, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLUNIFORMUI64NVPROC)(GLint location, GLuint64EXT value);
typedef void (APIENTRYP PFNGLPROGRAMPARAMETERS4FVNVPROC)(GLenum target, GLuint index, GLsizei count, const GLfloat *v);
typedef void (APIENTRYP PFNGLPATCHPARAMETERIPROC)(GLenum pname, GLint value);
typedef void (APIENTRYP PFNGLVERTEXARRAYVERTEXBINDINGDIVISOREXTPROC)(GLuint vaobj, GLuint bindingindex, GLuint divisor);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3SVARBPROC)(GLenum target, const GLshort *v);
typedef void (APIENTRYP PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC)(GLenum target, GLuint index, GLfloat *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4FNVPROC)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRYP PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC)(GLuint framebuffer, GLenum attachment, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGENVERTEXARRAYSAPPLEPROC)(GLsizei n, GLuint *arrays);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTUREFACEARBPROC)(GLenum target, GLenum attachment, GLuint texture, GLint level, GLenum face);
typedef void (APIENTRYP PFNGLTEXTURESTORAGE2DEXTPROC)(GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLVERTEXARRAYFOGCOORDOFFSETEXTPROC)(GLuint vaobj, GLuint buffer, GLenum type, GLsizei stride, GLintptr offset);
typedef void (APIENTRYP PFNGLBLENDEQUATIONSEPARATEIARBPROC)(GLuint buf, GLenum modeRGB, GLenum modeAlpha);
typedef void (APIENTRYP PFNGLCLIENTATTRIBDEFAULTEXTPROC)(GLbitfield mask);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2I64ARBPROC)(GLuint program, GLint location, GLint64 x, GLint64 y);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBFVARBPROC)(GLuint index, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2IEXTPROC)(GLuint program, GLint location, GLint v0, GLint v1);
typedef GLboolean (APIENTRYP PFNGLUNMAPNAMEDBUFFEREXTPROC)(GLuint buffer);
typedef void (APIENTRYP PFNGLUNIFORM4FVARBPROC)(GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLGETFLOATI_VEXTPROC)(GLenum pname, GLuint index, GLfloat *params);
typedef void (APIENTRYP PFNGLBINDATTRIBLOCATIONARBPROC)(GLhandleARB programObj, GLuint index, const GLcharARB *name);
typedef void (APIENTRYP PFNGLBUFFERADDRESSRANGENVPROC)(GLenum pname, GLuint index, GLuint64EXT address, GLsizeiptr length);
typedef void (APIENTRYP PFNGLMULTITEXENVIVEXTPROC)(GLenum texunit, GLenum target, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1UI64VNVPROC)(GLuint program, GLint location, GLsizei count, const GLuint64EXT *value);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBFVNVPROC)(GLuint index, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLUNIFORM2IPROC)(GLint location, GLint v0, GLint v1);
typedef void (APIENTRYP PFNGLUNIFORM2FPROC)(GLint location, GLfloat v0, GLfloat v1);
typedef void (APIENTRYP PFNGLUNIFORM2DPROC)(GLint location, GLdouble x, GLdouble y);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTUREFACEEXTPROC)(GLenum target, GLenum attachment, GLuint texture, GLint level, GLenum face);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURELAYERPROC)(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
typedef void (APIENTRYP PFNGLGETVERTEXARRAYPOINTERI_VEXTPROC)(GLuint vaobj, GLuint index, GLenum pname, void **param);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2FVPROC)(GLuint program, GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLINTERPOLATEPATHSNVPROC)(GLuint resultPath, GLuint pathA, GLuint pathB, GLfloat weight);
typedef void (APIENTRYP PFNGLCOPYMULTITEXSUBIMAGE3DEXTPROC)(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef GLenum (APIENTRYP PFNGLPATHMEMORYGLYPHINDEXARRAYNVPROC)(GLuint firstPathName, GLenum fontTarget, GLsizeiptr fontSize, const void *fontData, GLsizei faceIndex, GLuint firstGlyphIndex, GLsizei numGlyphs, GLuint pathParameterTemplate, GLfloat emScale);
typedef void (APIENTRYP PFNGLDRAWARRAYSEXTPROC)(GLenum mode, GLint first, GLsizei count);
typedef void (APIENTRYP PFNGLBLENDEQUATIONEXTPROC)(GLenum mode);
typedef void (APIENTRYP PFNGLCOPYTEXTURESUBIMAGE2DEXTPROC)(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2DVNVPROC)(GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLGETINTEGER64I_VPROC)(GLenum target, GLuint index, GLint64 *data);
typedef void (APIENTRYP PFNGLBLENDFUNCINDEXEDAMDPROC)(GLuint buf, GLenum src, GLenum dst);
typedef void (APIENTRYP PFNGLBLITFRAMEBUFFERPROC)(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
typedef GLboolean (APIENTRYP PFNGLISENABLEDIPROC)(GLenum target, GLuint index);
typedef void (APIENTRYP PFNGLMATRIXTRANSLATEDEXTPROC)(GLenum mode, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRYP PFNGLMAKETEXTUREHANDLERESIDENTARBPROC)(GLuint64 handle);
typedef void (APIENTRYP PFNGLUNIFORM3I64VNVPROC)(GLint location, GLsizei count, const GLint64EXT *value);
typedef void (APIENTRYP PFNGLPATHCOMMANDSNVPROC)(GLuint path, GLsizei numCommands, const GLubyte *commands, GLsizei numCoords, GLenum coordType, const void *coords);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLBINDVERTEXARRAYAPPLEPROC)(GLuint array);
typedef void (APIENTRYP PFNGLOBJECTPTRLABELPROC)(const void *ptr, GLsizei length, const GLchar *label);
typedef GLuint (APIENTRYP PFNGLGETDEBUGMESSAGELOGPROC)(GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog);
typedef void (APIENTRYP PFNGLTRACKMATRIXNVPROC)(GLenum target, GLuint address, GLenum matrix, GLenum transform);
typedef void (APIENTRYP PFNGLBINDFRAGDATALOCATIONPROC)(GLuint program, GLuint color, const GLchar *name);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS4UBVNVPROC)(GLuint index, GLsizei count, const GLubyte *v);
typedef void (APIENTRYP PFNGLBINDBUFFERBASENVPROC)(GLenum target, GLuint index, GLuint buffer);
typedef void (APIENTRYP PFNGLUNIFORM2I64VNVPROC)(GLint location, GLsizei count, const GLint64EXT *value);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURELAYERARBPROC)(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSINDIRECTPROC)(GLenum mode, GLenum type, const void *indirect, GLsizei drawcount, GLsizei stride);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1DEXTPROC)(GLuint program, GLint location, GLdouble x);
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE3DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef GLhandleARB (APIENTRYP PFNGLCREATESHADEROBJECTARBPROC)(GLenum shaderType);
typedef GLushort (APIENTRYP PFNGLGETSTAGEINDEXNVPROC)(GLenum shadertype);
typedef void (APIENTRYP PFNGLGETNAMEDBUFFERPOINTERVEXTPROC)(GLuint buffer, GLenum pname, void **params);
typedef GLuint (APIENTRYP PFNGLGETCOMMANDHEADERNVPROC)(GLenum tokenID, GLuint size);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1SVARBPROC)(GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLMATRIXMULTTRANSPOSEFEXTPROC)(GLenum mode, const GLfloat *m);
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMBLOCKIVPROC)(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2DPROC)(GLuint program, GLint location, GLdouble v0, GLdouble v1);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2FPROC)(GLuint program, GLint location, GLfloat v0, GLfloat v1);
typedef GLboolean (APIENTRYP PFNGLISFRAMEBUFFERPROC)(GLuint framebuffer);
typedef void (APIENTRYP PFNGLCLIENTACTIVETEXTUREARBPROC)(GLenum texture);
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);
typedef void (APIENTRYP PFNGLBINDFRAMEBUFFERPROC)(GLenum target, GLuint framebuffer);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS2DVNVPROC)(GLuint index, GLsizei count, const GLdouble *v);
typedef void (APIENTRYP PFNGLGETPROGRAMRESOURCEFVNVPROC)(GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLfloat *params);
typedef void (APIENTRYP PFNGLENDTRANSFORMFEEDBACKNVPROC)();
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4IVEXTPROC)(GLuint program, GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3UI64VNVPROC)(GLuint program, GLint location, GLsizei count, const GLuint64EXT *value);
typedef void (APIENTRYP PFNGLCOPYMULTITEXIMAGE1DEXTPROC)(GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
typedef void (APIENTRYP PFNGLCOLORMASKINDEXEDEXTPROC)(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2X4DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef GLint (APIENTRYP PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC)(GLuint program, GLenum programInterface, const GLchar *name);
typedef void (APIENTRYP PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLMATRIXLOADFEXTPROC)(GLenum mode, const GLfloat *m);
typedef void (APIENTRYP PFNGLTEXSTORAGE2DMULTISAMPLEPROC)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL1DVEXTPROC)(GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC)(GLuint program, GLenum shadertype, GLuint index, GLenum pname, GLint *values);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLTEXBUFFERPROC)(GLenum target, GLenum internalformat, GLuint buffer);
typedef void (APIENTRYP PFNGLPIXELSTOREIPROC)(GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLVALIDATEPROGRAMPROC)(GLuint program);
typedef void (APIENTRYP PFNGLPIXELSTOREFPROC)(GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLPATHCOVERDEPTHFUNCNVPROC)(GLenum func);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1UIEXTPROC)(GLuint program, GLint location, GLuint v0);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS2FVNVPROC)(GLuint index, GLsizei count, const GLfloat *v);
typedef void (APIENTRYP PFNGLGETPROGRAMPARAMETERDVNVPROC)(GLenum target, GLuint index, GLenum pname, GLdouble *params);
typedef void (APIENTRYP PFNGLACTIVEVARYINGNVPROC)(GLuint program, const GLchar *name);
typedef void (APIENTRYP PFNGLGETMULTITEXGENDVEXTPROC)(GLenum texunit, GLenum coord, GLenum pname, GLdouble *params);
typedef void (APIENTRYP PFNGLCLEARNAMEDBUFFERSUBDATAEXTPROC)(GLuint buffer, GLenum internalformat, GLsizeiptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data);
typedef GLuint64 (APIENTRYP PFNGLGETTEXTURESAMPLERHANDLEARBPROC)(GLuint texture, GLuint sampler);
typedef void (APIENTRYP PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
typedef void (APIENTRYP PFNGLGETACTIVEATTRIBARBPROC)(GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name);
typedef void (APIENTRYP PFNGLDETACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X4DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4FARBPROC)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef GLint (APIENTRYP PFNGLGETPROGRAMRESOURCELOCATIONPROC)(GLuint program, GLenum programInterface, const GLchar *name);
typedef void (APIENTRYP PFNGLVERTEXARRAYATTRIBLFORMATPROC)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
typedef void (APIENTRYP PFNGLNAMEDRENDERBUFFERSTORAGEPROC)(GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLVERTEXARRAYVERTEXATTRIBOFFSETEXTPROC)(GLuint vaobj, GLuint buffer, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr offset);
typedef void (APIENTRYP PFNGLINDEXFORMATNVPROC)(GLenum type, GLsizei stride);
typedef void (APIENTRYP PFNGLSTENCILOPSEPARATEATIPROC)(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
typedef void (APIENTRYP PFNGLDRAWELEMENTSBASEVERTEXPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1FNVPROC)(GLuint index, GLfloat x);
typedef void (APIENTRYP PFNGLSAMPLECOVERAGEPROC)(GLfloat value, GLboolean invert);
typedef void (APIENTRYP PFNGLTEXSTORAGE1DPROC)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI3IEXTPROC)(GLuint index, GLint x, GLint y, GLint z);
typedef void (APIENTRYP PFNGLMAKETEXTUREHANDLENONRESIDENTARBPROC)(GLuint64 handle);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2DVEXTPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLGETUNIFORMIVPROC)(GLuint program, GLint location, GLint *params);
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEPROC)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLUNIFORM4UIPROC)(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
typedef void (APIENTRYP PFNGLDISABLEVERTEXARRAYATTRIBEXTPROC)(GLuint vaobj, GLuint index);
typedef void (APIENTRYP PFNGLVALIDATEPROGRAMARBPROC)(GLhandleARB programObj);
typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERARBPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void (APIENTRYP PFNGLDEBUGMESSAGEINSERTARBPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf);
typedef void (APIENTRYP PFNGLDELETEFRAMEBUFFERSEXTPROC)(GLsizei n, const GLuint *framebuffers);
typedef void (APIENTRYP PFNGLGETTRANSFORMFEEDBACKI_VPROC)(GLuint xfb, GLenum pname, GLuint index, GLint *param);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1FVNVPROC)(GLuint index, const GLfloat *v);
typedef GLboolean (APIENTRYP PFNGLISQUERYARBPROC)(GLuint id);
typedef void (APIENTRYP PFNGLSCISSORINDEXEDPROC)(GLuint index, GLint left, GLint bottom, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLGENFRAMEBUFFERSEXTPROC)(GLsizei n, GLuint *framebuffers);
typedef void (APIENTRYP PFNGLVERTEXARRAYCOLOROFFSETEXTPROC)(GLuint vaobj, GLuint buffer, GLint size, GLenum type, GLsizei stride, GLintptr offset);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3IVARBPROC)(GLenum target, const GLint *v);
typedef GLboolean (APIENTRYP PFNGLISTEXTUREHANDLERESIDENTARBPROC)(GLuint64 handle);
typedef void * (APIENTRYP PFNGLMAPNAMEDBUFFERRANGEEXTPROC)(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);
typedef GLuint (APIENTRYP PFNGLCREATESHADERPROGRAMVPROC)(GLenum type, GLsizei count, const GLchar *const*strings);
typedef void (APIENTRYP PFNGLGETQUERYOBJECTIVPROC)(GLuint id, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3DVNVPROC)(GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3SARBPROC)(GLenum target, GLshort s, GLshort t, GLshort r);
typedef void (APIENTRYP PFNGLGENERATEMIPMAPPROC)(GLenum target);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)(GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLBLENDEQUATIONINDEXEDAMDPROC)(GLuint buf, GLenum mode);
typedef void (APIENTRYP PFNGLPOINTPARAMETERIPROC)(GLenum pname, GLint param);
typedef GLboolean (APIENTRYP PFNGLUNMAPBUFFERPROC)(GLenum target);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3SVPROC)(GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLPOINTPARAMETERFPROC)(GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1SARBPROC)(GLuint index, GLshort x);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4SVPROC)(GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLPROGRAMPARAMETER4DVNVPROC)(GLenum target, GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERIUIVEXTPROC)(GLuint texture, GLenum target, GLenum pname, const GLuint *params);
typedef void (APIENTRYP PFNGLDELETERENDERBUFFERSEXTPROC)(GLsizei n, const GLuint *renderbuffers);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1SVNVPROC)(GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2UI64VNVPROC)(GLuint program, GLint location, GLsizei count, const GLuint64EXT *value);
typedef void (APIENTRYP PFNGLRELEASESHADERCOMPILERPROC)();
typedef void (APIENTRYP PFNGLREADPIXELSPROC)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);
typedef void (APIENTRYP PFNGLSTATECAPTURENVPROC)(GLuint state, GLenum mode);
typedef void (APIENTRYP PFNGLGETDOUBLEI_VEXTPROC)(GLenum pname, GLuint index, GLdouble *params);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4DARBPROC)(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
typedef void (APIENTRYP PFNGLREADNPIXELSARBPROC)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void *data);
typedef void (APIENTRYP PFNGLMULTITEXBUFFEREXTPROC)(GLenum texunit, GLenum target, GLenum internalformat, GLuint buffer);
typedef void (APIENTRYP PFNGLUSEPROGRAMSTAGESPROC)(GLuint pipeline, GLbitfield stages, GLuint program);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTUREARBPROC)(GLenum target, GLenum attachment, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLUNIFORM3I64ARBPROC)(GLint location, GLint64 x, GLint64 y, GLint64 z);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2DEXTPROC)(GLuint program, GLint location, GLdouble x, GLdouble y);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC)(GLuint framebuffer, GLenum buf);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMHANDLEUI64ARBPROC)(GLuint program, GLint location, GLuint64 value);
typedef void (APIENTRYP PFNGLNAMEDCOPYBUFFERSUBDATAEXTPROC)(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBLDVPROC)(GLuint index, GLenum pname, GLdouble *params);
typedef void (APIENTRYP PFNGLGETNUNIFORMUIVPROC)(GLuint program, GLint location, GLsizei bufSize, GLuint *params);
typedef void (APIENTRYP PFNGLTEXSTORAGE3DMULTISAMPLEPROC)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3DNVPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRYP PFNGLTEXTURESTORAGE1DEXTPROC)(GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width);
typedef void (APIENTRYP PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffers);
typedef void (APIENTRYP PFNGLEXECUTEPROGRAMNVPROC)(GLenum target, GLuint id, const GLfloat *params);
typedef void (APIENTRYP PFNGLDELETEPROGRAMSNVPROC)(GLsizei n, const GLuint *programs);
typedef void (APIENTRYP PFNGLNORMALFORMATNVPROC)(GLenum type, GLsizei stride);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3FNVPROC)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRYP PFNGLCLEARNAMEDFRAMEBUFFERIVPROC)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint *value);
typedef void (APIENTRYP PFNGLTEXIMAGE3DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLGETOBJECTPARAMETERFVARBPROC)(GLhandleARB obj, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLCLEARNAMEDFRAMEBUFFERFVPROC)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat *value);
typedef void (APIENTRYP PFNGLTEXTURESTORAGE2DMULTISAMPLEEXTPROC)(GLuint texture, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
typedef void (APIENTRYP PFNGLMATRIXPOPEXTPROC)(GLenum mode);
typedef void (APIENTRYP PFNGLMULTITEXENVIEXTPROC)(GLenum texunit, GLenum target, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI2IEXTPROC)(GLuint index, GLint x, GLint y);
typedef void (APIENTRYP PFNGLCLAMPCOLORPROC)(GLenum target, GLenum clamp);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI2IVEXTPROC)(GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLDELETESTATESNVPROC)(GLsizei n, const GLuint *states);
typedef void (APIENTRYP PFNGLGETBUFFERPARAMETERI64VPROC)(GLenum target, GLenum pname, GLint64 *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4DVPROC)(GLuint program, GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLEDGEFLAGPOINTEREXTPROC)(GLsizei stride, GLsizei count, const GLboolean *pointer);
typedef void (APIENTRYP PFNGLMAKEBUFFERNONRESIDENTNVPROC)(GLenum target);
typedef void (APIENTRYP PFNGLUNIFORM1UI64ARBPROC)(GLint location, GLuint64 x);
typedef void (APIENTRYP PFNGLSTENCILSTROKEPATHNVPROC)(GLuint path, GLint reference, GLuint mask);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3UIVPROC)(GLuint program, GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1SARBPROC)(GLenum target, GLshort s);
typedef void (APIENTRYP PFNGLUNIFORM4I64ARBPROC)(GLint location, GLint64 x, GLint64 y, GLint64 z, GLint64 w);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2UIEXTPROC)(GLuint program, GLint location, GLuint v0, GLuint v1);
typedef void (APIENTRYP PFNGLCOMPRESSEDMULTITEXIMAGE3DEXTPROC)(GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *bits);
typedef void (APIENTRYP PFNGLNAMEDPROGRAMLOCALPARAMETER4DEXTPROC)(GLuint program, GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3DEXTPROC)(GLuint program, GLint location, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRYP PFNGLDISPATCHCOMPUTEGROUPSIZEARBPROC)(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z, GLuint group_size_x, GLuint group_size_y, GLuint group_size_z);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL1UI64ARBPROC)(GLuint index, GLuint64EXT x);
typedef void (APIENTRYP PFNGLCREATERENDERBUFFERSPROC)(GLsizei n, GLuint *renderbuffers);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUSVPROC)(GLuint index, const GLushort *v);
typedef void (APIENTRYP PFNGLDEPTHFUNCPROC)(GLenum func);
typedef void (APIENTRYP PFNGLENABLECLIENTSTATEINDEXEDEXTPROC)(GLenum array, GLuint index);
typedef void (APIENTRYP PFNGLBLENDFUNCIPROC)(GLuint buf, GLenum src, GLenum dst);
typedef void (APIENTRYP PFNGLBUFFERSTORAGEPROC)(GLenum target, GLsizeiptr size, const void *data, GLbitfield flags);
typedef void (APIENTRYP PFNGLMAKENAMEDBUFFERNONRESIDENTNVPROC)(GLuint buffer);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4FEXTPROC)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar *name);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC)(GLuint framebuffer, GLsizei n, const GLenum *bufs);
typedef void (APIENTRYP PFNGLUNIFORM2UI64VNVPROC)(GLint location, GLsizei count, const GLuint64EXT *value);
typedef void (APIENTRYP PFNGLUNIFORM4FVPROC)(GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERTEXTUREFACEEXTPROC)(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLenum face);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLGETPROGRAMSTAGEIVPROC)(GLuint program, GLenum shadertype, GLenum pname, GLint *values);
typedef void (APIENTRYP PFNGLPRIORITIZETEXTURESEXTPROC)(GLsizei n, const GLuint *textures, const GLclampf *priorities);
typedef void (APIENTRYP PFNGLENDCONDITIONALRENDERPROC)();
typedef void (APIENTRYP PFNGLTEXSUBIMAGE2DEXTPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2UIVPROC)(GLuint program, GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLGETQUERYOBJECTUIVPROC)(GLuint id, GLenum pname, GLuint *params);
typedef void (APIENTRYP PFNGLGENERATEMULTITEXMIPMAPEXTPROC)(GLenum texunit, GLenum target);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4IVPROC)(GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1UIVPROC)(GLuint program, GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTUREPROC)(GLenum target, GLenum attachment, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLMULTITEXENVFEXTPROC)(GLenum texunit, GLenum target, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL1DVPROC)(GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLNAMEDPROGRAMLOCALPARAMETER4FVEXTPROC)(GLuint program, GLenum target, GLuint index, const GLfloat *params);
typedef void (APIENTRYP PFNGLPOINTPARAMETERFVSGISPROC)(GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLDEPTHMASKPROC)(GLboolean flag);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef GLboolean (APIENTRYP PFNGLAREPROGRAMSRESIDENTNVPROC)(GLsizei n, const GLuint *programs, GLboolean *residences);
typedef void (APIENTRYP PFNGLFLUSHMAPPEDBUFFERRANGEAPPLEPROC)(GLenum target, GLintptr offset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS1DVNVPROC)(GLuint index, GLsizei count, const GLdouble *v);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4DEXTPROC)(GLuint program, GLint location, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLSAMPLEMASKINDEXEDNVPROC)(GLuint index, GLbitfield mask);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3I64ARBPROC)(GLuint program, GLint location, GLint64 x, GLint64 y, GLint64 z);
typedef void (APIENTRYP PFNGLGETTEXTUREPARAMETERFVEXTPROC)(GLuint texture, GLenum target, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETFLOATVPROC)(GLenum pname, GLfloat *data);
typedef void (APIENTRYP PFNGLMATRIXLOAD3X3FNVPROC)(GLenum matrixMode, const GLfloat *m);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2FARBPROC)(GLenum target, GLfloat s, GLfloat t);
typedef GLboolean (APIENTRYP PFNGLISTRANSFORMFEEDBACKNVPROC)(GLuint id);
typedef void (APIENTRYP PFNGLVERTEXATTRIBDIVISORARBPROC)(GLuint index, GLuint divisor);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI3IVPROC)(GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLGETNCOMPRESSEDTEXIMAGEARBPROC)(GLenum target, GLint lod, GLsizei bufSize, void *img);
typedef void (APIENTRYP PFNGLGETINTEGERVPROC)(GLenum pname, GLint *data);
typedef void (APIENTRYP PFNGLMATRIXLOADDEXTPROC)(GLenum mode, const GLdouble *m);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC)(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
typedef GLboolean (APIENTRYP PFNGLISQUERYPROC)(GLuint id);
typedef void (APIENTRYP PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLENABLEINDEXEDEXTPROC)(GLenum target, GLuint index);
typedef void (APIENTRYP PFNGLUNIFORM2UIEXTPROC)(GLint location, GLuint v0, GLuint v1);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1FARBPROC)(GLenum target, GLfloat s);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUBVPROC)(GLuint index, const GLubyte *v);
typedef void (APIENTRYP PFNGLVERTEXARRAYVERTEXATTRIBIOFFSETEXTPROC)(GLuint vaobj, GLuint buffer, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr offset);
typedef void (APIENTRYP PFNGLUNIFORM4UI64ARBPROC)(GLint location, GLuint64 x, GLuint64 y, GLuint64 z, GLuint64 w);
typedef void (APIENTRYP PFNGLTEXTURERENDERBUFFEREXTPROC)(GLuint texture, GLenum target, GLuint renderbuffer);
typedef void (APIENTRYP PFNGLGETFRAMEBUFFERPARAMETERIVPROC)(GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETPOINTERVEXTPROC)(GLenum pname, void **params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3DVEXTPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLCOPYBUFFERSUBDATAPROC)(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI1UIVPROC)(GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL4DVEXTPROC)(GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMPROC)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
typedef void (APIENTRYP PFNGLUNIFORM2UI64VARBPROC)(GLint location, GLsizei count, const GLuint64 *value);
typedef void (APIENTRYP PFNGLMAKENAMEDBUFFERRESIDENTNVPROC)(GLuint buffer, GLenum access);
typedef GLenum (APIENTRYP PFNGLPATHGLYPHINDEXRANGENVPROC)(GLenum fontTarget, const void *fontName, GLbitfield fontStyle, GLuint pathParameterTemplate, GLfloat emScale, GLuint baseAndCount[2]);
typedef void (APIENTRYP PFNGLMULTITEXPARAMETERFEXTPROC)(GLenum texunit, GLenum target, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC)(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer);
typedef void (APIENTRYP PFNGLGETVERTEXARRAYINTEGERVEXTPROC)(GLuint vaobj, GLenum pname, GLint *param);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3DARBPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRYP PFNGLUNIFORM3UIEXTPROC)(GLint location, GLuint v0, GLuint v1, GLuint v2);
typedef void (APIENTRYP PFNGLTEXPARAMETERIUIVEXTPROC)(GLenum target, GLenum pname, const GLuint *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL3DEXTPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL3DVPROC)(GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLFRAMEBUFFERRENDERBUFFERPROC)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONARBPROC)(GLhandleARB programObj, const GLcharARB *name);
typedef void (APIENTRYP PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC)(GLenum target, GLuint index, GLdouble *params);
typedef void (APIENTRYP PFNGLPATHPARAMETERFVNVPROC)(GLuint path, GLenum pname, const GLfloat *value);
typedef void (APIENTRYP PFNGLGETDOUBLEI_VPROC)(GLenum target, GLuint index, GLdouble *data);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1SVPROC)(GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLBINDSAMPLERPROC)(GLuint unit, GLuint sampler);
typedef void (APIENTRYP PFNGLBINDBUFFEROFFSETEXTPROC)(GLenum target, GLuint index, GLuint buffer, GLintptr offset);
typedef void (APIENTRYP PFNGLGETINTEGERI_VPROC)(GLenum target, GLuint index, GLint *data);
typedef void (APIENTRYP PFNGLGETTRANSFORMFEEDBACKVARYINGPROC)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name);
typedef void (APIENTRYP PFNGLGETTRANSFORMFEEDBACKVARYINGNVPROC)(GLuint program, GLuint index, GLint *location);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2DARBPROC)(GLuint index, GLdouble x, GLdouble y);
typedef void (APIENTRYP PFNGLDRAWRANGEELEMENTSEXTPROC)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices);
typedef void (APIENTRYP PFNGLDEPTHRANGEFPROC)(GLfloat n, GLfloat f);
typedef void (APIENTRYP PFNGLDELETEPROGRAMSARBPROC)(GLsizei n, const GLuint *programs);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXTURESUBIMAGE1DEXTPROC)(GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *bits);
typedef void (APIENTRYP PFNGLENABLEIPROC)(GLenum target, GLuint index);
typedef void (APIENTRYP PFNGLGETACTIVESUBROUTINENAMEPROC)(GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei *length, GLchar *name);
typedef void (APIENTRYP PFNGLBINDPROGRAMARBPROC)(GLenum target, GLuint program);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3UI64VARBPROC)(GLuint program, GLint location, GLsizei count, const GLuint64 *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLGETPATHMETRICRANGENVPROC)(GLbitfield metricQueryMask, GLuint firstPathName, GLsizei numPaths, GLsizei stride, GLfloat *metrics);
typedef void (APIENTRYP PFNGLSAMPLECOVERAGEARBPROC)(GLfloat value, GLboolean invert);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2UI64VARBPROC)(GLuint program, GLint location, GLsizei count, const GLuint64 *value);
typedef void (APIENTRYP PFNGLEDGEFLAGFORMATNVPROC)(GLsizei stride);
typedef void (APIENTRYP PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC)(GLuint framebuffer, GLsizei numAttachments, const GLenum *attachments);
typedef void (APIENTRYP PFNGLSAMPLEMASKIPROC)(GLuint maskNumber, GLbitfield mask);
typedef void (APIENTRYP PFNGLPOINTPARAMETERINVPROC)(GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X2FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLGETFRAMEBUFFERPARAMETERIVEXTPROC)(GLuint framebuffer, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLUNIFORM2I64ARBPROC)(GLint location, GLint64 x, GLint64 y);
typedef void (APIENTRYP PFNGLNAMEDBUFFERSTORAGEEXTPROC)(GLuint buffer, GLsizeiptr size, const void *data, GLbitfield flags);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2DVPROC)(GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4DVARBPROC)(GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLTEXSTORAGE2DPROC)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLENABLEVERTEXARRAYEXTPROC)(GLuint vaobj, GLenum array);
typedef void (APIENTRYP PFNGLDELETEBUFFERSARBPROC)(GLsizei n, const GLuint *buffers);
typedef void (APIENTRYP PFNGLMULTITEXRENDERBUFFEREXTPROC)(GLenum texunit, GLenum target, GLuint renderbuffer);
typedef GLboolean (APIENTRYP PFNGLUNMAPBUFFERARBPROC)(GLenum target);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4IVARBPROC)(GLenum target, const GLint *v);
typedef void (APIENTRYP PFNGLNAMEDPROGRAMLOCALPARAMETER4DVEXTPROC)(GLuint program, GLenum target, GLuint index, const GLdouble *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2FVEXTPROC)(GLuint program, GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLCOLORMASKIPROC)(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
typedef void (APIENTRYP PFNGLCOPYNAMEDBUFFERSUBDATAPROC)(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLGENPROGRAMSNVPROC)(GLsizei n, GLuint *programs);
typedef void (APIENTRYP PFNGLTEXSTORAGE3DPROC)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERIVPROC)(GLuint texture, GLenum pname, const GLint *param);
typedef void (APIENTRYP PFNGLNAMEDBUFFERDATAEXTPROC)(GLuint buffer, GLsizeiptr size, const void *data, GLenum usage);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI3UIEXTPROC)(GLuint index, GLuint x, GLuint y, GLuint z);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERTEXTUREPROC)(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NSVARBPROC)(GLuint index, const GLshort *v);
typedef GLboolean (APIENTRYP PFNGLISIMAGEHANDLERESIDENTARBPROC)(GLuint64 handle);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLGETCOMPRESSEDTEXTUREIMAGEEXTPROC)(GLuint texture, GLenum target, GLint lod, void *img);
typedef void (APIENTRYP PFNGLDRAWCOMMANDSSTATESNVPROC)(GLuint buffer, const GLintptr *indirects, const GLsizei *sizes, const GLuint *states, const GLuint *fbos, GLuint count);
typedef void (APIENTRYP PFNGLUNIFORM4UI64VNVPROC)(GLint location, GLsizei count, const GLuint64EXT *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2UIPROC)(GLuint program, GLint location, GLuint v0, GLuint v1);
typedef void (APIENTRYP PFNGLGENRENDERBUFFERSEXTPROC)(GLsizei n, GLuint *renderbuffers);
typedef void (APIENTRYP PFNGLNAMEDPROGRAMLOCALPARAMETERI4IVEXTPROC)(GLuint program, GLenum target, GLuint index, const GLint *params);
typedef void (APIENTRYP PFNGLGETMULTITEXPARAMETERIUIVEXTPROC)(GLenum texunit, GLenum target, GLenum pname, GLuint *params);
typedef void (APIENTRYP PFNGLBLENDEQUATIONIARBPROC)(GLuint buf, GLenum mode);
typedef void (APIENTRYP PFNGLTEXPARAMETERIVPROC)(GLenum target, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLUNIFORM4IVARBPROC)(GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLMATRIXORTHOEXTPROC)(GLenum mode, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
typedef void (APIENTRYP PFNGLVERTEXARRAYVERTEXBUFFERPROC)(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4UI64VNVPROC)(GLuint program, GLint location, GLsizei count, const GLuint64EXT *value);
typedef void (APIENTRYP PFNGLUNIFORM4UI64VARBPROC)(GLint location, GLsizei count, const GLuint64 *value);
typedef void (APIENTRYP PFNGLPOINTPARAMETERIVNVPROC)(GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERTEXTURELAYEREXTPROC)(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer);
typedef void (APIENTRYP PFNGLMULTITEXGENFVEXTPROC)(GLenum texunit, GLenum coord, GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLGETNAMEDBUFFERSUBDATAPROC)(GLuint buffer, GLintptr offset, GLsizeiptr size, void *data);
typedef void (APIENTRYP PFNGLSTENCILFUNCSEPARATEATIPROC)(GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2IVPROC)(GLuint program, GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLGETQUERYIVPROC)(GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETTRANSFORMFEEDBACKIVPROC)(GLuint xfb, GLenum pname, GLint *param);
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERFVPROC)(GLuint sampler, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3I64NVPROC)(GLuint program, GLint location, GLint64EXT x, GLint64EXT y, GLint64EXT z);
typedef void (APIENTRYP PFNGLUNIFORM4IARBPROC)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
typedef void (APIENTRYP PFNGLGETUNIFORMINDICESPROC)(GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames, GLuint *uniformIndices);
typedef void (APIENTRYP PFNGLMULTITEXIMAGE3DEXTPROC)(GLenum texunit, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLPOINTPARAMETERIVPROC)(GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X4DVEXTPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X3DVEXTPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLGETACTIVEVARYINGNVPROC)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name);
typedef void (APIENTRYP PFNGLUNIFORM1I64VARBPROC)(GLint location, GLsizei count, const GLint64 *value);
typedef void (APIENTRYP PFNGLGETBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, void *data);
typedef void (APIENTRYP PFNGLMATRIXTRANSLATEFEXTPROC)(GLenum mode, GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP3UIPROC)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
typedef void (APIENTRYP PFNGLTEXTUREBUFFERRANGEPROC)(GLuint texture, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLGETINTEGERUI64VNVPROC)(GLenum value, GLuint64EXT *result);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4UI64VARBPROC)(GLuint program, GLint location, GLsizei count, const GLuint64 *value);
typedef void (APIENTRYP PFNGLDEPTHRANGEPROC)(GLdouble near, GLdouble far);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBDVARBPROC)(GLuint index, GLenum pname, GLdouble *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4I64VNVPROC)(GLuint program, GLint location, GLsizei count, const GLint64EXT *value);
typedef void (APIENTRYP PFNGLDRAWARRAYSINSTANCEDEXTPROC)(GLenum mode, GLint start, GLsizei count, GLsizei primcount);
typedef void (APIENTRYP PFNGLDISABLECLIENTSTATEINDEXEDEXTPROC)(GLenum array, GLuint index);
typedef void (APIENTRYP PFNGLDRAWBUFFERPROC)(GLenum buf);
typedef void (APIENTRYP PFNGLMULTIDRAWARRAYSINDIRECTBINDLESSNVPROC)(GLenum mode, const void *indirect, GLsizei drawCount, GLsizei stride, GLint vertexBufferCount);
typedef void (APIENTRYP PFNGLCLEARBUFFERUIVPROC)(GLenum buffer, GLint drawbuffer, const GLuint *value);
typedef void (APIENTRYP PFNGLPATHSTENCILFUNCNVPROC)(GLenum func, GLint ref, GLuint mask);
typedef void (APIENTRYP PFNGLGETINTERNALFORMATI64VPROC)(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint64 *params);
typedef void (APIENTRYP PFNGLSHADERSOURCEARBPROC)(GLhandleARB shaderObj, GLsizei count, const GLcharARB **string, const GLint *length);
typedef void (APIENTRYP PFNGLPROVOKINGVERTEXEXTPROC)(GLenum mode);
typedef void (APIENTRYP PFNGLFLUSHPROC)();
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLCOVERSTROKEPATHINSTANCEDNVPROC)(GLsizei numPaths, GLenum pathNameType, const void *paths, GLuint pathBase, GLenum coverMode, GLenum transformType, const GLfloat *transformValues);
typedef void (APIENTRYP PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMHANDLEUI64VARBPROC)(GLuint program, GLint location, GLsizei count, const GLuint64 *values);
typedef void (APIENTRYP PFNGLPUSHDEBUGGROUPPROC)(GLenum source, GLuint id, GLsizei length, const GLchar *message);
typedef void (APIENTRYP PFNGLGETTEXLEVELPARAMETERIVPROC)(GLenum target, GLint level, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLSTENCILFILLPATHNVPROC)(GLuint path, GLenum fillMode, GLuint mask);
typedef void (APIENTRYP PFNGLPROGRAMVERTEXLIMITNVPROC)(GLenum target, GLint limit);
typedef void (APIENTRYP PFNGLUNIFORM3I64NVPROC)(GLint location, GLint64EXT x, GLint64EXT y, GLint64EXT z);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3UIVEXTPROC)(GLuint program, GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLCLEARSTENCILPROC)(GLint s);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS3SVNVPROC)(GLuint index, GLsizei count, const GLshort *v);
typedef void (APIENTRYP PFNGLGETBUFFERPARAMETERUI64VNVPROC)(GLenum target, GLenum pname, GLuint64EXT *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4BVARBPROC)(GLuint index, const GLbyte *v);
typedef void (APIENTRYP PFNGLGETDOUBLEVPROC)(GLenum pname, GLdouble *data);
typedef void (APIENTRYP PFNGLBLENDFUNCPROC)(GLenum sfactor, GLenum dfactor);
typedef void (APIENTRYP PFNGLCOPYTEXTUREIMAGE2DEXTPROC)(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
typedef void (APIENTRYP PFNGLBEGINQUERYINDEXEDPROC)(GLenum target, GLuint index, GLuint id);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4FARBPROC)(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef void (APIENTRYP PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETPROGRAMSTRINGARBPROC)(GLenum target, GLenum pname, void *string);
typedef void (APIENTRYP PFNGLFLUSHMAPPEDBUFFERRANGEPROC)(GLenum target, GLintptr offset, GLsizeiptr length);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLGETTEXTUREPARAMETERIUIVEXTPROC)(GLuint texture, GLenum target, GLenum pname, GLuint *params);
typedef void (APIENTRYP PFNGLGETMULTITEXENVIVEXTPROC)(GLenum texunit, GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGENQUERIESPROC)(GLsizei n, GLuint *ids);
typedef void (APIENTRYP PFNGLBLENDCOLOREXTPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (APIENTRYP PFNGLGETNUNIFORMIVARBPROC)(GLuint program, GLint location, GLsizei bufSize, GLint *params);
typedef void (APIENTRYP PFNGLTEXSUBIMAGE3DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLPATHSTENCILDEPTHOFFSETNVPROC)(GLfloat factor, GLfloat units);
typedef void (APIENTRYP PFNGLDELETESAMPLERSPROC)(GLsizei count, const GLuint *samplers);
typedef void (APIENTRYP PFNGLGETMULTITEXENVFVEXTPROC)(GLenum texunit, GLenum target, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLVERTEXARRAYNORMALOFFSETEXTPROC)(GLuint vaobj, GLuint buffer, GLenum type, GLsizei stride, GLintptr offset);
typedef void (APIENTRYP PFNGLGETBUFFERPARAMETERIVARBPROC)(GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETTEXTUREPARAMETERFVPROC)(GLuint texture, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4DNVPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRYP PFNGLGETMULTISAMPLEFVNVPROC)(GLenum pname, GLuint index, GLfloat *val);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4DVEXTPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLGETPATHCOMMANDSNVPROC)(GLuint path, GLubyte *commands);
typedef void (APIENTRYP PFNGLVERTEXARRAYTEXCOORDOFFSETEXTPROC)(GLuint vaobj, GLuint buffer, GLint size, GLenum type, GLsizei stride, GLintptr offset);
typedef void (APIENTRYP PFNGLCOLORFORMATNVPROC)(GLint size, GLenum type, GLsizei stride);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMUI64NVPROC)(GLuint program, GLint location, GLuint64EXT value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLBINDTEXTURESPROC)(GLuint first, GLsizei count, const GLuint *textures);
typedef void (APIENTRYP PFNGLUNIFORM2FARBPROC)(GLint location, GLfloat v0, GLfloat v1);
typedef void (APIENTRYP PFNGLBINDBUFFERRANGEEXTPROC)(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLCOPYTEXTURESUBIMAGE3DEXTPROC)(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void * (APIENTRYP PFNGLMAPBUFFERARBPROC)(GLenum target, GLenum access);
typedef void (APIENTRYP PFNGLGETPATHSPACINGNVPROC)(GLenum pathListMode, GLsizei numPaths, GLenum pathNameType, const void *paths, GLuint pathBase, GLfloat advanceScale, GLfloat kerningScale, GLenum transformType, GLfloat *returnedSpacing);
typedef void (APIENTRYP PFNGLUNIFORM4DVPROC)(GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3DVPROC)(GLuint program, GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLGETSHADERSOURCEARBPROC)(GLhandleARB obj, GLsizei maxLength, GLsizei *length, GLcharARB *source);
typedef void (APIENTRYP PFNGLINVALIDATEBUFFERDATAPROC)(GLuint buffer);
typedef void (APIENTRYP PFNGLMATRIXLOAD3X2FNVPROC)(GLenum matrixMode, const GLfloat *m);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC)(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLUNIFORM3FVPROC)(GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLCLEARTEXIMAGEPROC)(GLuint texture, GLint level, GLenum format, GLenum type, const void *data);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X3DVEXTPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLGETTEXTURELEVELPARAMETERFVEXTPROC)(GLuint texture, GLenum target, GLint level, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLCLEARNAMEDFRAMEBUFFERFIPROC)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSPROC)(GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL2DVEXTPROC)(GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLTEXSUBIMAGE1DEXTPROC)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC)(GLuint framebuffer, GLenum src);
typedef void (APIENTRYP PFNGLUNIFORM2I64VARBPROC)(GLint location, GLsizei count, const GLint64 *value);
typedef void (APIENTRYP PFNGLPROGRAMPARAMETER4FVNVPROC)(GLenum target, GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLUNIFORM1FVARBPROC)(GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLVERTEXFORMATNVPROC)(GLint size, GLenum type, GLsizei stride);
typedef void (APIENTRYP PFNGLBINDTRANSFORMFEEDBACKPROC)(GLenum target, GLuint id);
typedef void (APIENTRYP PFNGLUNIFORM1I64NVPROC)(GLint location, GLint64EXT x);
typedef void (APIENTRYP PFNGLUNIFORM2UIVPROC)(GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLFINISHPROC)();
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLVERTEXARRAYVERTEXOFFSETEXTPROC)(GLuint vaobj, GLuint buffer, GLint size, GLenum type, GLsizei stride, GLintptr offset);
typedef void (APIENTRYP PFNGLUNIFORM1UIVPROC)(GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLCOPYPATHNVPROC)(GLuint resultPath, GLuint srcPath);
typedef void (APIENTRYP PFNGLCLEARNAMEDBUFFERDATAEXTPROC)(GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void *data);
typedef void (APIENTRYP PFNGLCLEARDEPTHPROC)(GLdouble depth);
typedef void (APIENTRYP PFNGLDISABLEINDEXEDEXTPROC)(GLenum target, GLuint index);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE3DARBPROC)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X2DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLCREATEBUFFERSPROC)(GLsizei n, GLuint *buffers);
typedef void (APIENTRYP PFNGLBEGINTRANSFORMFEEDBACKEXTPROC)(GLenum primitiveMode);
typedef void (APIENTRYP PFNGLUNIFORM4I64VNVPROC)(GLint location, GLsizei count, const GLint64EXT *value);
typedef void (APIENTRYP PFNGLGETNUNIFORMUIVARBPROC)(GLuint program, GLint location, GLsizei bufSize, GLuint *params);
typedef void (APIENTRYP PFNGLGETPROGRAMPIPELINEINFOLOGPROC)(GLuint pipeline, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEEXTPROC)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X3DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLMAKEIMAGEHANDLENONRESIDENTARBPROC)(GLuint64 handle);
typedef void (APIENTRYP PFNGLPOLYGONMODEPROC)(GLenum face, GLenum mode);
typedef void (APIENTRYP PFNGLVERTEXARRAYVERTEXATTRIBDIVISOREXTPROC)(GLuint vaobj, GLuint index, GLuint divisor);
typedef void (APIENTRYP PFNGLWAITSYNCPROC)(GLsync sync, GLbitfield flags, GLuint64 timeout);
typedef void * (APIENTRYP PFNGLMAPNAMEDBUFFEREXTPROC)(GLuint buffer, GLenum access);
typedef void (APIENTRYP PFNGLUNIFORM3UIVEXTPROC)(GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1UI64NVPROC)(GLuint program, GLint location, GLuint64EXT x);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBFVPROC)(GLuint index, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLDISPATCHCOMPUTEPROC)(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
typedef void (APIENTRYP PFNGLGETACTIVEATTRIBPROC)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
typedef void (APIENTRYP PFNGLTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLCLEARBUFFERFVPROC)(GLenum buffer, GLint drawbuffer, const GLfloat *value);
typedef void (APIENTRYP PFNGLLOGICOPPROC)(GLenum opcode);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLGETTEXTUREPARAMETERIUIVPROC)(GLuint texture, GLenum pname, GLuint *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2FVARBPROC)(GLenum target, const GLfloat *v);
typedef void (APIENTRYP PFNGLNAMEDPROGRAMLOCALPARAMETERSI4IVEXTPROC)(GLuint program, GLenum target, GLuint index, GLsizei count, const GLint *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI1UIVEXTPROC)(GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3IARBPROC)(GLenum target, GLint s, GLint t, GLint r);
typedef void (APIENTRYP PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC)(GLenum mode, GLuint id, GLuint stream);
typedef void (APIENTRYP PFNGLVERTEXARRAYMULTITEXCOORDOFFSETEXTPROC)(GLuint vaobj, GLuint buffer, GLenum texunit, GLint size, GLenum type, GLsizei stride, GLintptr offset);
typedef void (APIENTRYP PFNGLPROVOKINGVERTEXPROC)(GLenum mode);
typedef void (APIENTRYP PFNGLSHADERBINARYPROC)(GLsizei count, const GLuint *shaders, GLenum binaryformat, const void *binary, GLsizei length);
typedef void (APIENTRYP PFNGLGETMULTITEXGENIVEXTPROC)(GLenum texunit, GLenum coord, GLenum pname, GLint *params);
typedef GLhandleARB (APIENTRYP PFNGLGETHANDLEARBPROC)(GLenum pname);
typedef void (APIENTRYP PFNGLTRANSFORMPATHNVPROC)(GLuint resultPath, GLuint srcPath, GLenum transformType, const GLfloat *transformValues);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXTUREIMAGE2DEXTPROC)(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *bits);
typedef void (APIENTRYP PFNGLGETNAMEDPROGRAMLOCALPARAMETERDVEXTPROC)(GLuint program, GLenum target, GLuint index, GLdouble *params);
typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3UI64NVPROC)(GLuint program, GLint location, GLuint64EXT x, GLuint64EXT y, GLuint64EXT z);
typedef void (APIENTRYP PFNGLUNIFORM1IVPROC)(GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLMATRIXMULTFEXTPROC)(GLenum mode, const GLfloat *m);
typedef void (APIENTRYP PFNGLDRAWARRAYSINSTANCEDPROC)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2SNVPROC)(GLuint index, GLshort x, GLshort y);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIVPROC)(GLuint sampler, GLenum pname, const GLint *param);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4UIVPROC)(GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLENDQUERYINDEXEDPROC)(GLenum target, GLuint index);
typedef void (APIENTRYP PFNGLPROGRAMPARAMETERIARBPROC)(GLuint program, GLenum pname, GLint value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1IVPROC)(GLuint program, GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLBINDRENDERBUFFERPROC)(GLenum target, GLuint renderbuffer);
typedef void (APIENTRYP PFNGLMULTITEXSUBIMAGE3DEXTPROC)(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
typedef GLboolean (APIENTRYP PFNGLISPROGRAMPROC)(GLuint program);
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSINDIRECTAMDPROC)(GLenum mode, GLenum type, const void *indirect, GLsizei primcount, GLsizei stride);
typedef void (APIENTRYP PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X2DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4FVPROC)(GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLGETNCOMPRESSEDTEXIMAGEPROC)(GLenum target, GLint lod, GLsizei bufSize, void *pixels);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS3FVNVPROC)(GLuint index, GLsizei count, const GLfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2FVPROC)(GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLDRAWARRAYSINDIRECTPROC)(GLenum mode, const void *indirect);
typedef void (APIENTRYP PFNGLUNIFORM3UI64NVPROC)(GLint location, GLuint64EXT x, GLuint64EXT y, GLuint64EXT z);
typedef void (APIENTRYP PFNGLBEGINCONDITIONALRENDERNVPROC)(GLuint id, GLenum mode);
typedef void (APIENTRYP PFNGLSTENCILTHENCOVERFILLPATHNVPROC)(GLuint path, GLenum fillMode, GLuint mask, GLenum coverMode);
typedef void (APIENTRYP PFNGLVERTEXARRAYINDEXOFFSETEXTPROC)(GLuint vaobj, GLuint buffer, GLenum type, GLsizei stride, GLintptr offset);
typedef void (APIENTRYP PFNGLDISABLECLIENTSTATEIEXTPROC)(GLenum array, GLuint index);
typedef void (APIENTRYP PFNGLCOPYMULTITEXSUBIMAGE1DEXTPROC)(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
typedef void (APIENTRYP PFNGLTEXBUFFERARBPROC)(GLenum target, GLenum internalformat, GLuint buffer);
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSINDIRECTBINDLESSCOUNTNVPROC)(GLenum mode, GLenum type, const void *indirect, GLsizei drawCount, GLsizei maxDrawCount, GLsizei stride, GLint vertexBufferCount);
typedef void (APIENTRYP PFNGLGETBUFFERPOINTERVARBPROC)(GLenum target, GLenum pname, void **params);
typedef void (APIENTRYP PFNGLMULTITEXPARAMETERIUIVEXTPROC)(GLenum texunit, GLenum target, GLenum pname, const GLuint *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3FPROC)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3DPROC)(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2);
typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERNVPROC)(GLuint index, GLint fsize, GLenum type, GLsizei stride, const void *pointer);
typedef void (APIENTRYP PFNGLENDCONDITIONALRENDERNVPROC)();
typedef void (APIENTRYP PFNGLUNIFORM3I64VARBPROC)(GLint location, GLsizei count, const GLint64 *value);
typedef void (APIENTRYP PFNGLGETQUERYIVARBPROC)(GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI1UIPROC)(GLuint index, GLuint x);
typedef void (APIENTRYP PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)(GLenum target, GLenum pname, GLint *params);
typedef GLboolean (APIENTRYP PFNGLISBUFFERRESIDENTNVPROC)(GLenum target);
typedef void (APIENTRYP PFNGLGETNAMEDRENDERBUFFERPARAMETERIVEXTPROC)(GLuint renderbuffer, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4BVEXTPROC)(GLuint index, const GLbyte *v);
typedef void (APIENTRYP PFNGLMULTITEXGENDVEXTPROC)(GLenum texunit, GLenum coord, GLenum pname, const GLdouble *params);
typedef void (APIENTRYP PFNGLGETPROGRAMENVPARAMETERDVARBPROC)(GLenum target, GLuint index, GLdouble *params);
typedef void (APIENTRYP PFNGLSECONDARYCOLORFORMATNVPROC)(GLint size, GLenum type, GLsizei stride);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4UBVNVPROC)(GLuint index, const GLubyte *v);
typedef void (APIENTRYP PFNGLINVALIDATETEXSUBIMAGEPROC)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth);
typedef void (APIENTRYP PFNGLGETINTEGER64VPROC)(GLenum pname, GLint64 *data);
typedef void (APIENTRYP PFNGLGETPROGRAMENVPARAMETERFVARBPROC)(GLenum target, GLuint index, GLfloat *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4USVARBPROC)(GLuint index, const GLushort *v);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC)(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void (APIENTRYP PFNGLPOINTPARAMETERFEXTPROC)(GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NIVPROC)(GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLCLEARBUFFERIVPROC)(GLenum buffer, GLint drawbuffer, const GLint *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4IVARBPROC)(GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLMATRIXMULT3X2FNVPROC)(GLenum matrixMode, const GLfloat *m);
typedef void (APIENTRYP PFNGLBEGINCONDITIONALRENDERNVXPROC)(GLuint id);
typedef void (APIENTRYP PFNGLGETNAMEDPROGRAMSTRINGEXTPROC)(GLuint program, GLenum target, GLenum pname, void *string);
typedef void (APIENTRYP PFNGLGETTEXTURELEVELPARAMETERFVPROC)(GLuint texture, GLint level, GLenum pname, GLfloat *params);
typedef GLint (APIENTRYP PFNGLGETATTRIBLOCATIONARBPROC)(GLhandleARB programObj, const GLcharARB *name);
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIUIVPROC)(GLuint sampler, GLenum pname, GLuint *params);
typedef void (APIENTRYP PFNGLCOPYTEXTURESUBIMAGE1DEXTPROC)(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
typedef void (APIENTRYP PFNGLTEXTURESUBIMAGE1DPROC)(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLGENTRANSFORMFEEDBACKSPROC)(GLsizei n, GLuint *ids);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS2SVNVPROC)(GLuint index, GLsizei count, const GLshort *v);
typedef void (APIENTRYP PFNGLTEXTUREIMAGE3DEXTPROC)(GLuint texture, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLGETNAMEDBUFFERPOINTERVPROC)(GLuint buffer, GLenum pname, void **params);
typedef GLboolean (APIENTRYP PFNGLISENABLEDINDEXEDEXTPROC)(GLenum target, GLuint index);
typedef void (APIENTRYP PFNGLGETNAMEDPROGRAMIVEXTPROC)(GLuint program, GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETTEXTURELEVELPARAMETERIVEXTPROC)(GLuint texture, GLenum target, GLint level, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETOBJECTPARAMETERIVARBPROC)(GLhandleARB obj, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLGETNAMEDPROGRAMLOCALPARAMETERIUIVEXTPROC)(GLuint program, GLenum target, GLuint index, GLuint *params);
typedef void (APIENTRYP PFNGLREADNPIXELSPROC)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void *data);
typedef void (APIENTRYP PFNGLREADBUFFERPROC)(GLenum src);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4FVEXTPROC)(GLuint program, GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2SARBPROC)(GLuint index, GLshort x, GLshort y);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data);
typedef GLenum (APIENTRYP PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)(GLenum target);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1FARBPROC)(GLuint index, GLfloat x);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1IVARBPROC)(GLenum target, const GLint *v);
typedef void (APIENTRYP PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)(GLuint index);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI1IEXTPROC)(GLuint index, GLint x);
typedef void (APIENTRYP PFNGLGETPATHDASHARRAYNVPROC)(GLuint path, GLfloat *dashArray);
typedef void (APIENTRYP PFNGLSTENCILTHENCOVERFILLPATHINSTANCEDNVPROC)(GLsizei numPaths, GLenum pathNameType, const void *paths, GLuint pathBase, GLenum fillMode, GLuint mask, GLenum coverMode, GLenum transformType, const GLfloat *transformValues);
typedef void (APIENTRYP PFNGLCOLORMASKPROC)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
typedef void (APIENTRYP PFNGLGETNTEXIMAGEPROC)(GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels);
typedef void (APIENTRYP PFNGLPROGRAMLOCALPARAMETER4DARBPROC)(GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRYP PFNGLBLENDEQUATIONPROC)(GLenum mode);
typedef void (APIENTRYP PFNGLGETQUERYOBJECTUI64VEXTPROC)(GLuint id, GLenum pname, GLuint64 *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4UIVEXTPROC)(GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLGETPROGRAMRESOURCEIVPROC)(GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params);
typedef void (APIENTRYP PFNGLCLEARBUFFERDATAPROC)(GLenum target, GLenum internalformat, GLenum format, GLenum type, const void *data);
typedef void (APIENTRYP PFNGLGETPROGRAMIVNVPROC)(GLuint id, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLDELETETRANSFORMFEEDBACKSPROC)(GLsizei n, const GLuint *ids);
typedef void (APIENTRYP PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLint basevertex);
typedef void (APIENTRYP PFNGLUNIFORM4I64VARBPROC)(GLint location, GLsizei count, const GLint64 *value);
typedef void (APIENTRYP PFNGLTEXTURESTORAGE3DMULTISAMPLEEXTPROC)(GLuint texture, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
typedef void (APIENTRYP PFNGLCOMPRESSEDMULTITEXIMAGE1DEXTPROC)(GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void *bits);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2IVEXTPROC)(GLuint program, GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1DVPROC)(GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS3DVNVPROC)(GLuint index, GLsizei count, const GLdouble *v);
typedef void (APIENTRYP PFNGLBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEIPROC)(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
typedef void (APIENTRYP PFNGLGETBOOLEANINDEXEDVEXTPROC)(GLenum target, GLuint index, GLboolean *data);
typedef void (APIENTRYP PFNGLDISABLEIPROC)(GLenum target, GLuint index);
typedef void (APIENTRYP PFNGLGETUNIFORMUI64VARBPROC)(GLuint program, GLint location, GLuint64 *params);
typedef void (APIENTRYP PFNGLGETSYNCIVPROC)(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values);
typedef void (APIENTRYP PFNGLMAKEIMAGEHANDLERESIDENTARBPROC)(GLuint64 handle, GLenum access);
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2IPROC)(GLuint program, GLint location, GLint v0, GLint v1);
typedef void (APIENTRYP PFNGLLISTDRAWCOMMANDSSTATESCLIENTNVPROC)(GLuint list, GLuint segment, const void **indirects, const GLsizei *sizes, const GLuint *states, const GLuint *fbos, GLuint count);
typedef void (APIENTRYP PFNGLBEGINTRANSFORMFEEDBACKNVPROC)(GLenum primitiveMode);
typedef void (APIENTRYP PFNGLGETPROGRAMBINARYPROC)(GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary);
typedef void (APIENTRYP PFNGLPROGRAMENVPARAMETER4FVARBPROC)(GLenum target, GLuint index, const GLfloat *params);
typedef void (APIENTRYP PFNGLPAUSETRANSFORMFEEDBACKPROC)();
typedef void (APIENTRYP PFNGLUNIFORM2UI64ARBPROC)(GLint location, GLuint64 x, GLuint64 y);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP2UIPROC)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
typedef void (APIENTRYP PFNGLTRANSFORMFEEDBACKVARYINGSNVPROC)(GLuint program, GLsizei count, const GLint *locations, GLenum bufferMode);
typedef void (APIENTRYP PFNGLTEXSUBIMAGE1DPROC)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLBLITFRAMEBUFFEREXTPROC)(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVARBPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP3UIVPROC)(GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4IVPROC)(GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLGETMULTITEXLEVELPARAMETERIVEXTPROC)(GLenum texunit, GLenum target, GLint level, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETVERTEXARRAYIVPROC)(GLuint vaobj, GLenum pname, GLint *param);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4FVARBPROC)(GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLGETNAMEDPROGRAMLOCALPARAMETERIIVEXTPROC)(GLuint program, GLenum target, GLuint index, GLint *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3UIEXTPROC)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X2DVEXTPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLTEXPARAMETERFVPROC)(GLenum target, GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLCALLCOMMANDLISTNVPROC)(GLuint list);
typedef void (APIENTRYP PFNGLUNIFORM3DVPROC)(GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3FVPROC)(GLuint program, GLint location, GLsizei count, const GLfloat *value);
typedef GLboolean (APIENTRYP PFNGLISBUFFERARBPROC)(GLuint buffer);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4DVNVPROC)(GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1FVEXTPROC)(GLuint program, GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1DVNVPROC)(GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE1DARBPROC)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLBINDSAMPLERSPROC)(GLuint first, GLsizei count, const GLuint *samplers);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4USVPROC)(GLuint index, const GLushort *v);
typedef void (APIENTRYP PFNGLGETQUERYOBJECTUI64VPROC)(GLuint id, GLenum pname, GLuint64 *params);
typedef void (APIENTRYP PFNGLCREATETEXTURESPROC)(GLenum target, GLsizei n, GLuint *textures);
typedef void (APIENTRYP PFNGLGETTEXTUREIMAGEPROC)(GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1FVPROC)(GLuint program, GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLNORMALPOINTEREXTPROC)(GLenum type, GLsizei stride, GLsizei count, const void *pointer);
typedef void (APIENTRYP PFNGLMULTIDRAWARRAYSEXTPROC)(GLenum mode, const GLint *first, const GLsizei *count, GLsizei primcount);
typedef void (APIENTRYP PFNGLPATHPARAMETERINVPROC)(GLuint path, GLenum pname, GLint value);
typedef void (APIENTRYP PFNGLCREATECOMMANDLISTSNVPROC)(GLsizei n, GLuint *lists);
typedef void (APIENTRYP PFNGLCREATETRANSFORMFEEDBACKSPROC)(GLsizei n, GLuint *ids);
typedef void (APIENTRYP PFNGLDELETEPROGRAMPIPELINESPROC)(GLsizei n, const GLuint *pipelines);
typedef void (APIENTRYP PFNGLGENPROGRAMPIPELINESPROC)(GLsizei n, GLuint *pipelines);
typedef void (APIENTRYP PFNGLVERTEXARRAYEDGEFLAGOFFSETEXTPROC)(GLuint vaobj, GLuint buffer, GLsizei stride, GLintptr offset);
typedef void (APIENTRYP PFNGLUNIFORM4UIVEXTPROC)(GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLUNIFORMSUBROUTINESUIVPROC)(GLenum shadertype, GLsizei count, const GLuint *indices);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1I64VNVPROC)(GLuint program, GLint location, GLsizei count, const GLint64EXT *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4UIEXTPROC)(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
typedef void (APIENTRYP PFNGLCOMPRESSEDMULTITEXSUBIMAGE1DEXTPROC)(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *bits);
typedef void (APIENTRYP PFNGLMULTITEXGENDEXTPROC)(GLenum texunit, GLenum coord, GLenum pname, GLdouble param);
typedef void (APIENTRYP PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef void (APIENTRYP PFNGLUNIFORM1UIVEXTPROC)(GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLGETUNIFORMUIVEXTPROC)(GLuint program, GLint location, GLuint *params);
typedef void (APIENTRYP PFNGLGETQUERYOBJECTI64VPROC)(GLuint id, GLenum pname, GLint64 *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIBIPOINTERPROC)(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
typedef void (APIENTRYP PFNGLDEPTHRANGEFOESPROC)(GLclampf n, GLclampf f);
typedef void (APIENTRYP PFNGLGETNAMEDBUFFERPARAMETERIVPROC)(GLuint buffer, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETNAMEDPROGRAMLOCALPARAMETERFVEXTPROC)(GLuint program, GLenum target, GLuint index, GLfloat *params);
typedef void (APIENTRYP PFNGLLINEWIDTHPROC)(GLfloat width);
typedef void (APIENTRYP PFNGLINVALIDATEFRAMEBUFFERPROC)(GLenum target, GLsizei numAttachments, const GLenum *attachments);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE2DPROC)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLGETQUERYBUFFEROBJECTUIVPROC)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
typedef void (APIENTRYP PFNGLDRAWBUFFERSPROC)(GLsizei n, const GLenum *bufs);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1SPROC)(GLuint index, GLshort x);
typedef void (APIENTRYP PFNGLUNIFORM1IARBPROC)(GLint location, GLint v0);
typedef void (APIENTRYP PFNGLUNIFORM1UI64VNVPROC)(GLint location, GLsizei count, const GLuint64EXT *value);
typedef void (APIENTRYP PFNGLGETCLIPPLANEFOESPROC)(GLenum plane, GLfloat *equation);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3IVEXTPROC)(GLuint program, GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLPROGRAMPARAMETER4FNVPROC)(GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRYP PFNGLDELETEPROGRAMPROC)(GLuint program);
typedef void (APIENTRYP PFNGLPROGRAMPARAMETER4DNVPROC)(GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X3FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLTEXTUREBARRIERPROC)();
typedef void (APIENTRYP PFNGLMULTITEXPARAMETERIIVEXTPROC)(GLenum texunit, GLenum target, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLGETINTERNALFORMATIVPROC)(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint *params);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1DARBPROC)(GLenum target, GLdouble s);
typedef void (APIENTRYP PFNGLCLEARBUFFERFIPROC)(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
typedef void (APIENTRYP PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint *arrays);
typedef void (APIENTRYP PFNGLGETTRANSFORMFEEDBACKI64_VPROC)(GLuint xfb, GLenum pname, GLuint index, GLint64 *param);
typedef void (APIENTRYP PFNGLENABLEVERTEXARRAYATTRIBPROC)(GLuint vaobj, GLuint index);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLVERTEXBINDINGDIVISORPROC)(GLuint bindingindex, GLuint divisor);
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIIVPROC)(GLuint sampler, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIBFORMATNVPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP4UIVPROC)(GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
typedef void (APIENTRYP PFNGLGETQUERYBUFFEROBJECTI64VPROC)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBDVPROC)(GLuint index, GLenum pname, GLdouble *params);
typedef GLboolean (APIENTRYP PFNGLARETEXTURESRESIDENTEXTPROC)(GLsizei n, const GLuint *textures, GLboolean *residences);
typedef void (APIENTRYP PFNGLGETPROGRAMRESOURCENAMEPROC)(GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name);
typedef void (APIENTRYP PFNGLGETFLOATINDEXEDVEXTPROC)(GLenum target, GLuint index, GLfloat *data);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2SVARBPROC)(GLenum target, const GLshort *v);
typedef void (APIENTRYP PFNGLGETTEXPARAMETERIIVEXTPROC)(GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLUNIFORM1UIEXTPROC)(GLint location, GLuint v0);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUBARBPROC)(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL1UI64VARBPROC)(GLuint index, const GLuint64EXT *v);
typedef void (APIENTRYP PFNGLUNIFORM1I64VNVPROC)(GLint location, GLsizei count, const GLint64EXT *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS4SVNVPROC)(GLuint index, GLsizei count, const GLshort *v);
typedef void (APIENTRYP PFNGLUNIFORM1UIPROC)(GLint location, GLuint v0);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2FVARBPROC)(GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLCLEARDEPTHFPROC)(GLfloat d);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4SVEXTPROC)(GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLMEMORYBARRIERPROC)(GLbitfield barriers);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2IARBPROC)(GLenum target, GLint s, GLint t);
typedef GLuint64 (APIENTRYP PFNGLGETTEXTUREHANDLEARBPROC)(GLuint texture);
typedef GLboolean (APIENTRYP PFNGLISPROGRAMARBPROC)(GLuint program);
typedef void (APIENTRYP PFNGLBINDIMAGETEXTUREEXTPROC)(GLuint index, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLint format);
typedef GLint (APIENTRYP PFNGLGETFRAGDATALOCATIONPROC)(GLuint program, const GLchar *name);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1SVARBPROC)(GLenum target, const GLshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4IEXTPROC)(GLuint index, GLint x, GLint y, GLint z, GLint w);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NBVARBPROC)(GLuint index, const GLbyte *v);
typedef void (APIENTRYP PFNGLCOLORPOINTEREXTPROC)(GLint size, GLenum type, GLsizei stride, GLsizei count, const void *pointer);
typedef void (APIENTRYP PFNGLENABLECLIENTSTATEIEXTPROC)(GLenum array, GLuint index);
typedef void (APIENTRYP PFNGLCLEARTEXSUBIMAGEPROC)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *data);
typedef void (APIENTRYP PFNGLDETACHOBJECTARBPROC)(GLhandleARB containerObj, GLhandleARB attachedObj);
typedef void (APIENTRYP PFNGLGETTEXTUREPARAMETERIIVPROC)(GLuint texture, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXTUREIMAGE3DEXTPROC)(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *bits);
typedef void (APIENTRYP PFNGLGETVERTEXARRAYINTEGERI_VEXTPROC)(GLuint vaobj, GLuint index, GLenum pname, GLint *param);
typedef GLenum (APIENTRYP PFNGLCHECKNAMEDFRAMEBUFFERSTATUSEXTPROC)(GLuint framebuffer, GLenum target);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLUNIFORM3FARBPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef GLboolean (APIENTRYP PFNGLISSHADERPROC)(GLuint shader);
typedef void (APIENTRYP PFNGLCOPYMULTITEXSUBIMAGE2DEXTPROC)(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYARBPROC)(GLuint index);
typedef void (APIENTRYP PFNGLENABLEPROC)(GLenum cap);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIVARBPROC)(GLuint index, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLDELETEQUERIESPROC)(GLsizei n, const GLuint *ids);
typedef void (APIENTRYP PFNGLUSEPROGRAMOBJECTARBPROC)(GLhandleARB programObj);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4DVPROC)(GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4FPROC)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRYP PFNGLUNIFORM3UI64VNVPROC)(GLint location, GLsizei count, const GLuint64EXT *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4DPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRYP PFNGLMATRIXLOADIDENTITYEXTPROC)(GLenum mode);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3UIPROC)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3SVARBPROC)(GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLMATRIXSCALEDEXTPROC)(GLenum mode, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBPOINTERVNVPROC)(GLuint index, GLenum pname, void **pointer);
typedef void (APIENTRYP PFNGLGENERATEMIPMAPEXTPROC)(GLenum target);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI2UIVEXTPROC)(GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLPATHCOORDSNVPROC)(GLuint path, GLsizei numCoords, GLenum coordType, const void *coords);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1IPROC)(GLuint program, GLint location, GLint v0);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1DPROC)(GLuint program, GLint location, GLdouble v0);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1FPROC)(GLuint program, GLint location, GLfloat v0);
typedef void (APIENTRYP PFNGLPROGRAMPARAMETERIEXTPROC)(GLuint program, GLenum pname, GLint value);
typedef void (APIENTRYP PFNGLBEGINTRANSFORMFEEDBACKPROC)(GLenum primitiveMode);
typedef void (APIENTRYP PFNGLCOPYTEXTURESUBIMAGE2DPROC)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3IVPROC)(GLuint program, GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLBINDFRAGDATALOCATIONEXTPROC)(GLuint program, GLuint color, const GLchar *name);
typedef void (APIENTRYP PFNGLFRAMEBUFFERREADBUFFEREXTPROC)(GLuint framebuffer, GLenum mode);
typedef void (APIENTRYP PFNGLNAMEDBUFFERSUBDATAPROC)(GLuint buffer, GLintptr offset, GLsizeiptr size, const void *data);
typedef void (APIENTRYP PFNGLGENBUFFERSARBPROC)(GLsizei n, GLuint *buffers);
typedef void (APIENTRYP PFNGLBLENDEQUATIONIPROC)(GLuint buf, GLenum mode);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4SPROC)(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4FVNVPROC)(GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLGETBUFFERSUBDATAARBPROC)(GLenum target, GLintptrARB offset, GLsizeiptrARB size, void *data);
typedef void (APIENTRYP PFNGLGETPATHPARAMETERIVNVPROC)(GLuint path, GLenum pname, GLint *value);
typedef GLint (APIENTRYP PFNGLGETATTRIBLOCATIONPROC)(GLuint program, const GLchar *name);
typedef void (APIENTRYP PFNGLDISABLEVERTEXARRAYATTRIBPROC)(GLuint vaobj, GLuint index);
typedef void (APIENTRYP PFNGLGETTEXTUREPARAMETERIVPROC)(GLuint texture, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETATTACHEDOBJECTSARBPROC)(GLhandleARB containerObj, GLsizei maxCount, GLsizei *count, GLhandleARB *obj);
typedef void (APIENTRYP PFNGLGETTEXIMAGEPROC)(GLenum target, GLint level, GLenum format, GLenum type, void *pixels);
typedef void (APIENTRYP PFNGLCLEARDEPTHFOESPROC)(GLclampf depth);
typedef void (APIENTRYP PFNGLDELETECOMMANDLISTSNVPROC)(GLsizei n, const GLuint *lists);
typedef void (APIENTRYP PFNGLPATHSUBCOORDSNVPROC)(GLuint path, GLsizei coordStart, GLsizei numCoords, GLenum coordType, const void *coords);
typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEINDEXEDAMDPROC)(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
typedef void (APIENTRYP PFNGLPROGRAMSTRINGARBPROC)(GLenum target, GLenum format, GLsizei len, const void *string);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIVNVPROC)(GLuint index, GLenum pname, GLint *params);
typedef GLboolean (APIENTRYP PFNGLISTEXTUREPROC)(GLuint texture);
typedef void (APIENTRYP PFNGLTEXTURESUBIMAGE1DEXTPROC)(GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC)(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2DVARBPROC)(GLuint index, const GLdouble *v);
typedef void * (APIENTRYP PFNGLMAPNAMEDBUFFERPROC)(GLuint buffer, GLenum access);
typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void (APIENTRYP PFNGLDELETETEXTURESEXTPROC)(GLsizei n, const GLuint *textures);
typedef void (APIENTRYP PFNGLDRAWBUFFERSATIPROC)(GLsizei n, const GLenum *bufs);
typedef void (APIENTRYP PFNGLGETFLOATI_VPROC)(GLenum target, GLuint index, GLfloat *data);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1IVEXTPROC)(GLuint program, GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4BVPROC)(GLuint index, const GLbyte *v);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4UI64NVPROC)(GLuint program, GLint location, GLuint64EXT x, GLuint64EXT y, GLuint64EXT z, GLuint64EXT w);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP1UIPROC)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4DVEXTPROC)(GLuint program, GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLENDTRANSFORMFEEDBACKEXTPROC)();
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLENDCONDITIONALRENDERNVXPROC)();
typedef void (APIENTRYP PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC)(GLuint renderbuffer, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLBINDBUFFERSRANGEPROC)(GLenum target, GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizeiptr *sizes);
typedef void (APIENTRYP PFNGLDRAWCOMMANDSADDRESSNVPROC)(GLenum primitiveMode, const GLuint64 *indirects, const GLsizei *sizes, GLuint count);
typedef void (APIENTRYP PFNGLBINDFRAGDATALOCATIONINDEXEDPROC)(GLuint program, GLuint colorNumber, GLuint index, const GLchar *name);
typedef void (APIENTRYP PFNGLUNIFORM2IVPROC)(GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERRENDERBUFFEREXTPROC)(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance);
typedef void (APIENTRYP PFNGLGETNAMEDBUFFERPARAMETERIVEXTPROC)(GLuint buffer, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLTEXTURESUBIMAGE3DPROC)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE1DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (APIENTRYP PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLMATRIXLOADTRANSPOSEDEXTPROC)(GLenum mode, const GLdouble *m);
typedef void (APIENTRYP PFNGLGETPOINTERI_VEXTPROC)(GLenum pname, GLuint index, void **params);
typedef void (APIENTRYP PFNGLNAMEDBUFFERSTORAGEPROC)(GLuint buffer, GLsizeiptr size, const void *data, GLbitfield flags);
typedef void (APIENTRYP PFNGLPOLYGONOFFSETPROC)(GLfloat factor, GLfloat units);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2UI64NVPROC)(GLuint program, GLint location, GLuint64EXT x, GLuint64EXT y);
typedef void (APIENTRYP PFNGLUNIFORM1DVPROC)(GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLPROGRAMENVPARAMETER4DVARBPROC)(GLenum target, GLuint index, const GLdouble *params);
typedef void (APIENTRYP PFNGLMULTITEXSUBIMAGE2DEXTPROC)(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLDEBUGMESSAGECONTROLARBPROC)(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);
typedef GLboolean (APIENTRYP PFNGLISTEXTUREEXTPROC)(GLuint texture);
typedef void (APIENTRYP PFNGLLINKPROGRAMARBPROC)(GLhandleARB programObj);
typedef void (APIENTRYP PFNGLVERTEXARRAYVERTEXATTRIBBINDINGEXTPROC)(GLuint vaobj, GLuint attribindex, GLuint bindingindex);
typedef void (APIENTRYP PFNGLUNIFORM1UI64VARBPROC)(GLint location, GLsizei count, const GLuint64 *value);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE3DPROC)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLGETPATHCOORDSNVPROC)(GLuint path, GLfloat *coords);
typedef void (APIENTRYP PFNGLCOMMANDLISTSEGMENTSNVPROC)(GLuint list, GLuint segments);
typedef void (APIENTRYP PFNGLCLAMPCOLORARBPROC)(GLenum target, GLenum clamp);
typedef void (APIENTRYP PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC)(GLuint program, GLsizei count, const GLchar *const*varyings, GLenum bufferMode);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2SVNVPROC)(GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLMULTITEXPARAMETERFVEXTPROC)(GLenum texunit, GLenum target, GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2IVARBPROC)(GLenum target, const GLint *v);
typedef void (APIENTRYP PFNGLGETINFOLOGARBPROC)(GLhandleARB obj, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog);
typedef void (APIENTRYP PFNGLGETTRACKMATRIXIVNVPROC)(GLenum target, GLuint address, GLenum pname, GLint *params);
typedef GLboolean (APIENTRYP PFNGLISNAMEDBUFFERRESIDENTNVPROC)(GLuint buffer);
typedef void (APIENTRYP PFNGLMATRIXMULTDEXTPROC)(GLenum mode, const GLdouble *m);
typedef void (APIENTRYP PFNGLCOVERFILLPATHNVPROC)(GLuint path, GLenum coverMode);
typedef void (APIENTRYP PFNGLGENERATETEXTUREMIPMAPPROC)(GLuint texture);
typedef void (APIENTRYP PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIVEXTPROC)(GLuint program, GLenum target, GLuint index, const GLuint *params);
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC)(GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount, const GLint *basevertex);
typedef void (APIENTRYP PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)(GLenum target, GLuint index, const GLfloat *params);
typedef void (APIENTRYP PFNGLMEMORYBARRIEREXTPROC)(GLbitfield barriers);
typedef void (APIENTRYP PFNGLGETTEXPARAMETERIIVPROC)(GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERPARAMETERIEXTPROC)(GLuint framebuffer, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4USVEXTPROC)(GLuint index, const GLushort *v);
typedef void (APIENTRYP PFNGLTEXTUREBUFFERRANGEEXTPROC)(GLuint texture, GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLTRANSFORMFEEDBACKVARYINGSPROC)(GLuint program, GLsizei count, const GLchar *const*varyings, GLenum bufferMode);
typedef void (APIENTRYP PFNGLGETNUNIFORMI64VARBPROC)(GLuint program, GLint location, GLsizei bufSize, GLint64 *params);
typedef void (APIENTRYP PFNGLCOMPRESSEDMULTITEXSUBIMAGE2DEXTPROC)(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *bits);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4SVARBPROC)(GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLDEBUGMESSAGECALLBACKPROC)(GLDEBUGPROC callback, const void *userParam);
typedef void (APIENTRYP PFNGLSTENCILTHENCOVERSTROKEPATHINSTANCEDNVPROC)(GLsizei numPaths, GLenum pathNameType, const void *paths, GLuint pathBase, GLint reference, GLuint mask, GLenum coverMode, GLenum transformType, const GLfloat *transformValues);
typedef void (APIENTRYP PFNGLBUFFERSUBDATAARBPROC)(GLenum target, GLintptrARB offset, GLsizeiptrARB size, const void *data);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBLUI64VARBPROC)(GLuint index, GLenum pname, GLuint64EXT *params);
typedef void (APIENTRYP PFNGLGETPATHMETRICSNVPROC)(GLbitfield metricQueryMask, GLsizei numPaths, GLenum pathNameType, const void *paths, GLuint pathBase, GLsizei stride, GLfloat *metrics);
typedef void (APIENTRYP PFNGLGETBOOLEANI_VPROC)(GLenum target, GLuint index, GLboolean *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDMULTITEXIMAGE2DEXTPROC)(GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *bits);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERIIVPROC)(GLuint texture, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLBINDBUFFERRANGENVPROC)(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2DVEXTPROC)(GLuint program, GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)(GLuint framebuffer, GLenum attachment, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLVERTEXARRAYVERTEXATTRIBIFORMATEXTPROC)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
typedef void (APIENTRYP PFNGLHINTPROC)(GLenum target, GLenum mode);
typedef void (APIENTRYP PFNGLGETTEXTUREIMAGEEXTPROC)(GLuint texture, GLenum target, GLint level, GLenum format, GLenum type, void *pixels);
typedef void (APIENTRYP PFNGLMULTIDRAWARRAYSINDIRECTPROC)(GLenum mode, const void *indirect, GLsizei drawcount, GLsizei stride);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP2UIVPROC)(GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1I64VARBPROC)(GLuint program, GLint location, GLsizei count, const GLint64 *value);
typedef void (APIENTRYP PFNGLPUSHCLIENTATTRIBDEFAULTEXTPROC)(GLbitfield mask);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4SVNVPROC)(GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLCOVERFILLPATHINSTANCEDNVPROC)(GLsizei numPaths, GLenum pathNameType, const void *paths, GLuint pathBase, GLenum coverMode, GLenum transformType, const GLfloat *transformValues);
typedef void (APIENTRYP PFNGLMULTITEXPARAMETERIVEXTPROC)(GLenum texunit, GLenum target, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC)(GLuint program, GLuint bufferIndex, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLVERTEXARRAYATTRIBBINDINGPROC)(GLuint vaobj, GLuint attribindex, GLuint bindingindex);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3FVNVPROC)(GLuint index, const GLfloat *v);
typedef void (APIENTRYP PFNGLPROGRAMPARAMETERS4DVNVPROC)(GLenum target, GLuint index, GLsizei count, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBL4DVPROC)(GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLGETTEXPARAMETERIVPROC)(GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBPOINTERVPROC)(GLuint index, GLenum pname, void **pointer);
typedef void (APIENTRYP PFNGLDRAWARRAYSINSTANCEDARBPROC)(GLenum mode, GLint first, GLsizei count, GLsizei primcount);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3I64VNVPROC)(GLuint program, GLint location, GLsizei count, const GLint64EXT *value);
typedef void (APIENTRYP PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC)(GLuint xfb, GLuint index, GLuint buffer);
typedef void (APIENTRYP PFNGLDISABLEPROC)(GLenum cap);
typedef GLboolean (APIENTRYP PFNGLISPOINTINFILLPATHNVPROC)(GLuint path, GLuint mask, GLfloat x, GLfloat y);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI2UIEXTPROC)(GLuint index, GLuint x, GLuint y);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4UIVPROC)(GLuint program, GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void (APIENTRYP PFNGLBINDBUFFEROFFSETNVPROC)(GLenum target, GLuint index, GLuint buffer, GLintptr offset);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIVPROC)(GLuint index, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI2UIVPROC)(GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLUNIFORM2IVARBPROC)(GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLBINDBUFFERSBASEPROC)(GLenum target, GLuint first, GLsizei count, const GLuint *buffers);
typedef void (APIENTRYP PFNGLTEXTURESUBIMAGE2DPROC)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
typedef GLfloat (APIENTRYP PFNGLGETPATHLENGTHNVPROC)(GLuint path, GLsizei startSegment, GLsizei numSegments);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC)(GLuint framebuffer, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLTEXTUREBUFFERPROC)(GLuint texture, GLenum internalformat, GLuint buffer);
typedef void (APIENTRYP PFNGLCLIPPLANEFOESPROC)(GLenum plane, const GLfloat *equation);
typedef void (APIENTRYP PFNGLUNIFORM4I64NVPROC)(GLint location, GLint64EXT x, GLint64EXT y, GLint64EXT z, GLint64EXT w);
typedef void (APIENTRYP PFNGLUNIFORMHANDLEUI64VARBPROC)(GLint location, GLsizei count, const GLuint64 *value);
typedef GLboolean (APIENTRYP PFNGLUNMAPNAMEDBUFFERPROC)(GLuint buffer);
typedef void (APIENTRYP PFNGLCREATESTATESNVPROC)(GLsizei n, GLuint *states);
typedef void (APIENTRYP PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC)(GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLCLEARNAMEDBUFFERDATAPROC)(GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void *data);
typedef void (APIENTRYP PFNGLSTENCILFILLPATHINSTANCEDNVPROC)(GLsizei numPaths, GLenum pathNameType, const void *paths, GLuint pathBase, GLenum fillMode, GLuint mask, GLenum transformType, const GLfloat *transformValues);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4I64NVPROC)(GLuint program, GLint location, GLint64EXT x, GLint64EXT y, GLint64EXT z, GLint64EXT w);
typedef void (APIENTRYP PFNGLTEXTUREPAGECOMMITMENTEXTPROC)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLboolean commit);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUIVARBPROC)(GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLBINDMULTITEXTUREEXTPROC)(GLenum texunit, GLenum target, GLuint texture);
typedef void (APIENTRYP PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount, GLuint baseinstance);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3FPROC)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3DPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRYP PFNGLBLENDCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (APIENTRYP PFNGLBINDTRANSFORMFEEDBACKNVPROC)(GLenum target, GLuint id);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIUIVPROC)(GLuint sampler, GLenum pname, const GLuint *param);
typedef GLenum (APIENTRYP PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC)(GLuint framebuffer, GLenum target);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3SPROC)(GLuint index, GLshort x, GLshort y, GLshort z);
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDEXTPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei primcount);
typedef void (APIENTRYP PFNGLNAMEDPROGRAMLOCALPARAMETERI4IEXTPROC)(GLuint program, GLenum target, GLuint index, GLint x, GLint y, GLint z, GLint w);
typedef void (APIENTRYP PFNGLDISABLEVERTEXARRAYEXTPROC)(GLuint vaobj, GLenum array);
typedef GLenum (APIENTRYP PFNGLGETGRAPHICSRESETSTATUSARBPROC)();
typedef void (APIENTRYP PFNGLUNIFORM4IPROC)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC)(GLenum texture);
typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (APIENTRYP PFNGLUNIFORM4DPROC)(GLint location, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRYP PFNGLUNIFORM4FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (APIENTRYP PFNGLCREATEPROGRAMPIPELINESPROC)(GLsizei n, GLuint *pipelines);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4SVPROC)(GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBLPOINTERPROC)(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex);
typedef void (APIENTRYP PFNGLDRAWTRANSFORMFEEDBACKNVPROC)(GLenum mode, GLuint id);
typedef void (APIENTRYP PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLECOVERAGEEXTPROC)(GLuint renderbuffer, GLsizei coverageSamples, GLsizei colorSamples, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLPOPDEBUGGROUPPROC)();
typedef void (APIENTRYP PFNGLUNIFORMBLOCKBINDINGPROC)(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
typedef void (APIENTRYP PFNGLGENERATETEXTUREMIPMAPEXTPROC)(GLuint texture, GLenum target);
typedef void (APIENTRYP PFNGLVERTEXATTRIBIFORMATNVPROC)(GLuint index, GLint size, GLenum type, GLsizei stride);
typedef void (APIENTRYP PFNGLPATHGLYPHRANGENVPROC)(GLuint firstPathName, GLenum fontTarget, const void *fontName, GLbitfield fontStyle, GLuint firstGlyph, GLsizei numGlyphs, GLenum handleMissingGlyphs, GLuint pathParameterTemplate, GLfloat emScale);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLUNIFORM2FVARBPROC)(GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4I64ARBPROC)(GLuint program, GLint location, GLint64 x, GLint64 y, GLint64 z, GLint64 w);
typedef void (APIENTRYP PFNGLGETQUERYINDEXEDIVPROC)(GLenum target, GLuint index, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
typedef GLboolean (APIENTRYP PFNGLISCOMMANDLISTNVPROC)(GLuint list);
typedef void (APIENTRYP PFNGLOBJECTLABELPROC)(GLenum identifier, GLuint name, GLsizei length, const GLchar *label);
typedef void (APIENTRYP PFNGLDELETETRANSFORMFEEDBACKSNVPROC)(GLsizei n, const GLuint *ids);
typedef void (APIENTRYP PFNGLVERTEXATTRIBLPOINTEREXTPROC)(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
typedef void (APIENTRYP PFNGLSTENCILFUNCSEPARATEPROC)(GLenum face, GLenum func, GLint ref, GLuint mask);
typedef void (APIENTRYP PFNGLCOMPILESHADERARBPROC)(GLhandleARB shaderObj);
typedef void (APIENTRYP PFNGLNAMEDBUFFERSUBDATAEXTPROC)(GLuint buffer, GLintptr offset, GLsizeiptr size, const void *data);
typedef void (APIENTRYP PFNGLTEXBUFFEREXTPROC)(GLenum target, GLenum internalformat, GLuint buffer);
typedef GLuint (APIENTRYP PFNGLCREATEPROGRAMPROC)();
typedef void (APIENTRYP PFNGLBLENDEQUATIONSEPARATEEXTPROC)(GLenum modeRGB, GLenum modeAlpha);
typedef void (APIENTRYP PFNGLPRIMITIVERESTARTINDEXPROC)(GLuint index);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2DNVPROC)(GLuint index, GLdouble x, GLdouble y);
typedef void (APIENTRYP PFNGLBINDIMAGETEXTURESPROC)(GLuint first, GLsizei count, const GLuint *textures);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2DARBPROC)(GLenum target, GLdouble s, GLdouble t);
typedef void (APIENTRYP PFNGLPATHSTRINGNVPROC)(GLuint path, GLenum format, GLsizei length, const void *pathString);
typedef void (APIENTRYP PFNGLGETMULTITEXPARAMETERIIVEXTPROC)(GLenum texunit, GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVEXTPROC)(GLuint framebuffer, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLARRAYELEMENTEXTPROC)(GLint i);
typedef void (APIENTRYP PFNGLUNIFORM4IVPROC)(GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLPATHDASHARRAYNVPROC)(GLuint path, GLsizei dashCount, const GLfloat *dashArray);
typedef void (APIENTRYP PFNGLUNIFORM3UIVPROC)(GLint location, GLsizei count, const GLuint *value);
typedef void (APIENTRYP PFNGLCLEARCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef GLuint64 (APIENTRYP PFNGLGETIMAGEHANDLEARBPROC)(GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum format);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X4DVEXTPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
typedef void (APIENTRYP PFNGLGETQUERYOBJECTIVARBPROC)(GLuint id, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3SNVPROC)(GLuint index, GLshort x, GLshort y, GLshort z);
typedef void (APIENTRYP PFNGLGETNUNIFORMIVPROC)(GLuint program, GLint location, GLsizei bufSize, GLint *params);
typedef void (APIENTRYP PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC)(GLuint framebuffer, GLenum pname, GLint *param);
typedef void (APIENTRYP PFNGLUNIFORM3UIPROC)(GLint location, GLuint v0, GLuint v1, GLuint v2);
typedef void (APIENTRYP PFNGLCLEARCOLORIUIEXTPROC)(GLuint red, GLuint green, GLuint blue, GLuint alpha);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI4UIVPROC)(GLuint index, const GLuint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4BVPROC)(GLuint index, const GLbyte *v);
typedef GLuint (APIENTRYP PFNGLGETPROGRAMRESOURCEINDEXPROC)(GLuint program, GLenum programInterface, const GLchar *name);
typedef void (APIENTRYP PFNGLDRAWELEMENTSINDIRECTPROC)(GLenum mode, GLenum type, const void *indirect);
typedef GLint (APIENTRYP PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC)(GLuint program, GLenum shadertype, const GLchar *name);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1DARBPROC)(GLuint index, GLdouble x);
typedef void (APIENTRYP PFNGLCOMPILECOMMANDLISTNVPROC)(GLuint list);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1FEXTPROC)(GLuint program, GLint location, GLfloat v0);
typedef void (APIENTRYP PFNGLGETQUERYBUFFEROBJECTUI64VPROC)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3SARBPROC)(GLuint index, GLshort x, GLshort y, GLshort z);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2X3FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2I64VARBPROC)(GLuint program, GLint location, GLsizei count, const GLint64 *value);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIUIVPROC)(GLuint index, GLenum pname, GLuint *params);
typedef void (APIENTRYP PFNGLGETTEXPARAMETERIUIVEXTPROC)(GLenum target, GLenum pname, GLuint *params);
typedef void (APIENTRYP PFNGLVERTEXARRAYSECONDARYCOLOROFFSETEXTPROC)(GLuint vaobj, GLuint buffer, GLint size, GLenum type, GLsizei stride, GLintptr offset);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);
typedef void (APIENTRYP PFNGLPROGRAMBINARYPROC)(GLuint program, GLenum binaryFormat, const void *binary, GLsizei length);
typedef void (APIENTRYP PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC)(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
typedef void (APIENTRYP PFNGLGETTEXPARAMETERFVPROC)(GLenum target, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLTRANSFORMFEEDBACKSTREAMATTRIBSNVPROC)(GLsizei count, const GLint *attribs, GLsizei nbuffers, const GLint *bufstreams, GLenum bufferMode);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1FPROC)(GLuint index, GLfloat x);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4SARBPROC)(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
typedef void (APIENTRYP PFNGLGETVERTEXARRAYINDEXED64IVPROC)(GLuint vaobj, GLuint index, GLenum pname, GLint64 *param);
typedef void (APIENTRYP PFNGLMATRIXMULTTRANSPOSE3X3FNVPROC)(GLenum matrixMode, const GLfloat *m);
typedef void (APIENTRYP PFNGLTEXPARAMETERIIVPROC)(GLenum target, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIVPROC)(GLuint sampler, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLVERTEXARRAYATTRIBFORMATPROC)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
typedef void (APIENTRYP PFNGLENDTRANSFORMFEEDBACKPROC)();
typedef void (APIENTRYP PFNGLDRAWCOMMANDSSTATESADDRESSNVPROC)(const GLuint64 *indirects, const GLsizei *sizes, const GLuint *states, const GLuint *fbos, GLuint count);
typedef void (APIENTRYP PFNGLUNIFORM4UI64NVPROC)(GLint location, GLuint64EXT x, GLuint64EXT y, GLuint64EXT z, GLuint64EXT w);
typedef void (APIENTRYP PFNGLTEXIMAGE2DMULTISAMPLEPROC)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
typedef void (APIENTRYP PFNGLBINDTEXTUREEXTPROC)(GLenum target, GLuint texture);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2DVPROC)(GLuint program, GLint location, GLsizei count, const GLdouble *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI1IVPROC)(GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBDIVISORPROC)(GLuint index, GLuint divisor);
typedef void (APIENTRYP PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC)(GLuint texture, GLint level, GLsizei bufSize, void *pixels);
typedef void (APIENTRYP PFNGLMATRIXROTATEFEXTPROC)(GLenum mode, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRYP PFNGLACTIVETEXTUREARBPROC)(GLenum texture);
typedef void (APIENTRYP PFNGLGETUNIFORMUI64VNVPROC)(GLuint program, GLint location, GLuint64EXT *params);
typedef void (APIENTRYP PFNGLTEXPARAMETERIIVEXTPROC)(GLenum target, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X2FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLGETUNIFORMI64VARBPROC)(GLuint program, GLint location, GLint64 *params);
typedef void (APIENTRYP PFNGLBINDVERTEXBUFFERSPROC)(GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizei *strides);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2FPROC)(GLuint index, GLfloat x, GLfloat y);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NSVPROC)(GLuint index, const GLshort *v);
typedef void (APIENTRYP PFNGLINVALIDATEBUFFERSUBDATAPROC)(GLuint buffer, GLintptr offset, GLsizeiptr length);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3DVPROC)(GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLNAMEDFRAMEBUFFERTEXTURE1DEXTPROC)(GLuint framebuffer, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void * (APIENTRYP PFNGLMAPBUFFERRANGEPROC)(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLGETTEXTUREPARAMETERIIVEXTPROC)(GLuint texture, GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3DVARBPROC)(GLuint index, const GLdouble *v);
typedef void (APIENTRYP PFNGLNAMEDPROGRAMLOCALPARAMETERSI4UIVEXTPROC)(GLuint program, GLenum target, GLuint index, GLsizei count, const GLuint *params);
typedef void (APIENTRYP PFNGLGENPROGRAMSARBPROC)(GLsizei n, GLuint *programs);
typedef void (APIENTRYP PFNGLGENTRANSFORMFEEDBACKSNVPROC)(GLsizei n, GLuint *ids);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI2IVPROC)(GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLCOPYMULTITEXIMAGE2DEXTPROC)(GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4UBNVPROC)(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
typedef void (APIENTRYP PFNGLGETSHADERPRECISIONFORMATPROC)(GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision);
typedef void (APIENTRYP PFNGLTEXTUREVIEWPROC)(GLuint texture, GLenum target, GLuint origtexture, GLenum internalformat, GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers);
typedef GLint (APIENTRYP PFNGLGETFRAGDATALOCATIONEXTPROC)(GLuint program, const GLchar *name);
typedef void (APIENTRYP PFNGLGETCOMPRESSEDMULTITEXIMAGEEXTPROC)(GLenum texunit, GLenum target, GLint lod, void *img);
typedef void (APIENTRYP PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void (APIENTRYP PFNGLDELETERENDERBUFFERSPROC)(GLsizei n, const GLuint *renderbuffers);
typedef void (APIENTRYP PFNGLGENTEXTURESEXTPROC)(GLsizei n, GLuint *textures);
typedef void (APIENTRYP PFNGLCOPYTEXIMAGE2DEXTPROC)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
typedef void (APIENTRYP PFNGLGETNAMEDBUFFERPARAMETERI64VPROC)(GLuint buffer, GLenum pname, GLint64 *params);
typedef void (APIENTRYP PFNGLTEXTUREPARAMETERIIVEXTPROC)(GLuint texture, GLenum target, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMUI64VNVPROC)(GLuint program, GLint location, GLsizei count, const GLuint64EXT *value);
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2UI64ARBPROC)(GLuint program, GLint location, GLuint64 x, GLuint64 y);
typedef void (APIENTRYP PFNGLUNIFORM2UI64NVPROC)(GLint location, GLuint64EXT x, GLuint64EXT y);
typedef void (APIENTRYP PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (APIENTRYP PFNGLTEXTUREIMAGE1DEXTPROC)(GLuint texture, GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRYP PFNGLATTACHOBJECTARBPROC)(GLhandleARB containerObj, GLhandleARB obj);
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE1DEXTPROC)(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3FVARBPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP PFNGLMAKEBUFFERRESIDENTNVPROC)(GLenum target, GLenum access);
typedef void (APIENTRYP PFNGLPOINTPARAMETERFVEXTPROC)(GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLUNIFORM4FARBPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (APIENTRYP PFNGLCOPYTEXTURESUBIMAGE3DPROC)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLMULTITEXENVFVEXTPROC)(GLenum texunit, GLenum target, GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIBI1IPROC)(GLuint index, GLint x);
typedef GLuint (APIENTRYP PFNGLGETDEBUGMESSAGELOGARBPROC)(GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog);
typedef void (APIENTRYP PFNGLNAMEDPROGRAMLOCALPARAMETERS4FVEXTPROC)(GLuint program, GLenum target, GLuint index, GLsizei count, const GLfloat *params);
typedef void (APIENTRYP PFNGLGETBUFFERPOINTERVPROC)(GLenum target, GLenum pname, void **params);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE3DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERFVPROC)(GLuint sampler, GLenum pname, const GLfloat *param);
typedef void (APIENTRYP PFNGLBINDBUFFERARBPROC)(GLenum target, GLuint buffer);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1SNVPROC)(GLuint index, GLshort x);
typedef void (APIENTRYP PFNGLUNIFORM1FVPROC)(GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRYP PFNGLSCISSORINDEXEDVPROC)(GLuint index, const GLint *v);
typedef void (APIENTRYP PFNGLUNIFORM1IVARBPROC)(GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4DVARBPROC)(GLenum target, const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2DPROC)(GLuint index, GLdouble x, GLdouble y);
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSEXTPROC)(GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei primcount);
typedef void (APIENTRYP PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2SPROC)(GLuint index, GLshort x, GLshort y);
typedef void (APIENTRYP PFNGLTEXIMAGE3DMULTISAMPLEPROC)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
typedef GLuint (APIENTRYP PFNGLGETUNIFORMBLOCKINDEXPROC)(GLuint program, const GLchar *uniformBlockName);
typedef void (APIENTRYP PFNGLFRONTFACEPROC)(GLenum mode);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4DARBPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef GLboolean (APIENTRYP PFNGLISPOINTINSTROKEPATHNVPROC)(GLuint path, GLfloat x, GLfloat y);

GLAPI PFNGLMULTIDRAWARRAYSINDIRECTBINDLESSCOUNTNVPROC glad_glMultiDrawArraysIndirectBindlessCountNV;
#define glMultiDrawArraysIndirectBindlessCountNV glad_glMultiDrawArraysIndirectBindlessCountNV
GLAPI PFNGLCOPYTEXIMAGE1DPROC glad_glCopyTexImage1D;
#define glCopyTexImage1D glad_glCopyTexImage1D
GLAPI PFNGLPROGRAMUNIFORM1I64ARBPROC glad_glProgramUniform1i64ARB;
#define glProgramUniform1i64ARB glad_glProgramUniform1i64ARB
GLAPI PFNGLSTENCILMASKSEPARATEPROC glad_glStencilMaskSeparate;
#define glStencilMaskSeparate glad_glStencilMaskSeparate
GLAPI PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glad_glGetFramebufferAttachmentParameterivEXT;
#define glGetFramebufferAttachmentParameterivEXT glad_glGetFramebufferAttachmentParameterivEXT
GLAPI PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC glad_glTextureStorage3DMultisample;
#define glTextureStorage3DMultisample glad_glTextureStorage3DMultisample
GLAPI PFNGLGETINTEGERINDEXEDVEXTPROC glad_glGetIntegerIndexedvEXT;
#define glGetIntegerIndexedvEXT glad_glGetIntegerIndexedvEXT
GLAPI PFNGLTEXTUREPARAMETERFVPROC glad_glTextureParameterfv;
#define glTextureParameterfv glad_glTextureParameterfv
GLAPI PFNGLPROGRAMUNIFORM4UI64ARBPROC glad_glProgramUniform4ui64ARB;
#define glProgramUniform4ui64ARB glad_glProgramUniform4ui64ARB
GLAPI PFNGLMULTITEXCOORD2SARBPROC glad_glMultiTexCoord2sARB;
#define glMultiTexCoord2sARB glad_glMultiTexCoord2sARB
GLAPI PFNGLPROGRAMENVPARAMETER4FARBPROC glad_glProgramEnvParameter4fARB;
#define glProgramEnvParameter4fARB glad_glProgramEnvParameter4fARB
GLAPI PFNGLCOPYTEXTUREIMAGE1DEXTPROC glad_glCopyTextureImage1DEXT;
#define glCopyTextureImage1DEXT glad_glCopyTextureImage1DEXT
GLAPI PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glad_glCompressedTexSubImage3D;
#define glCompressedTexSubImage3D glad_glCompressedTexSubImage3D
GLAPI PFNGLTEXTURESTORAGE3DEXTPROC glad_glTextureStorage3DEXT;
#define glTextureStorage3DEXT glad_glTextureStorage3DEXT
GLAPI PFNGLUNIFORM4UIEXTPROC glad_glUniform4uiEXT;
#define glUniform4uiEXT glad_glUniform4uiEXT
GLAPI PFNGLPROGRAMUNIFORM3UI64ARBPROC glad_glProgramUniform3ui64ARB;
#define glProgramUniform3ui64ARB glad_glProgramUniform3ui64ARB
GLAPI PFNGLVERTEXARRAYBINDVERTEXBUFFEREXTPROC glad_glVertexArrayBindVertexBufferEXT;
#define glVertexArrayBindVertexBufferEXT glad_glVertexArrayBindVertexBufferEXT
GLAPI PFNGLVERTEXARRAYELEMENTBUFFERPROC glad_glVertexArrayElementBuffer;
#define glVertexArrayElementBuffer glad_glVertexArrayElementBuffer
GLAPI PFNGLVERTEXATTRIBI3UIVEXTPROC glad_glVertexAttribI3uivEXT;
#define glVertexAttribI3uivEXT glad_glVertexAttribI3uivEXT
GLAPI PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC glad_glTransformFeedbackBufferRange;
#define glTransformFeedbackBufferRange glad_glTransformFeedbackBufferRange
GLAPI PFNGLPATHGLYPHSNVPROC glad_glPathGlyphsNV;
#define glPathGlyphsNV glad_glPathGlyphsNV
GLAPI PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC glad_glDrawElementsInstancedBaseInstance;
#define glDrawElementsInstancedBaseInstance glad_glDrawElementsInstancedBaseInstance
GLAPI PFNGLGETTEXTUREPARAMETERIVEXTPROC glad_glGetTextureParameterivEXT;
#define glGetTextureParameterivEXT glad_glGetTextureParameterivEXT
GLAPI PFNGLVERTEXATTRIBP4UIPROC glad_glVertexAttribP4ui;
#define glVertexAttribP4ui glad_glVertexAttribP4ui
GLAPI PFNGLISBUFFERPROC glad_glIsBuffer;
#define glIsBuffer glad_glIsBuffer
GLAPI PFNGLGETMULTISAMPLEFVPROC glad_glGetMultisamplefv;
#define glGetMultisamplefv glad_glGetMultisamplefv
GLAPI PFNGLCOMPRESSEDMULTITEXSUBIMAGE3DEXTPROC glad_glCompressedMultiTexSubImage3DEXT;
#define glCompressedMultiTexSubImage3DEXT glad_glCompressedMultiTexSubImage3DEXT
GLAPI PFNGLMINSAMPLESHADINGPROC glad_glMinSampleShading;
#define glMinSampleShading glad_glMinSampleShading
GLAPI PFNGLCOVERSTROKEPATHNVPROC glad_glCoverStrokePathNV;
#define glCoverStrokePathNV glad_glCoverStrokePathNV
GLAPI PFNGLDEBUGMESSAGECONTROLPROC glad_glDebugMessageControl;
#define glDebugMessageControl glad_glDebugMessageControl
GLAPI PFNGLPROGRAMUNIFORM4IEXTPROC glad_glProgramUniform4iEXT;
#define glProgramUniform4iEXT glad_glProgramUniform4iEXT
GLAPI PFNGLVERTEXATTRIB2SVARBPROC glad_glVertexAttrib2svARB;
#define glVertexAttrib2svARB glad_glVertexAttrib2svARB
GLAPI PFNGLVERTEXATTRIB4UBVPROC glad_glVertexAttrib4ubv;
#define glVertexAttrib4ubv glad_glVertexAttrib4ubv
GLAPI PFNGLVERTEXATTRIB4NUBVARBPROC glad_glVertexAttrib4NubvARB;
#define glVertexAttrib4NubvARB glad_glVertexAttrib4NubvARB
GLAPI PFNGLMATRIXPUSHEXTPROC glad_glMatrixPushEXT;
#define glMatrixPushEXT glad_glMatrixPushEXT
GLAPI PFNGLPROGRAMUNIFORM2FEXTPROC glad_glProgramUniform2fEXT;
#define glProgramUniform2fEXT glad_glProgramUniform2fEXT
GLAPI PFNGLMULTIDRAWELEMENTSINDIRECTBINDLESSNVPROC glad_glMultiDrawElementsIndirectBindlessNV;
#define glMultiDrawElementsIndirectBindlessNV glad_glMultiDrawElementsIndirectBindlessNV
GLAPI PFNGLUNIFORMMATRIX3DVPROC glad_glUniformMatrix3dv;
#define glUniformMatrix3dv glad_glUniformMatrix3dv
GLAPI PFNGLGETVERTEXATTRIBLDVEXTPROC glad_glGetVertexAttribLdvEXT;
#define glGetVertexAttribLdvEXT glad_glGetVertexAttribLdvEXT
GLAPI PFNGLGETVERTEXARRAYINDEXEDIVPROC glad_glGetVertexArrayIndexediv;
#define glGetVertexArrayIndexediv glad_glGetVertexArrayIndexediv
GLAPI PFNGLRESUMETRANSFORMFEEDBACKPROC glad_glResumeTransformFeedback;
#define glResumeTransformFeedback glad_glResumeTransformFeedback
GLAPI PFNGLGETMULTITEXPARAMETERIVEXTPROC glad_glGetMultiTexParameterivEXT;
#define glGetMultiTexParameterivEXT glad_glGetMultiTexParameterivEXT
GLAPI PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC glad_glProgramUniformMatrix3x4fvEXT;
#define glProgramUniformMatrix3x4fvEXT glad_glProgramUniformMatrix3x4fvEXT
GLAPI PFNGLVERTEXATTRIBI2IPROC glad_glVertexAttribI2i;
#define glVertexAttribI2i glad_glVertexAttribI2i
GLAPI PFNGLBINDPROGRAMNVPROC glad_glBindProgramNV;
#define glBindProgramNV glad_glBindProgramNV
GLAPI PFNGLLOADPROGRAMNVPROC glad_glLoadProgramNV;
#define glLoadProgramNV glad_glLoadProgramNV
GLAPI PFNGLGETGRAPHICSRESETSTATUSPROC glad_glGetGraphicsResetStatus;
#define glGetGraphicsResetStatus glad_glGetGraphicsResetStatus
GLAPI PFNGLVERTEXATTRIB1FVPROC glad_glVertexAttrib1fv;
#define glVertexAttrib1fv glad_glVertexAttrib1fv
GLAPI PFNGLISENABLEDPROC glad_glIsEnabled;
#define glIsEnabled glad_glIsEnabled
GLAPI PFNGLSTENCILOPPROC glad_glStencilOp;
#define glStencilOp glad_glStencilOp
GLAPI PFNGLFRAMEBUFFERTEXTURE2DPROC glad_glFramebufferTexture2D;
#define glFramebufferTexture2D glad_glFramebufferTexture2D
GLAPI PFNGLVERTEXARRAYATTRIBIFORMATPROC glad_glVertexArrayAttribIFormat;
#define glVertexArrayAttribIFormat glad_glVertexArrayAttribIFormat
GLAPI PFNGLGENFRAMEBUFFERSPROC glad_glGenFramebuffers;
#define glGenFramebuffers glad_glGenFramebuffers
GLAPI PFNGLTEXSUBIMAGE3DEXTPROC glad_glTexSubImage3DEXT;
#define glTexSubImage3DEXT glad_glTexSubImage3DEXT
GLAPI PFNGLGETATTACHEDSHADERSPROC glad_glGetAttachedShaders;
#define glGetAttachedShaders glad_glGetAttachedShaders
GLAPI PFNGLDELETEVERTEXARRAYSPROC glad_glDeleteVertexArrays;
#define glDeleteVertexArrays glad_glDeleteVertexArrays
GLAPI PFNGLVIEWPORTARRAYVPROC glad_glViewportArrayv;
#define glViewportArrayv glad_glViewportArrayv
GLAPI PFNGLMULTITEXCOORD3DVARBPROC glad_glMultiTexCoord3dvARB;
#define glMultiTexCoord3dvARB glad_glMultiTexCoord3dvARB
GLAPI PFNGLUNIFORM3UI64ARBPROC glad_glUniform3ui64ARB;
#define glUniform3ui64ARB glad_glUniform3ui64ARB
GLAPI PFNGLVERTEXATTRIB2FVNVPROC glad_glVertexAttrib2fvNV;
#define glVertexAttrib2fvNV glad_glVertexAttrib2fvNV
GLAPI PFNGLGETNAMEDBUFFERSUBDATAEXTPROC glad_glGetNamedBufferSubDataEXT;
#define glGetNamedBufferSubDataEXT glad_glGetNamedBufferSubDataEXT
GLAPI PFNGLMEMORYBARRIERBYREGIONPROC glad_glMemoryBarrierByRegion;
#define glMemoryBarrierByRegion glad_glMemoryBarrierByRegion
GLAPI PFNGLMATRIXROTATEDEXTPROC glad_glMatrixRotatedEXT;
#define glMatrixRotatedEXT glad_glMatrixRotatedEXT
GLAPI PFNGLISPROGRAMNVPROC glad_glIsProgramNV;
#define glIsProgramNV glad_glIsProgramNV
GLAPI PFNGLUNIFORM2DVPROC glad_glUniform2dv;
#define glUniform2dv glad_glUniform2dv
GLAPI PFNGLMULTITEXPARAMETERIEXTPROC glad_glMultiTexParameteriEXT;
#define glMultiTexParameteriEXT glad_glMultiTexParameteriEXT
GLAPI PFNGLGETPROGRAMSTRINGNVPROC glad_glGetProgramStringNV;
#define glGetProgramStringNV glad_glGetProgramStringNV
GLAPI PFNGLGETUNIFORMFVPROC glad_glGetUniformfv;
#define glGetUniformfv glad_glGetUniformfv
GLAPI PFNGLGETUNIFORMUIVPROC glad_glGetUniformuiv;
#define glGetUniformuiv glad_glGetUniformuiv
GLAPI PFNGLGETNUNIFORMFVARBPROC glad_glGetnUniformfvARB;
#define glGetnUniformfvARB glad_glGetnUniformfvARB
GLAPI PFNGLVERTEXARRAYBINDINGDIVISORPROC glad_glVertexArrayBindingDivisor;
#define glVertexArrayBindingDivisor glad_glVertexArrayBindingDivisor
GLAPI PFNGLPROGRAMUNIFORM1DVEXTPROC glad_glProgramUniform1dvEXT;
#define glProgramUniform1dvEXT glad_glProgramUniform1dvEXT
GLAPI PFNGLMULTITEXCOORD4IARBPROC glad_glMultiTexCoord4iARB;
#define glMultiTexCoord4iARB glad_glMultiTexCoord4iARB
GLAPI PFNGLGETRENDERBUFFERPARAMETERIVPROC glad_glGetRenderbufferParameteriv;
#define glGetRenderbufferParameteriv glad_glGetRenderbufferParameteriv
GLAPI PFNGLPROGRAMUNIFORM3FEXTPROC glad_glProgramUniform3fEXT;
#define glProgramUniform3fEXT glad_glProgramUniform3fEXT
GLAPI PFNGLPROGRAMPATHFRAGMENTINPUTGENNVPROC glad_glProgramPathFragmentInputGenNV;
#define glProgramPathFragmentInputGenNV glad_glProgramPathFragmentInputGenNV
GLAPI PFNGLFENCESYNCPROC glad_glFenceSync;
#define glFenceSync glad_glFenceSync
GLAPI PFNGLGETVERTEXATTRIBIIVEXTPROC glad_glGetVertexAttribIivEXT;
#define glGetVertexAttribIivEXT glad_glGetVertexAttribIivEXT
GLAPI PFNGLBINDRENDERBUFFEREXTPROC glad_glBindRenderbufferEXT;
#define glBindRenderbufferEXT glad_glBindRenderbufferEXT
GLAPI PFNGLFRAMEBUFFERDRAWBUFFERSEXTPROC glad_glFramebufferDrawBuffersEXT;
#define glFramebufferDrawBuffersEXT glad_glFramebufferDrawBuffersEXT
GLAPI PFNGLVERTEXATTRIBS4FVNVPROC glad_glVertexAttribs4fvNV;
#define glVertexAttribs4fvNV glad_glVertexAttribs4fvNV
GLAPI PFNGLPAUSETRANSFORMFEEDBACKNVPROC glad_glPauseTransformFeedbackNV;
#define glPauseTransformFeedbackNV glad_glPauseTransformFeedbackNV
GLAPI PFNGLGETQUERYOBJECTI64VEXTPROC glad_glGetQueryObjecti64vEXT;
#define glGetQueryObjecti64vEXT glad_glGetQueryObjecti64vEXT
GLAPI PFNGLPROGRAMUNIFORM4UIEXTPROC glad_glProgramUniform4uiEXT;
#define glProgramUniform4uiEXT glad_glProgramUniform4uiEXT
GLAPI PFNGLVALIDATEPROGRAMPIPELINEPROC glad_glValidateProgramPipeline;
#define glValidateProgramPipeline glad_glValidateProgramPipeline
GLAPI PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC glad_glStencilStrokePathInstancedNV;
#define glStencilStrokePathInstancedNV glad_glStencilStrokePathInstancedNV
GLAPI PFNGLGENQUERIESARBPROC glad_glGenQueriesARB;
#define glGenQueriesARB glad_glGenQueriesARB
GLAPI PFNGLPROGRAMUNIFORM2I64NVPROC glad_glProgramUniform2i64NV;
#define glProgramUniform2i64NV glad_glProgramUniform2i64NV
GLAPI PFNGLGENSAMPLERSPROC glad_glGenSamplers;
#define glGenSamplers glad_glGenSamplers
GLAPI PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC glad_glDrawTransformFeedbackInstanced;
#define glDrawTransformFeedbackInstanced glad_glDrawTransformFeedbackInstanced
GLAPI PFNGLSTENCILTHENCOVERSTROKEPATHNVPROC glad_glStencilThenCoverStrokePathNV;
#define glStencilThenCoverStrokePathNV glad_glStencilThenCoverStrokePathNV
GLAPI PFNGLDELETEVERTEXARRAYSAPPLEPROC glad_glDeleteVertexArraysAPPLE;
#define glDeleteVertexArraysAPPLE glad_glDeleteVertexArraysAPPLE
GLAPI PFNGLGETNAMEDBUFFERPARAMETERUI64VNVPROC glad_glGetNamedBufferParameterui64vNV;
#define glGetNamedBufferParameterui64vNV glad_glGetNamedBufferParameterui64vNV
GLAPI PFNGLDRAWTRANSFORMFEEDBACKPROC glad_glDrawTransformFeedback;
#define glDrawTransformFeedback glad_glDrawTransformFeedback
GLAPI PFNGLGETTEXPARAMETERIUIVPROC glad_glGetTexParameterIuiv;
#define glGetTexParameterIuiv glad_glGetTexParameterIuiv
GLAPI PFNGLVERTEXATTRIB4NBVPROC glad_glVertexAttrib4Nbv;
#define glVertexAttrib4Nbv glad_glVertexAttrib4Nbv
GLAPI PFNGLVERTEXATTRIBL4DEXTPROC glad_glVertexAttribL4dEXT;
#define glVertexAttribL4dEXT glad_glVertexAttribL4dEXT
GLAPI PFNGLUNIFORMUI64VNVPROC glad_glUniformui64vNV;
#define glUniformui64vNV glad_glUniformui64vNV
GLAPI PFNGLISSYNCPROC glad_glIsSync;
#define glIsSync glad_glIsSync
GLAPI PFNGLGETOBJECTPTRLABELPROC glad_glGetObjectPtrLabel;
#define glGetObjectPtrLabel glad_glGetObjectPtrLabel
GLAPI PFNGLTEXTUREPARAMETERIPROC glad_glTextureParameteri;
#define glTextureParameteri glad_glTextureParameteri
GLAPI PFNGLTEXTUREPARAMETERFPROC glad_glTextureParameterf;
#define glTextureParameterf glad_glTextureParameterf
GLAPI PFNGLUNIFORMMATRIX2X3DVPROC glad_glUniformMatrix2x3dv;
#define glUniformMatrix2x3dv glad_glUniformMatrix2x3dv
GLAPI PFNGLFRAMEBUFFERDRAWBUFFEREXTPROC glad_glFramebufferDrawBufferEXT;
#define glFramebufferDrawBufferEXT glad_glFramebufferDrawBufferEXT
GLAPI PFNGLVERTEXATTRIBL3DPROC glad_glVertexAttribL3d;
#define glVertexAttribL3d glad_glVertexAttribL3d
GLAPI PFNGLCOMPRESSEDTEXTUREIMAGE1DEXTPROC glad_glCompressedTextureImage1DEXT;
#define glCompressedTextureImage1DEXT glad_glCompressedTextureImage1DEXT
GLAPI PFNGLMATRIXLOADTRANSPOSE3X3FNVPROC glad_glMatrixLoadTranspose3x3fNV;
#define glMatrixLoadTranspose3x3fNV glad_glMatrixLoadTranspose3x3fNV
GLAPI PFNGLPROGRAMUNIFORM1UIPROC glad_glProgramUniform1ui;
#define glProgramUniform1ui glad_glProgramUniform1ui
GLAPI PFNGLCOPYTEXIMAGE2DPROC glad_glCopyTexImage2D;
#define glCopyTexImage2D glad_glCopyTexImage2D
GLAPI PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC glad_glProgramUniformMatrix2x4fvEXT;
#define glProgramUniformMatrix2x4fvEXT glad_glProgramUniformMatrix2x4fvEXT
GLAPI PFNGLCOPYTEXSUBIMAGE3DEXTPROC glad_glCopyTexSubImage3DEXT;
#define glCopyTexSubImage3DEXT glad_glCopyTexSubImage3DEXT
GLAPI PFNGLRENDERBUFFERSTORAGEEXTPROC glad_glRenderbufferStorageEXT;
#define glRenderbufferStorageEXT glad_glRenderbufferStorageEXT
GLAPI PFNGLPATHPARAMETERIVNVPROC glad_glPathParameterivNV;
#define glPathParameterivNV glad_glPathParameterivNV
GLAPI PFNGLUNIFORM4UIVPROC glad_glUniform4uiv;
#define glUniform4uiv glad_glUniform4uiv
GLAPI PFNGLSCISSORARRAYVPROC glad_glScissorArrayv;
#define glScissorArrayv glad_glScissorArrayv
GLAPI PFNGLPOINTPARAMETERFVARBPROC glad_glPointParameterfvARB;
#define glPointParameterfvARB glad_glPointParameterfvARB
GLAPI PFNGLINVALIDATESUBFRAMEBUFFERPROC glad_glInvalidateSubFramebuffer;
#define glInvalidateSubFramebuffer glad_glInvalidateSubFramebuffer
GLAPI PFNGLMAPBUFFERPROC glad_glMapBuffer;
#define glMapBuffer glad_glMapBuffer
GLAPI PFNGLVERTEXATTRIBLFORMATPROC glad_glVertexAttribLFormat;
#define glVertexAttribLFormat glad_glVertexAttribLFormat
GLAPI PFNGLDELETESYNCPROC glad_glDeleteSync;
#define glDeleteSync glad_glDeleteSync
GLAPI PFNGLGETPROGRAMPARAMETERFVNVPROC glad_glGetProgramParameterfvNV;
#define glGetProgramParameterfvNV glad_glGetProgramParameterfvNV
GLAPI PFNGLVERTEXARRAYVERTEXATTRIBLOFFSETEXTPROC glad_glVertexArrayVertexAttribLOffsetEXT;
#define glVertexArrayVertexAttribLOffsetEXT glad_glVertexArrayVertexAttribLOffsetEXT
GLAPI PFNGLUNIFORM3IVPROC glad_glUniform3iv;
#define glUniform3iv glad_glUniform3iv
GLAPI PFNGLPROGRAMUNIFORM4IVPROC glad_glProgramUniform4iv;
#define glProgramUniform4iv glad_glProgramUniform4iv
GLAPI PFNGLVERTEXATTRIB1DVARBPROC glad_glVertexAttrib1dvARB;
#define glVertexAttrib1dvARB glad_glVertexAttrib1dvARB
GLAPI PFNGLTEXTURESUBIMAGE2DEXTPROC glad_glTextureSubImage2DEXT;
#define glTextureSubImage2DEXT glad_glTextureSubImage2DEXT
GLAPI PFNGLISVERTEXARRAYAPPLEPROC glad_glIsVertexArrayAPPLE;
#define glIsVertexArrayAPPLE glad_glIsVertexArrayAPPLE
GLAPI PFNGLUSEPROGRAMPROC glad_glUseProgram;
#define glUseProgram glad_glUseProgram
GLAPI PFNGLMULTITEXCOORD4FVARBPROC glad_glMultiTexCoord4fvARB;
#define glMultiTexCoord4fvARB glad_glMultiTexCoord4fvARB
GLAPI PFNGLUNIFORM3FVARBPROC glad_glUniform3fvARB;
#define glUniform3fvARB glad_glUniform3fvARB
GLAPI PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC glad_glProgramUniformMatrix3x2fvEXT;
#define glProgramUniformMatrix3x2fvEXT glad_glProgramUniformMatrix3x2fvEXT
GLAPI PFNGLDELETEBUFFERSPROC glad_glDeleteBuffers;
#define glDeleteBuffers glad_glDeleteBuffers
GLAPI PFNGLMULTITEXIMAGE1DEXTPROC glad_glMultiTexImage1DEXT;
#define glMultiTexImage1DEXT glad_glMultiTexImage1DEXT
GLAPI PFNGLDELETEOBJECTARBPROC glad_glDeleteObjectARB;
#define glDeleteObjectARB glad_glDeleteObjectARB
GLAPI PFNGLMULTITEXCOORD3DARBPROC glad_glMultiTexCoord3dARB;
#define glMultiTexCoord3dARB glad_glMultiTexCoord3dARB
GLAPI PFNGLSCISSORPROC glad_glScissor;
#define glScissor glad_glScissor
GLAPI PFNGLNAMEDFRAMEBUFFERTEXTUREEXTPROC glad_glNamedFramebufferTextureEXT;
#define glNamedFramebufferTextureEXT glad_glNamedFramebufferTextureEXT
GLAPI PFNGLTEXTUREPARAMETERIEXTPROC glad_glTextureParameteriEXT;
#define glTextureParameteriEXT glad_glTextureParameteriEXT
GLAPI PFNGLMULTITEXCOORD2DVARBPROC glad_glMultiTexCoord2dvARB;
#define glMultiTexCoord2dvARB glad_glMultiTexCoord2dvARB
GLAPI PFNGLDELETESHADERPROC glad_glDeleteShader;
#define glDeleteShader glad_glDeleteShader
GLAPI PFNGLCOPYTEXTURESUBIMAGE1DPROC glad_glCopyTextureSubImage1D;
#define glCopyTextureSubImage1D glad_glCopyTextureSubImage1D
GLAPI PFNGLTEXTUREPARAMETERIUIVPROC glad_glTextureParameterIuiv;
#define glTextureParameterIuiv glad_glTextureParameterIuiv
GLAPI PFNGLTRANSFORMFEEDBACKATTRIBSNVPROC glad_glTransformFeedbackAttribsNV;
#define glTransformFeedbackAttribsNV glad_glTransformFeedbackAttribsNV
GLAPI PFNGLUNIFORM2UIPROC glad_glUniform2ui;
#define glUniform2ui glad_glUniform2ui
GLAPI PFNGLVERTEXATTRIBIPOINTEREXTPROC glad_glVertexAttribIPointerEXT;
#define glVertexAttribIPointerEXT glad_glVertexAttribIPointerEXT
GLAPI PFNGLVERTEXATTRIBI3IPROC glad_glVertexAttribI3i;
#define glVertexAttribI3i glad_glVertexAttribI3i
GLAPI PFNGLVERTEXATTRIBFORMATPROC glad_glVertexAttribFormat;
#define glVertexAttribFormat glad_glVertexAttribFormat
GLAPI PFNGLGETPROGRAMIVARBPROC glad_glGetProgramivARB;
#define glGetProgramivARB glad_glGetProgramivARB
GLAPI PFNGLVERTEXATTRIBI4USVPROC glad_glVertexAttribI4usv;
#define glVertexAttribI4usv glad_glVertexAttribI4usv
GLAPI PFNGLTEXPARAMETERFPROC glad_glTexParameterf;
#define glTexParameterf glad_glTexParameterf
GLAPI PFNGLVERTEXATTRIBBINDINGPROC glad_glVertexAttribBinding;
#define glVertexAttribBinding glad_glVertexAttribBinding
GLAPI PFNGLTEXPARAMETERIPROC glad_glTexParameteri;
#define glTexParameteri glad_glTexParameteri
GLAPI PFNGLGETSHADERSOURCEPROC glad_glGetShaderSource;
#define glGetShaderSource glad_glGetShaderSource
GLAPI PFNGLTEXTUREBUFFEREXTPROC glad_glTextureBufferEXT;
#define glTextureBufferEXT glad_glTextureBufferEXT
GLAPI PFNGLGETMULTITEXIMAGEEXTPROC glad_glGetMultiTexImageEXT;
#define glGetMultiTexImageEXT glad_glGetMultiTexImageEXT
GLAPI PFNGLGETPOINTERINDEXEDVEXTPROC glad_glGetPointerIndexedvEXT;
#define glGetPointerIndexedvEXT glad_glGetPointerIndexedvEXT
GLAPI PFNGLBUFFERPARAMETERIAPPLEPROC glad_glBufferParameteriAPPLE;
#define glBufferParameteriAPPLE glad_glBufferParameteriAPPLE
GLAPI PFNGLVERTEXATTRIB3FVPROC glad_glVertexAttrib3fv;
#define glVertexAttrib3fv glad_glVertexAttrib3fv
GLAPI PFNGLVERTEXATTRIBP1UIVPROC glad_glVertexAttribP1uiv;
#define glVertexAttribP1uiv glad_glVertexAttribP1uiv
GLAPI PFNGLLINKPROGRAMPROC glad_glLinkProgram;
#define glLinkProgram glad_glLinkProgram
GLAPI PFNGLGETOBJECTLABELPROC glad_glGetObjectLabel;
#define glGetObjectLabel glad_glGetObjectLabel
GLAPI PFNGLVERTEXPOINTEREXTPROC glad_glVertexPointerEXT;
#define glVertexPointerEXT glad_glVertexPointerEXT
GLAPI PFNGLDEBUGMESSAGECALLBACKARBPROC glad_glDebugMessageCallbackARB;
#define glDebugMessageCallbackARB glad_glDebugMessageCallbackARB
GLAPI PFNGLGETSTRINGPROC glad_glGetString;
#define glGetString glad_glGetString
GLAPI PFNGLGETPATHPARAMETERFVNVPROC glad_glGetPathParameterfvNV;
#define glGetPathParameterfvNV glad_glGetPathParameterfvNV
GLAPI PFNGLPROGRAMUNIFORM3IPROC glad_glProgramUniform3i;
#define glProgramUniform3i glad_glProgramUniform3i
GLAPI PFNGLENDQUERYPROC glad_glEndQuery;
#define glEndQuery glad_glEndQuery
GLAPI PFNGLVERTEXATTRIBI4IVEXTPROC glad_glVertexAttribI4ivEXT;
#define glVertexAttribI4ivEXT glad_glVertexAttribI4ivEXT
GLAPI PFNGLFRAMEBUFFERPARAMETERIPROC glad_glFramebufferParameteri;
#define glFramebufferParameteri glad_glFramebufferParameteri
GLAPI PFNGLVERTEXATTRIBI2UIPROC glad_glVertexAttribI2ui;
#define glVertexAttribI2ui glad_glVertexAttribI2ui
GLAPI PFNGLUNIFORM2IARBPROC glad_glUniform2iARB;
#define glUniform2iARB glad_glUniform2iARB
GLAPI PFNGLDELETETEXTURESPROC glad_glDeleteTextures;
#define glDeleteTextures glad_glDeleteTextures
GLAPI PFNGLCREATEVERTEXARRAYSPROC glad_glCreateVertexArrays;
#define glCreateVertexArrays glad_glCreateVertexArrays
GLAPI PFNGLBEGINCONDITIONALRENDERPROC glad_glBeginConditionalRender;
#define glBeginConditionalRender glad_glBeginConditionalRender
GLAPI PFNGLMULTITEXCOORD3FARBPROC glad_glMultiTexCoord3fARB;
#define glMultiTexCoord3fARB glad_glMultiTexCoord3fARB
GLAPI PFNGLGETQUERYOBJECTUIVARBPROC glad_glGetQueryObjectuivARB;
#define glGetQueryObjectuivARB glad_glGetQueryObjectuivARB
GLAPI PFNGLSAMPLERPARAMETERIPROC glad_glSamplerParameteri;
#define glSamplerParameteri glad_glSamplerParameteri
GLAPI PFNGLSAMPLERPARAMETERFPROC glad_glSamplerParameterf;
#define glSamplerParameterf glad_glSamplerParameterf
GLAPI PFNGLUNIFORM1FPROC glad_glUniform1f;
#define glUniform1f glad_glUniform1f
GLAPI PFNGLUNIFORM1DPROC glad_glUniform1d;
#define glUniform1d glad_glUniform1d
GLAPI PFNGLCLEARCOLORIIEXTPROC glad_glClearColorIiEXT;
#define glClearColorIiEXT glad_glClearColorIiEXT
GLAPI PFNGLGETCOMPRESSEDTEXIMAGEPROC glad_glGetCompressedTexImage;
#define glGetCompressedTexImage glad_glGetCompressedTexImage
GLAPI PFNGLMINSAMPLESHADINGARBPROC glad_glMinSampleShadingARB;
#define glMinSampleShadingARB glad_glMinSampleShadingARB
GLAPI PFNGLUNIFORM1IPROC glad_glUniform1i;
#define glUniform1i glad_glUniform1i
GLAPI PFNGLFRAMEBUFFERTEXTUREEXTPROC glad_glFramebufferTextureEXT;
#define glFramebufferTextureEXT glad_glFramebufferTextureEXT
GLAPI PFNGLCULLFACEPROC glad_glCullFace;
#define glCullFace glad_glCullFace
GLAPI PFNGLPROGRAMUNIFORM4IPROC glad_glProgramUniform4i;
#define glProgramUniform4i glad_glProgramUniform4i
GLAPI PFNGLUNIFORMHANDLEUI64ARBPROC glad_glUniformHandleui64ARB;
#define glUniformHandleui64ARB glad_glUniformHandleui64ARB
GLAPI PFNGLPROGRAMUNIFORM4FPROC glad_glProgramUniform4f;
#define glProgramUniform4f glad_glProgramUniform4f
GLAPI PFNGLVIEWPORTINDEXEDFPROC glad_glViewportIndexedf;
#define glViewportIndexedf glad_glViewportIndexedf
GLAPI PFNGLPROGRAMUNIFORM4DPROC glad_glProgramUniform4d;
#define glProgramUniform4d glad_glProgramUniform4d
GLAPI PFNGLPOINTALONGPATHNVPROC glad_glPointAlongPathNV;
#define glPointAlongPathNV glad_glPointAlongPathNV
GLAPI PFNGLENDQUERYARBPROC glad_glEndQueryARB;
#define glEndQueryARB glad_glEndQueryARB
GLAPI PFNGLVERTEXATTRIB4UIVARBPROC glad_glVertexAttrib4uivARB;
#define glVertexAttrib4uivARB glad_glVertexAttrib4uivARB
GLAPI PFNGLRESUMETRANSFORMFEEDBACKNVPROC glad_glResumeTransformFeedbackNV;
#define glResumeTransformFeedbackNV glad_glResumeTransformFeedbackNV
GLAPI PFNGLQUERYCOUNTERPROC glad_glQueryCounter;
#define glQueryCounter glad_glQueryCounter
GLAPI PFNGLVERTEXATTRIB3FARBPROC glad_glVertexAttrib3fARB;
#define glVertexAttrib3fARB glad_glVertexAttrib3fARB
GLAPI PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC glad_glDrawTransformFeedbackStreamInstanced;
#define glDrawTransformFeedbackStreamInstanced glad_glDrawTransformFeedbackStreamInstanced
GLAPI PFNGLGETBUFFERPARAMETERIVPROC glad_glGetBufferParameteriv;
#define glGetBufferParameteriv glad_glGetBufferParameteriv
GLAPI PFNGLNAMEDFRAMEBUFFERTEXTURE3DEXTPROC glad_glNamedFramebufferTexture3DEXT;
#define glNamedFramebufferTexture3DEXT glad_glNamedFramebufferTexture3DEXT
GLAPI PFNGLTEXPARAMETERIUIVPROC glad_glTexParameterIuiv;
#define glTexParameterIuiv glad_glTexParameterIuiv
GLAPI PFNGLGETCOMPRESSEDTEXIMAGEARBPROC glad_glGetCompressedTexImageARB;
#define glGetCompressedTexImageARB glad_glGetCompressedTexImageARB
GLAPI PFNGLDELETEPATHSNVPROC glad_glDeletePathsNV;
#define glDeletePathsNV glad_glDeletePathsNV
GLAPI PFNGLVERTEXATTRIBI4UBVEXTPROC glad_glVertexAttribI4ubvEXT;
#define glVertexAttribI4ubvEXT glad_glVertexAttribI4ubvEXT
GLAPI PFNGLVERTEXATTRIB1DNVPROC glad_glVertexAttrib1dNV;
#define glVertexAttrib1dNV glad_glVertexAttrib1dNV
GLAPI PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIEXTPROC glad_glNamedProgramLocalParameterI4uiEXT;
#define glNamedProgramLocalParameterI4uiEXT glad_glNamedProgramLocalParameterI4uiEXT
GLAPI PFNGLPROGRAMUNIFORM1IEXTPROC glad_glProgramUniform1iEXT;
#define glProgramUniform1iEXT glad_glProgramUniform1iEXT
GLAPI PFNGLDELETEQUERIESARBPROC glad_glDeleteQueriesARB;
#define glDeleteQueriesARB glad_glDeleteQueriesARB
GLAPI PFNGLTEXTURESTORAGE3DPROC glad_glTextureStorage3D;
#define glTextureStorage3D glad_glTextureStorage3D
GLAPI PFNGLISTRANSFORMFEEDBACKPROC glad_glIsTransformFeedback;
#define glIsTransformFeedback glad_glIsTransformFeedback
GLAPI PFNGLISPROGRAMPIPELINEPROC glad_glIsProgramPipeline;
#define glIsProgramPipeline glad_glIsProgramPipeline
GLAPI PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC glad_glGetActiveSubroutineUniformName;
#define glGetActiveSubroutineUniformName glad_glGetActiveSubroutineUniformName
GLAPI PFNGLVERTEXATTRIB4SNVPROC glad_glVertexAttrib4sNV;
#define glVertexAttrib4sNV glad_glVertexAttrib4sNV
GLAPI PFNGLPROGRAMLOCALPARAMETER4DVARBPROC glad_glProgramLocalParameter4dvARB;
#define glProgramLocalParameter4dvARB glad_glProgramLocalParameter4dvARB
GLAPI PFNGLTEXTUREPARAMETERFVEXTPROC glad_glTextureParameterfvEXT;
#define glTextureParameterfvEXT glad_glTextureParameterfvEXT
GLAPI PFNGLGETNUNIFORMFVPROC glad_glGetnUniformfv;
#define glGetnUniformfv glad_glGetnUniformfv
GLAPI PFNGLVERTEXATTRIBL2DVPROC glad_glVertexAttribL2dv;
#define glVertexAttribL2dv glad_glVertexAttribL2dv
GLAPI PFNGLTEXRENDERBUFFERNVPROC glad_glTexRenderbufferNV;
#define glTexRenderbufferNV glad_glTexRenderbufferNV
GLAPI PFNGLBINDVERTEXARRAYPROC glad_glBindVertexArray;
#define glBindVertexArray glad_glBindVertexArray
GLAPI PFNGLVERTEXATTRIBL2DPROC glad_glVertexAttribL2d;
#define glVertexAttribL2d glad_glVertexAttribL2d
GLAPI PFNGLSTENCILFUNCPROC glad_glStencilFunc;
#define glStencilFunc glad_glStencilFunc
GLAPI PFNGLGETVERTEXATTRIBDVNVPROC glad_glGetVertexAttribdvNV;
#define glGetVertexAttribdvNV glad_glGetVertexAttribdvNV
GLAPI PFNGLGETQUERYBUFFEROBJECTIVPROC glad_glGetQueryBufferObjectiv;
#define glGetQueryBufferObjectiv glad_glGetQueryBufferObjectiv
GLAPI PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glad_glGetFramebufferAttachmentParameteriv;
#define glGetFramebufferAttachmentParameteriv glad_glGetFramebufferAttachmentParameteriv
GLAPI PFNGLGETPROGRAMPIPELINEIVPROC glad_glGetProgramPipelineiv;
#define glGetProgramPipelineiv glad_glGetProgramPipelineiv
GLAPI PFNGLVERTEXATTRIBL4DPROC glad_glVertexAttribL4d;
#define glVertexAttribL4d glad_glVertexAttribL4d
GLAPI PFNGLGETSHADERINFOLOGPROC glad_glGetShaderInfoLog;
#define glGetShaderInfoLog glad_glGetShaderInfoLog
GLAPI PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXTPROC glad_glCompressedTextureSubImage2DEXT;
#define glCompressedTextureSubImage2DEXT glad_glCompressedTextureSubImage2DEXT
GLAPI PFNGLCLEARBUFFERSUBDATAPROC glad_glClearBufferSubData;
#define glClearBufferSubData glad_glClearBufferSubData
GLAPI PFNGLVERTEXATTRIBI4IPROC glad_glVertexAttribI4i;
#define glVertexAttribI4i glad_glVertexAttribI4i
GLAPI PFNGLGETINTEGERUI64I_VNVPROC glad_glGetIntegerui64i_vNV;
#define glGetIntegerui64i_vNV glad_glGetIntegerui64i_vNV
GLAPI PFNGLBINDATTRIBLOCATIONPROC glad_glBindAttribLocation;
#define glBindAttribLocation glad_glBindAttribLocation
GLAPI PFNGLBLENDEQUATIONSEPARATEPROC glad_glBlendEquationSeparate;
#define glBlendEquationSeparate glad_glBlendEquationSeparate
GLAPI PFNGLDRAWELEMENTSINSTANCEDARBPROC glad_glDrawElementsInstancedARB;
#define glDrawElementsInstancedARB glad_glDrawElementsInstancedARB
GLAPI PFNGLBEGINQUERYARBPROC glad_glBeginQueryARB;
#define glBeginQueryARB glad_glBeginQueryARB
GLAPI PFNGLGETSUBROUTINEINDEXPROC glad_glGetSubroutineIndex;
#define glGetSubroutineIndex glad_glGetSubroutineIndex
GLAPI PFNGLVERTEXATTRIB2SVPROC glad_glVertexAttrib2sv;
#define glVertexAttrib2sv glad_glVertexAttrib2sv
GLAPI PFNGLPATHPARAMETERFNVPROC glad_glPathParameterfNV;
#define glPathParameterfNV glad_glPathParameterfNV
GLAPI PFNGLMULTITEXCOORD3FVARBPROC glad_glMultiTexCoord3fvARB;
#define glMultiTexCoord3fvARB glad_glMultiTexCoord3fvARB
GLAPI PFNGLVERTEXATTRIB1FVARBPROC glad_glVertexAttrib1fvARB;
#define glVertexAttrib1fvARB glad_glVertexAttrib1fvARB
GLAPI PFNGLNAMEDPROGRAMLOCALPARAMETER4FEXTPROC glad_glNamedProgramLocalParameter4fEXT;
#define glNamedProgramLocalParameter4fEXT glad_glNamedProgramLocalParameter4fEXT
GLAPI PFNGLVERTEXATTRIBL1DEXTPROC glad_glVertexAttribL1dEXT;
#define glVertexAttribL1dEXT glad_glVertexAttribL1dEXT
GLAPI PFNGLGETNUNIFORMDVARBPROC glad_glGetnUniformdvARB;
#define glGetnUniformdvARB glad_glGetnUniformdvARB
GLAPI PFNGLPROGRAMUNIFORM1UIVEXTPROC glad_glProgramUniform1uivEXT;
#define glProgramUniform1uivEXT glad_glProgramUniform1uivEXT
GLAPI PFNGLVERTEXARRAYVERTEXATTRIBLFORMATEXTPROC glad_glVertexArrayVertexAttribLFormatEXT;
#define glVertexArrayVertexAttribLFormatEXT glad_glVertexArrayVertexAttribLFormatEXT
GLAPI PFNGLBINDPROGRAMPIPELINEPROC glad_glBindProgramPipeline;
#define glBindProgramPipeline glad_glBindProgramPipeline
GLAPI PFNGLPROGRAMUNIFORM4FVPROC glad_glProgramUniform4fv;
#define glProgramUniform4fv glad_glProgramUniform4fv
GLAPI PFNGLGETBOOLEANVPROC glad_glGetBooleanv;
#define glGetBooleanv glad_glGetBooleanv
GLAPI PFNGLVERTEXATTRIB1DPROC glad_glVertexAttrib1d;
#define glVertexAttrib1d glad_glVertexAttrib1d
GLAPI PFNGLMULTIDRAWARRAYSINDIRECTAMDPROC glad_glMultiDrawArraysIndirectAMD;
#define glMultiDrawArraysIndirectAMD glad_glMultiDrawArraysIndirectAMD
GLAPI PFNGLGENTEXTURESPROC glad_glGenTextures;
#define glGenTextures glad_glGenTextures
GLAPI PFNGLPROGRAMUNIFORM4UIVEXTPROC glad_glProgramUniform4uivEXT;
#define glProgramUniform4uivEXT glad_glProgramUniform4uivEXT
GLAPI PFNGLGETTEXTURELEVELPARAMETERIVPROC glad_glGetTextureLevelParameteriv;
#define glGetTextureLevelParameteriv glad_glGetTextureLevelParameteriv
GLAPI PFNGLGETSTRINGIPROC glad_glGetStringi;
#define glGetStringi glad_glGetStringi
GLAPI PFNGLPOINTPARAMETERFVPROC glad_glPointParameterfv;
#define glPointParameterfv glad_glPointParameterfv
GLAPI PFNGLUNIFORMMATRIX2FVARBPROC glad_glUniformMatrix2fvARB;
#define glUniformMatrix2fvARB glad_glUniformMatrix2fvARB
GLAPI PFNGLUNIFORM2FVPROC glad_glUniform2fv;
#define glUniform2fv glad_glUniform2fv
GLAPI PFNGLFOGCOORDFORMATNVPROC glad_glFogCoordFormatNV;
#define glFogCoordFormatNV glad_glFogCoordFormatNV
GLAPI PFNGLGETMULTITEXGENFVEXTPROC glad_glGetMultiTexGenfvEXT;
#define glGetMultiTexGenfvEXT glad_glGetMultiTexGenfvEXT
GLAPI PFNGLMATRIXFRUSTUMEXTPROC glad_glMatrixFrustumEXT;
#define glMatrixFrustumEXT glad_glMatrixFrustumEXT
GLAPI PFNGLDISPATCHCOMPUTEINDIRECTPROC glad_glDispatchComputeIndirect;
#define glDispatchComputeIndirect glad_glDispatchComputeIndirect
GLAPI PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC glad_glInvalidateNamedFramebufferSubData;
#define glInvalidateNamedFramebufferSubData glad_glInvalidateNamedFramebufferSubData
GLAPI PFNGLPROGRAMENVPARAMETER4DARBPROC glad_glProgramEnvParameter4dARB;
#define glProgramEnvParameter4dARB glad_glProgramEnvParameter4dARB
GLAPI PFNGLBINDBUFFERRANGEPROC glad_glBindBufferRange;
#define glBindBufferRange glad_glBindBufferRange
GLAPI PFNGLGETUNIFORMI64VNVPROC glad_glGetUniformi64vNV;
#define glGetUniformi64vNV glad_glGetUniformi64vNV
GLAPI PFNGLGETUNIFORMDVPROC glad_glGetUniformdv;
#define glGetUniformdv glad_glGetUniformdv
GLAPI PFNGLGETMULTITEXLEVELPARAMETERFVEXTPROC glad_glGetMultiTexLevelParameterfvEXT;
#define glGetMultiTexLevelParameterfvEXT glad_glGetMultiTexLevelParameterfvEXT
GLAPI PFNGLPROGRAMUNIFORM3FVEXTPROC glad_glProgramUniform3fvEXT;
#define glProgramUniform3fvEXT glad_glProgramUniform3fvEXT
GLAPI PFNGLISVERTEXARRAYPROC glad_glIsVertexArray;
#define glIsVertexArray glad_glIsVertexArray
GLAPI PFNGLGETVERTEXARRAYPOINTERVEXTPROC glad_glGetVertexArrayPointervEXT;
#define glGetVertexArrayPointervEXT glad_glGetVertexArrayPointervEXT
GLAPI PFNGLPOINTPARAMETERFSGISPROC glad_glPointParameterfSGIS;
#define glPointParameterfSGIS glad_glPointParameterfSGIS
GLAPI PFNGLUNIFORM1FARBPROC glad_glUniform1fARB;
#define glUniform1fARB glad_glUniform1fARB
GLAPI PFNGLCLIENTWAITSYNCPROC glad_glClientWaitSync;
#define glClientWaitSync glad_glClientWaitSync
GLAPI PFNGLVERTEXATTRIBS1FVNVPROC glad_glVertexAttribs1fvNV;
#define glVertexAttribs1fvNV glad_glVertexAttribs1fvNV
GLAPI PFNGLVERTEXATTRIB4NUSVARBPROC glad_glVertexAttrib4NusvARB;
#define glVertexAttrib4NusvARB glad_glVertexAttrib4NusvARB
GLAPI PFNGLBLENDFUNCIARBPROC glad_glBlendFunciARB;
#define glBlendFunciARB glad_glBlendFunciARB
GLAPI PFNGLINVALIDATETEXIMAGEPROC glad_glInvalidateTexImage;
#define glInvalidateTexImage glad_glInvalidateTexImage
GLAPI PFNGLCOPYIMAGESUBDATAPROC glad_glCopyImageSubData;
#define glCopyImageSubData glad_glCopyImageSubData
GLAPI PFNGLGETUNIFORMSUBROUTINEUIVPROC glad_glGetUniformSubroutineuiv;
#define glGetUniformSubroutineuiv glad_glGetUniformSubroutineuiv
GLAPI PFNGLBINDVERTEXBUFFERPROC glad_glBindVertexBuffer;
#define glBindVertexBuffer glad_glBindVertexBuffer
GLAPI PFNGLMULTITEXCOORD1IARBPROC glad_glMultiTexCoord1iARB;
#define glMultiTexCoord1iARB glad_glMultiTexCoord1iARB
GLAPI PFNGLDEBUGMESSAGEINSERTPROC glad_glDebugMessageInsert;
#define glDebugMessageInsert glad_glDebugMessageInsert
GLAPI PFNGLISSAMPLERPROC glad_glIsSampler;
#define glIsSampler glad_glIsSampler
GLAPI PFNGLMULTITEXGENIVEXTPROC glad_glMultiTexGenivEXT;
#define glMultiTexGenivEXT glad_glMultiTexGenivEXT
GLAPI PFNGLNAMEDFRAMEBUFFERTEXTURE2DEXTPROC glad_glNamedFramebufferTexture2DEXT;
#define glNamedFramebufferTexture2DEXT glad_glNamedFramebufferTexture2DEXT
GLAPI PFNGLCOPYTEXSUBIMAGE1DPROC glad_glCopyTexSubImage1D;
#define glCopyTexSubImage1D glad_glCopyTexSubImage1D
GLAPI PFNGLCHECKFRAMEBUFFERSTATUSPROC glad_glCheckFramebufferStatus;
#define glCheckFramebufferStatus glad_glCheckFramebufferStatus
GLAPI PFNGLBINDIMAGETEXTUREPROC glad_glBindImageTexture;
#define glBindImageTexture glad_glBindImageTexture
GLAPI PFNGLISPATHNVPROC glad_glIsPathNV;
#define glIsPathNV glad_glIsPathNV
GLAPI PFNGLDRAWRANGEELEMENTSPROC glad_glDrawRangeElements;
#define glDrawRangeElements glad_glDrawRangeElements
GLAPI PFNGLMULTITEXCOORDPOINTEREXTPROC glad_glMultiTexCoordPointerEXT;
#define glMultiTexCoordPointerEXT glad_glMultiTexCoordPointerEXT
GLAPI PFNGLBINDBUFFERBASEPROC glad_glBindBufferBase;
#define glBindBufferBase glad_glBindBufferBase
GLAPI PFNGLMULTIDRAWARRAYSPROC glad_glMultiDrawArrays;
#define glMultiDrawArrays glad_glMultiDrawArrays
GLAPI PFNGLNAMEDRENDERBUFFERSTORAGEEXTPROC glad_glNamedRenderbufferStorageEXT;
#define glNamedRenderbufferStorageEXT glad_glNamedRenderbufferStorageEXT
GLAPI PFNGLVERTEXATTRIBL1DPROC glad_glVertexAttribL1d;
#define glVertexAttribL1d glad_glVertexAttribL1d
GLAPI PFNGLMULTITEXSUBIMAGE1DEXTPROC glad_glMultiTexSubImage1DEXT;
#define glMultiTexSubImage1DEXT glad_glMultiTexSubImage1DEXT
GLAPI PFNGLVERTEXATTRIBI3UIVPROC glad_glVertexAttribI3uiv;
#define glVertexAttribI3uiv glad_glVertexAttribI3uiv
GLAPI PFNGLBUFFERDATAARBPROC glad_glBufferDataARB;
#define glBufferDataARB glad_glBufferDataARB
GLAPI PFNGLVERTEXATTRIBIFORMATPROC glad_glVertexAttribIFormat;
#define glVertexAttribIFormat glad_glVertexAttribIFormat
GLAPI PFNGLCREATEFRAMEBUFFERSPROC glad_glCreateFramebuffers;
#define glCreateFramebuffers glad_glCreateFramebuffers
GLAPI PFNGLGETACTIVEUNIFORMNAMEPROC glad_glGetActiveUniformName;
#define glGetActiveUniformName glad_glGetActiveUniformName
GLAPI PFNGLUNIFORM2UIVEXTPROC glad_glUniform2uivEXT;
#define glUniform2uivEXT glad_glUniform2uivEXT
GLAPI PFNGLVERTEXATTRIBI4UIPROC glad_glVertexAttribI4ui;
#define glVertexAttribI4ui glad_glVertexAttribI4ui
GLAPI PFNGLBEGINQUERYPROC glad_glBeginQuery;
#define glBeginQuery glad_glBeginQuery
GLAPI PFNGLBINDBUFFERPROC glad_glBindBuffer;
#define glBindBuffer glad_glBindBuffer
GLAPI PFNGLUNIFORMMATRIX2X4FVPROC glad_glUniformMatrix2x4fv;
#define glUniformMatrix2x4fv glad_glUniformMatrix2x4fv
GLAPI PFNGLGETMULTITEXPARAMETERFVEXTPROC glad_glGetMultiTexParameterfvEXT;
#define glGetMultiTexParameterfvEXT glad_glGetMultiTexParameterfvEXT
GLAPI PFNGLDEPTHRANGEINDEXEDPROC glad_glDepthRangeIndexed;
#define glDepthRangeIndexed glad_glDepthRangeIndexed
GLAPI PFNGLGETVERTEXATTRIBIUIVEXTPROC glad_glGetVertexAttribIuivEXT;
#define glGetVertexAttribIuivEXT glad_glGetVertexAttribIuivEXT
GLAPI PFNGLMATRIXSCALEFEXTPROC glad_glMatrixScalefEXT;
#define glMatrixScalefEXT glad_glMatrixScalefEXT
GLAPI PFNGLGETERRORPROC glad_glGetError;
#define glGetError glad_glGetError
GLAPI PFNGLGETUNIFORMIVARBPROC glad_glGetUniformivARB;
#define glGetUniformivARB glad_glGetUniformivARB
GLAPI PFNGLGETACTIVEUNIFORMARBPROC glad_glGetActiveUniformARB;
#define glGetActiveUniformARB glad_glGetActiveUniformARB
GLAPI PFNGLVERTEXATTRIBI3IVEXTPROC glad_glVertexAttribI3ivEXT;
#define glVertexAttribI3ivEXT glad_glVertexAttribI3ivEXT
GLAPI PFNGLGETTEXLEVELPARAMETERFVPROC glad_glGetTexLevelParameterfv;
#define glGetTexLevelParameterfv glad_glGetTexLevelParameterfv
GLAPI PFNGLVERTEXATTRIBI1IVEXTPROC glad_glVertexAttribI1ivEXT;
#define glVertexAttribI1ivEXT glad_glVertexAttribI1ivEXT
GLAPI PFNGLVERTEXATTRIB3SVNVPROC glad_glVertexAttrib3svNV;
#define glVertexAttrib3svNV glad_glVertexAttrib3svNV
GLAPI PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC glad_glFlushMappedNamedBufferRange;
#define glFlushMappedNamedBufferRange glad_glFlushMappedNamedBufferRange
GLAPI PFNGLGETNUNIFORMUI64VARBPROC glad_glGetnUniformui64vARB;
#define glGetnUniformui64vARB glad_glGetnUniformui64vARB
GLAPI PFNGLGETACTIVEUNIFORMSIVPROC glad_glGetActiveUniformsiv;
#define glGetActiveUniformsiv glad_glGetActiveUniformsiv
GLAPI PFNGLVERTEXATTRIBI1UIEXTPROC glad_glVertexAttribI1uiEXT;
#define glVertexAttribI1uiEXT glad_glVertexAttribI1uiEXT
GLAPI PFNGLPROGRAMUNIFORM3I64VARBPROC glad_glProgramUniform3i64vARB;
#define glProgramUniform3i64vARB glad_glProgramUniform3i64vARB
GLAPI PFNGLPROGRAMUNIFORM4UIPROC glad_glProgramUniform4ui;
#define glProgramUniform4ui glad_glProgramUniform4ui
GLAPI PFNGLCOPYTEXSUBIMAGE2DEXTPROC glad_glCopyTexSubImage2DEXT;
#define glCopyTexSubImage2DEXT glad_glCopyTexSubImage2DEXT
GLAPI PFNGLSTENCILMASKPROC glad_glStencilMask;
#define glStencilMask glad_glStencilMask
GLAPI PFNGLPROGRAMUNIFORM1DVPROC glad_glProgramUniform1dv;
#define glProgramUniform1dv glad_glProgramUniform1dv
GLAPI PFNGLNAMEDBUFFERDATAPROC glad_glNamedBufferData;
#define glNamedBufferData glad_glNamedBufferData
GLAPI PFNGLVERTEXATTRIB3FVARBPROC glad_glVertexAttrib3fvARB;
#define glVertexAttrib3fvARB glad_glVertexAttrib3fvARB
GLAPI PFNGLCLEARNAMEDBUFFERSUBDATAPROC glad_glClearNamedBufferSubData;
#define glClearNamedBufferSubData glad_glClearNamedBufferSubData
GLAPI PFNGLMAPNAMEDBUFFERRANGEPROC glad_glMapNamedBufferRange;
#define glMapNamedBufferRange glad_glMapNamedBufferRange
GLAPI PFNGLUNIFORM3IARBPROC glad_glUniform3iARB;
#define glUniform3iARB glad_glUniform3iARB
GLAPI PFNGLCREATEPROGRAMOBJECTARBPROC glad_glCreateProgramObjectARB;
#define glCreateProgramObjectARB glad_glCreateProgramObjectARB
GLAPI PFNGLMULTITEXCOORD1DVARBPROC glad_glMultiTexCoord1dvARB;
#define glMultiTexCoord1dvARB glad_glMultiTexCoord1dvARB
GLAPI PFNGLPROGRAMUNIFORM3DVEXTPROC glad_glProgramUniform3dvEXT;
#define glProgramUniform3dvEXT glad_glProgramUniform3dvEXT
GLAPI PFNGLMULTITEXIMAGE2DEXTPROC glad_glMultiTexImage2DEXT;
#define glMultiTexImage2DEXT glad_glMultiTexImage2DEXT
GLAPI PFNGLPROGRAMUNIFORM1I64NVPROC glad_glProgramUniform1i64NV;
#define glProgramUniform1i64NV glad_glProgramUniform1i64NV
GLAPI PFNGLMULTITEXCOORD1FVARBPROC glad_glMultiTexCoord1fvARB;
#define glMultiTexCoord1fvARB glad_glMultiTexCoord1fvARB
GLAPI PFNGLBLENDFUNCSEPARATEINGRPROC glad_glBlendFuncSeparateINGR;
#define glBlendFuncSeparateINGR glad_glBlendFuncSeparateINGR
GLAPI PFNGLVERTEXATTRIB2FARBPROC glad_glVertexAttrib2fARB;
#define glVertexAttrib2fARB glad_glVertexAttrib2fARB
GLAPI PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC glad_glGetCompressedTextureSubImage;
#define glGetCompressedTextureSubImage glad_glGetCompressedTextureSubImage
GLAPI PFNGLPATHGLYPHINDEXARRAYNVPROC glad_glPathGlyphIndexArrayNV;
#define glPathGlyphIndexArrayNV glad_glPathGlyphIndexArrayNV
GLAPI PFNGLINDEXPOINTEREXTPROC glad_glIndexPointerEXT;
#define glIndexPointerEXT glad_glIndexPointerEXT
GLAPI PFNGLTEXTUREIMAGE2DEXTPROC glad_glTextureImage2DEXT;
#define glTextureImage2DEXT glad_glTextureImage2DEXT
GLAPI PFNGLACTIVESHADERPROGRAMPROC glad_glActiveShaderProgram;
#define glActiveShaderProgram glad_glActiveShaderProgram
GLAPI PFNGLPATCHPARAMETERFVPROC glad_glPatchParameterfv;
#define glPatchParameterfv glad_glPatchParameterfv
GLAPI PFNGLISFRAMEBUFFEREXTPROC glad_glIsFramebufferEXT;
#define glIsFramebufferEXT glad_glIsFramebufferEXT
GLAPI PFNGLTEXTURESTORAGE2DPROC glad_glTextureStorage2D;
#define glTextureStorage2D glad_glTextureStorage2D
GLAPI PFNGLPROGRAMUNIFORM1UI64ARBPROC glad_glProgramUniform1ui64ARB;
#define glProgramUniform1ui64ARB glad_glProgramUniform1ui64ARB
GLAPI PFNGLTEXBUFFERRANGEPROC glad_glTexBufferRange;
#define glTexBufferRange glad_glTexBufferRange
GLAPI PFNGLTEXTURESUBIMAGE3DEXTPROC glad_glTextureSubImage3DEXT;
#define glTextureSubImage3DEXT glad_glTextureSubImage3DEXT
GLAPI PFNGLCLIPCONTROLPROC glad_glClipControl;
#define glClipControl glad_glClipControl
GLAPI PFNGLUNIFORM2I64NVPROC glad_glUniform2i64NV;
#define glUniform2i64NV glad_glUniform2i64NV
GLAPI PFNGLVERTEXATTRIBI3UIPROC glad_glVertexAttribI3ui;
#define glVertexAttribI3ui glad_glVertexAttribI3ui
GLAPI PFNGLVERTEXATTRIB4NIVARBPROC glad_glVertexAttrib4NivARB;
#define glVertexAttrib4NivARB glad_glVertexAttrib4NivARB
GLAPI PFNGLBLENDEQUATIONSEPARATEINDEXEDAMDPROC glad_glBlendEquationSeparateIndexedAMD;
#define glBlendEquationSeparateIndexedAMD glad_glBlendEquationSeparateIndexedAMD
GLAPI PFNGLVERTEXARRAYVERTEXBUFFERSPROC glad_glVertexArrayVertexBuffers;
#define glVertexArrayVertexBuffers glad_glVertexArrayVertexBuffers
GLAPI PFNGLUNIFORM1UI64NVPROC glad_glUniform1ui64NV;
#define glUniform1ui64NV glad_glUniform1ui64NV
GLAPI PFNGLPROGRAMPARAMETERIPROC glad_glProgramParameteri;
#define glProgramParameteri glad_glProgramParameteri
GLAPI PFNGLPROGRAMUNIFORM2I64VNVPROC glad_glProgramUniform2i64vNV;
#define glProgramUniform2i64vNV glad_glProgramUniform2i64vNV
GLAPI PFNGLISSTATENVPROC glad_glIsStateNV;
#define glIsStateNV glad_glIsStateNV
GLAPI PFNGLVERTEXATTRIBL2DEXTPROC glad_glVertexAttribL2dEXT;
#define glVertexAttribL2dEXT glad_glVertexAttribL2dEXT
GLAPI PFNGLPROGRAMUNIFORMMATRIX3X2DVEXTPROC glad_glProgramUniformMatrix3x2dvEXT;
#define glProgramUniformMatrix3x2dvEXT glad_glProgramUniformMatrix3x2dvEXT
GLAPI PFNGLENABLEVERTEXARRAYATTRIBEXTPROC glad_glEnableVertexArrayAttribEXT;
#define glEnableVertexArrayAttribEXT glad_glEnableVertexArrayAttribEXT
GLAPI PFNGLPROGRAMUNIFORM2UIVEXTPROC glad_glProgramUniform2uivEXT;
#define glProgramUniform2uivEXT glad_glProgramUniform2uivEXT
GLAPI PFNGLGETDOUBLEINDEXEDVEXTPROC glad_glGetDoubleIndexedvEXT;
#define glGetDoubleIndexedvEXT glad_glGetDoubleIndexedvEXT
GLAPI PFNGLGETVARYINGLOCATIONNVPROC glad_glGetVaryingLocationNV;
#define glGetVaryingLocationNV glad_glGetVaryingLocationNV
GLAPI PFNGLBINDFRAMEBUFFEREXTPROC glad_glBindFramebufferEXT;
#define glBindFramebufferEXT glad_glBindFramebufferEXT
GLAPI PFNGLCREATESHADERPROC glad_glCreateShader;
#define glCreateShader glad_glCreateShader
GLAPI PFNGLGENPATHSNVPROC glad_glGenPathsNV;
#define glGenPathsNV glad_glGenPathsNV
GLAPI PFNGLGENRENDERBUFFERSPROC glad_glGenRenderbuffers;
#define glGenRenderbuffers glad_glGenRenderbuffers
GLAPI PFNGLCOPYTEXSUBIMAGE2DPROC glad_glCopyTexSubImage2D;
#define glCopyTexSubImage2D glad_glCopyTexSubImage2D
GLAPI PFNGLVERTEXATTRIB4UBVARBPROC glad_glVertexAttrib4ubvARB;
#define glVertexAttrib4ubvARB glad_glVertexAttrib4ubvARB
GLAPI PFNGLVERTEXATTRIBS1SVNVPROC glad_glVertexAttribs1svNV;
#define glVertexAttribs1svNV glad_glVertexAttribs1svNV
GLAPI PFNGLBINDBUFFERBASEEXTPROC glad_glBindBufferBaseEXT;
#define glBindBufferBaseEXT glad_glBindBufferBaseEXT
GLAPI PFNGLCOMPRESSEDTEXIMAGE1DPROC glad_glCompressedTexImage1D;
#define glCompressedTexImage1D glad_glCompressedTexImage1D
GLAPI PFNGLBLENDFUNCSEPARATEPROC glad_glBlendFuncSeparate;
#define glBlendFuncSeparate glad_glBlendFuncSeparate
GLAPI PFNGLPOINTPARAMETERFARBPROC glad_glPointParameterfARB;
#define glPointParameterfARB glad_glPointParameterfARB
GLAPI PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC glad_glCompressedTexSubImage1DARB;
#define glCompressedTexSubImage1DARB glad_glCompressedTexSubImage1DARB
GLAPI PFNGLPOINTSIZEPROC glad_glPointSize;
#define glPointSize glad_glPointSize
GLAPI PFNGLBINDTEXTUREUNITPROC glad_glBindTextureUnit;
#define glBindTextureUnit glad_glBindTextureUnit
GLAPI PFNGLPROGRAMUNIFORM4I64VARBPROC glad_glProgramUniform4i64vARB;
#define glProgramUniform4i64vARB glad_glProgramUniform4i64vARB
GLAPI PFNGLVERTEXATTRIB4NUIVPROC glad_glVertexAttrib4Nuiv;
#define glVertexAttrib4Nuiv glad_glVertexAttrib4Nuiv
GLAPI PFNGLGETUNIFORMFVARBPROC glad_glGetUniformfvARB;
#define glGetUniformfvARB glad_glGetUniformfvARB
GLAPI PFNGLPROGRAMLOCALPARAMETER4FARBPROC glad_glProgramLocalParameter4fARB;
#define glProgramLocalParameter4fARB glad_glProgramLocalParameter4fARB
GLAPI PFNGLUNIFORM3IPROC glad_glUniform3i;
#define glUniform3i glad_glUniform3i
GLAPI PFNGLBLENDEQUATIONSEPARATEIPROC glad_glBlendEquationSeparatei;
#define glBlendEquationSeparatei glad_glBlendEquationSeparatei
GLAPI PFNGLTEXTUREPARAMETERIVEXTPROC glad_glTextureParameterivEXT;
#define glTextureParameterivEXT glad_glTextureParameterivEXT
GLAPI PFNGLUNIFORM3DPROC glad_glUniform3d;
#define glUniform3d glad_glUniform3d
GLAPI PFNGLUNIFORM3FPROC glad_glUniform3f;
#define glUniform3f glad_glUniform3f
GLAPI PFNGLVERTEXATTRIBS4DVNVPROC glad_glVertexAttribs4dvNV;
#define glVertexAttribs4dvNV glad_glVertexAttribs4dvNV
GLAPI PFNGLUNIFORM3UI64VARBPROC glad_glUniform3ui64vARB;
#define glUniform3ui64vARB glad_glUniform3ui64vARB
GLAPI PFNGLPROGRAMUNIFORM1UI64VARBPROC glad_glProgramUniform1ui64vARB;
#define glProgramUniform1ui64vARB glad_glProgramUniform1ui64vARB
GLAPI PFNGLTEXTUREPARAMETERFEXTPROC glad_glTextureParameterfEXT;
#define glTextureParameterfEXT glad_glTextureParameterfEXT
GLAPI PFNGLVERTEXATTRIB4SARBPROC glad_glVertexAttrib4sARB;
#define glVertexAttrib4sARB glad_glVertexAttrib4sARB
GLAPI PFNGLGETFRAGDATAINDEXPROC glad_glGetFragDataIndex;
#define glGetFragDataIndex glad_glGetFragDataIndex
GLAPI PFNGLORTHOFOESPROC glad_glOrthofOES;
#define glOrthofOES glad_glOrthofOES
GLAPI PFNGLWEIGHTPATHSNVPROC glad_glWeightPathsNV;
#define glWeightPathsNV glad_glWeightPathsNV
GLAPI PFNGLCOPYTEXIMAGE1DEXTPROC glad_glCopyTexImage1DEXT;
#define glCopyTexImage1DEXT glad_glCopyTexImage1DEXT
GLAPI PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXTPROC glad_glCompressedTextureSubImage3DEXT;
#define glCompressedTextureSubImage3DEXT glad_glCompressedTextureSubImage3DEXT
GLAPI PFNGLVERTEXATTRIBL3DVEXTPROC glad_glVertexAttribL3dvEXT;
#define glVertexAttribL3dvEXT glad_glVertexAttribL3dvEXT
GLAPI PFNGLDELETEFRAMEBUFFERSPROC glad_glDeleteFramebuffers;
#define glDeleteFramebuffers glad_glDeleteFramebuffers
GLAPI PFNGLDRAWARRAYSPROC glad_glDrawArrays;
#define glDrawArrays glad_glDrawArrays
GLAPI PFNGLGETNTEXIMAGEARBPROC glad_glGetnTexImageARB;
#define glGetnTexImageARB glad_glGetnTexImageARB
GLAPI PFNGLCLEARPROC glad_glClear;
#define glClear glad_glClear
GLAPI PFNGLUNIFORM1I64ARBPROC glad_glUniform1i64ARB;
#define glUniform1i64ARB glad_glUniform1i64ARB
GLAPI PFNGLBLENDFUNCSEPARATEIARBPROC glad_glBlendFuncSeparateiARB;
#define glBlendFuncSeparateiARB glad_glBlendFuncSeparateiARB
GLAPI PFNGLCREATEQUERIESPROC glad_glCreateQueries;
#define glCreateQueries glad_glCreateQueries
GLAPI PFNGLPROGRAMUNIFORM3IEXTPROC glad_glProgramUniform3iEXT;
#define glProgramUniform3iEXT glad_glProgramUniform3iEXT
GLAPI PFNGLVERTEXATTRIB4NUBPROC glad_glVertexAttrib4Nub;
#define glVertexAttrib4Nub glad_glVertexAttrib4Nub
GLAPI PFNGLDEPTHRANGEARRAYVPROC glad_glDepthRangeArrayv;
#define glDepthRangeArrayv glad_glDepthRangeArrayv
GLAPI PFNGLSAMPLERPARAMETERIIVPROC glad_glSamplerParameterIiv;
#define glSamplerParameterIiv glad_glSamplerParameterIiv
GLAPI PFNGLTEXIMAGE1DPROC glad_glTexImage1D;
#define glTexImage1D glad_glTexImage1D
GLAPI PFNGLISRENDERBUFFEREXTPROC glad_glIsRenderbufferEXT;
#define glIsRenderbufferEXT glad_glIsRenderbufferEXT
GLAPI PFNGLDRAWCOMMANDSNVPROC glad_glDrawCommandsNV;
#define glDrawCommandsNV glad_glDrawCommandsNV
GLAPI PFNGLUNIFORM3IVARBPROC glad_glUniform3ivARB;
#define glUniform3ivARB glad_glUniform3ivARB
GLAPI PFNGLMATRIXMULTTRANSPOSEDEXTPROC glad_glMatrixMultTransposedEXT;
#define glMatrixMultTransposedEXT glad_glMatrixMultTransposedEXT
GLAPI PFNGLREQUESTRESIDENTPROGRAMSNVPROC glad_glRequestResidentProgramsNV;
#define glRequestResidentProgramsNV glad_glRequestResidentProgramsNV
GLAPI PFNGLMATRIXMULT3X3FNVPROC glad_glMatrixMult3x3fNV;
#define glMatrixMult3x3fNV glad_glMatrixMult3x3fNV
GLAPI PFNGLPATHSUBCOMMANDSNVPROC glad_glPathSubCommandsNV;
#define glPathSubCommandsNV glad_glPathSubCommandsNV
GLAPI PFNGLISRENDERBUFFERPROC glad_glIsRenderbuffer;
#define glIsRenderbuffer glad_glIsRenderbuffer
GLAPI PFNGLDRAWBUFFERSARBPROC glad_glDrawBuffersARB;
#define glDrawBuffersARB glad_glDrawBuffersARB
GLAPI PFNGLFRUSTUMFOESPROC glad_glFrustumfOES;
#define glFrustumfOES glad_glFrustumfOES
GLAPI PFNGLCREATESAMPLERSPROC glad_glCreateSamplers;
#define glCreateSamplers glad_glCreateSamplers
GLAPI PFNGLDISABLEVERTEXATTRIBARRAYPROC glad_glDisableVertexAttribArray;
#define glDisableVertexAttribArray glad_glDisableVertexAttribArray
GLAPI PFNGLTEXCOORDFORMATNVPROC glad_glTexCoordFormatNV;
#define glTexCoordFormatNV glad_glTexCoordFormatNV
GLAPI PFNGLBLITNAMEDFRAMEBUFFERPROC glad_glBlitNamedFramebuffer;
#define glBlitNamedFramebuffer glad_glBlitNamedFramebuffer
GLAPI PFNGLMATRIXLOADTRANSPOSEFEXTPROC glad_glMatrixLoadTransposefEXT;
#define glMatrixLoadTransposefEXT glad_glMatrixLoadTransposefEXT
GLAPI PFNGLMULTITEXGENFEXTPROC glad_glMultiTexGenfEXT;
#define glMultiTexGenfEXT glad_glMultiTexGenfEXT
GLAPI PFNGLSHADERSTORAGEBLOCKBINDINGPROC glad_glShaderStorageBlockBinding;
#define glShaderStorageBlockBinding glad_glShaderStorageBlockBinding
GLAPI PFNGLSTENCILOPSEPARATEPROC glad_glStencilOpSeparate;
#define glStencilOpSeparate glad_glStencilOpSeparate
GLAPI PFNGLVERTEXATTRIBI4UBVPROC glad_glVertexAttribI4ubv;
#define glVertexAttribI4ubv glad_glVertexAttribI4ubv
GLAPI PFNGLVERTEXARRAYVERTEXATTRIBFORMATEXTPROC glad_glVertexArrayVertexAttribFormatEXT;
#define glVertexArrayVertexAttribFormatEXT glad_glVertexArrayVertexAttribFormatEXT
GLAPI PFNGLTEXIMAGE3DEXTPROC glad_glTexImage3DEXT;
#define glTexImage3DEXT glad_glTexImage3DEXT
GLAPI PFNGLTEXTURESTORAGE1DPROC glad_glTextureStorage1D;
#define glTextureStorage1D glad_glTextureStorage1D
GLAPI PFNGLGETPROGRAMINTERFACEIVPROC glad_glGetProgramInterfaceiv;
#define glGetProgramInterfaceiv glad_glGetProgramInterfaceiv
GLAPI PFNGLTEXCOORDPOINTEREXTPROC glad_glTexCoordPointerEXT;
#define glTexCoordPointerEXT glad_glTexCoordPointerEXT
GLAPI PFNGLMULTITEXGENIEXTPROC glad_glMultiTexGeniEXT;
#define glMultiTexGeniEXT glad_glMultiTexGeniEXT
GLAPI PFNGLVERTEXATTRIB2FNVPROC glad_glVertexAttrib2fNV;
#define glVertexAttrib2fNV glad_glVertexAttrib2fNV
GLAPI PFNGLNAMEDPROGRAMSTRINGEXTPROC glad_glNamedProgramStringEXT;
#define glNamedProgramStringEXT glad_glNamedProgramStringEXT
GLAPI PFNGLGETNUNIFORMDVPROC glad_glGetnUniformdv;
#define glGetnUniformdv glad_glGetnUniformdv
GLAPI PFNGLGETTEXTURESUBIMAGEPROC glad_glGetTextureSubImage;
#define glGetTextureSubImage glad_glGetTextureSubImage
GLAPI PFNGLVIEWPORTINDEXEDFVPROC glad_glViewportIndexedfv;
#define glViewportIndexedfv glad_glViewportIndexedfv
GLAPI PFNGLMULTITEXCOORD4SVARBPROC glad_glMultiTexCoord4svARB;
#define glMultiTexCoord4svARB glad_glMultiTexCoord4svARB
GLAPI PFNGLGETVERTEXATTRIBPOINTERVARBPROC glad_glGetVertexAttribPointervARB;
#define glGetVertexAttribPointervARB glad_glGetVertexAttribPointervARB
GLAPI PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEEXTPROC glad_glFlushMappedNamedBufferRangeEXT;
#define glFlushMappedNamedBufferRangeEXT glad_glFlushMappedNamedBufferRangeEXT
GLAPI PFNGLGETVERTEXATTRIBIIVPROC glad_glGetVertexAttribIiv;
#define glGetVertexAttribIiv glad_glGetVertexAttribIiv
GLAPI PFNGLUNIFORMUI64NVPROC glad_glUniformui64NV;
#define glUniformui64NV glad_glUniformui64NV
GLAPI PFNGLPROGRAMPARAMETERS4FVNVPROC glad_glProgramParameters4fvNV;
#define glProgramParameters4fvNV glad_glProgramParameters4fvNV
GLAPI PFNGLPATCHPARAMETERIPROC glad_glPatchParameteri;
#define glPatchParameteri glad_glPatchParameteri
GLAPI PFNGLVERTEXARRAYVERTEXBINDINGDIVISOREXTPROC glad_glVertexArrayVertexBindingDivisorEXT;
#define glVertexArrayVertexBindingDivisorEXT glad_glVertexArrayVertexBindingDivisorEXT
GLAPI PFNGLMULTITEXCOORD3SVARBPROC glad_glMultiTexCoord3svARB;
#define glMultiTexCoord3svARB glad_glMultiTexCoord3svARB
GLAPI PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC glad_glGetProgramLocalParameterfvARB;
#define glGetProgramLocalParameterfvARB glad_glGetProgramLocalParameterfvARB
GLAPI PFNGLVERTEXATTRIB4FNVPROC glad_glVertexAttrib4fNV;
#define glVertexAttrib4fNV glad_glVertexAttrib4fNV
GLAPI PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC glad_glGetNamedFramebufferAttachmentParameteriv;
#define glGetNamedFramebufferAttachmentParameteriv glad_glGetNamedFramebufferAttachmentParameteriv
GLAPI PFNGLGENVERTEXARRAYSAPPLEPROC glad_glGenVertexArraysAPPLE;
#define glGenVertexArraysAPPLE glad_glGenVertexArraysAPPLE
GLAPI PFNGLFRAMEBUFFERTEXTUREFACEARBPROC glad_glFramebufferTextureFaceARB;
#define glFramebufferTextureFaceARB glad_glFramebufferTextureFaceARB
GLAPI PFNGLTEXTURESTORAGE2DEXTPROC glad_glTextureStorage2DEXT;
#define glTextureStorage2DEXT glad_glTextureStorage2DEXT
GLAPI PFNGLVERTEXARRAYFOGCOORDOFFSETEXTPROC glad_glVertexArrayFogCoordOffsetEXT;
#define glVertexArrayFogCoordOffsetEXT glad_glVertexArrayFogCoordOffsetEXT
GLAPI PFNGLBLENDEQUATIONSEPARATEIARBPROC glad_glBlendEquationSeparateiARB;
#define glBlendEquationSeparateiARB glad_glBlendEquationSeparateiARB
GLAPI PFNGLCLIENTATTRIBDEFAULTEXTPROC glad_glClientAttribDefaultEXT;
#define glClientAttribDefaultEXT glad_glClientAttribDefaultEXT
GLAPI PFNGLPROGRAMUNIFORM2I64ARBPROC glad_glProgramUniform2i64ARB;
#define glProgramUniform2i64ARB glad_glProgramUniform2i64ARB
GLAPI PFNGLGETVERTEXATTRIBFVARBPROC glad_glGetVertexAttribfvARB;
#define glGetVertexAttribfvARB glad_glGetVertexAttribfvARB
GLAPI PFNGLPROGRAMUNIFORM2IEXTPROC glad_glProgramUniform2iEXT;
#define glProgramUniform2iEXT glad_glProgramUniform2iEXT
GLAPI PFNGLUNMAPNAMEDBUFFEREXTPROC glad_glUnmapNamedBufferEXT;
#define glUnmapNamedBufferEXT glad_glUnmapNamedBufferEXT
GLAPI PFNGLUNIFORM4FVARBPROC glad_glUniform4fvARB;
#define glUniform4fvARB glad_glUniform4fvARB
GLAPI PFNGLGETFLOATI_VEXTPROC glad_glGetFloati_vEXT;
#define glGetFloati_vEXT glad_glGetFloati_vEXT
GLAPI PFNGLBINDATTRIBLOCATIONARBPROC glad_glBindAttribLocationARB;
#define glBindAttribLocationARB glad_glBindAttribLocationARB
GLAPI PFNGLBUFFERADDRESSRANGENVPROC glad_glBufferAddressRangeNV;
#define glBufferAddressRangeNV glad_glBufferAddressRangeNV
GLAPI PFNGLMULTITEXENVIVEXTPROC glad_glMultiTexEnvivEXT;
#define glMultiTexEnvivEXT glad_glMultiTexEnvivEXT
GLAPI PFNGLPROGRAMUNIFORM1UI64VNVPROC glad_glProgramUniform1ui64vNV;
#define glProgramUniform1ui64vNV glad_glProgramUniform1ui64vNV
GLAPI PFNGLGETVERTEXATTRIBFVNVPROC glad_glGetVertexAttribfvNV;
#define glGetVertexAttribfvNV glad_glGetVertexAttribfvNV
GLAPI PFNGLUNIFORM2IPROC glad_glUniform2i;
#define glUniform2i glad_glUniform2i
GLAPI PFNGLUNIFORM2FPROC glad_glUniform2f;
#define glUniform2f glad_glUniform2f
GLAPI PFNGLUNIFORM2DPROC glad_glUniform2d;
#define glUniform2d glad_glUniform2d
GLAPI PFNGLFRAMEBUFFERTEXTUREFACEEXTPROC glad_glFramebufferTextureFaceEXT;
#define glFramebufferTextureFaceEXT glad_glFramebufferTextureFaceEXT
GLAPI PFNGLFRAMEBUFFERTEXTURELAYERPROC glad_glFramebufferTextureLayer;
#define glFramebufferTextureLayer glad_glFramebufferTextureLayer
GLAPI PFNGLGETVERTEXARRAYPOINTERI_VEXTPROC glad_glGetVertexArrayPointeri_vEXT;
#define glGetVertexArrayPointeri_vEXT glad_glGetVertexArrayPointeri_vEXT
GLAPI PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC glad_glProgramUniformMatrix2x3fvEXT;
#define glProgramUniformMatrix2x3fvEXT glad_glProgramUniformMatrix2x3fvEXT
GLAPI PFNGLPROGRAMUNIFORM2FVPROC glad_glProgramUniform2fv;
#define glProgramUniform2fv glad_glProgramUniform2fv
GLAPI PFNGLINTERPOLATEPATHSNVPROC glad_glInterpolatePathsNV;
#define glInterpolatePathsNV glad_glInterpolatePathsNV
GLAPI PFNGLCOPYMULTITEXSUBIMAGE3DEXTPROC glad_glCopyMultiTexSubImage3DEXT;
#define glCopyMultiTexSubImage3DEXT glad_glCopyMultiTexSubImage3DEXT
GLAPI PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC glad_glProgramUniformMatrix2x4dv;
#define glProgramUniformMatrix2x4dv glad_glProgramUniformMatrix2x4dv
GLAPI PFNGLPATHMEMORYGLYPHINDEXARRAYNVPROC glad_glPathMemoryGlyphIndexArrayNV;
#define glPathMemoryGlyphIndexArrayNV glad_glPathMemoryGlyphIndexArrayNV
GLAPI PFNGLDRAWARRAYSEXTPROC glad_glDrawArraysEXT;
#define glDrawArraysEXT glad_glDrawArraysEXT
GLAPI PFNGLBLENDEQUATIONEXTPROC glad_glBlendEquationEXT;
#define glBlendEquationEXT glad_glBlendEquationEXT
GLAPI PFNGLCOPYTEXTURESUBIMAGE2DEXTPROC glad_glCopyTextureSubImage2DEXT;
#define glCopyTextureSubImage2DEXT glad_glCopyTextureSubImage2DEXT
GLAPI PFNGLVERTEXATTRIB2DVNVPROC glad_glVertexAttrib2dvNV;
#define glVertexAttrib2dvNV glad_glVertexAttrib2dvNV
GLAPI PFNGLGETINTEGER64I_VPROC glad_glGetInteger64i_v;
#define glGetInteger64i_v glad_glGetInteger64i_v
GLAPI PFNGLBLENDFUNCINDEXEDAMDPROC glad_glBlendFuncIndexedAMD;
#define glBlendFuncIndexedAMD glad_glBlendFuncIndexedAMD
GLAPI PFNGLBLITFRAMEBUFFERPROC glad_glBlitFramebuffer;
#define glBlitFramebuffer glad_glBlitFramebuffer
GLAPI PFNGLISENABLEDIPROC glad_glIsEnabledi;
#define glIsEnabledi glad_glIsEnabledi
GLAPI PFNGLMATRIXTRANSLATEDEXTPROC glad_glMatrixTranslatedEXT;
#define glMatrixTranslatedEXT glad_glMatrixTranslatedEXT
GLAPI PFNGLMAKETEXTUREHANDLERESIDENTARBPROC glad_glMakeTextureHandleResidentARB;
#define glMakeTextureHandleResidentARB glad_glMakeTextureHandleResidentARB
GLAPI PFNGLUNIFORM3I64VNVPROC glad_glUniform3i64vNV;
#define glUniform3i64vNV glad_glUniform3i64vNV
GLAPI PFNGLPATHCOMMANDSNVPROC glad_glPathCommandsNV;
#define glPathCommandsNV glad_glPathCommandsNV
GLAPI PFNGLPROGRAMUNIFORMMATRIX3FVPROC glad_glProgramUniformMatrix3fv;
#define glProgramUniformMatrix3fv glad_glProgramUniformMatrix3fv
GLAPI PFNGLBINDVERTEXARRAYAPPLEPROC glad_glBindVertexArrayAPPLE;
#define glBindVertexArrayAPPLE glad_glBindVertexArrayAPPLE
GLAPI PFNGLOBJECTPTRLABELPROC glad_glObjectPtrLabel;
#define glObjectPtrLabel glad_glObjectPtrLabel
GLAPI PFNGLGETDEBUGMESSAGELOGPROC glad_glGetDebugMessageLog;
#define glGetDebugMessageLog glad_glGetDebugMessageLog
GLAPI PFNGLTRACKMATRIXNVPROC glad_glTrackMatrixNV;
#define glTrackMatrixNV glad_glTrackMatrixNV
GLAPI PFNGLBINDFRAGDATALOCATIONPROC glad_glBindFragDataLocation;
#define glBindFragDataLocation glad_glBindFragDataLocation
GLAPI PFNGLVERTEXATTRIBS4UBVNVPROC glad_glVertexAttribs4ubvNV;
#define glVertexAttribs4ubvNV glad_glVertexAttribs4ubvNV
GLAPI PFNGLBINDBUFFERBASENVPROC glad_glBindBufferBaseNV;
#define glBindBufferBaseNV glad_glBindBufferBaseNV
GLAPI PFNGLUNIFORM2I64VNVPROC glad_glUniform2i64vNV;
#define glUniform2i64vNV glad_glUniform2i64vNV
GLAPI PFNGLFRAMEBUFFERTEXTURELAYERARBPROC glad_glFramebufferTextureLayerARB;
#define glFramebufferTextureLayerARB glad_glFramebufferTextureLayerARB
GLAPI PFNGLMULTIDRAWELEMENTSINDIRECTPROC glad_glMultiDrawElementsIndirect;
#define glMultiDrawElementsIndirect glad_glMultiDrawElementsIndirect
GLAPI PFNGLPROGRAMUNIFORM1DEXTPROC glad_glProgramUniform1dEXT;
#define glProgramUniform1dEXT glad_glProgramUniform1dEXT
GLAPI PFNGLCOPYTEXSUBIMAGE3DPROC glad_glCopyTexSubImage3D;
#define glCopyTexSubImage3D glad_glCopyTexSubImage3D
GLAPI PFNGLCREATESHADEROBJECTARBPROC glad_glCreateShaderObjectARB;
#define glCreateShaderObjectARB glad_glCreateShaderObjectARB
GLAPI PFNGLGETSTAGEINDEXNVPROC glad_glGetStageIndexNV;
#define glGetStageIndexNV glad_glGetStageIndexNV
GLAPI PFNGLGETNAMEDBUFFERPOINTERVEXTPROC glad_glGetNamedBufferPointervEXT;
#define glGetNamedBufferPointervEXT glad_glGetNamedBufferPointervEXT
GLAPI PFNGLGETCOMMANDHEADERNVPROC glad_glGetCommandHeaderNV;
#define glGetCommandHeaderNV glad_glGetCommandHeaderNV
GLAPI PFNGLVERTEXATTRIB1SVARBPROC glad_glVertexAttrib1svARB;
#define glVertexAttrib1svARB glad_glVertexAttrib1svARB
GLAPI PFNGLMATRIXMULTTRANSPOSEFEXTPROC glad_glMatrixMultTransposefEXT;
#define glMatrixMultTransposefEXT glad_glMatrixMultTransposefEXT
GLAPI PFNGLGETACTIVEUNIFORMBLOCKIVPROC glad_glGetActiveUniformBlockiv;
#define glGetActiveUniformBlockiv glad_glGetActiveUniformBlockiv
GLAPI PFNGLPROGRAMUNIFORM2DPROC glad_glProgramUniform2d;
#define glProgramUniform2d glad_glProgramUniform2d
GLAPI PFNGLPROGRAMUNIFORM2FPROC glad_glProgramUniform2f;
#define glProgramUniform2f glad_glProgramUniform2f
GLAPI PFNGLISFRAMEBUFFERPROC glad_glIsFramebuffer;
#define glIsFramebuffer glad_glIsFramebuffer
GLAPI PFNGLCLIENTACTIVETEXTUREARBPROC glad_glClientActiveTextureARB;
#define glClientActiveTextureARB glad_glClientActiveTextureARB
GLAPI PFNGLDRAWELEMENTSINSTANCEDPROC glad_glDrawElementsInstanced;
#define glDrawElementsInstanced glad_glDrawElementsInstanced
GLAPI PFNGLBINDFRAMEBUFFERPROC glad_glBindFramebuffer;
#define glBindFramebuffer glad_glBindFramebuffer
GLAPI PFNGLVERTEXATTRIBS2DVNVPROC glad_glVertexAttribs2dvNV;
#define glVertexAttribs2dvNV glad_glVertexAttribs2dvNV
GLAPI PFNGLGETPROGRAMRESOURCEFVNVPROC glad_glGetProgramResourcefvNV;
#define glGetProgramResourcefvNV glad_glGetProgramResourcefvNV
GLAPI PFNGLENDTRANSFORMFEEDBACKNVPROC glad_glEndTransformFeedbackNV;
#define glEndTransformFeedbackNV glad_glEndTransformFeedbackNV
GLAPI PFNGLPROGRAMUNIFORM4IVEXTPROC glad_glProgramUniform4ivEXT;
#define glProgramUniform4ivEXT glad_glProgramUniform4ivEXT
GLAPI PFNGLPROGRAMUNIFORM3UI64VNVPROC glad_glProgramUniform3ui64vNV;
#define glProgramUniform3ui64vNV glad_glProgramUniform3ui64vNV
GLAPI PFNGLCOPYMULTITEXIMAGE1DEXTPROC glad_glCopyMultiTexImage1DEXT;
#define glCopyMultiTexImage1DEXT glad_glCopyMultiTexImage1DEXT
GLAPI PFNGLCOLORMASKINDEXEDEXTPROC glad_glColorMaskIndexedEXT;
#define glColorMaskIndexedEXT glad_glColorMaskIndexedEXT
GLAPI PFNGLUNIFORMMATRIX2X4DVPROC glad_glUniformMatrix2x4dv;
#define glUniformMatrix2x4dv glad_glUniformMatrix2x4dv
GLAPI PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC glad_glGetProgramResourceLocationIndex;
#define glGetProgramResourceLocationIndex glad_glGetProgramResourceLocationIndex
GLAPI PFNGLVIEWPORTPROC glad_glViewport;
#define glViewport glad_glViewport
GLAPI PFNGLMATRIXLOADFEXTPROC glad_glMatrixLoadfEXT;
#define glMatrixLoadfEXT glad_glMatrixLoadfEXT
GLAPI PFNGLTEXSTORAGE2DMULTISAMPLEPROC glad_glTexStorage2DMultisample;
#define glTexStorage2DMultisample glad_glTexStorage2DMultisample
GLAPI PFNGLVERTEXATTRIBL1DVEXTPROC glad_glVertexAttribL1dvEXT;
#define glVertexAttribL1dvEXT glad_glVertexAttribL1dvEXT
GLAPI PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC glad_glGetActiveSubroutineUniformiv;
#define glGetActiveSubroutineUniformiv glad_glGetActiveSubroutineUniformiv
GLAPI PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC glad_glCompressedTexSubImage2DARB;
#define glCompressedTexSubImage2DARB glad_glCompressedTexSubImage2DARB
GLAPI PFNGLTEXBUFFERPROC glad_glTexBuffer;
#define glTexBuffer glad_glTexBuffer
GLAPI PFNGLPIXELSTOREIPROC glad_glPixelStorei;
#define glPixelStorei glad_glPixelStorei
GLAPI PFNGLVALIDATEPROGRAMPROC glad_glValidateProgram;
#define glValidateProgram glad_glValidateProgram
GLAPI PFNGLPIXELSTOREFPROC glad_glPixelStoref;
#define glPixelStoref glad_glPixelStoref
GLAPI PFNGLPATHCOVERDEPTHFUNCNVPROC glad_glPathCoverDepthFuncNV;
#define glPathCoverDepthFuncNV glad_glPathCoverDepthFuncNV
GLAPI PFNGLPROGRAMUNIFORM1UIEXTPROC glad_glProgramUniform1uiEXT;
#define glProgramUniform1uiEXT glad_glProgramUniform1uiEXT
GLAPI PFNGLVERTEXATTRIBS2FVNVPROC glad_glVertexAttribs2fvNV;
#define glVertexAttribs2fvNV glad_glVertexAttribs2fvNV
GLAPI PFNGLGETPROGRAMPARAMETERDVNVPROC glad_glGetProgramParameterdvNV;
#define glGetProgramParameterdvNV glad_glGetProgramParameterdvNV
GLAPI PFNGLACTIVEVARYINGNVPROC glad_glActiveVaryingNV;
#define glActiveVaryingNV glad_glActiveVaryingNV
GLAPI PFNGLGETMULTITEXGENDVEXTPROC glad_glGetMultiTexGendvEXT;
#define glGetMultiTexGendvEXT glad_glGetMultiTexGendvEXT
GLAPI PFNGLCLEARNAMEDBUFFERSUBDATAEXTPROC glad_glClearNamedBufferSubDataEXT;
#define glClearNamedBufferSubDataEXT glad_glClearNamedBufferSubDataEXT
GLAPI PFNGLGETTEXTURESAMPLERHANDLEARBPROC glad_glGetTextureSamplerHandleARB;
#define glGetTextureSamplerHandleARB glad_glGetTextureSamplerHandleARB
GLAPI PFNGLBINDTEXTUREPROC glad_glBindTexture;
#define glBindTexture glad_glBindTexture
GLAPI PFNGLGETACTIVEATTRIBARBPROC glad_glGetActiveAttribARB;
#define glGetActiveAttribARB glad_glGetActiveAttribARB
GLAPI PFNGLDETACHSHADERPROC glad_glDetachShader;
#define glDetachShader glad_glDetachShader
GLAPI PFNGLUNIFORMMATRIX3X4DVPROC glad_glUniformMatrix3x4dv;
#define glUniformMatrix3x4dv glad_glUniformMatrix3x4dv
GLAPI PFNGLVERTEXATTRIB4FARBPROC glad_glVertexAttrib4fARB;
#define glVertexAttrib4fARB glad_glVertexAttrib4fARB
GLAPI PFNGLGETPROGRAMRESOURCELOCATIONPROC glad_glGetProgramResourceLocation;
#define glGetProgramResourceLocation glad_glGetProgramResourceLocation
GLAPI PFNGLVERTEXARRAYATTRIBLFORMATPROC glad_glVertexArrayAttribLFormat;
#define glVertexArrayAttribLFormat glad_glVertexArrayAttribLFormat
GLAPI PFNGLNAMEDRENDERBUFFERSTORAGEPROC glad_glNamedRenderbufferStorage;
#define glNamedRenderbufferStorage glad_glNamedRenderbufferStorage
GLAPI PFNGLVERTEXARRAYVERTEXATTRIBOFFSETEXTPROC glad_glVertexArrayVertexAttribOffsetEXT;
#define glVertexArrayVertexAttribOffsetEXT glad_glVertexArrayVertexAttribOffsetEXT
GLAPI PFNGLINDEXFORMATNVPROC glad_glIndexFormatNV;
#define glIndexFormatNV glad_glIndexFormatNV
GLAPI PFNGLSTENCILOPSEPARATEATIPROC glad_glStencilOpSeparateATI;
#define glStencilOpSeparateATI glad_glStencilOpSeparateATI
GLAPI PFNGLDRAWELEMENTSBASEVERTEXPROC glad_glDrawElementsBaseVertex;
#define glDrawElementsBaseVertex glad_glDrawElementsBaseVertex
GLAPI PFNGLVERTEXATTRIB1FNVPROC glad_glVertexAttrib1fNV;
#define glVertexAttrib1fNV glad_glVertexAttrib1fNV
GLAPI PFNGLSAMPLECOVERAGEPROC glad_glSampleCoverage;
#define glSampleCoverage glad_glSampleCoverage
GLAPI PFNGLTEXSTORAGE1DPROC glad_glTexStorage1D;
#define glTexStorage1D glad_glTexStorage1D
GLAPI PFNGLVERTEXATTRIBI3IEXTPROC glad_glVertexAttribI3iEXT;
#define glVertexAttribI3iEXT glad_glVertexAttribI3iEXT
GLAPI PFNGLMAKETEXTUREHANDLENONRESIDENTARBPROC glad_glMakeTextureHandleNonResidentARB;
#define glMakeTextureHandleNonResidentARB glad_glMakeTextureHandleNonResidentARB
GLAPI PFNGLPROGRAMUNIFORMMATRIX2DVEXTPROC glad_glProgramUniformMatrix2dvEXT;
#define glProgramUniformMatrix2dvEXT glad_glProgramUniformMatrix2dvEXT
GLAPI PFNGLGETUNIFORMIVPROC glad_glGetUniformiv;
#define glGetUniformiv glad_glGetUniformiv
GLAPI PFNGLRENDERBUFFERSTORAGEPROC glad_glRenderbufferStorage;
#define glRenderbufferStorage glad_glRenderbufferStorage
GLAPI PFNGLUNIFORM4UIPROC glad_glUniform4ui;
#define glUniform4ui glad_glUniform4ui
GLAPI PFNGLDISABLEVERTEXARRAYATTRIBEXTPROC glad_glDisableVertexArrayAttribEXT;
#define glDisableVertexArrayAttribEXT glad_glDisableVertexArrayAttribEXT
GLAPI PFNGLVALIDATEPROGRAMARBPROC glad_glValidateProgramARB;
#define glValidateProgramARB glad_glValidateProgramARB
GLAPI PFNGLVERTEXATTRIBPOINTERARBPROC glad_glVertexAttribPointerARB;
#define glVertexAttribPointerARB glad_glVertexAttribPointerARB
GLAPI PFNGLDEBUGMESSAGEINSERTARBPROC glad_glDebugMessageInsertARB;
#define glDebugMessageInsertARB glad_glDebugMessageInsertARB
GLAPI PFNGLDELETEFRAMEBUFFERSEXTPROC glad_glDeleteFramebuffersEXT;
#define glDeleteFramebuffersEXT glad_glDeleteFramebuffersEXT
GLAPI PFNGLGETTRANSFORMFEEDBACKI_VPROC glad_glGetTransformFeedbacki_v;
#define glGetTransformFeedbacki_v glad_glGetTransformFeedbacki_v
GLAPI PFNGLVERTEXATTRIB1FVNVPROC glad_glVertexAttrib1fvNV;
#define glVertexAttrib1fvNV glad_glVertexAttrib1fvNV
GLAPI PFNGLISQUERYARBPROC glad_glIsQueryARB;
#define glIsQueryARB glad_glIsQueryARB
GLAPI PFNGLSCISSORINDEXEDPROC glad_glScissorIndexed;
#define glScissorIndexed glad_glScissorIndexed
GLAPI PFNGLGENFRAMEBUFFERSEXTPROC glad_glGenFramebuffersEXT;
#define glGenFramebuffersEXT glad_glGenFramebuffersEXT
GLAPI PFNGLVERTEXARRAYCOLOROFFSETEXTPROC glad_glVertexArrayColorOffsetEXT;
#define glVertexArrayColorOffsetEXT glad_glVertexArrayColorOffsetEXT
GLAPI PFNGLMULTITEXCOORD3IVARBPROC glad_glMultiTexCoord3ivARB;
#define glMultiTexCoord3ivARB glad_glMultiTexCoord3ivARB
GLAPI PFNGLISTEXTUREHANDLERESIDENTARBPROC glad_glIsTextureHandleResidentARB;
#define glIsTextureHandleResidentARB glad_glIsTextureHandleResidentARB
GLAPI PFNGLMAPNAMEDBUFFERRANGEEXTPROC glad_glMapNamedBufferRangeEXT;
#define glMapNamedBufferRangeEXT glad_glMapNamedBufferRangeEXT
GLAPI PFNGLCREATESHADERPROGRAMVPROC glad_glCreateShaderProgramv;
#define glCreateShaderProgramv glad_glCreateShaderProgramv
GLAPI PFNGLGETQUERYOBJECTIVPROC glad_glGetQueryObjectiv;
#define glGetQueryObjectiv glad_glGetQueryObjectiv
GLAPI PFNGLVERTEXATTRIB3DVNVPROC glad_glVertexAttrib3dvNV;
#define glVertexAttrib3dvNV glad_glVertexAttrib3dvNV
GLAPI PFNGLMULTITEXCOORD3SARBPROC glad_glMultiTexCoord3sARB;
#define glMultiTexCoord3sARB glad_glMultiTexCoord3sARB
GLAPI PFNGLGENERATEMIPMAPPROC glad_glGenerateMipmap;
#define glGenerateMipmap glad_glGenerateMipmap
GLAPI PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC glad_glCompressedTextureSubImage2D;
#define glCompressedTextureSubImage2D glad_glCompressedTextureSubImage2D
GLAPI PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glad_glNamedRenderbufferStorageMultisampleEXT;
#define glNamedRenderbufferStorageMultisampleEXT glad_glNamedRenderbufferStorageMultisampleEXT
GLAPI PFNGLBLENDEQUATIONINDEXEDAMDPROC glad_glBlendEquationIndexedAMD;
#define glBlendEquationIndexedAMD glad_glBlendEquationIndexedAMD
GLAPI PFNGLPOINTPARAMETERIPROC glad_glPointParameteri;
#define glPointParameteri glad_glPointParameteri
GLAPI PFNGLUNMAPBUFFERPROC glad_glUnmapBuffer;
#define glUnmapBuffer glad_glUnmapBuffer
GLAPI PFNGLVERTEXATTRIB3SVPROC glad_glVertexAttrib3sv;
#define glVertexAttrib3sv glad_glVertexAttrib3sv
GLAPI PFNGLPOINTPARAMETERFPROC glad_glPointParameterf;
#define glPointParameterf glad_glPointParameterf
GLAPI PFNGLVERTEXATTRIB1SARBPROC glad_glVertexAttrib1sARB;
#define glVertexAttrib1sARB glad_glVertexAttrib1sARB
GLAPI PFNGLVERTEXATTRIB4SVPROC glad_glVertexAttrib4sv;
#define glVertexAttrib4sv glad_glVertexAttrib4sv
GLAPI PFNGLPROGRAMPARAMETER4DVNVPROC glad_glProgramParameter4dvNV;
#define glProgramParameter4dvNV glad_glProgramParameter4dvNV
GLAPI PFNGLTEXTUREPARAMETERIUIVEXTPROC glad_glTextureParameterIuivEXT;
#define glTextureParameterIuivEXT glad_glTextureParameterIuivEXT
GLAPI PFNGLDELETERENDERBUFFERSEXTPROC glad_glDeleteRenderbuffersEXT;
#define glDeleteRenderbuffersEXT glad_glDeleteRenderbuffersEXT
GLAPI PFNGLVERTEXATTRIB1SVNVPROC glad_glVertexAttrib1svNV;
#define glVertexAttrib1svNV glad_glVertexAttrib1svNV
GLAPI PFNGLPROGRAMUNIFORM2UI64VNVPROC glad_glProgramUniform2ui64vNV;
#define glProgramUniform2ui64vNV glad_glProgramUniform2ui64vNV
GLAPI PFNGLRELEASESHADERCOMPILERPROC glad_glReleaseShaderCompiler;
#define glReleaseShaderCompiler glad_glReleaseShaderCompiler
GLAPI PFNGLREADPIXELSPROC glad_glReadPixels;
#define glReadPixels glad_glReadPixels
GLAPI PFNGLSTATECAPTURENVPROC glad_glStateCaptureNV;
#define glStateCaptureNV glad_glStateCaptureNV
GLAPI PFNGLGETDOUBLEI_VEXTPROC glad_glGetDoublei_vEXT;
#define glGetDoublei_vEXT glad_glGetDoublei_vEXT
GLAPI PFNGLMULTITEXCOORD4DARBPROC glad_glMultiTexCoord4dARB;
#define glMultiTexCoord4dARB glad_glMultiTexCoord4dARB
GLAPI PFNGLREADNPIXELSARBPROC glad_glReadnPixelsARB;
#define glReadnPixelsARB glad_glReadnPixelsARB
GLAPI PFNGLMULTITEXBUFFEREXTPROC glad_glMultiTexBufferEXT;
#define glMultiTexBufferEXT glad_glMultiTexBufferEXT
GLAPI PFNGLUSEPROGRAMSTAGESPROC glad_glUseProgramStages;
#define glUseProgramStages glad_glUseProgramStages
GLAPI PFNGLFRAMEBUFFERTEXTUREARBPROC glad_glFramebufferTextureARB;
#define glFramebufferTextureARB glad_glFramebufferTextureARB
GLAPI PFNGLUNIFORM3I64ARBPROC glad_glUniform3i64ARB;
#define glUniform3i64ARB glad_glUniform3i64ARB
GLAPI PFNGLPROGRAMUNIFORM2DEXTPROC glad_glProgramUniform2dEXT;
#define glProgramUniform2dEXT glad_glProgramUniform2dEXT
GLAPI PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC glad_glNamedFramebufferDrawBuffer;
#define glNamedFramebufferDrawBuffer glad_glNamedFramebufferDrawBuffer
GLAPI PFNGLPROGRAMUNIFORMHANDLEUI64ARBPROC glad_glProgramUniformHandleui64ARB;
#define glProgramUniformHandleui64ARB glad_glProgramUniformHandleui64ARB
GLAPI PFNGLNAMEDCOPYBUFFERSUBDATAEXTPROC glad_glNamedCopyBufferSubDataEXT;
#define glNamedCopyBufferSubDataEXT glad_glNamedCopyBufferSubDataEXT
GLAPI PFNGLGETVERTEXATTRIBLDVPROC glad_glGetVertexAttribLdv;
#define glGetVertexAttribLdv glad_glGetVertexAttribLdv
GLAPI PFNGLGETNUNIFORMUIVPROC glad_glGetnUniformuiv;
#define glGetnUniformuiv glad_glGetnUniformuiv
GLAPI PFNGLTEXSTORAGE3DMULTISAMPLEPROC glad_glTexStorage3DMultisample;
#define glTexStorage3DMultisample glad_glTexStorage3DMultisample
GLAPI PFNGLVERTEXATTRIB3DNVPROC glad_glVertexAttrib3dNV;
#define glVertexAttrib3dNV glad_glVertexAttrib3dNV
GLAPI PFNGLTEXTURESTORAGE1DEXTPROC glad_glTextureStorage1DEXT;
#define glTextureStorage1DEXT glad_glTextureStorage1DEXT
GLAPI PFNGLGENBUFFERSPROC glad_glGenBuffers;
#define glGenBuffers glad_glGenBuffers
GLAPI PFNGLEXECUTEPROGRAMNVPROC glad_glExecuteProgramNV;
#define glExecuteProgramNV glad_glExecuteProgramNV
GLAPI PFNGLDELETEPROGRAMSNVPROC glad_glDeleteProgramsNV;
#define glDeleteProgramsNV glad_glDeleteProgramsNV
GLAPI PFNGLNORMALFORMATNVPROC glad_glNormalFormatNV;
#define glNormalFormatNV glad_glNormalFormatNV
GLAPI PFNGLVERTEXATTRIB3FNVPROC glad_glVertexAttrib3fNV;
#define glVertexAttrib3fNV glad_glVertexAttrib3fNV
GLAPI PFNGLCLEARNAMEDFRAMEBUFFERIVPROC glad_glClearNamedFramebufferiv;
#define glClearNamedFramebufferiv glad_glClearNamedFramebufferiv
GLAPI PFNGLTEXIMAGE3DPROC glad_glTexImage3D;
#define glTexImage3D glad_glTexImage3D
GLAPI PFNGLGETOBJECTPARAMETERFVARBPROC glad_glGetObjectParameterfvARB;
#define glGetObjectParameterfvARB glad_glGetObjectParameterfvARB
GLAPI PFNGLCLEARNAMEDFRAMEBUFFERFVPROC glad_glClearNamedFramebufferfv;
#define glClearNamedFramebufferfv glad_glClearNamedFramebufferfv
GLAPI PFNGLTEXTURESTORAGE2DMULTISAMPLEEXTPROC glad_glTextureStorage2DMultisampleEXT;
#define glTextureStorage2DMultisampleEXT glad_glTextureStorage2DMultisampleEXT
GLAPI PFNGLMATRIXPOPEXTPROC glad_glMatrixPopEXT;
#define glMatrixPopEXT glad_glMatrixPopEXT
GLAPI PFNGLMULTITEXENVIEXTPROC glad_glMultiTexEnviEXT;
#define glMultiTexEnviEXT glad_glMultiTexEnviEXT
GLAPI PFNGLVERTEXATTRIBI2IEXTPROC glad_glVertexAttribI2iEXT;
#define glVertexAttribI2iEXT glad_glVertexAttribI2iEXT
GLAPI PFNGLCLAMPCOLORPROC glad_glClampColor;
#define glClampColor glad_glClampColor
GLAPI PFNGLVERTEXATTRIBI2IVEXTPROC glad_glVertexAttribI2ivEXT;
#define glVertexAttribI2ivEXT glad_glVertexAttribI2ivEXT
GLAPI PFNGLDELETESTATESNVPROC glad_glDeleteStatesNV;
#define glDeleteStatesNV glad_glDeleteStatesNV
GLAPI PFNGLGETBUFFERPARAMETERI64VPROC glad_glGetBufferParameteri64v;
#define glGetBufferParameteri64v glad_glGetBufferParameteri64v
GLAPI PFNGLPROGRAMUNIFORM4DVPROC glad_glProgramUniform4dv;
#define glProgramUniform4dv glad_glProgramUniform4dv
GLAPI PFNGLEDGEFLAGPOINTEREXTPROC glad_glEdgeFlagPointerEXT;
#define glEdgeFlagPointerEXT glad_glEdgeFlagPointerEXT
GLAPI PFNGLMAKEBUFFERNONRESIDENTNVPROC glad_glMakeBufferNonResidentNV;
#define glMakeBufferNonResidentNV glad_glMakeBufferNonResidentNV
GLAPI PFNGLUNIFORM1UI64ARBPROC glad_glUniform1ui64ARB;
#define glUniform1ui64ARB glad_glUniform1ui64ARB
GLAPI PFNGLSTENCILSTROKEPATHNVPROC glad_glStencilStrokePathNV;
#define glStencilStrokePathNV glad_glStencilStrokePathNV
GLAPI PFNGLPROGRAMUNIFORM3UIVPROC glad_glProgramUniform3uiv;
#define glProgramUniform3uiv glad_glProgramUniform3uiv
GLAPI PFNGLMULTITEXCOORD1SARBPROC glad_glMultiTexCoord1sARB;
#define glMultiTexCoord1sARB glad_glMultiTexCoord1sARB
GLAPI PFNGLUNIFORM4I64ARBPROC glad_glUniform4i64ARB;
#define glUniform4i64ARB glad_glUniform4i64ARB
GLAPI PFNGLPROGRAMUNIFORM2UIEXTPROC glad_glProgramUniform2uiEXT;
#define glProgramUniform2uiEXT glad_glProgramUniform2uiEXT
GLAPI PFNGLCOMPRESSEDMULTITEXIMAGE3DEXTPROC glad_glCompressedMultiTexImage3DEXT;
#define glCompressedMultiTexImage3DEXT glad_glCompressedMultiTexImage3DEXT
GLAPI PFNGLNAMEDPROGRAMLOCALPARAMETER4DEXTPROC glad_glNamedProgramLocalParameter4dEXT;
#define glNamedProgramLocalParameter4dEXT glad_glNamedProgramLocalParameter4dEXT
GLAPI PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC glad_glProgramUniformMatrix4x3fvEXT;
#define glProgramUniformMatrix4x3fvEXT glad_glProgramUniformMatrix4x3fvEXT
GLAPI PFNGLPROGRAMUNIFORM3DEXTPROC glad_glProgramUniform3dEXT;
#define glProgramUniform3dEXT glad_glProgramUniform3dEXT
GLAPI PFNGLDISPATCHCOMPUTEGROUPSIZEARBPROC glad_glDispatchComputeGroupSizeARB;
#define glDispatchComputeGroupSizeARB glad_glDispatchComputeGroupSizeARB
GLAPI PFNGLVERTEXATTRIBL1UI64ARBPROC glad_glVertexAttribL1ui64ARB;
#define glVertexAttribL1ui64ARB glad_glVertexAttribL1ui64ARB
GLAPI PFNGLCREATERENDERBUFFERSPROC glad_glCreateRenderbuffers;
#define glCreateRenderbuffers glad_glCreateRenderbuffers
GLAPI PFNGLVERTEXATTRIB4NUSVPROC glad_glVertexAttrib4Nusv;
#define glVertexAttrib4Nusv glad_glVertexAttrib4Nusv
GLAPI PFNGLDEPTHFUNCPROC glad_glDepthFunc;
#define glDepthFunc glad_glDepthFunc
GLAPI PFNGLENABLECLIENTSTATEINDEXEDEXTPROC glad_glEnableClientStateIndexedEXT;
#define glEnableClientStateIndexedEXT glad_glEnableClientStateIndexedEXT
GLAPI PFNGLBLENDFUNCIPROC glad_glBlendFunci;
#define glBlendFunci glad_glBlendFunci
GLAPI PFNGLBUFFERSTORAGEPROC glad_glBufferStorage;
#define glBufferStorage glad_glBufferStorage
GLAPI PFNGLMAKENAMEDBUFFERNONRESIDENTNVPROC glad_glMakeNamedBufferNonResidentNV;
#define glMakeNamedBufferNonResidentNV glad_glMakeNamedBufferNonResidentNV
GLAPI PFNGLPROGRAMUNIFORM4FEXTPROC glad_glProgramUniform4fEXT;
#define glProgramUniform4fEXT glad_glProgramUniform4fEXT
GLAPI PFNGLGETUNIFORMLOCATIONPROC glad_glGetUniformLocation;
#define glGetUniformLocation glad_glGetUniformLocation
GLAPI PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC glad_glNamedFramebufferDrawBuffers;
#define glNamedFramebufferDrawBuffers glad_glNamedFramebufferDrawBuffers
GLAPI PFNGLUNIFORM2UI64VNVPROC glad_glUniform2ui64vNV;
#define glUniform2ui64vNV glad_glUniform2ui64vNV
GLAPI PFNGLUNIFORM4FVPROC glad_glUniform4fv;
#define glUniform4fv glad_glUniform4fv
GLAPI PFNGLNAMEDFRAMEBUFFERTEXTUREFACEEXTPROC glad_glNamedFramebufferTextureFaceEXT;
#define glNamedFramebufferTextureFaceEXT glad_glNamedFramebufferTextureFaceEXT
GLAPI PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC glad_glCompressedTextureSubImage3D;
#define glCompressedTextureSubImage3D glad_glCompressedTextureSubImage3D
GLAPI PFNGLGETPROGRAMSTAGEIVPROC glad_glGetProgramStageiv;
#define glGetProgramStageiv glad_glGetProgramStageiv
GLAPI PFNGLPRIORITIZETEXTURESEXTPROC glad_glPrioritizeTexturesEXT;
#define glPrioritizeTexturesEXT glad_glPrioritizeTexturesEXT
GLAPI PFNGLENDCONDITIONALRENDERPROC glad_glEndConditionalRender;
#define glEndConditionalRender glad_glEndConditionalRender
GLAPI PFNGLTEXSUBIMAGE2DEXTPROC glad_glTexSubImage2DEXT;
#define glTexSubImage2DEXT glad_glTexSubImage2DEXT
GLAPI PFNGLPROGRAMUNIFORM2UIVPROC glad_glProgramUniform2uiv;
#define glProgramUniform2uiv glad_glProgramUniform2uiv
GLAPI PFNGLGETQUERYOBJECTUIVPROC glad_glGetQueryObjectuiv;
#define glGetQueryObjectuiv glad_glGetQueryObjectuiv
GLAPI PFNGLGENERATEMULTITEXMIPMAPEXTPROC glad_glGenerateMultiTexMipmapEXT;
#define glGenerateMultiTexMipmapEXT glad_glGenerateMultiTexMipmapEXT
GLAPI PFNGLVERTEXATTRIB4IVPROC glad_glVertexAttrib4iv;
#define glVertexAttrib4iv glad_glVertexAttrib4iv
GLAPI PFNGLPROGRAMUNIFORM1UIVPROC glad_glProgramUniform1uiv;
#define glProgramUniform1uiv glad_glProgramUniform1uiv
GLAPI PFNGLFRAMEBUFFERTEXTUREPROC glad_glFramebufferTexture;
#define glFramebufferTexture glad_glFramebufferTexture
GLAPI PFNGLMULTITEXENVFEXTPROC glad_glMultiTexEnvfEXT;
#define glMultiTexEnvfEXT glad_glMultiTexEnvfEXT
GLAPI PFNGLVERTEXATTRIBL1DVPROC glad_glVertexAttribL1dv;
#define glVertexAttribL1dv glad_glVertexAttribL1dv
GLAPI PFNGLNAMEDPROGRAMLOCALPARAMETER4FVEXTPROC glad_glNamedProgramLocalParameter4fvEXT;
#define glNamedProgramLocalParameter4fvEXT glad_glNamedProgramLocalParameter4fvEXT
GLAPI PFNGLPOINTPARAMETERFVSGISPROC glad_glPointParameterfvSGIS;
#define glPointParameterfvSGIS glad_glPointParameterfvSGIS
GLAPI PFNGLDEPTHMASKPROC glad_glDepthMask;
#define glDepthMask glad_glDepthMask
GLAPI PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC glad_glProgramUniformMatrix2x4fv;
#define glProgramUniformMatrix2x4fv glad_glProgramUniformMatrix2x4fv
GLAPI PFNGLAREPROGRAMSRESIDENTNVPROC glad_glAreProgramsResidentNV;
#define glAreProgramsResidentNV glad_glAreProgramsResidentNV
GLAPI PFNGLFLUSHMAPPEDBUFFERRANGEAPPLEPROC glad_glFlushMappedBufferRangeAPPLE;
#define glFlushMappedBufferRangeAPPLE glad_glFlushMappedBufferRangeAPPLE
GLAPI PFNGLVERTEXATTRIBS1DVNVPROC glad_glVertexAttribs1dvNV;
#define glVertexAttribs1dvNV glad_glVertexAttribs1dvNV
GLAPI PFNGLPROGRAMUNIFORM4DEXTPROC glad_glProgramUniform4dEXT;
#define glProgramUniform4dEXT glad_glProgramUniform4dEXT
GLAPI PFNGLUNIFORMMATRIX2FVPROC glad_glUniformMatrix2fv;
#define glUniformMatrix2fv glad_glUniformMatrix2fv
GLAPI PFNGLSAMPLEMASKINDEXEDNVPROC glad_glSampleMaskIndexedNV;
#define glSampleMaskIndexedNV glad_glSampleMaskIndexedNV
GLAPI PFNGLPROGRAMUNIFORM3I64ARBPROC glad_glProgramUniform3i64ARB;
#define glProgramUniform3i64ARB glad_glProgramUniform3i64ARB
GLAPI PFNGLGETTEXTUREPARAMETERFVEXTPROC glad_glGetTextureParameterfvEXT;
#define glGetTextureParameterfvEXT glad_glGetTextureParameterfvEXT
GLAPI PFNGLGETFLOATVPROC glad_glGetFloatv;
#define glGetFloatv glad_glGetFloatv
GLAPI PFNGLMATRIXLOAD3X3FNVPROC glad_glMatrixLoad3x3fNV;
#define glMatrixLoad3x3fNV glad_glMatrixLoad3x3fNV
GLAPI PFNGLMULTITEXCOORD2FARBPROC glad_glMultiTexCoord2fARB;
#define glMultiTexCoord2fARB glad_glMultiTexCoord2fARB
GLAPI PFNGLISTRANSFORMFEEDBACKNVPROC glad_glIsTransformFeedbackNV;
#define glIsTransformFeedbackNV glad_glIsTransformFeedbackNV
GLAPI PFNGLVERTEXATTRIBDIVISORARBPROC glad_glVertexAttribDivisorARB;
#define glVertexAttribDivisorARB glad_glVertexAttribDivisorARB
GLAPI PFNGLVERTEXATTRIBI3IVPROC glad_glVertexAttribI3iv;
#define glVertexAttribI3iv glad_glVertexAttribI3iv
GLAPI PFNGLGETNCOMPRESSEDTEXIMAGEARBPROC glad_glGetnCompressedTexImageARB;
#define glGetnCompressedTexImageARB glad_glGetnCompressedTexImageARB
GLAPI PFNGLGETINTEGERVPROC glad_glGetIntegerv;
#define glGetIntegerv glad_glGetIntegerv
GLAPI PFNGLMATRIXLOADDEXTPROC glad_glMatrixLoaddEXT;
#define glMatrixLoaddEXT glad_glMatrixLoaddEXT
GLAPI PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC glad_glFramebufferTextureLayerEXT;
#define glFramebufferTextureLayerEXT glad_glFramebufferTextureLayerEXT
GLAPI PFNGLPROGRAMUNIFORMMATRIX3DVPROC glad_glProgramUniformMatrix3dv;
#define glProgramUniformMatrix3dv glad_glProgramUniformMatrix3dv
GLAPI PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glad_glRenderbufferStorageMultisample;
#define glRenderbufferStorageMultisample glad_glRenderbufferStorageMultisample
GLAPI PFNGLISQUERYPROC glad_glIsQuery;
#define glIsQuery glad_glIsQuery
GLAPI PFNGLTEXIMAGE2DPROC glad_glTexImage2D;
#define glTexImage2D glad_glTexImage2D
GLAPI PFNGLENABLEINDEXEDEXTPROC glad_glEnableIndexedEXT;
#define glEnableIndexedEXT glad_glEnableIndexedEXT
GLAPI PFNGLUNIFORM2UIEXTPROC glad_glUniform2uiEXT;
#define glUniform2uiEXT glad_glUniform2uiEXT
GLAPI PFNGLMULTITEXCOORD1FARBPROC glad_glMultiTexCoord1fARB;
#define glMultiTexCoord1fARB glad_glMultiTexCoord1fARB
GLAPI PFNGLVERTEXATTRIB4NUBVPROC glad_glVertexAttrib4Nubv;
#define glVertexAttrib4Nubv glad_glVertexAttrib4Nubv
GLAPI PFNGLVERTEXARRAYVERTEXATTRIBIOFFSETEXTPROC glad_glVertexArrayVertexAttribIOffsetEXT;
#define glVertexArrayVertexAttribIOffsetEXT glad_glVertexArrayVertexAttribIOffsetEXT
GLAPI PFNGLUNIFORM4UI64ARBPROC glad_glUniform4ui64ARB;
#define glUniform4ui64ARB glad_glUniform4ui64ARB
GLAPI PFNGLTEXTURERENDERBUFFEREXTPROC glad_glTextureRenderbufferEXT;
#define glTextureRenderbufferEXT glad_glTextureRenderbufferEXT
GLAPI PFNGLGETFRAMEBUFFERPARAMETERIVPROC glad_glGetFramebufferParameteriv;
#define glGetFramebufferParameteriv glad_glGetFramebufferParameteriv
GLAPI PFNGLGETPOINTERVEXTPROC glad_glGetPointervEXT;
#define glGetPointervEXT glad_glGetPointervEXT
GLAPI PFNGLPROGRAMUNIFORMMATRIX3DVEXTPROC glad_glProgramUniformMatrix3dvEXT;
#define glProgramUniformMatrix3dvEXT glad_glProgramUniformMatrix3dvEXT
GLAPI PFNGLCOPYBUFFERSUBDATAPROC glad_glCopyBufferSubData;
#define glCopyBufferSubData glad_glCopyBufferSubData
GLAPI PFNGLVERTEXATTRIBI1UIVPROC glad_glVertexAttribI1uiv;
#define glVertexAttribI1uiv glad_glVertexAttribI1uiv
GLAPI PFNGLVERTEXATTRIBL4DVEXTPROC glad_glVertexAttribL4dvEXT;
#define glVertexAttribL4dvEXT glad_glVertexAttribL4dvEXT
GLAPI PFNGLGETACTIVEUNIFORMPROC glad_glGetActiveUniform;
#define glGetActiveUniform glad_glGetActiveUniform
GLAPI PFNGLUNIFORM2UI64VARBPROC glad_glUniform2ui64vARB;
#define glUniform2ui64vARB glad_glUniform2ui64vARB
GLAPI PFNGLMAKENAMEDBUFFERRESIDENTNVPROC glad_glMakeNamedBufferResidentNV;
#define glMakeNamedBufferResidentNV glad_glMakeNamedBufferResidentNV
GLAPI PFNGLPATHGLYPHINDEXRANGENVPROC glad_glPathGlyphIndexRangeNV;
#define glPathGlyphIndexRangeNV glad_glPathGlyphIndexRangeNV
GLAPI PFNGLMULTITEXPARAMETERFEXTPROC glad_glMultiTexParameterfEXT;
#define glMultiTexParameterfEXT glad_glMultiTexParameterfEXT
GLAPI PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC glad_glNamedFramebufferTextureLayer;
#define glNamedFramebufferTextureLayer glad_glNamedFramebufferTextureLayer
GLAPI PFNGLGETVERTEXARRAYINTEGERVEXTPROC glad_glGetVertexArrayIntegervEXT;
#define glGetVertexArrayIntegervEXT glad_glGetVertexArrayIntegervEXT
GLAPI PFNGLVERTEXATTRIB3DARBPROC glad_glVertexAttrib3dARB;
#define glVertexAttrib3dARB glad_glVertexAttrib3dARB
GLAPI PFNGLUNIFORM3UIEXTPROC glad_glUniform3uiEXT;
#define glUniform3uiEXT glad_glUniform3uiEXT
GLAPI PFNGLTEXPARAMETERIUIVEXTPROC glad_glTexParameterIuivEXT;
#define glTexParameterIuivEXT glad_glTexParameterIuivEXT
GLAPI PFNGLPROGRAMUNIFORMMATRIX4FVPROC glad_glProgramUniformMatrix4fv;
#define glProgramUniformMatrix4fv glad_glProgramUniformMatrix4fv
GLAPI PFNGLVERTEXATTRIBL3DEXTPROC glad_glVertexAttribL3dEXT;
#define glVertexAttribL3dEXT glad_glVertexAttribL3dEXT
GLAPI PFNGLVERTEXATTRIBL3DVPROC glad_glVertexAttribL3dv;
#define glVertexAttribL3dv glad_glVertexAttribL3dv
GLAPI PFNGLFRAMEBUFFERRENDERBUFFERPROC glad_glFramebufferRenderbuffer;
#define glFramebufferRenderbuffer glad_glFramebufferRenderbuffer
GLAPI PFNGLGETUNIFORMLOCATIONARBPROC glad_glGetUniformLocationARB;
#define glGetUniformLocationARB glad_glGetUniformLocationARB
GLAPI PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC glad_glGetProgramLocalParameterdvARB;
#define glGetProgramLocalParameterdvARB glad_glGetProgramLocalParameterdvARB
GLAPI PFNGLPATHPARAMETERFVNVPROC glad_glPathParameterfvNV;
#define glPathParameterfvNV glad_glPathParameterfvNV
GLAPI PFNGLGETDOUBLEI_VPROC glad_glGetDoublei_v;
#define glGetDoublei_v glad_glGetDoublei_v
GLAPI PFNGLVERTEXATTRIB1SVPROC glad_glVertexAttrib1sv;
#define glVertexAttrib1sv glad_glVertexAttrib1sv
GLAPI PFNGLBINDSAMPLERPROC glad_glBindSampler;
#define glBindSampler glad_glBindSampler
GLAPI PFNGLBINDBUFFEROFFSETEXTPROC glad_glBindBufferOffsetEXT;
#define glBindBufferOffsetEXT glad_glBindBufferOffsetEXT
GLAPI PFNGLGETINTEGERI_VPROC glad_glGetIntegeri_v;
#define glGetIntegeri_v glad_glGetIntegeri_v
GLAPI PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glad_glGetTransformFeedbackVarying;
#define glGetTransformFeedbackVarying glad_glGetTransformFeedbackVarying
GLAPI PFNGLGETTRANSFORMFEEDBACKVARYINGNVPROC glad_glGetTransformFeedbackVaryingNV;
#define glGetTransformFeedbackVaryingNV glad_glGetTransformFeedbackVaryingNV
GLAPI PFNGLVERTEXATTRIB2DARBPROC glad_glVertexAttrib2dARB;
#define glVertexAttrib2dARB glad_glVertexAttrib2dARB
GLAPI PFNGLDRAWRANGEELEMENTSEXTPROC glad_glDrawRangeElementsEXT;
#define glDrawRangeElementsEXT glad_glDrawRangeElementsEXT
GLAPI PFNGLDEPTHRANGEFPROC glad_glDepthRangef;
#define glDepthRangef glad_glDepthRangef
GLAPI PFNGLDELETEPROGRAMSARBPROC glad_glDeleteProgramsARB;
#define glDeleteProgramsARB glad_glDeleteProgramsARB
GLAPI PFNGLCOMPRESSEDTEXTURESUBIMAGE1DEXTPROC glad_glCompressedTextureSubImage1DEXT;
#define glCompressedTextureSubImage1DEXT glad_glCompressedTextureSubImage1DEXT
GLAPI PFNGLENABLEIPROC glad_glEnablei;
#define glEnablei glad_glEnablei
GLAPI PFNGLGETACTIVESUBROUTINENAMEPROC glad_glGetActiveSubroutineName;
#define glGetActiveSubroutineName glad_glGetActiveSubroutineName
GLAPI PFNGLBINDPROGRAMARBPROC glad_glBindProgramARB;
#define glBindProgramARB glad_glBindProgramARB
GLAPI PFNGLPROGRAMUNIFORM3UI64VARBPROC glad_glProgramUniform3ui64vARB;
#define glProgramUniform3ui64vARB glad_glProgramUniform3ui64vARB
GLAPI PFNGLPROGRAMUNIFORMMATRIX2DVPROC glad_glProgramUniformMatrix2dv;
#define glProgramUniformMatrix2dv glad_glProgramUniformMatrix2dv
GLAPI PFNGLGETPATHMETRICRANGENVPROC glad_glGetPathMetricRangeNV;
#define glGetPathMetricRangeNV glad_glGetPathMetricRangeNV
GLAPI PFNGLSAMPLECOVERAGEARBPROC glad_glSampleCoverageARB;
#define glSampleCoverageARB glad_glSampleCoverageARB
GLAPI PFNGLPROGRAMUNIFORM2UI64VARBPROC glad_glProgramUniform2ui64vARB;
#define glProgramUniform2ui64vARB glad_glProgramUniform2ui64vARB
GLAPI PFNGLEDGEFLAGFORMATNVPROC glad_glEdgeFlagFormatNV;
#define glEdgeFlagFormatNV glad_glEdgeFlagFormatNV
GLAPI PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC glad_glInvalidateNamedFramebufferData;
#define glInvalidateNamedFramebufferData glad_glInvalidateNamedFramebufferData
GLAPI PFNGLSAMPLEMASKIPROC glad_glSampleMaski;
#define glSampleMaski glad_glSampleMaski
GLAPI PFNGLPOINTPARAMETERINVPROC glad_glPointParameteriNV;
#define glPointParameteriNV glad_glPointParameteriNV
GLAPI PFNGLUNIFORMMATRIX3X2FVPROC glad_glUniformMatrix3x2fv;
#define glUniformMatrix3x2fv glad_glUniformMatrix3x2fv
GLAPI PFNGLGETFRAMEBUFFERPARAMETERIVEXTPROC glad_glGetFramebufferParameterivEXT;
#define glGetFramebufferParameterivEXT glad_glGetFramebufferParameterivEXT
GLAPI PFNGLUNIFORM2I64ARBPROC glad_glUniform2i64ARB;
#define glUniform2i64ARB glad_glUniform2i64ARB
GLAPI PFNGLNAMEDBUFFERSTORAGEEXTPROC glad_glNamedBufferStorageEXT;
#define glNamedBufferStorageEXT glad_glNamedBufferStorageEXT
GLAPI PFNGLVERTEXATTRIB2DVPROC glad_glVertexAttrib2dv;
#define glVertexAttrib2dv glad_glVertexAttrib2dv
GLAPI PFNGLVERTEXATTRIB4DVARBPROC glad_glVertexAttrib4dvARB;
#define glVertexAttrib4dvARB glad_glVertexAttrib4dvARB
GLAPI PFNGLTEXSTORAGE2DPROC glad_glTexStorage2D;
#define glTexStorage2D glad_glTexStorage2D
GLAPI PFNGLUNIFORMMATRIX3X4FVPROC glad_glUniformMatrix3x4fv;
#define glUniformMatrix3x4fv glad_glUniformMatrix3x4fv
GLAPI PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC glad_glProgramUniformMatrix3fvEXT;
#define glProgramUniformMatrix3fvEXT glad_glProgramUniformMatrix3fvEXT
GLAPI PFNGLENABLEVERTEXARRAYEXTPROC glad_glEnableVertexArrayEXT;
#define glEnableVertexArrayEXT glad_glEnableVertexArrayEXT
GLAPI PFNGLDELETEBUFFERSARBPROC glad_glDeleteBuffersARB;
#define glDeleteBuffersARB glad_glDeleteBuffersARB
GLAPI PFNGLMULTITEXRENDERBUFFEREXTPROC glad_glMultiTexRenderbufferEXT;
#define glMultiTexRenderbufferEXT glad_glMultiTexRenderbufferEXT
GLAPI PFNGLUNMAPBUFFERARBPROC glad_glUnmapBufferARB;
#define glUnmapBufferARB glad_glUnmapBufferARB
GLAPI PFNGLMULTITEXCOORD4IVARBPROC glad_glMultiTexCoord4ivARB;
#define glMultiTexCoord4ivARB glad_glMultiTexCoord4ivARB
GLAPI PFNGLNAMEDPROGRAMLOCALPARAMETER4DVEXTPROC glad_glNamedProgramLocalParameter4dvEXT;
#define glNamedProgramLocalParameter4dvEXT glad_glNamedProgramLocalParameter4dvEXT
GLAPI PFNGLPROGRAMUNIFORM2FVEXTPROC glad_glProgramUniform2fvEXT;
#define glProgramUniform2fvEXT glad_glProgramUniform2fvEXT
GLAPI PFNGLCOLORMASKIPROC glad_glColorMaski;
#define glColorMaski glad_glColorMaski
GLAPI PFNGLCOPYNAMEDBUFFERSUBDATAPROC glad_glCopyNamedBufferSubData;
#define glCopyNamedBufferSubData glad_glCopyNamedBufferSubData
GLAPI PFNGLGENPROGRAMSNVPROC glad_glGenProgramsNV;
#define glGenProgramsNV glad_glGenProgramsNV
GLAPI PFNGLTEXSTORAGE3DPROC glad_glTexStorage3D;
#define glTexStorage3D glad_glTexStorage3D
GLAPI PFNGLTEXTUREPARAMETERIVPROC glad_glTextureParameteriv;
#define glTextureParameteriv glad_glTextureParameteriv
GLAPI PFNGLNAMEDBUFFERDATAEXTPROC glad_glNamedBufferDataEXT;
#define glNamedBufferDataEXT glad_glNamedBufferDataEXT
GLAPI PFNGLVERTEXATTRIBI3UIEXTPROC glad_glVertexAttribI3uiEXT;
#define glVertexAttribI3uiEXT glad_glVertexAttribI3uiEXT
GLAPI PFNGLNAMEDFRAMEBUFFERTEXTUREPROC glad_glNamedFramebufferTexture;
#define glNamedFramebufferTexture glad_glNamedFramebufferTexture
GLAPI PFNGLVERTEXATTRIB4NSVARBPROC glad_glVertexAttrib4NsvARB;
#define glVertexAttrib4NsvARB glad_glVertexAttrib4NsvARB
GLAPI PFNGLISIMAGEHANDLERESIDENTARBPROC glad_glIsImageHandleResidentARB;
#define glIsImageHandleResidentARB glad_glIsImageHandleResidentARB
GLAPI PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC glad_glProgramUniformMatrix4x3fv;
#define glProgramUniformMatrix4x3fv glad_glProgramUniformMatrix4x3fv
GLAPI PFNGLGETCOMPRESSEDTEXTUREIMAGEEXTPROC glad_glGetCompressedTextureImageEXT;
#define glGetCompressedTextureImageEXT glad_glGetCompressedTextureImageEXT
GLAPI PFNGLDRAWCOMMANDSSTATESNVPROC glad_glDrawCommandsStatesNV;
#define glDrawCommandsStatesNV glad_glDrawCommandsStatesNV
GLAPI PFNGLUNIFORM4UI64VNVPROC glad_glUniform4ui64vNV;
#define glUniform4ui64vNV glad_glUniform4ui64vNV
GLAPI PFNGLPROGRAMUNIFORM2UIPROC glad_glProgramUniform2ui;
#define glProgramUniform2ui glad_glProgramUniform2ui
GLAPI PFNGLGENRENDERBUFFERSEXTPROC glad_glGenRenderbuffersEXT;
#define glGenRenderbuffersEXT glad_glGenRenderbuffersEXT
GLAPI PFNGLNAMEDPROGRAMLOCALPARAMETERI4IVEXTPROC glad_glNamedProgramLocalParameterI4ivEXT;
#define glNamedProgramLocalParameterI4ivEXT glad_glNamedProgramLocalParameterI4ivEXT
GLAPI PFNGLGETMULTITEXPARAMETERIUIVEXTPROC glad_glGetMultiTexParameterIuivEXT;
#define glGetMultiTexParameterIuivEXT glad_glGetMultiTexParameterIuivEXT
GLAPI PFNGLBLENDEQUATIONIARBPROC glad_glBlendEquationiARB;
#define glBlendEquationiARB glad_glBlendEquationiARB
GLAPI PFNGLTEXPARAMETERIVPROC glad_glTexParameteriv;
#define glTexParameteriv glad_glTexParameteriv
GLAPI PFNGLUNIFORM4IVARBPROC glad_glUniform4ivARB;
#define glUniform4ivARB glad_glUniform4ivARB
GLAPI PFNGLMATRIXORTHOEXTPROC glad_glMatrixOrthoEXT;
#define glMatrixOrthoEXT glad_glMatrixOrthoEXT
GLAPI PFNGLVERTEXARRAYVERTEXBUFFERPROC glad_glVertexArrayVertexBuffer;
#define glVertexArrayVertexBuffer glad_glVertexArrayVertexBuffer
GLAPI PFNGLPROGRAMUNIFORM4UI64VNVPROC glad_glProgramUniform4ui64vNV;
#define glProgramUniform4ui64vNV glad_glProgramUniform4ui64vNV
GLAPI PFNGLUNIFORM4UI64VARBPROC glad_glUniform4ui64vARB;
#define glUniform4ui64vARB glad_glUniform4ui64vARB
GLAPI PFNGLPOINTPARAMETERIVNVPROC glad_glPointParameterivNV;
#define glPointParameterivNV glad_glPointParameterivNV
GLAPI PFNGLNAMEDFRAMEBUFFERTEXTURELAYEREXTPROC glad_glNamedFramebufferTextureLayerEXT;
#define glNamedFramebufferTextureLayerEXT glad_glNamedFramebufferTextureLayerEXT
GLAPI PFNGLMULTITEXGENFVEXTPROC glad_glMultiTexGenfvEXT;
#define glMultiTexGenfvEXT glad_glMultiTexGenfvEXT
GLAPI PFNGLGETNAMEDBUFFERSUBDATAPROC glad_glGetNamedBufferSubData;
#define glGetNamedBufferSubData glad_glGetNamedBufferSubData
GLAPI PFNGLSTENCILFUNCSEPARATEATIPROC glad_glStencilFuncSeparateATI;
#define glStencilFuncSeparateATI glad_glStencilFuncSeparateATI
GLAPI PFNGLPROGRAMUNIFORM2IVPROC glad_glProgramUniform2iv;
#define glProgramUniform2iv glad_glProgramUniform2iv
GLAPI PFNGLGETQUERYIVPROC glad_glGetQueryiv;
#define glGetQueryiv glad_glGetQueryiv
GLAPI PFNGLGETTRANSFORMFEEDBACKIVPROC glad_glGetTransformFeedbackiv;
#define glGetTransformFeedbackiv glad_glGetTransformFeedbackiv
GLAPI PFNGLGETSAMPLERPARAMETERFVPROC glad_glGetSamplerParameterfv;
#define glGetSamplerParameterfv glad_glGetSamplerParameterfv
GLAPI PFNGLPROGRAMUNIFORM3I64NVPROC glad_glProgramUniform3i64NV;
#define glProgramUniform3i64NV glad_glProgramUniform3i64NV
GLAPI PFNGLUNIFORM4IARBPROC glad_glUniform4iARB;
#define glUniform4iARB glad_glUniform4iARB
GLAPI PFNGLGETUNIFORMINDICESPROC glad_glGetUniformIndices;
#define glGetUniformIndices glad_glGetUniformIndices
GLAPI PFNGLMULTITEXIMAGE3DEXTPROC glad_glMultiTexImage3DEXT;
#define glMultiTexImage3DEXT glad_glMultiTexImage3DEXT
GLAPI PFNGLPOINTPARAMETERIVPROC glad_glPointParameteriv;
#define glPointParameteriv glad_glPointParameteriv
GLAPI PFNGLPROGRAMUNIFORMMATRIX3X4DVEXTPROC glad_glProgramUniformMatrix3x4dvEXT;
#define glProgramUniformMatrix3x4dvEXT glad_glProgramUniformMatrix3x4dvEXT
GLAPI PFNGLPROGRAMUNIFORMMATRIX4X3DVEXTPROC glad_glProgramUniformMatrix4x3dvEXT;
#define glProgramUniformMatrix4x3dvEXT glad_glProgramUniformMatrix4x3dvEXT
GLAPI PFNGLGETACTIVEVARYINGNVPROC glad_glGetActiveVaryingNV;
#define glGetActiveVaryingNV glad_glGetActiveVaryingNV
GLAPI PFNGLUNIFORM1I64VARBPROC glad_glUniform1i64vARB;
#define glUniform1i64vARB glad_glUniform1i64vARB
GLAPI PFNGLGETBUFFERSUBDATAPROC glad_glGetBufferSubData;
#define glGetBufferSubData glad_glGetBufferSubData
GLAPI PFNGLMATRIXTRANSLATEFEXTPROC glad_glMatrixTranslatefEXT;
#define glMatrixTranslatefEXT glad_glMatrixTranslatefEXT
GLAPI PFNGLVERTEXATTRIBP3UIPROC glad_glVertexAttribP3ui;
#define glVertexAttribP3ui glad_glVertexAttribP3ui
GLAPI PFNGLTEXTUREBUFFERRANGEPROC glad_glTextureBufferRange;
#define glTextureBufferRange glad_glTextureBufferRange
GLAPI PFNGLGETINTEGERUI64VNVPROC glad_glGetIntegerui64vNV;
#define glGetIntegerui64vNV glad_glGetIntegerui64vNV
GLAPI PFNGLPROGRAMUNIFORM4UI64VARBPROC glad_glProgramUniform4ui64vARB;
#define glProgramUniform4ui64vARB glad_glProgramUniform4ui64vARB
GLAPI PFNGLDEPTHRANGEPROC glad_glDepthRange;
#define glDepthRange glad_glDepthRange
GLAPI PFNGLGETVERTEXATTRIBDVARBPROC glad_glGetVertexAttribdvARB;
#define glGetVertexAttribdvARB glad_glGetVertexAttribdvARB
GLAPI PFNGLPROGRAMUNIFORM4I64VNVPROC glad_glProgramUniform4i64vNV;
#define glProgramUniform4i64vNV glad_glProgramUniform4i64vNV
GLAPI PFNGLDRAWARRAYSINSTANCEDEXTPROC glad_glDrawArraysInstancedEXT;
#define glDrawArraysInstancedEXT glad_glDrawArraysInstancedEXT
GLAPI PFNGLDISABLECLIENTSTATEINDEXEDEXTPROC glad_glDisableClientStateIndexedEXT;
#define glDisableClientStateIndexedEXT glad_glDisableClientStateIndexedEXT
GLAPI PFNGLDRAWBUFFERPROC glad_glDrawBuffer;
#define glDrawBuffer glad_glDrawBuffer
GLAPI PFNGLMULTIDRAWARRAYSINDIRECTBINDLESSNVPROC glad_glMultiDrawArraysIndirectBindlessNV;
#define glMultiDrawArraysIndirectBindlessNV glad_glMultiDrawArraysIndirectBindlessNV
GLAPI PFNGLCLEARBUFFERUIVPROC glad_glClearBufferuiv;
#define glClearBufferuiv glad_glClearBufferuiv
GLAPI PFNGLPATHSTENCILFUNCNVPROC glad_glPathStencilFuncNV;
#define glPathStencilFuncNV glad_glPathStencilFuncNV
GLAPI PFNGLGETINTERNALFORMATI64VPROC glad_glGetInternalformati64v;
#define glGetInternalformati64v glad_glGetInternalformati64v
GLAPI PFNGLSHADERSOURCEARBPROC glad_glShaderSourceARB;
#define glShaderSourceARB glad_glShaderSourceARB
GLAPI PFNGLPROVOKINGVERTEXEXTPROC glad_glProvokingVertexEXT;
#define glProvokingVertexEXT glad_glProvokingVertexEXT
GLAPI PFNGLFLUSHPROC glad_glFlush;
#define glFlush glad_glFlush
GLAPI PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glad_glFramebufferTexture2DEXT;
#define glFramebufferTexture2DEXT glad_glFramebufferTexture2DEXT
GLAPI PFNGLCOVERSTROKEPATHINSTANCEDNVPROC glad_glCoverStrokePathInstancedNV;
#define glCoverStrokePathInstancedNV glad_glCoverStrokePathInstancedNV
GLAPI PFNGLATTACHSHADERPROC glad_glAttachShader;
#define glAttachShader glad_glAttachShader
GLAPI PFNGLPROGRAMUNIFORMHANDLEUI64VARBPROC glad_glProgramUniformHandleui64vARB;
#define glProgramUniformHandleui64vARB glad_glProgramUniformHandleui64vARB
GLAPI PFNGLPUSHDEBUGGROUPPROC glad_glPushDebugGroup;
#define glPushDebugGroup glad_glPushDebugGroup
GLAPI PFNGLGETTEXLEVELPARAMETERIVPROC glad_glGetTexLevelParameteriv;
#define glGetTexLevelParameteriv glad_glGetTexLevelParameteriv
GLAPI PFNGLSTENCILFILLPATHNVPROC glad_glStencilFillPathNV;
#define glStencilFillPathNV glad_glStencilFillPathNV
GLAPI PFNGLPROGRAMVERTEXLIMITNVPROC glad_glProgramVertexLimitNV;
#define glProgramVertexLimitNV glad_glProgramVertexLimitNV
GLAPI PFNGLUNIFORM3I64NVPROC glad_glUniform3i64NV;
#define glUniform3i64NV glad_glUniform3i64NV
GLAPI PFNGLPROGRAMUNIFORM3UIVEXTPROC glad_glProgramUniform3uivEXT;
#define glProgramUniform3uivEXT glad_glProgramUniform3uivEXT
GLAPI PFNGLCLEARSTENCILPROC glad_glClearStencil;
#define glClearStencil glad_glClearStencil
GLAPI PFNGLVERTEXATTRIBS3SVNVPROC glad_glVertexAttribs3svNV;
#define glVertexAttribs3svNV glad_glVertexAttribs3svNV
GLAPI PFNGLGETBUFFERPARAMETERUI64VNVPROC glad_glGetBufferParameterui64vNV;
#define glGetBufferParameterui64vNV glad_glGetBufferParameterui64vNV
GLAPI PFNGLVERTEXATTRIB4BVARBPROC glad_glVertexAttrib4bvARB;
#define glVertexAttrib4bvARB glad_glVertexAttrib4bvARB
GLAPI PFNGLGETDOUBLEVPROC glad_glGetDoublev;
#define glGetDoublev glad_glGetDoublev
GLAPI PFNGLBLENDFUNCPROC glad_glBlendFunc;
#define glBlendFunc glad_glBlendFunc
GLAPI PFNGLCOPYTEXTUREIMAGE2DEXTPROC glad_glCopyTextureImage2DEXT;
#define glCopyTextureImage2DEXT glad_glCopyTextureImage2DEXT
GLAPI PFNGLBEGINQUERYINDEXEDPROC glad_glBeginQueryIndexed;
#define glBeginQueryIndexed glad_glBeginQueryIndexed
GLAPI PFNGLMULTITEXCOORD4FARBPROC glad_glMultiTexCoord4fARB;
#define glMultiTexCoord4fARB glad_glMultiTexCoord4fARB
GLAPI PFNGLGETPROGRAMIVPROC glad_glGetProgramiv;
#define glGetProgramiv glad_glGetProgramiv
GLAPI PFNGLGETPROGRAMSTRINGARBPROC glad_glGetProgramStringARB;
#define glGetProgramStringARB glad_glGetProgramStringARB
GLAPI PFNGLFLUSHMAPPEDBUFFERRANGEPROC glad_glFlushMappedBufferRange;
#define glFlushMappedBufferRange glad_glFlushMappedBufferRange
GLAPI PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC glad_glProgramUniformMatrix2fvEXT;
#define glProgramUniformMatrix2fvEXT glad_glProgramUniformMatrix2fvEXT
GLAPI PFNGLGETTEXTUREPARAMETERIUIVEXTPROC glad_glGetTextureParameterIuivEXT;
#define glGetTextureParameterIuivEXT glad_glGetTextureParameterIuivEXT
GLAPI PFNGLGETMULTITEXENVIVEXTPROC glad_glGetMultiTexEnvivEXT;
#define glGetMultiTexEnvivEXT glad_glGetMultiTexEnvivEXT
GLAPI PFNGLGENQUERIESPROC glad_glGenQueries;
#define glGenQueries glad_glGenQueries
GLAPI PFNGLBLENDCOLOREXTPROC glad_glBlendColorEXT;
#define glBlendColorEXT glad_glBlendColorEXT
GLAPI PFNGLGETNUNIFORMIVARBPROC glad_glGetnUniformivARB;
#define glGetnUniformivARB glad_glGetnUniformivARB
GLAPI PFNGLTEXSUBIMAGE3DPROC glad_glTexSubImage3D;
#define glTexSubImage3D glad_glTexSubImage3D
GLAPI PFNGLPATHSTENCILDEPTHOFFSETNVPROC glad_glPathStencilDepthOffsetNV;
#define glPathStencilDepthOffsetNV glad_glPathStencilDepthOffsetNV
GLAPI PFNGLDELETESAMPLERSPROC glad_glDeleteSamplers;
#define glDeleteSamplers glad_glDeleteSamplers
GLAPI PFNGLGETMULTITEXENVFVEXTPROC glad_glGetMultiTexEnvfvEXT;
#define glGetMultiTexEnvfvEXT glad_glGetMultiTexEnvfvEXT
GLAPI PFNGLVERTEXARRAYNORMALOFFSETEXTPROC glad_glVertexArrayNormalOffsetEXT;
#define glVertexArrayNormalOffsetEXT glad_glVertexArrayNormalOffsetEXT
GLAPI PFNGLGETBUFFERPARAMETERIVARBPROC glad_glGetBufferParameterivARB;
#define glGetBufferParameterivARB glad_glGetBufferParameterivARB
GLAPI PFNGLGETTEXTUREPARAMETERFVPROC glad_glGetTextureParameterfv;
#define glGetTextureParameterfv glad_glGetTextureParameterfv
GLAPI PFNGLVERTEXATTRIB4DNVPROC glad_glVertexAttrib4dNV;
#define glVertexAttrib4dNV glad_glVertexAttrib4dNV
GLAPI PFNGLGETMULTISAMPLEFVNVPROC glad_glGetMultisamplefvNV;
#define glGetMultisamplefvNV glad_glGetMultisamplefvNV
GLAPI PFNGLPROGRAMUNIFORMMATRIX4DVEXTPROC glad_glProgramUniformMatrix4dvEXT;
#define glProgramUniformMatrix4dvEXT glad_glProgramUniformMatrix4dvEXT
GLAPI PFNGLGETPATHCOMMANDSNVPROC glad_glGetPathCommandsNV;
#define glGetPathCommandsNV glad_glGetPathCommandsNV
GLAPI PFNGLVERTEXARRAYTEXCOORDOFFSETEXTPROC glad_glVertexArrayTexCoordOffsetEXT;
#define glVertexArrayTexCoordOffsetEXT glad_glVertexArrayTexCoordOffsetEXT
GLAPI PFNGLCOLORFORMATNVPROC glad_glColorFormatNV;
#define glColorFormatNV glad_glColorFormatNV
GLAPI PFNGLPROGRAMUNIFORMUI64NVPROC glad_glProgramUniformui64NV;
#define glProgramUniformui64NV glad_glProgramUniformui64NV
GLAPI PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC glad_glProgramUniformMatrix4x2fvEXT;
#define glProgramUniformMatrix4x2fvEXT glad_glProgramUniformMatrix4x2fvEXT
GLAPI PFNGLBINDTEXTURESPROC glad_glBindTextures;
#define glBindTextures glad_glBindTextures
GLAPI PFNGLUNIFORM2FARBPROC glad_glUniform2fARB;
#define glUniform2fARB glad_glUniform2fARB
GLAPI PFNGLBINDBUFFERRANGEEXTPROC glad_glBindBufferRangeEXT;
#define glBindBufferRangeEXT glad_glBindBufferRangeEXT
GLAPI PFNGLCOPYTEXTURESUBIMAGE3DEXTPROC glad_glCopyTextureSubImage3DEXT;
#define glCopyTextureSubImage3DEXT glad_glCopyTextureSubImage3DEXT
GLAPI PFNGLMAPBUFFERARBPROC glad_glMapBufferARB;
#define glMapBufferARB glad_glMapBufferARB
GLAPI PFNGLGETPATHSPACINGNVPROC glad_glGetPathSpacingNV;
#define glGetPathSpacingNV glad_glGetPathSpacingNV
GLAPI PFNGLUNIFORM4DVPROC glad_glUniform4dv;
#define glUniform4dv glad_glUniform4dv
GLAPI PFNGLPROGRAMUNIFORM3DVPROC glad_glProgramUniform3dv;
#define glProgramUniform3dv glad_glProgramUniform3dv
GLAPI PFNGLGETSHADERSOURCEARBPROC glad_glGetShaderSourceARB;
#define glGetShaderSourceARB glad_glGetShaderSourceARB
GLAPI PFNGLINVALIDATEBUFFERDATAPROC glad_glInvalidateBufferData;
#define glInvalidateBufferData glad_glInvalidateBufferData
GLAPI PFNGLMATRIXLOAD3X2FNVPROC glad_glMatrixLoad3x2fNV;
#define glMatrixLoad3x2fNV glad_glMatrixLoad3x2fNV
GLAPI PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC glad_glCompressedTextureSubImage1D;
#define glCompressedTextureSubImage1D glad_glCompressedTextureSubImage1D
GLAPI PFNGLUNIFORM3FVPROC glad_glUniform3fv;
#define glUniform3fv glad_glUniform3fv
GLAPI PFNGLCLEARTEXIMAGEPROC glad_glClearTexImage;
#define glClearTexImage glad_glClearTexImage
GLAPI PFNGLUNIFORMMATRIX4DVPROC glad_glUniformMatrix4dv;
#define glUniformMatrix4dv glad_glUniformMatrix4dv
GLAPI PFNGLPROGRAMUNIFORMMATRIX2X3DVEXTPROC glad_glProgramUniformMatrix2x3dvEXT;
#define glProgramUniformMatrix2x3dvEXT glad_glProgramUniformMatrix2x3dvEXT
GLAPI PFNGLGETTEXTURELEVELPARAMETERFVEXTPROC glad_glGetTextureLevelParameterfvEXT;
#define glGetTextureLevelParameterfvEXT glad_glGetTextureLevelParameterfvEXT
GLAPI PFNGLCLEARNAMEDFRAMEBUFFERFIPROC glad_glClearNamedFramebufferfi;
#define glClearNamedFramebufferfi glad_glClearNamedFramebufferfi
GLAPI PFNGLMULTIDRAWELEMENTSPROC glad_glMultiDrawElements;
#define glMultiDrawElements glad_glMultiDrawElements
GLAPI PFNGLVERTEXATTRIBL2DVEXTPROC glad_glVertexAttribL2dvEXT;
#define glVertexAttribL2dvEXT glad_glVertexAttribL2dvEXT
GLAPI PFNGLTEXSUBIMAGE1DEXTPROC glad_glTexSubImage1DEXT;
#define glTexSubImage1DEXT glad_glTexSubImage1DEXT
GLAPI PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC glad_glNamedFramebufferReadBuffer;
#define glNamedFramebufferReadBuffer glad_glNamedFramebufferReadBuffer
GLAPI PFNGLUNIFORM2I64VARBPROC glad_glUniform2i64vARB;
#define glUniform2i64vARB glad_glUniform2i64vARB
GLAPI PFNGLPROGRAMPARAMETER4FVNVPROC glad_glProgramParameter4fvNV;
#define glProgramParameter4fvNV glad_glProgramParameter4fvNV
GLAPI PFNGLUNIFORM1FVARBPROC glad_glUniform1fvARB;
#define glUniform1fvARB glad_glUniform1fvARB
GLAPI PFNGLVERTEXFORMATNVPROC glad_glVertexFormatNV;
#define glVertexFormatNV glad_glVertexFormatNV
GLAPI PFNGLBINDTRANSFORMFEEDBACKPROC glad_glBindTransformFeedback;
#define glBindTransformFeedback glad_glBindTransformFeedback
GLAPI PFNGLUNIFORM1I64NVPROC glad_glUniform1i64NV;
#define glUniform1i64NV glad_glUniform1i64NV
GLAPI PFNGLUNIFORM2UIVPROC glad_glUniform2uiv;
#define glUniform2uiv glad_glUniform2uiv
GLAPI PFNGLFINISHPROC glad_glFinish;
#define glFinish glad_glFinish
GLAPI PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glad_glCompressedTexSubImage1D;
#define glCompressedTexSubImage1D glad_glCompressedTexSubImage1D
GLAPI PFNGLVERTEXARRAYVERTEXOFFSETEXTPROC glad_glVertexArrayVertexOffsetEXT;
#define glVertexArrayVertexOffsetEXT glad_glVertexArrayVertexOffsetEXT
GLAPI PFNGLUNIFORM1UIVPROC glad_glUniform1uiv;
#define glUniform1uiv glad_glUniform1uiv
GLAPI PFNGLCOPYPATHNVPROC glad_glCopyPathNV;
#define glCopyPathNV glad_glCopyPathNV
GLAPI PFNGLCLEARNAMEDBUFFERDATAEXTPROC glad_glClearNamedBufferDataEXT;
#define glClearNamedBufferDataEXT glad_glClearNamedBufferDataEXT
GLAPI PFNGLCLEARDEPTHPROC glad_glClearDepth;
#define glClearDepth glad_glClearDepth
GLAPI PFNGLDISABLEINDEXEDEXTPROC glad_glDisableIndexedEXT;
#define glDisableIndexedEXT glad_glDisableIndexedEXT
GLAPI PFNGLCOMPRESSEDTEXIMAGE3DARBPROC glad_glCompressedTexImage3DARB;
#define glCompressedTexImage3DARB glad_glCompressedTexImage3DARB
GLAPI PFNGLUNIFORMMATRIX4X2DVPROC glad_glUniformMatrix4x2dv;
#define glUniformMatrix4x2dv glad_glUniformMatrix4x2dv
GLAPI PFNGLCREATEBUFFERSPROC glad_glCreateBuffers;
#define glCreateBuffers glad_glCreateBuffers
GLAPI PFNGLBEGINTRANSFORMFEEDBACKEXTPROC glad_glBeginTransformFeedbackEXT;
#define glBeginTransformFeedbackEXT glad_glBeginTransformFeedbackEXT
GLAPI PFNGLUNIFORM4I64VNVPROC glad_glUniform4i64vNV;
#define glUniform4i64vNV glad_glUniform4i64vNV
GLAPI PFNGLGETNUNIFORMUIVARBPROC glad_glGetnUniformuivARB;
#define glGetnUniformuivARB glad_glGetnUniformuivARB
GLAPI PFNGLGETPROGRAMPIPELINEINFOLOGPROC glad_glGetProgramPipelineInfoLog;
#define glGetProgramPipelineInfoLog glad_glGetProgramPipelineInfoLog
GLAPI PFNGLBLENDFUNCSEPARATEEXTPROC glad_glBlendFuncSeparateEXT;
#define glBlendFuncSeparateEXT glad_glBlendFuncSeparateEXT
GLAPI PFNGLUNIFORMMATRIX4X3DVPROC glad_glUniformMatrix4x3dv;
#define glUniformMatrix4x3dv glad_glUniformMatrix4x3dv
GLAPI PFNGLMAKEIMAGEHANDLENONRESIDENTARBPROC glad_glMakeImageHandleNonResidentARB;
#define glMakeImageHandleNonResidentARB glad_glMakeImageHandleNonResidentARB
GLAPI PFNGLPOLYGONMODEPROC glad_glPolygonMode;
#define glPolygonMode glad_glPolygonMode
GLAPI PFNGLVERTEXARRAYVERTEXATTRIBDIVISOREXTPROC glad_glVertexArrayVertexAttribDivisorEXT;
#define glVertexArrayVertexAttribDivisorEXT glad_glVertexArrayVertexAttribDivisorEXT
GLAPI PFNGLWAITSYNCPROC glad_glWaitSync;
#define glWaitSync glad_glWaitSync
GLAPI PFNGLMAPNAMEDBUFFEREXTPROC glad_glMapNamedBufferEXT;
#define glMapNamedBufferEXT glad_glMapNamedBufferEXT
GLAPI PFNGLUNIFORM3UIVEXTPROC glad_glUniform3uivEXT;
#define glUniform3uivEXT glad_glUniform3uivEXT
GLAPI PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC glad_glClearNamedFramebufferuiv;
#define glClearNamedFramebufferuiv glad_glClearNamedFramebufferuiv
GLAPI PFNGLPROGRAMUNIFORM1UI64NVPROC glad_glProgramUniform1ui64NV;
#define glProgramUniform1ui64NV glad_glProgramUniform1ui64NV
GLAPI PFNGLGETVERTEXATTRIBFVPROC glad_glGetVertexAttribfv;
#define glGetVertexAttribfv glad_glGetVertexAttribfv
GLAPI PFNGLDISPATCHCOMPUTEPROC glad_glDispatchCompute;
#define glDispatchCompute glad_glDispatchCompute
GLAPI PFNGLGETACTIVEATTRIBPROC glad_glGetActiveAttrib;
#define glGetActiveAttrib glad_glGetActiveAttrib
GLAPI PFNGLTEXSUBIMAGE2DPROC glad_glTexSubImage2D;
#define glTexSubImage2D glad_glTexSubImage2D
GLAPI PFNGLCLEARBUFFERFVPROC glad_glClearBufferfv;
#define glClearBufferfv glad_glClearBufferfv
GLAPI PFNGLLOGICOPPROC glad_glLogicOp;
#define glLogicOp glad_glLogicOp
GLAPI PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC glad_glProgramUniformMatrix3x4fv;
#define glProgramUniformMatrix3x4fv glad_glProgramUniformMatrix3x4fv
GLAPI PFNGLGETTEXTUREPARAMETERIUIVPROC glad_glGetTextureParameterIuiv;
#define glGetTextureParameterIuiv glad_glGetTextureParameterIuiv
GLAPI PFNGLPROGRAMUNIFORMMATRIX4DVPROC glad_glProgramUniformMatrix4dv;
#define glProgramUniformMatrix4dv glad_glProgramUniformMatrix4dv
GLAPI PFNGLFRAMEBUFFERTEXTURE1DEXTPROC glad_glFramebufferTexture1DEXT;
#define glFramebufferTexture1DEXT glad_glFramebufferTexture1DEXT
GLAPI PFNGLMULTITEXCOORD2FVARBPROC glad_glMultiTexCoord2fvARB;
#define glMultiTexCoord2fvARB glad_glMultiTexCoord2fvARB
GLAPI PFNGLNAMEDPROGRAMLOCALPARAMETERSI4IVEXTPROC glad_glNamedProgramLocalParametersI4ivEXT;
#define glNamedProgramLocalParametersI4ivEXT glad_glNamedProgramLocalParametersI4ivEXT
GLAPI PFNGLVERTEXATTRIBI1UIVEXTPROC glad_glVertexAttribI1uivEXT;
#define glVertexAttribI1uivEXT glad_glVertexAttribI1uivEXT
GLAPI PFNGLMULTITEXCOORD3IARBPROC glad_glMultiTexCoord3iARB;
#define glMultiTexCoord3iARB glad_glMultiTexCoord3iARB
GLAPI PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC glad_glDrawTransformFeedbackStream;
#define glDrawTransformFeedbackStream glad_glDrawTransformFeedbackStream
GLAPI PFNGLVERTEXARRAYMULTITEXCOORDOFFSETEXTPROC glad_glVertexArrayMultiTexCoordOffsetEXT;
#define glVertexArrayMultiTexCoordOffsetEXT glad_glVertexArrayMultiTexCoordOffsetEXT
GLAPI PFNGLPROVOKINGVERTEXPROC glad_glProvokingVertex;
#define glProvokingVertex glad_glProvokingVertex
GLAPI PFNGLSHADERBINARYPROC glad_glShaderBinary;
#define glShaderBinary glad_glShaderBinary
GLAPI PFNGLGETMULTITEXGENIVEXTPROC glad_glGetMultiTexGenivEXT;
#define glGetMultiTexGenivEXT glad_glGetMultiTexGenivEXT
GLAPI PFNGLGETHANDLEARBPROC glad_glGetHandleARB;
#define glGetHandleARB glad_glGetHandleARB
GLAPI PFNGLTRANSFORMPATHNVPROC glad_glTransformPathNV;
#define glTransformPathNV glad_glTransformPathNV
GLAPI PFNGLCOMPRESSEDTEXTUREIMAGE2DEXTPROC glad_glCompressedTextureImage2DEXT;
#define glCompressedTextureImage2DEXT glad_glCompressedTextureImage2DEXT
GLAPI PFNGLGETNAMEDPROGRAMLOCALPARAMETERDVEXTPROC glad_glGetNamedProgramLocalParameterdvEXT;
#define glGetNamedProgramLocalParameterdvEXT glad_glGetNamedProgramLocalParameterdvEXT
GLAPI PFNGLDRAWELEMENTSPROC glad_glDrawElements;
#define glDrawElements glad_glDrawElements
GLAPI PFNGLPROGRAMUNIFORM3UI64NVPROC glad_glProgramUniform3ui64NV;
#define glProgramUniform3ui64NV glad_glProgramUniform3ui64NV
GLAPI PFNGLUNIFORM1IVPROC glad_glUniform1iv;
#define glUniform1iv glad_glUniform1iv
GLAPI PFNGLMATRIXMULTFEXTPROC glad_glMatrixMultfEXT;
#define glMatrixMultfEXT glad_glMatrixMultfEXT
GLAPI PFNGLDRAWARRAYSINSTANCEDPROC glad_glDrawArraysInstanced;
#define glDrawArraysInstanced glad_glDrawArraysInstanced
GLAPI PFNGLVERTEXATTRIB2SNVPROC glad_glVertexAttrib2sNV;
#define glVertexAttrib2sNV glad_glVertexAttrib2sNV
GLAPI PFNGLPROGRAMUNIFORMMATRIX2FVPROC glad_glProgramUniformMatrix2fv;
#define glProgramUniformMatrix2fv glad_glProgramUniformMatrix2fv
GLAPI PFNGLSAMPLERPARAMETERIVPROC glad_glSamplerParameteriv;
#define glSamplerParameteriv glad_glSamplerParameteriv
GLAPI PFNGLVERTEXATTRIB4UIVPROC glad_glVertexAttrib4uiv;
#define glVertexAttrib4uiv glad_glVertexAttrib4uiv
GLAPI PFNGLENDQUERYINDEXEDPROC glad_glEndQueryIndexed;
#define glEndQueryIndexed glad_glEndQueryIndexed
GLAPI PFNGLPROGRAMPARAMETERIARBPROC glad_glProgramParameteriARB;
#define glProgramParameteriARB glad_glProgramParameteriARB
GLAPI PFNGLPROGRAMUNIFORM1IVPROC glad_glProgramUniform1iv;
#define glProgramUniform1iv glad_glProgramUniform1iv
GLAPI PFNGLBINDRENDERBUFFERPROC glad_glBindRenderbuffer;
#define glBindRenderbuffer glad_glBindRenderbuffer
GLAPI PFNGLMULTITEXSUBIMAGE3DEXTPROC glad_glMultiTexSubImage3DEXT;
#define glMultiTexSubImage3DEXT glad_glMultiTexSubImage3DEXT
GLAPI PFNGLISPROGRAMPROC glad_glIsProgram;
#define glIsProgram glad_glIsProgram
GLAPI PFNGLMULTIDRAWELEMENTSINDIRECTAMDPROC glad_glMultiDrawElementsIndirectAMD;
#define glMultiDrawElementsIndirectAMD glad_glMultiDrawElementsIndirectAMD
GLAPI PFNGLGETPROGRAMINFOLOGPROC glad_glGetProgramInfoLog;
#define glGetProgramInfoLog glad_glGetProgramInfoLog
GLAPI PFNGLUNIFORMMATRIX3X2DVPROC glad_glUniformMatrix3x2dv;
#define glUniformMatrix3x2dv glad_glUniformMatrix3x2dv
GLAPI PFNGLVERTEXATTRIB4FVPROC glad_glVertexAttrib4fv;
#define glVertexAttrib4fv glad_glVertexAttrib4fv
GLAPI PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC glad_glProgramUniformMatrix2x3dv;
#define glProgramUniformMatrix2x3dv glad_glProgramUniformMatrix2x3dv
GLAPI PFNGLGETNCOMPRESSEDTEXIMAGEPROC glad_glGetnCompressedTexImage;
#define glGetnCompressedTexImage glad_glGetnCompressedTexImage
GLAPI PFNGLVERTEXATTRIBS3FVNVPROC glad_glVertexAttribs3fvNV;
#define glVertexAttribs3fvNV glad_glVertexAttribs3fvNV
GLAPI PFNGLVERTEXATTRIB2FVPROC glad_glVertexAttrib2fv;
#define glVertexAttrib2fv glad_glVertexAttrib2fv
GLAPI PFNGLDRAWARRAYSINDIRECTPROC glad_glDrawArraysIndirect;
#define glDrawArraysIndirect glad_glDrawArraysIndirect
GLAPI PFNGLUNIFORM3UI64NVPROC glad_glUniform3ui64NV;
#define glUniform3ui64NV glad_glUniform3ui64NV
GLAPI PFNGLBEGINCONDITIONALRENDERNVPROC glad_glBeginConditionalRenderNV;
#define glBeginConditionalRenderNV glad_glBeginConditionalRenderNV
GLAPI PFNGLSTENCILTHENCOVERFILLPATHNVPROC glad_glStencilThenCoverFillPathNV;
#define glStencilThenCoverFillPathNV glad_glStencilThenCoverFillPathNV
GLAPI PFNGLVERTEXARRAYINDEXOFFSETEXTPROC glad_glVertexArrayIndexOffsetEXT;
#define glVertexArrayIndexOffsetEXT glad_glVertexArrayIndexOffsetEXT
GLAPI PFNGLDISABLECLIENTSTATEIEXTPROC glad_glDisableClientStateiEXT;
#define glDisableClientStateiEXT glad_glDisableClientStateiEXT
GLAPI PFNGLCOPYMULTITEXSUBIMAGE1DEXTPROC glad_glCopyMultiTexSubImage1DEXT;
#define glCopyMultiTexSubImage1DEXT glad_glCopyMultiTexSubImage1DEXT
GLAPI PFNGLTEXBUFFERARBPROC glad_glTexBufferARB;
#define glTexBufferARB glad_glTexBufferARB
GLAPI PFNGLMULTIDRAWELEMENTSINDIRECTBINDLESSCOUNTNVPROC glad_glMultiDrawElementsIndirectBindlessCountNV;
#define glMultiDrawElementsIndirectBindlessCountNV glad_glMultiDrawElementsIndirectBindlessCountNV
GLAPI PFNGLGETBUFFERPOINTERVARBPROC glad_glGetBufferPointervARB;
#define glGetBufferPointervARB glad_glGetBufferPointervARB
GLAPI PFNGLMULTITEXPARAMETERIUIVEXTPROC glad_glMultiTexParameterIuivEXT;
#define glMultiTexParameterIuivEXT glad_glMultiTexParameterIuivEXT
GLAPI PFNGLPROGRAMUNIFORM3FPROC glad_glProgramUniform3f;
#define glProgramUniform3f glad_glProgramUniform3f
GLAPI PFNGLPROGRAMUNIFORM3DPROC glad_glProgramUniform3d;
#define glProgramUniform3d glad_glProgramUniform3d
GLAPI PFNGLVERTEXATTRIBPOINTERNVPROC glad_glVertexAttribPointerNV;
#define glVertexAttribPointerNV glad_glVertexAttribPointerNV
GLAPI PFNGLENDCONDITIONALRENDERNVPROC glad_glEndConditionalRenderNV;
#define glEndConditionalRenderNV glad_glEndConditionalRenderNV
GLAPI PFNGLUNIFORM3I64VARBPROC glad_glUniform3i64vARB;
#define glUniform3i64vARB glad_glUniform3i64vARB
GLAPI PFNGLGETQUERYIVARBPROC glad_glGetQueryivARB;
#define glGetQueryivARB glad_glGetQueryivARB
GLAPI PFNGLVERTEXATTRIBI1UIPROC glad_glVertexAttribI1ui;
#define glVertexAttribI1ui glad_glVertexAttribI1ui
GLAPI PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC glad_glGetRenderbufferParameterivEXT;
#define glGetRenderbufferParameterivEXT glad_glGetRenderbufferParameterivEXT
GLAPI PFNGLISBUFFERRESIDENTNVPROC glad_glIsBufferResidentNV;
#define glIsBufferResidentNV glad_glIsBufferResidentNV
GLAPI PFNGLGETNAMEDRENDERBUFFERPARAMETERIVEXTPROC glad_glGetNamedRenderbufferParameterivEXT;
#define glGetNamedRenderbufferParameterivEXT glad_glGetNamedRenderbufferParameterivEXT
GLAPI PFNGLVERTEXATTRIBI4BVEXTPROC glad_glVertexAttribI4bvEXT;
#define glVertexAttribI4bvEXT glad_glVertexAttribI4bvEXT
GLAPI PFNGLMULTITEXGENDVEXTPROC glad_glMultiTexGendvEXT;
#define glMultiTexGendvEXT glad_glMultiTexGendvEXT
GLAPI PFNGLGETPROGRAMENVPARAMETERDVARBPROC glad_glGetProgramEnvParameterdvARB;
#define glGetProgramEnvParameterdvARB glad_glGetProgramEnvParameterdvARB
GLAPI PFNGLSECONDARYCOLORFORMATNVPROC glad_glSecondaryColorFormatNV;
#define glSecondaryColorFormatNV glad_glSecondaryColorFormatNV
GLAPI PFNGLVERTEXATTRIB4UBVNVPROC glad_glVertexAttrib4ubvNV;
#define glVertexAttrib4ubvNV glad_glVertexAttrib4ubvNV
GLAPI PFNGLINVALIDATETEXSUBIMAGEPROC glad_glInvalidateTexSubImage;
#define glInvalidateTexSubImage glad_glInvalidateTexSubImage
GLAPI PFNGLGETINTEGER64VPROC glad_glGetInteger64v;
#define glGetInteger64v glad_glGetInteger64v
GLAPI PFNGLGETPROGRAMENVPARAMETERFVARBPROC glad_glGetProgramEnvParameterfvARB;
#define glGetProgramEnvParameterfvARB glad_glGetProgramEnvParameterfvARB
GLAPI PFNGLVERTEXATTRIB4USVARBPROC glad_glVertexAttrib4usvARB;
#define glVertexAttrib4usvARB glad_glVertexAttrib4usvARB
GLAPI PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC glad_glNamedFramebufferRenderbuffer;
#define glNamedFramebufferRenderbuffer glad_glNamedFramebufferRenderbuffer
GLAPI PFNGLPOINTPARAMETERFEXTPROC glad_glPointParameterfEXT;
#define glPointParameterfEXT glad_glPointParameterfEXT
GLAPI PFNGLVERTEXATTRIB4NIVPROC glad_glVertexAttrib4Niv;
#define glVertexAttrib4Niv glad_glVertexAttrib4Niv
GLAPI PFNGLCLEARBUFFERIVPROC glad_glClearBufferiv;
#define glClearBufferiv glad_glClearBufferiv
GLAPI PFNGLVERTEXATTRIB4IVARBPROC glad_glVertexAttrib4ivARB;
#define glVertexAttrib4ivARB glad_glVertexAttrib4ivARB
GLAPI PFNGLMATRIXMULT3X2FNVPROC glad_glMatrixMult3x2fNV;
#define glMatrixMult3x2fNV glad_glMatrixMult3x2fNV
GLAPI PFNGLBEGINCONDITIONALRENDERNVXPROC glad_glBeginConditionalRenderNVX;
#define glBeginConditionalRenderNVX glad_glBeginConditionalRenderNVX
GLAPI PFNGLGETNAMEDPROGRAMSTRINGEXTPROC glad_glGetNamedProgramStringEXT;
#define glGetNamedProgramStringEXT glad_glGetNamedProgramStringEXT
GLAPI PFNGLGETTEXTURELEVELPARAMETERFVPROC glad_glGetTextureLevelParameterfv;
#define glGetTextureLevelParameterfv glad_glGetTextureLevelParameterfv
GLAPI PFNGLGETATTRIBLOCATIONARBPROC glad_glGetAttribLocationARB;
#define glGetAttribLocationARB glad_glGetAttribLocationARB
GLAPI PFNGLGETSAMPLERPARAMETERIUIVPROC glad_glGetSamplerParameterIuiv;
#define glGetSamplerParameterIuiv glad_glGetSamplerParameterIuiv
GLAPI PFNGLCOPYTEXTURESUBIMAGE1DEXTPROC glad_glCopyTextureSubImage1DEXT;
#define glCopyTextureSubImage1DEXT glad_glCopyTextureSubImage1DEXT
GLAPI PFNGLTEXTURESUBIMAGE1DPROC glad_glTextureSubImage1D;
#define glTextureSubImage1D glad_glTextureSubImage1D
GLAPI PFNGLGENTRANSFORMFEEDBACKSPROC glad_glGenTransformFeedbacks;
#define glGenTransformFeedbacks glad_glGenTransformFeedbacks
GLAPI PFNGLVERTEXATTRIBS2SVNVPROC glad_glVertexAttribs2svNV;
#define glVertexAttribs2svNV glad_glVertexAttribs2svNV
GLAPI PFNGLTEXTUREIMAGE3DEXTPROC glad_glTextureImage3DEXT;
#define glTextureImage3DEXT glad_glTextureImage3DEXT
GLAPI PFNGLGETNAMEDBUFFERPOINTERVPROC glad_glGetNamedBufferPointerv;
#define glGetNamedBufferPointerv glad_glGetNamedBufferPointerv
GLAPI PFNGLISENABLEDINDEXEDEXTPROC glad_glIsEnabledIndexedEXT;
#define glIsEnabledIndexedEXT glad_glIsEnabledIndexedEXT
GLAPI PFNGLGETNAMEDPROGRAMIVEXTPROC glad_glGetNamedProgramivEXT;
#define glGetNamedProgramivEXT glad_glGetNamedProgramivEXT
GLAPI PFNGLGETTEXTURELEVELPARAMETERIVEXTPROC glad_glGetTextureLevelParameterivEXT;
#define glGetTextureLevelParameterivEXT glad_glGetTextureLevelParameterivEXT
GLAPI PFNGLGETOBJECTPARAMETERIVARBPROC glad_glGetObjectParameterivARB;
#define glGetObjectParameterivARB glad_glGetObjectParameterivARB
GLAPI PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC glad_glProgramUniformMatrix4x3dv;
#define glProgramUniformMatrix4x3dv glad_glProgramUniformMatrix4x3dv
GLAPI PFNGLGETNAMEDPROGRAMLOCALPARAMETERIUIVEXTPROC glad_glGetNamedProgramLocalParameterIuivEXT;
#define glGetNamedProgramLocalParameterIuivEXT glad_glGetNamedProgramLocalParameterIuivEXT
GLAPI PFNGLREADNPIXELSPROC glad_glReadnPixels;
#define glReadnPixels glad_glReadnPixels
GLAPI PFNGLREADBUFFERPROC glad_glReadBuffer;
#define glReadBuffer glad_glReadBuffer
GLAPI PFNGLPROGRAMUNIFORM4FVEXTPROC glad_glProgramUniform4fvEXT;
#define glProgramUniform4fvEXT glad_glProgramUniform4fvEXT
GLAPI PFNGLVERTEXATTRIB2SARBPROC glad_glVertexAttrib2sARB;
#define glVertexAttrib2sARB glad_glVertexAttrib2sARB
GLAPI PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glad_glCompressedTexImage2DARB;
#define glCompressedTexImage2DARB glad_glCompressedTexImage2DARB
GLAPI PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glad_glCheckFramebufferStatusEXT;
#define glCheckFramebufferStatusEXT glad_glCheckFramebufferStatusEXT
GLAPI PFNGLVERTEXATTRIB1FARBPROC glad_glVertexAttrib1fARB;
#define glVertexAttrib1fARB glad_glVertexAttrib1fARB
GLAPI PFNGLMULTITEXCOORD1IVARBPROC glad_glMultiTexCoord1ivARB;
#define glMultiTexCoord1ivARB glad_glMultiTexCoord1ivARB
GLAPI PFNGLDISABLEVERTEXATTRIBARRAYARBPROC glad_glDisableVertexAttribArrayARB;
#define glDisableVertexAttribArrayARB glad_glDisableVertexAttribArrayARB
GLAPI PFNGLVERTEXATTRIBI1IEXTPROC glad_glVertexAttribI1iEXT;
#define glVertexAttribI1iEXT glad_glVertexAttribI1iEXT
GLAPI PFNGLGETPATHDASHARRAYNVPROC glad_glGetPathDashArrayNV;
#define glGetPathDashArrayNV glad_glGetPathDashArrayNV
GLAPI PFNGLSTENCILTHENCOVERFILLPATHINSTANCEDNVPROC glad_glStencilThenCoverFillPathInstancedNV;
#define glStencilThenCoverFillPathInstancedNV glad_glStencilThenCoverFillPathInstancedNV
GLAPI PFNGLCOLORMASKPROC glad_glColorMask;
#define glColorMask glad_glColorMask
GLAPI PFNGLGETNTEXIMAGEPROC glad_glGetnTexImage;
#define glGetnTexImage glad_glGetnTexImage
GLAPI PFNGLPROGRAMLOCALPARAMETER4DARBPROC glad_glProgramLocalParameter4dARB;
#define glProgramLocalParameter4dARB glad_glProgramLocalParameter4dARB
GLAPI PFNGLBLENDEQUATIONPROC glad_glBlendEquation;
#define glBlendEquation glad_glBlendEquation
GLAPI PFNGLGETQUERYOBJECTUI64VEXTPROC glad_glGetQueryObjectui64vEXT;
#define glGetQueryObjectui64vEXT glad_glGetQueryObjectui64vEXT
GLAPI PFNGLVERTEXATTRIBI4UIVEXTPROC glad_glVertexAttribI4uivEXT;
#define glVertexAttribI4uivEXT glad_glVertexAttribI4uivEXT
GLAPI PFNGLGETPROGRAMRESOURCEIVPROC glad_glGetProgramResourceiv;
#define glGetProgramResourceiv glad_glGetProgramResourceiv
GLAPI PFNGLCLEARBUFFERDATAPROC glad_glClearBufferData;
#define glClearBufferData glad_glClearBufferData
GLAPI PFNGLGETPROGRAMIVNVPROC glad_glGetProgramivNV;
#define glGetProgramivNV glad_glGetProgramivNV
GLAPI PFNGLDELETETRANSFORMFEEDBACKSPROC glad_glDeleteTransformFeedbacks;
#define glDeleteTransformFeedbacks glad_glDeleteTransformFeedbacks
GLAPI PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glad_glDrawRangeElementsBaseVertex;
#define glDrawRangeElementsBaseVertex glad_glDrawRangeElementsBaseVertex
GLAPI PFNGLUNIFORM4I64VARBPROC glad_glUniform4i64vARB;
#define glUniform4i64vARB glad_glUniform4i64vARB
GLAPI PFNGLTEXTURESTORAGE3DMULTISAMPLEEXTPROC glad_glTextureStorage3DMultisampleEXT;
#define glTextureStorage3DMultisampleEXT glad_glTextureStorage3DMultisampleEXT
GLAPI PFNGLCOMPRESSEDMULTITEXIMAGE1DEXTPROC glad_glCompressedMultiTexImage1DEXT;
#define glCompressedMultiTexImage1DEXT glad_glCompressedMultiTexImage1DEXT
GLAPI PFNGLUNIFORMMATRIX2DVPROC glad_glUniformMatrix2dv;
#define glUniformMatrix2dv glad_glUniformMatrix2dv
GLAPI PFNGLPROGRAMUNIFORM2IVEXTPROC glad_glProgramUniform2ivEXT;
#define glProgramUniform2ivEXT glad_glProgramUniform2ivEXT
GLAPI PFNGLVERTEXATTRIB1DVPROC glad_glVertexAttrib1dv;
#define glVertexAttrib1dv glad_glVertexAttrib1dv
GLAPI PFNGLVERTEXATTRIBS3DVNVPROC glad_glVertexAttribs3dvNV;
#define glVertexAttribs3dvNV glad_glVertexAttribs3dvNV
GLAPI PFNGLBUFFERSUBDATAPROC glad_glBufferSubData;
#define glBufferSubData glad_glBufferSubData
GLAPI PFNGLBLENDFUNCSEPARATEIPROC glad_glBlendFuncSeparatei;
#define glBlendFuncSeparatei glad_glBlendFuncSeparatei
GLAPI PFNGLGETBOOLEANINDEXEDVEXTPROC glad_glGetBooleanIndexedvEXT;
#define glGetBooleanIndexedvEXT glad_glGetBooleanIndexedvEXT
GLAPI PFNGLDISABLEIPROC glad_glDisablei;
#define glDisablei glad_glDisablei
GLAPI PFNGLGETUNIFORMUI64VARBPROC glad_glGetUniformui64vARB;
#define glGetUniformui64vARB glad_glGetUniformui64vARB
GLAPI PFNGLGETSYNCIVPROC glad_glGetSynciv;
#define glGetSynciv glad_glGetSynciv
GLAPI PFNGLMAKEIMAGEHANDLERESIDENTARBPROC glad_glMakeImageHandleResidentARB;
#define glMakeImageHandleResidentARB glad_glMakeImageHandleResidentARB
GLAPI PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glad_glRenderbufferStorageMultisampleEXT;
#define glRenderbufferStorageMultisampleEXT glad_glRenderbufferStorageMultisampleEXT
GLAPI PFNGLPROGRAMUNIFORM2IPROC glad_glProgramUniform2i;
#define glProgramUniform2i glad_glProgramUniform2i
GLAPI PFNGLLISTDRAWCOMMANDSSTATESCLIENTNVPROC glad_glListDrawCommandsStatesClientNV;
#define glListDrawCommandsStatesClientNV glad_glListDrawCommandsStatesClientNV
GLAPI PFNGLBEGINTRANSFORMFEEDBACKNVPROC glad_glBeginTransformFeedbackNV;
#define glBeginTransformFeedbackNV glad_glBeginTransformFeedbackNV
GLAPI PFNGLGETPROGRAMBINARYPROC glad_glGetProgramBinary;
#define glGetProgramBinary glad_glGetProgramBinary
GLAPI PFNGLPROGRAMENVPARAMETER4FVARBPROC glad_glProgramEnvParameter4fvARB;
#define glProgramEnvParameter4fvARB glad_glProgramEnvParameter4fvARB
GLAPI PFNGLPAUSETRANSFORMFEEDBACKPROC glad_glPauseTransformFeedback;
#define glPauseTransformFeedback glad_glPauseTransformFeedback
GLAPI PFNGLUNIFORM2UI64ARBPROC glad_glUniform2ui64ARB;
#define glUniform2ui64ARB glad_glUniform2ui64ARB
GLAPI PFNGLVERTEXATTRIBP2UIPROC glad_glVertexAttribP2ui;
#define glVertexAttribP2ui glad_glVertexAttribP2ui
GLAPI PFNGLTRANSFORMFEEDBACKVARYINGSNVPROC glad_glTransformFeedbackVaryingsNV;
#define glTransformFeedbackVaryingsNV glad_glTransformFeedbackVaryingsNV
GLAPI PFNGLTEXSUBIMAGE1DPROC glad_glTexSubImage1D;
#define glTexSubImage1D glad_glTexSubImage1D
GLAPI PFNGLBLITFRAMEBUFFEREXTPROC glad_glBlitFramebufferEXT;
#define glBlitFramebufferEXT glad_glBlitFramebufferEXT
GLAPI PFNGLUNIFORMMATRIX4FVARBPROC glad_glUniformMatrix4fvARB;
#define glUniformMatrix4fvARB glad_glUniformMatrix4fvARB
GLAPI PFNGLVERTEXATTRIBP3UIVPROC glad_glVertexAttribP3uiv;
#define glVertexAttribP3uiv glad_glVertexAttribP3uiv
GLAPI PFNGLVERTEXATTRIBI4IVPROC glad_glVertexAttribI4iv;
#define glVertexAttribI4iv glad_glVertexAttribI4iv
GLAPI PFNGLGETMULTITEXLEVELPARAMETERIVEXTPROC glad_glGetMultiTexLevelParameterivEXT;
#define glGetMultiTexLevelParameterivEXT glad_glGetMultiTexLevelParameterivEXT
GLAPI PFNGLGETVERTEXARRAYIVPROC glad_glGetVertexArrayiv;
#define glGetVertexArrayiv glad_glGetVertexArrayiv
GLAPI PFNGLVERTEXATTRIB4FVARBPROC glad_glVertexAttrib4fvARB;
#define glVertexAttrib4fvARB glad_glVertexAttrib4fvARB
GLAPI PFNGLGETNAMEDPROGRAMLOCALPARAMETERIIVEXTPROC glad_glGetNamedProgramLocalParameterIivEXT;
#define glGetNamedProgramLocalParameterIivEXT glad_glGetNamedProgramLocalParameterIivEXT
GLAPI PFNGLPROGRAMUNIFORM3UIEXTPROC glad_glProgramUniform3uiEXT;
#define glProgramUniform3uiEXT glad_glProgramUniform3uiEXT
GLAPI PFNGLPROGRAMUNIFORMMATRIX4X2DVEXTPROC glad_glProgramUniformMatrix4x2dvEXT;
#define glProgramUniformMatrix4x2dvEXT glad_glProgramUniformMatrix4x2dvEXT
GLAPI PFNGLTEXPARAMETERFVPROC glad_glTexParameterfv;
#define glTexParameterfv glad_glTexParameterfv
GLAPI PFNGLCALLCOMMANDLISTNVPROC glad_glCallCommandListNV;
#define glCallCommandListNV glad_glCallCommandListNV
GLAPI PFNGLUNIFORM3DVPROC glad_glUniform3dv;
#define glUniform3dv glad_glUniform3dv
GLAPI PFNGLPROGRAMUNIFORM3FVPROC glad_glProgramUniform3fv;
#define glProgramUniform3fv glad_glProgramUniform3fv
GLAPI PFNGLISBUFFERARBPROC glad_glIsBufferARB;
#define glIsBufferARB glad_glIsBufferARB
GLAPI PFNGLVERTEXATTRIB4DVNVPROC glad_glVertexAttrib4dvNV;
#define glVertexAttrib4dvNV glad_glVertexAttrib4dvNV
GLAPI PFNGLPROGRAMUNIFORM1FVEXTPROC glad_glProgramUniform1fvEXT;
#define glProgramUniform1fvEXT glad_glProgramUniform1fvEXT
GLAPI PFNGLVERTEXATTRIB1DVNVPROC glad_glVertexAttrib1dvNV;
#define glVertexAttrib1dvNV glad_glVertexAttrib1dvNV
GLAPI PFNGLCOMPRESSEDTEXIMAGE1DARBPROC glad_glCompressedTexImage1DARB;
#define glCompressedTexImage1DARB glad_glCompressedTexImage1DARB
GLAPI PFNGLBINDSAMPLERSPROC glad_glBindSamplers;
#define glBindSamplers glad_glBindSamplers
GLAPI PFNGLVERTEXATTRIB4USVPROC glad_glVertexAttrib4usv;
#define glVertexAttrib4usv glad_glVertexAttrib4usv
GLAPI PFNGLGETQUERYOBJECTUI64VPROC glad_glGetQueryObjectui64v;
#define glGetQueryObjectui64v glad_glGetQueryObjectui64v
GLAPI PFNGLCREATETEXTURESPROC glad_glCreateTextures;
#define glCreateTextures glad_glCreateTextures
GLAPI PFNGLGETTEXTUREIMAGEPROC glad_glGetTextureImage;
#define glGetTextureImage glad_glGetTextureImage
GLAPI PFNGLPROGRAMUNIFORM1FVPROC glad_glProgramUniform1fv;
#define glProgramUniform1fv glad_glProgramUniform1fv
GLAPI PFNGLUNIFORMMATRIX4FVPROC glad_glUniformMatrix4fv;
#define glUniformMatrix4fv glad_glUniformMatrix4fv
GLAPI PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC glad_glProgramUniformMatrix4fvEXT;
#define glProgramUniformMatrix4fvEXT glad_glProgramUniformMatrix4fvEXT
GLAPI PFNGLNORMALPOINTEREXTPROC glad_glNormalPointerEXT;
#define glNormalPointerEXT glad_glNormalPointerEXT
GLAPI PFNGLMULTIDRAWARRAYSEXTPROC glad_glMultiDrawArraysEXT;
#define glMultiDrawArraysEXT glad_glMultiDrawArraysEXT
GLAPI PFNGLPATHPARAMETERINVPROC glad_glPathParameteriNV;
#define glPathParameteriNV glad_glPathParameteriNV
GLAPI PFNGLCREATECOMMANDLISTSNVPROC glad_glCreateCommandListsNV;
#define glCreateCommandListsNV glad_glCreateCommandListsNV
GLAPI PFNGLCREATETRANSFORMFEEDBACKSPROC glad_glCreateTransformFeedbacks;
#define glCreateTransformFeedbacks glad_glCreateTransformFeedbacks
GLAPI PFNGLDELETEPROGRAMPIPELINESPROC glad_glDeleteProgramPipelines;
#define glDeleteProgramPipelines glad_glDeleteProgramPipelines
GLAPI PFNGLGENPROGRAMPIPELINESPROC glad_glGenProgramPipelines;
#define glGenProgramPipelines glad_glGenProgramPipelines
GLAPI PFNGLVERTEXARRAYEDGEFLAGOFFSETEXTPROC glad_glVertexArrayEdgeFlagOffsetEXT;
#define glVertexArrayEdgeFlagOffsetEXT glad_glVertexArrayEdgeFlagOffsetEXT
GLAPI PFNGLUNIFORM4UIVEXTPROC glad_glUniform4uivEXT;
#define glUniform4uivEXT glad_glUniform4uivEXT
GLAPI PFNGLUNIFORMSUBROUTINESUIVPROC glad_glUniformSubroutinesuiv;
#define glUniformSubroutinesuiv glad_glUniformSubroutinesuiv
GLAPI PFNGLPROGRAMUNIFORM1I64VNVPROC glad_glProgramUniform1i64vNV;
#define glProgramUniform1i64vNV glad_glProgramUniform1i64vNV
GLAPI PFNGLVERTEXATTRIBI4UIEXTPROC glad_glVertexAttribI4uiEXT;
#define glVertexAttribI4uiEXT glad_glVertexAttribI4uiEXT
GLAPI PFNGLCOMPRESSEDMULTITEXSUBIMAGE1DEXTPROC glad_glCompressedMultiTexSubImage1DEXT;
#define glCompressedMultiTexSubImage1DEXT glad_glCompressedMultiTexSubImage1DEXT
GLAPI PFNGLMULTITEXGENDEXTPROC glad_glMultiTexGendEXT;
#define glMultiTexGendEXT glad_glMultiTexGendEXT
GLAPI PFNGLCOMPILESHADERPROC glad_glCompileShader;
#define glCompileShader glad_glCompileShader
GLAPI PFNGLUNIFORM1UIVEXTPROC glad_glUniform1uivEXT;
#define glUniform1uivEXT glad_glUniform1uivEXT
GLAPI PFNGLUNIFORMMATRIX3FVPROC glad_glUniformMatrix3fv;
#define glUniformMatrix3fv glad_glUniformMatrix3fv
GLAPI PFNGLGETUNIFORMUIVEXTPROC glad_glGetUniformuivEXT;
#define glGetUniformuivEXT glad_glGetUniformuivEXT
GLAPI PFNGLGETQUERYOBJECTI64VPROC glad_glGetQueryObjecti64v;
#define glGetQueryObjecti64v glad_glGetQueryObjecti64v
GLAPI PFNGLVERTEXATTRIBIPOINTERPROC glad_glVertexAttribIPointer;
#define glVertexAttribIPointer glad_glVertexAttribIPointer
GLAPI PFNGLDEPTHRANGEFOESPROC glad_glDepthRangefOES;
#define glDepthRangefOES glad_glDepthRangefOES
GLAPI PFNGLGETNAMEDBUFFERPARAMETERIVPROC glad_glGetNamedBufferParameteriv;
#define glGetNamedBufferParameteriv glad_glGetNamedBufferParameteriv
GLAPI PFNGLGETNAMEDPROGRAMLOCALPARAMETERFVEXTPROC glad_glGetNamedProgramLocalParameterfvEXT;
#define glGetNamedProgramLocalParameterfvEXT glad_glGetNamedProgramLocalParameterfvEXT
GLAPI PFNGLLINEWIDTHPROC glad_glLineWidth;
#define glLineWidth glad_glLineWidth
GLAPI PFNGLINVALIDATEFRAMEBUFFERPROC glad_glInvalidateFramebuffer;
#define glInvalidateFramebuffer glad_glInvalidateFramebuffer
GLAPI PFNGLCOMPRESSEDTEXIMAGE2DPROC glad_glCompressedTexImage2D;
#define glCompressedTexImage2D glad_glCompressedTexImage2D
GLAPI PFNGLGETQUERYBUFFEROBJECTUIVPROC glad_glGetQueryBufferObjectuiv;
#define glGetQueryBufferObjectuiv glad_glGetQueryBufferObjectuiv
GLAPI PFNGLDRAWBUFFERSPROC glad_glDrawBuffers;
#define glDrawBuffers glad_glDrawBuffers
GLAPI PFNGLVERTEXATTRIB1SPROC glad_glVertexAttrib1s;
#define glVertexAttrib1s glad_glVertexAttrib1s
GLAPI PFNGLUNIFORM1IARBPROC glad_glUniform1iARB;
#define glUniform1iARB glad_glUniform1iARB
GLAPI PFNGLUNIFORM1UI64VNVPROC glad_glUniform1ui64vNV;
#define glUniform1ui64vNV glad_glUniform1ui64vNV
GLAPI PFNGLGETCLIPPLANEFOESPROC glad_glGetClipPlanefOES;
#define glGetClipPlanefOES glad_glGetClipPlanefOES
GLAPI PFNGLPROGRAMUNIFORM3IVEXTPROC glad_glProgramUniform3ivEXT;
#define glProgramUniform3ivEXT glad_glProgramUniform3ivEXT
GLAPI PFNGLPROGRAMPARAMETER4FNVPROC glad_glProgramParameter4fNV;
#define glProgramParameter4fNV glad_glProgramParameter4fNV
GLAPI PFNGLDELETEPROGRAMPROC glad_glDeleteProgram;
#define glDeleteProgram glad_glDeleteProgram
GLAPI PFNGLPROGRAMPARAMETER4DNVPROC glad_glProgramParameter4dNV;
#define glProgramParameter4dNV glad_glProgramParameter4dNV
GLAPI PFNGLUNIFORMMATRIX4X3FVPROC glad_glUniformMatrix4x3fv;
#define glUniformMatrix4x3fv glad_glUniformMatrix4x3fv
GLAPI PFNGLTEXTUREBARRIERPROC glad_glTextureBarrier;
#define glTextureBarrier glad_glTextureBarrier
GLAPI PFNGLMULTITEXPARAMETERIIVEXTPROC glad_glMultiTexParameterIivEXT;
#define glMultiTexParameterIivEXT glad_glMultiTexParameterIivEXT
GLAPI PFNGLGETINTERNALFORMATIVPROC glad_glGetInternalformativ;
#define glGetInternalformativ glad_glGetInternalformativ
GLAPI PFNGLMULTITEXCOORD1DARBPROC glad_glMultiTexCoord1dARB;
#define glMultiTexCoord1dARB glad_glMultiTexCoord1dARB
GLAPI PFNGLCLEARBUFFERFIPROC glad_glClearBufferfi;
#define glClearBufferfi glad_glClearBufferfi
GLAPI PFNGLGENVERTEXARRAYSPROC glad_glGenVertexArrays;
#define glGenVertexArrays glad_glGenVertexArrays
GLAPI PFNGLGETTRANSFORMFEEDBACKI64_VPROC glad_glGetTransformFeedbacki64_v;
#define glGetTransformFeedbacki64_v glad_glGetTransformFeedbacki64_v
GLAPI PFNGLENABLEVERTEXARRAYATTRIBPROC glad_glEnableVertexArrayAttrib;
#define glEnableVertexArrayAttrib glad_glEnableVertexArrayAttrib
GLAPI PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC glad_glProgramUniformMatrix3x2dv;
#define glProgramUniformMatrix3x2dv glad_glProgramUniformMatrix3x2dv
GLAPI PFNGLVERTEXBINDINGDIVISORPROC glad_glVertexBindingDivisor;
#define glVertexBindingDivisor glad_glVertexBindingDivisor
GLAPI PFNGLGETSAMPLERPARAMETERIIVPROC glad_glGetSamplerParameterIiv;
#define glGetSamplerParameterIiv glad_glGetSamplerParameterIiv
GLAPI PFNGLVERTEXATTRIBFORMATNVPROC glad_glVertexAttribFormatNV;
#define glVertexAttribFormatNV glad_glVertexAttribFormatNV
GLAPI PFNGLVERTEXATTRIBP4UIVPROC glad_glVertexAttribP4uiv;
#define glVertexAttribP4uiv glad_glVertexAttribP4uiv
GLAPI PFNGLGETQUERYBUFFEROBJECTI64VPROC glad_glGetQueryBufferObjecti64v;
#define glGetQueryBufferObjecti64v glad_glGetQueryBufferObjecti64v
GLAPI PFNGLGETVERTEXATTRIBDVPROC glad_glGetVertexAttribdv;
#define glGetVertexAttribdv glad_glGetVertexAttribdv
GLAPI PFNGLARETEXTURESRESIDENTEXTPROC glad_glAreTexturesResidentEXT;
#define glAreTexturesResidentEXT glad_glAreTexturesResidentEXT
GLAPI PFNGLGETPROGRAMRESOURCENAMEPROC glad_glGetProgramResourceName;
#define glGetProgramResourceName glad_glGetProgramResourceName
GLAPI PFNGLGETFLOATINDEXEDVEXTPROC glad_glGetFloatIndexedvEXT;
#define glGetFloatIndexedvEXT glad_glGetFloatIndexedvEXT
GLAPI PFNGLMULTITEXCOORD2SVARBPROC glad_glMultiTexCoord2svARB;
#define glMultiTexCoord2svARB glad_glMultiTexCoord2svARB
GLAPI PFNGLGETTEXPARAMETERIIVEXTPROC glad_glGetTexParameterIivEXT;
#define glGetTexParameterIivEXT glad_glGetTexParameterIivEXT
GLAPI PFNGLUNIFORM1UIEXTPROC glad_glUniform1uiEXT;
#define glUniform1uiEXT glad_glUniform1uiEXT
GLAPI PFNGLVERTEXATTRIB4NUBARBPROC glad_glVertexAttrib4NubARB;
#define glVertexAttrib4NubARB glad_glVertexAttrib4NubARB
GLAPI PFNGLVERTEXATTRIBL1UI64VARBPROC glad_glVertexAttribL1ui64vARB;
#define glVertexAttribL1ui64vARB glad_glVertexAttribL1ui64vARB
GLAPI PFNGLUNIFORM1I64VNVPROC glad_glUniform1i64vNV;
#define glUniform1i64vNV glad_glUniform1i64vNV
GLAPI PFNGLVERTEXATTRIBS4SVNVPROC glad_glVertexAttribs4svNV;
#define glVertexAttribs4svNV glad_glVertexAttribs4svNV
GLAPI PFNGLUNIFORM1UIPROC glad_glUniform1ui;
#define glUniform1ui glad_glUniform1ui
GLAPI PFNGLVERTEXATTRIB2FVARBPROC glad_glVertexAttrib2fvARB;
#define glVertexAttrib2fvARB glad_glVertexAttrib2fvARB
GLAPI PFNGLCLEARDEPTHFPROC glad_glClearDepthf;
#define glClearDepthf glad_glClearDepthf
GLAPI PFNGLVERTEXATTRIBI4SVEXTPROC glad_glVertexAttribI4svEXT;
#define glVertexAttribI4svEXT glad_glVertexAttribI4svEXT
GLAPI PFNGLMEMORYBARRIERPROC glad_glMemoryBarrier;
#define glMemoryBarrier glad_glMemoryBarrier
GLAPI PFNGLMULTITEXCOORD2IARBPROC glad_glMultiTexCoord2iARB;
#define glMultiTexCoord2iARB glad_glMultiTexCoord2iARB
GLAPI PFNGLGETTEXTUREHANDLEARBPROC glad_glGetTextureHandleARB;
#define glGetTextureHandleARB glad_glGetTextureHandleARB
GLAPI PFNGLISPROGRAMARBPROC glad_glIsProgramARB;
#define glIsProgramARB glad_glIsProgramARB
GLAPI PFNGLBINDIMAGETEXTUREEXTPROC glad_glBindImageTextureEXT;
#define glBindImageTextureEXT glad_glBindImageTextureEXT
GLAPI PFNGLGETFRAGDATALOCATIONPROC glad_glGetFragDataLocation;
#define glGetFragDataLocation glad_glGetFragDataLocation
GLAPI PFNGLMULTITEXCOORD1SVARBPROC glad_glMultiTexCoord1svARB;
#define glMultiTexCoord1svARB glad_glMultiTexCoord1svARB
GLAPI PFNGLVERTEXATTRIBI4IEXTPROC glad_glVertexAttribI4iEXT;
#define glVertexAttribI4iEXT glad_glVertexAttribI4iEXT
GLAPI PFNGLVERTEXATTRIB4NBVARBPROC glad_glVertexAttrib4NbvARB;
#define glVertexAttrib4NbvARB glad_glVertexAttrib4NbvARB
GLAPI PFNGLCOLORPOINTEREXTPROC glad_glColorPointerEXT;
#define glColorPointerEXT glad_glColorPointerEXT
GLAPI PFNGLENABLECLIENTSTATEIEXTPROC glad_glEnableClientStateiEXT;
#define glEnableClientStateiEXT glad_glEnableClientStateiEXT
GLAPI PFNGLCLEARTEXSUBIMAGEPROC glad_glClearTexSubImage;
#define glClearTexSubImage glad_glClearTexSubImage
GLAPI PFNGLDETACHOBJECTARBPROC glad_glDetachObjectARB;
#define glDetachObjectARB glad_glDetachObjectARB
GLAPI PFNGLGETTEXTUREPARAMETERIIVPROC glad_glGetTextureParameterIiv;
#define glGetTextureParameterIiv glad_glGetTextureParameterIiv
GLAPI PFNGLCOMPRESSEDTEXTUREIMAGE3DEXTPROC glad_glCompressedTextureImage3DEXT;
#define glCompressedTextureImage3DEXT glad_glCompressedTextureImage3DEXT
GLAPI PFNGLGETVERTEXARRAYINTEGERI_VEXTPROC glad_glGetVertexArrayIntegeri_vEXT;
#define glGetVertexArrayIntegeri_vEXT glad_glGetVertexArrayIntegeri_vEXT
GLAPI PFNGLCHECKNAMEDFRAMEBUFFERSTATUSEXTPROC glad_glCheckNamedFramebufferStatusEXT;
#define glCheckNamedFramebufferStatusEXT glad_glCheckNamedFramebufferStatusEXT
GLAPI PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC glad_glProgramUniformMatrix4x2dv;
#define glProgramUniformMatrix4x2dv glad_glProgramUniformMatrix4x2dv
GLAPI PFNGLUNIFORM3FARBPROC glad_glUniform3fARB;
#define glUniform3fARB glad_glUniform3fARB
GLAPI PFNGLISSHADERPROC glad_glIsShader;
#define glIsShader glad_glIsShader
GLAPI PFNGLCOPYMULTITEXSUBIMAGE2DEXTPROC glad_glCopyMultiTexSubImage2DEXT;
#define glCopyMultiTexSubImage2DEXT glad_glCopyMultiTexSubImage2DEXT
GLAPI PFNGLENABLEVERTEXATTRIBARRAYARBPROC glad_glEnableVertexAttribArrayARB;
#define glEnableVertexAttribArrayARB glad_glEnableVertexAttribArrayARB
GLAPI PFNGLENABLEPROC glad_glEnable;
#define glEnable glad_glEnable
GLAPI PFNGLGETVERTEXATTRIBIVARBPROC glad_glGetVertexAttribivARB;
#define glGetVertexAttribivARB glad_glGetVertexAttribivARB
GLAPI PFNGLDELETEQUERIESPROC glad_glDeleteQueries;
#define glDeleteQueries glad_glDeleteQueries
GLAPI PFNGLUSEPROGRAMOBJECTARBPROC glad_glUseProgramObjectARB;
#define glUseProgramObjectARB glad_glUseProgramObjectARB
GLAPI PFNGLVERTEXATTRIB4DVPROC glad_glVertexAttrib4dv;
#define glVertexAttrib4dv glad_glVertexAttrib4dv
GLAPI PFNGLVERTEXATTRIB4FPROC glad_glVertexAttrib4f;
#define glVertexAttrib4f glad_glVertexAttrib4f
GLAPI PFNGLUNIFORM3UI64VNVPROC glad_glUniform3ui64vNV;
#define glUniform3ui64vNV glad_glUniform3ui64vNV
GLAPI PFNGLVERTEXATTRIB4DPROC glad_glVertexAttrib4d;
#define glVertexAttrib4d glad_glVertexAttrib4d
GLAPI PFNGLMATRIXLOADIDENTITYEXTPROC glad_glMatrixLoadIdentityEXT;
#define glMatrixLoadIdentityEXT glad_glMatrixLoadIdentityEXT
GLAPI PFNGLPROGRAMUNIFORM3UIPROC glad_glProgramUniform3ui;
#define glProgramUniform3ui glad_glProgramUniform3ui
GLAPI PFNGLVERTEXATTRIB3SVARBPROC glad_glVertexAttrib3svARB;
#define glVertexAttrib3svARB glad_glVertexAttrib3svARB
GLAPI PFNGLMATRIXSCALEDEXTPROC glad_glMatrixScaledEXT;
#define glMatrixScaledEXT glad_glMatrixScaledEXT
GLAPI PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC glad_glProgramUniformMatrix2x3fv;
#define glProgramUniformMatrix2x3fv glad_glProgramUniformMatrix2x3fv
GLAPI PFNGLGETVERTEXATTRIBPOINTERVNVPROC glad_glGetVertexAttribPointervNV;
#define glGetVertexAttribPointervNV glad_glGetVertexAttribPointervNV
GLAPI PFNGLGENERATEMIPMAPEXTPROC glad_glGenerateMipmapEXT;
#define glGenerateMipmapEXT glad_glGenerateMipmapEXT
GLAPI PFNGLVERTEXATTRIBI2UIVEXTPROC glad_glVertexAttribI2uivEXT;
#define glVertexAttribI2uivEXT glad_glVertexAttribI2uivEXT
GLAPI PFNGLPATHCOORDSNVPROC glad_glPathCoordsNV;
#define glPathCoordsNV glad_glPathCoordsNV
GLAPI PFNGLPROGRAMUNIFORM1IPROC glad_glProgramUniform1i;
#define glProgramUniform1i glad_glProgramUniform1i
GLAPI PFNGLPROGRAMUNIFORM1DPROC glad_glProgramUniform1d;
#define glProgramUniform1d glad_glProgramUniform1d
GLAPI PFNGLPROGRAMUNIFORM1FPROC glad_glProgramUniform1f;
#define glProgramUniform1f glad_glProgramUniform1f
GLAPI PFNGLPROGRAMPARAMETERIEXTPROC glad_glProgramParameteriEXT;
#define glProgramParameteriEXT glad_glProgramParameteriEXT
GLAPI PFNGLBEGINTRANSFORMFEEDBACKPROC glad_glBeginTransformFeedback;
#define glBeginTransformFeedback glad_glBeginTransformFeedback
GLAPI PFNGLCOPYTEXTURESUBIMAGE2DPROC glad_glCopyTextureSubImage2D;
#define glCopyTextureSubImage2D glad_glCopyTextureSubImage2D
GLAPI PFNGLPROGRAMUNIFORM3IVPROC glad_glProgramUniform3iv;
#define glProgramUniform3iv glad_glProgramUniform3iv
GLAPI PFNGLBINDFRAGDATALOCATIONEXTPROC glad_glBindFragDataLocationEXT;
#define glBindFragDataLocationEXT glad_glBindFragDataLocationEXT
GLAPI PFNGLFRAMEBUFFERREADBUFFEREXTPROC glad_glFramebufferReadBufferEXT;
#define glFramebufferReadBufferEXT glad_glFramebufferReadBufferEXT
GLAPI PFNGLNAMEDBUFFERSUBDATAPROC glad_glNamedBufferSubData;
#define glNamedBufferSubData glad_glNamedBufferSubData
GLAPI PFNGLGENBUFFERSARBPROC glad_glGenBuffersARB;
#define glGenBuffersARB glad_glGenBuffersARB
GLAPI PFNGLBLENDEQUATIONIPROC glad_glBlendEquationi;
#define glBlendEquationi glad_glBlendEquationi
GLAPI PFNGLVERTEXATTRIB4SPROC glad_glVertexAttrib4s;
#define glVertexAttrib4s glad_glVertexAttrib4s
GLAPI PFNGLVERTEXATTRIB4FVNVPROC glad_glVertexAttrib4fvNV;
#define glVertexAttrib4fvNV glad_glVertexAttrib4fvNV
GLAPI PFNGLGETBUFFERSUBDATAARBPROC glad_glGetBufferSubDataARB;
#define glGetBufferSubDataARB glad_glGetBufferSubDataARB
GLAPI PFNGLGETPATHPARAMETERIVNVPROC glad_glGetPathParameterivNV;
#define glGetPathParameterivNV glad_glGetPathParameterivNV
GLAPI PFNGLGETATTRIBLOCATIONPROC glad_glGetAttribLocation;
#define glGetAttribLocation glad_glGetAttribLocation
GLAPI PFNGLDISABLEVERTEXARRAYATTRIBPROC glad_glDisableVertexArrayAttrib;
#define glDisableVertexArrayAttrib glad_glDisableVertexArrayAttrib
GLAPI PFNGLGETTEXTUREPARAMETERIVPROC glad_glGetTextureParameteriv;
#define glGetTextureParameteriv glad_glGetTextureParameteriv
GLAPI PFNGLGETATTACHEDOBJECTSARBPROC glad_glGetAttachedObjectsARB;
#define glGetAttachedObjectsARB glad_glGetAttachedObjectsARB
GLAPI PFNGLGETTEXIMAGEPROC glad_glGetTexImage;
#define glGetTexImage glad_glGetTexImage
GLAPI PFNGLCLEARDEPTHFOESPROC glad_glClearDepthfOES;
#define glClearDepthfOES glad_glClearDepthfOES
GLAPI PFNGLDELETECOMMANDLISTSNVPROC glad_glDeleteCommandListsNV;
#define glDeleteCommandListsNV glad_glDeleteCommandListsNV
GLAPI PFNGLPATHSUBCOORDSNVPROC glad_glPathSubCoordsNV;
#define glPathSubCoordsNV glad_glPathSubCoordsNV
GLAPI PFNGLBLENDFUNCSEPARATEINDEXEDAMDPROC glad_glBlendFuncSeparateIndexedAMD;
#define glBlendFuncSeparateIndexedAMD glad_glBlendFuncSeparateIndexedAMD
GLAPI PFNGLPROGRAMSTRINGARBPROC glad_glProgramStringARB;
#define glProgramStringARB glad_glProgramStringARB
GLAPI PFNGLGETVERTEXATTRIBIVNVPROC glad_glGetVertexAttribivNV;
#define glGetVertexAttribivNV glad_glGetVertexAttribivNV
GLAPI PFNGLISTEXTUREPROC glad_glIsTexture;
#define glIsTexture glad_glIsTexture
GLAPI PFNGLTEXTURESUBIMAGE1DEXTPROC glad_glTextureSubImage1DEXT;
#define glTextureSubImage1DEXT glad_glTextureSubImage1DEXT
GLAPI PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glad_glGetActiveUniformBlockName;
#define glGetActiveUniformBlockName glad_glGetActiveUniformBlockName
GLAPI PFNGLVERTEXATTRIB2DVARBPROC glad_glVertexAttrib2dvARB;
#define glVertexAttrib2dvARB glad_glVertexAttrib2dvARB
GLAPI PFNGLMAPNAMEDBUFFERPROC glad_glMapNamedBuffer;
#define glMapNamedBuffer glad_glMapNamedBuffer
GLAPI PFNGLVERTEXATTRIBPOINTERPROC glad_glVertexAttribPointer;
#define glVertexAttribPointer glad_glVertexAttribPointer
GLAPI PFNGLDELETETEXTURESEXTPROC glad_glDeleteTexturesEXT;
#define glDeleteTexturesEXT glad_glDeleteTexturesEXT
GLAPI PFNGLDRAWBUFFERSATIPROC glad_glDrawBuffersATI;
#define glDrawBuffersATI glad_glDrawBuffersATI
GLAPI PFNGLGETFLOATI_VPROC glad_glGetFloati_v;
#define glGetFloati_v glad_glGetFloati_v
GLAPI PFNGLPROGRAMUNIFORM1IVEXTPROC glad_glProgramUniform1ivEXT;
#define glProgramUniform1ivEXT glad_glProgramUniform1ivEXT
GLAPI PFNGLVERTEXATTRIBI4BVPROC glad_glVertexAttribI4bv;
#define glVertexAttribI4bv glad_glVertexAttribI4bv
GLAPI PFNGLPROGRAMUNIFORM4UI64NVPROC glad_glProgramUniform4ui64NV;
#define glProgramUniform4ui64NV glad_glProgramUniform4ui64NV
GLAPI PFNGLVERTEXATTRIBP1UIPROC glad_glVertexAttribP1ui;
#define glVertexAttribP1ui glad_glVertexAttribP1ui
GLAPI PFNGLPROGRAMUNIFORM4DVEXTPROC glad_glProgramUniform4dvEXT;
#define glProgramUniform4dvEXT glad_glProgramUniform4dvEXT
GLAPI PFNGLENDTRANSFORMFEEDBACKEXTPROC glad_glEndTransformFeedbackEXT;
#define glEndTransformFeedbackEXT glad_glEndTransformFeedbackEXT
GLAPI PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC glad_glProgramUniformMatrix3x4dv;
#define glProgramUniformMatrix3x4dv glad_glProgramUniformMatrix3x4dv
GLAPI PFNGLENDCONDITIONALRENDERNVXPROC glad_glEndConditionalRenderNVX;
#define glEndConditionalRenderNVX glad_glEndConditionalRenderNVX
GLAPI PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC glad_glGetNamedRenderbufferParameteriv;
#define glGetNamedRenderbufferParameteriv glad_glGetNamedRenderbufferParameteriv
GLAPI PFNGLBINDBUFFERSRANGEPROC glad_glBindBuffersRange;
#define glBindBuffersRange glad_glBindBuffersRange
GLAPI PFNGLDRAWCOMMANDSADDRESSNVPROC glad_glDrawCommandsAddressNV;
#define glDrawCommandsAddressNV glad_glDrawCommandsAddressNV
GLAPI PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glad_glBindFragDataLocationIndexed;
#define glBindFragDataLocationIndexed glad_glBindFragDataLocationIndexed
GLAPI PFNGLUNIFORM2IVPROC glad_glUniform2iv;
#define glUniform2iv glad_glUniform2iv
GLAPI PFNGLNAMEDFRAMEBUFFERRENDERBUFFEREXTPROC glad_glNamedFramebufferRenderbufferEXT;
#define glNamedFramebufferRenderbufferEXT glad_glNamedFramebufferRenderbufferEXT
GLAPI PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC glad_glDrawElementsInstancedBaseVertexBaseInstance;
#define glDrawElementsInstancedBaseVertexBaseInstance glad_glDrawElementsInstancedBaseVertexBaseInstance
GLAPI PFNGLGETNAMEDBUFFERPARAMETERIVEXTPROC glad_glGetNamedBufferParameterivEXT;
#define glGetNamedBufferParameterivEXT glad_glGetNamedBufferParameterivEXT
GLAPI PFNGLTEXTURESUBIMAGE3DPROC glad_glTextureSubImage3D;
#define glTextureSubImage3D glad_glTextureSubImage3D
GLAPI PFNGLFRAMEBUFFERTEXTURE1DPROC glad_glFramebufferTexture1D;
#define glFramebufferTexture1D glad_glFramebufferTexture1D
GLAPI PFNGLGETSHADERIVPROC glad_glGetShaderiv;
#define glGetShaderiv glad_glGetShaderiv
GLAPI PFNGLMATRIXLOADTRANSPOSEDEXTPROC glad_glMatrixLoadTransposedEXT;
#define glMatrixLoadTransposedEXT glad_glMatrixLoadTransposedEXT
GLAPI PFNGLGETPOINTERI_VEXTPROC glad_glGetPointeri_vEXT;
#define glGetPointeri_vEXT glad_glGetPointeri_vEXT
GLAPI PFNGLNAMEDBUFFERSTORAGEPROC glad_glNamedBufferStorage;
#define glNamedBufferStorage glad_glNamedBufferStorage
GLAPI PFNGLPOLYGONOFFSETPROC glad_glPolygonOffset;
#define glPolygonOffset glad_glPolygonOffset
GLAPI PFNGLPROGRAMUNIFORM2UI64NVPROC glad_glProgramUniform2ui64NV;
#define glProgramUniform2ui64NV glad_glProgramUniform2ui64NV
GLAPI PFNGLUNIFORM1DVPROC glad_glUniform1dv;
#define glUniform1dv glad_glUniform1dv
GLAPI PFNGLPROGRAMENVPARAMETER4DVARBPROC glad_glProgramEnvParameter4dvARB;
#define glProgramEnvParameter4dvARB glad_glProgramEnvParameter4dvARB
GLAPI PFNGLMULTITEXSUBIMAGE2DEXTPROC glad_glMultiTexSubImage2DEXT;
#define glMultiTexSubImage2DEXT glad_glMultiTexSubImage2DEXT
GLAPI PFNGLDEBUGMESSAGECONTROLARBPROC glad_glDebugMessageControlARB;
#define glDebugMessageControlARB glad_glDebugMessageControlARB
GLAPI PFNGLISTEXTUREEXTPROC glad_glIsTextureEXT;
#define glIsTextureEXT glad_glIsTextureEXT
GLAPI PFNGLLINKPROGRAMARBPROC glad_glLinkProgramARB;
#define glLinkProgramARB glad_glLinkProgramARB
GLAPI PFNGLVERTEXARRAYVERTEXATTRIBBINDINGEXTPROC glad_glVertexArrayVertexAttribBindingEXT;
#define glVertexArrayVertexAttribBindingEXT glad_glVertexArrayVertexAttribBindingEXT
GLAPI PFNGLUNIFORM1UI64VARBPROC glad_glUniform1ui64vARB;
#define glUniform1ui64vARB glad_glUniform1ui64vARB
GLAPI PFNGLCOMPRESSEDTEXIMAGE3DPROC glad_glCompressedTexImage3D;
#define glCompressedTexImage3D glad_glCompressedTexImage3D
GLAPI PFNGLGETPATHCOORDSNVPROC glad_glGetPathCoordsNV;
#define glGetPathCoordsNV glad_glGetPathCoordsNV
GLAPI PFNGLCOMMANDLISTSEGMENTSNVPROC glad_glCommandListSegmentsNV;
#define glCommandListSegmentsNV glad_glCommandListSegmentsNV
GLAPI PFNGLCLAMPCOLORARBPROC glad_glClampColorARB;
#define glClampColorARB glad_glClampColorARB
GLAPI PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC glad_glTransformFeedbackVaryingsEXT;
#define glTransformFeedbackVaryingsEXT glad_glTransformFeedbackVaryingsEXT
GLAPI PFNGLVERTEXATTRIB2SVNVPROC glad_glVertexAttrib2svNV;
#define glVertexAttrib2svNV glad_glVertexAttrib2svNV
GLAPI PFNGLMULTITEXPARAMETERFVEXTPROC glad_glMultiTexParameterfvEXT;
#define glMultiTexParameterfvEXT glad_glMultiTexParameterfvEXT
GLAPI PFNGLMULTITEXCOORD2IVARBPROC glad_glMultiTexCoord2ivARB;
#define glMultiTexCoord2ivARB glad_glMultiTexCoord2ivARB
GLAPI PFNGLGETINFOLOGARBPROC glad_glGetInfoLogARB;
#define glGetInfoLogARB glad_glGetInfoLogARB
GLAPI PFNGLGETTRACKMATRIXIVNVPROC glad_glGetTrackMatrixivNV;
#define glGetTrackMatrixivNV glad_glGetTrackMatrixivNV
GLAPI PFNGLISNAMEDBUFFERRESIDENTNVPROC glad_glIsNamedBufferResidentNV;
#define glIsNamedBufferResidentNV glad_glIsNamedBufferResidentNV
GLAPI PFNGLMATRIXMULTDEXTPROC glad_glMatrixMultdEXT;
#define glMatrixMultdEXT glad_glMatrixMultdEXT
GLAPI PFNGLCOVERFILLPATHNVPROC glad_glCoverFillPathNV;
#define glCoverFillPathNV glad_glCoverFillPathNV
GLAPI PFNGLGENERATETEXTUREMIPMAPPROC glad_glGenerateTextureMipmap;
#define glGenerateTextureMipmap glad_glGenerateTextureMipmap
GLAPI PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIVEXTPROC glad_glNamedProgramLocalParameterI4uivEXT;
#define glNamedProgramLocalParameterI4uivEXT glad_glNamedProgramLocalParameterI4uivEXT
GLAPI PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glad_glMultiDrawElementsBaseVertex;
#define glMultiDrawElementsBaseVertex glad_glMultiDrawElementsBaseVertex
GLAPI PFNGLPROGRAMLOCALPARAMETER4FVARBPROC glad_glProgramLocalParameter4fvARB;
#define glProgramLocalParameter4fvARB glad_glProgramLocalParameter4fvARB
GLAPI PFNGLMEMORYBARRIEREXTPROC glad_glMemoryBarrierEXT;
#define glMemoryBarrierEXT glad_glMemoryBarrierEXT
GLAPI PFNGLGETTEXPARAMETERIIVPROC glad_glGetTexParameterIiv;
#define glGetTexParameterIiv glad_glGetTexParameterIiv
GLAPI PFNGLNAMEDFRAMEBUFFERPARAMETERIEXTPROC glad_glNamedFramebufferParameteriEXT;
#define glNamedFramebufferParameteriEXT glad_glNamedFramebufferParameteriEXT
GLAPI PFNGLVERTEXATTRIBI4USVEXTPROC glad_glVertexAttribI4usvEXT;
#define glVertexAttribI4usvEXT glad_glVertexAttribI4usvEXT
GLAPI PFNGLTEXTUREBUFFERRANGEEXTPROC glad_glTextureBufferRangeEXT;
#define glTextureBufferRangeEXT glad_glTextureBufferRangeEXT
GLAPI PFNGLTRANSFORMFEEDBACKVARYINGSPROC glad_glTransformFeedbackVaryings;
#define glTransformFeedbackVaryings glad_glTransformFeedbackVaryings
GLAPI PFNGLGETNUNIFORMI64VARBPROC glad_glGetnUniformi64vARB;
#define glGetnUniformi64vARB glad_glGetnUniformi64vARB
GLAPI PFNGLCOMPRESSEDMULTITEXSUBIMAGE2DEXTPROC glad_glCompressedMultiTexSubImage2DEXT;
#define glCompressedMultiTexSubImage2DEXT glad_glCompressedMultiTexSubImage2DEXT
GLAPI PFNGLVERTEXATTRIB4SVARBPROC glad_glVertexAttrib4svARB;
#define glVertexAttrib4svARB glad_glVertexAttrib4svARB
GLAPI PFNGLDEBUGMESSAGECALLBACKPROC glad_glDebugMessageCallback;
#define glDebugMessageCallback glad_glDebugMessageCallback
GLAPI PFNGLSTENCILTHENCOVERSTROKEPATHINSTANCEDNVPROC glad_glStencilThenCoverStrokePathInstancedNV;
#define glStencilThenCoverStrokePathInstancedNV glad_glStencilThenCoverStrokePathInstancedNV
GLAPI PFNGLBUFFERSUBDATAARBPROC glad_glBufferSubDataARB;
#define glBufferSubDataARB glad_glBufferSubDataARB
GLAPI PFNGLGETVERTEXATTRIBLUI64VARBPROC glad_glGetVertexAttribLui64vARB;
#define glGetVertexAttribLui64vARB glad_glGetVertexAttribLui64vARB
GLAPI PFNGLGETPATHMETRICSNVPROC glad_glGetPathMetricsNV;
#define glGetPathMetricsNV glad_glGetPathMetricsNV
GLAPI PFNGLGETBOOLEANI_VPROC glad_glGetBooleani_v;
#define glGetBooleani_v glad_glGetBooleani_v
GLAPI PFNGLCOMPRESSEDMULTITEXIMAGE2DEXTPROC glad_glCompressedMultiTexImage2DEXT;
#define glCompressedMultiTexImage2DEXT glad_glCompressedMultiTexImage2DEXT
GLAPI PFNGLTEXTUREPARAMETERIIVPROC glad_glTextureParameterIiv;
#define glTextureParameterIiv glad_glTextureParameterIiv
GLAPI PFNGLBINDBUFFERRANGENVPROC glad_glBindBufferRangeNV;
#define glBindBufferRangeNV glad_glBindBufferRangeNV
GLAPI PFNGLPROGRAMUNIFORM2DVEXTPROC glad_glProgramUniform2dvEXT;
#define glProgramUniform2dvEXT glad_glProgramUniform2dvEXT
GLAPI PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glad_glGetNamedFramebufferAttachmentParameterivEXT;
#define glGetNamedFramebufferAttachmentParameterivEXT glad_glGetNamedFramebufferAttachmentParameterivEXT
GLAPI PFNGLVERTEXARRAYVERTEXATTRIBIFORMATEXTPROC glad_glVertexArrayVertexAttribIFormatEXT;
#define glVertexArrayVertexAttribIFormatEXT glad_glVertexArrayVertexAttribIFormatEXT
GLAPI PFNGLHINTPROC glad_glHint;
#define glHint glad_glHint
GLAPI PFNGLGETTEXTUREIMAGEEXTPROC glad_glGetTextureImageEXT;
#define glGetTextureImageEXT glad_glGetTextureImageEXT
GLAPI PFNGLMULTIDRAWARRAYSINDIRECTPROC glad_glMultiDrawArraysIndirect;
#define glMultiDrawArraysIndirect glad_glMultiDrawArraysIndirect
GLAPI PFNGLVERTEXATTRIBP2UIVPROC glad_glVertexAttribP2uiv;
#define glVertexAttribP2uiv glad_glVertexAttribP2uiv
GLAPI PFNGLPROGRAMUNIFORM1I64VARBPROC glad_glProgramUniform1i64vARB;
#define glProgramUniform1i64vARB glad_glProgramUniform1i64vARB
GLAPI PFNGLPUSHCLIENTATTRIBDEFAULTEXTPROC glad_glPushClientAttribDefaultEXT;
#define glPushClientAttribDefaultEXT glad_glPushClientAttribDefaultEXT
GLAPI PFNGLVERTEXATTRIB4SVNVPROC glad_glVertexAttrib4svNV;
#define glVertexAttrib4svNV glad_glVertexAttrib4svNV
GLAPI PFNGLCOVERFILLPATHINSTANCEDNVPROC glad_glCoverFillPathInstancedNV;
#define glCoverFillPathInstancedNV glad_glCoverFillPathInstancedNV
GLAPI PFNGLMULTITEXPARAMETERIVEXTPROC glad_glMultiTexParameterivEXT;
#define glMultiTexParameterivEXT glad_glMultiTexParameterivEXT
GLAPI PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC glad_glGetActiveAtomicCounterBufferiv;
#define glGetActiveAtomicCounterBufferiv glad_glGetActiveAtomicCounterBufferiv
GLAPI PFNGLVERTEXARRAYATTRIBBINDINGPROC glad_glVertexArrayAttribBinding;
#define glVertexArrayAttribBinding glad_glVertexArrayAttribBinding
GLAPI PFNGLVERTEXATTRIB3FVNVPROC glad_glVertexAttrib3fvNV;
#define glVertexAttrib3fvNV glad_glVertexAttrib3fvNV
GLAPI PFNGLPROGRAMPARAMETERS4DVNVPROC glad_glProgramParameters4dvNV;
#define glProgramParameters4dvNV glad_glProgramParameters4dvNV
GLAPI PFNGLVERTEXATTRIBL4DVPROC glad_glVertexAttribL4dv;
#define glVertexAttribL4dv glad_glVertexAttribL4dv
GLAPI PFNGLGETTEXPARAMETERIVPROC glad_glGetTexParameteriv;
#define glGetTexParameteriv glad_glGetTexParameteriv
GLAPI PFNGLGETVERTEXATTRIBPOINTERVPROC glad_glGetVertexAttribPointerv;
#define glGetVertexAttribPointerv glad_glGetVertexAttribPointerv
GLAPI PFNGLDRAWARRAYSINSTANCEDARBPROC glad_glDrawArraysInstancedARB;
#define glDrawArraysInstancedARB glad_glDrawArraysInstancedARB
GLAPI PFNGLPROGRAMUNIFORM3I64VNVPROC glad_glProgramUniform3i64vNV;
#define glProgramUniform3i64vNV glad_glProgramUniform3i64vNV
GLAPI PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC glad_glTransformFeedbackBufferBase;
#define glTransformFeedbackBufferBase glad_glTransformFeedbackBufferBase
GLAPI PFNGLDISABLEPROC glad_glDisable;
#define glDisable glad_glDisable
GLAPI PFNGLISPOINTINFILLPATHNVPROC glad_glIsPointInFillPathNV;
#define glIsPointInFillPathNV glad_glIsPointInFillPathNV
GLAPI PFNGLVERTEXATTRIBI2UIEXTPROC glad_glVertexAttribI2uiEXT;
#define glVertexAttribI2uiEXT glad_glVertexAttribI2uiEXT
GLAPI PFNGLPROGRAMUNIFORM4UIVPROC glad_glProgramUniform4uiv;
#define glProgramUniform4uiv glad_glProgramUniform4uiv
GLAPI PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glad_glFramebufferRenderbufferEXT;
#define glFramebufferRenderbufferEXT glad_glFramebufferRenderbufferEXT
GLAPI PFNGLBINDBUFFEROFFSETNVPROC glad_glBindBufferOffsetNV;
#define glBindBufferOffsetNV glad_glBindBufferOffsetNV
GLAPI PFNGLGETVERTEXATTRIBIVPROC glad_glGetVertexAttribiv;
#define glGetVertexAttribiv glad_glGetVertexAttribiv
GLAPI PFNGLVERTEXATTRIBI2UIVPROC glad_glVertexAttribI2uiv;
#define glVertexAttribI2uiv glad_glVertexAttribI2uiv
GLAPI PFNGLUNIFORM2IVARBPROC glad_glUniform2ivARB;
#define glUniform2ivARB glad_glUniform2ivARB
GLAPI PFNGLBINDBUFFERSBASEPROC glad_glBindBuffersBase;
#define glBindBuffersBase glad_glBindBuffersBase
GLAPI PFNGLTEXTURESUBIMAGE2DPROC glad_glTextureSubImage2D;
#define glTextureSubImage2D glad_glTextureSubImage2D
GLAPI PFNGLGETPATHLENGTHNVPROC glad_glGetPathLengthNV;
#define glGetPathLengthNV glad_glGetPathLengthNV
GLAPI PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC glad_glNamedFramebufferParameteri;
#define glNamedFramebufferParameteri glad_glNamedFramebufferParameteri
GLAPI PFNGLTEXTUREBUFFERPROC glad_glTextureBuffer;
#define glTextureBuffer glad_glTextureBuffer
GLAPI PFNGLCLIPPLANEFOESPROC glad_glClipPlanefOES;
#define glClipPlanefOES glad_glClipPlanefOES
GLAPI PFNGLUNIFORM4I64NVPROC glad_glUniform4i64NV;
#define glUniform4i64NV glad_glUniform4i64NV
GLAPI PFNGLUNIFORMHANDLEUI64VARBPROC glad_glUniformHandleui64vARB;
#define glUniformHandleui64vARB glad_glUniformHandleui64vARB
GLAPI PFNGLUNMAPNAMEDBUFFERPROC glad_glUnmapNamedBuffer;
#define glUnmapNamedBuffer glad_glUnmapNamedBuffer
GLAPI PFNGLCREATESTATESNVPROC glad_glCreateStatesNV;
#define glCreateStatesNV glad_glCreateStatesNV
GLAPI PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC glad_glNamedRenderbufferStorageMultisample;
#define glNamedRenderbufferStorageMultisample glad_glNamedRenderbufferStorageMultisample
GLAPI PFNGLCLEARNAMEDBUFFERDATAPROC glad_glClearNamedBufferData;
#define glClearNamedBufferData glad_glClearNamedBufferData
GLAPI PFNGLSTENCILFILLPATHINSTANCEDNVPROC glad_glStencilFillPathInstancedNV;
#define glStencilFillPathInstancedNV glad_glStencilFillPathInstancedNV
GLAPI PFNGLPROGRAMUNIFORM4I64NVPROC glad_glProgramUniform4i64NV;
#define glProgramUniform4i64NV glad_glProgramUniform4i64NV
GLAPI PFNGLTEXTUREPAGECOMMITMENTEXTPROC glad_glTexturePageCommitmentEXT;
#define glTexturePageCommitmentEXT glad_glTexturePageCommitmentEXT
GLAPI PFNGLVERTEXATTRIB4NUIVARBPROC glad_glVertexAttrib4NuivARB;
#define glVertexAttrib4NuivARB glad_glVertexAttrib4NuivARB
GLAPI PFNGLBINDMULTITEXTUREEXTPROC glad_glBindMultiTextureEXT;
#define glBindMultiTextureEXT glad_glBindMultiTextureEXT
GLAPI PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC glad_glDrawArraysInstancedBaseInstance;
#define glDrawArraysInstancedBaseInstance glad_glDrawArraysInstancedBaseInstance
GLAPI PFNGLVERTEXATTRIB3FPROC glad_glVertexAttrib3f;
#define glVertexAttrib3f glad_glVertexAttrib3f
GLAPI PFNGLVERTEXATTRIB3DPROC glad_glVertexAttrib3d;
#define glVertexAttrib3d glad_glVertexAttrib3d
GLAPI PFNGLBLENDCOLORPROC glad_glBlendColor;
#define glBlendColor glad_glBlendColor
GLAPI PFNGLBINDTRANSFORMFEEDBACKNVPROC glad_glBindTransformFeedbackNV;
#define glBindTransformFeedbackNV glad_glBindTransformFeedbackNV
GLAPI PFNGLSAMPLERPARAMETERIUIVPROC glad_glSamplerParameterIuiv;
#define glSamplerParameterIuiv glad_glSamplerParameterIuiv
GLAPI PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC glad_glCheckNamedFramebufferStatus;
#define glCheckNamedFramebufferStatus glad_glCheckNamedFramebufferStatus
GLAPI PFNGLVERTEXATTRIB3SPROC glad_glVertexAttrib3s;
#define glVertexAttrib3s glad_glVertexAttrib3s
GLAPI PFNGLDRAWELEMENTSINSTANCEDEXTPROC glad_glDrawElementsInstancedEXT;
#define glDrawElementsInstancedEXT glad_glDrawElementsInstancedEXT
GLAPI PFNGLNAMEDPROGRAMLOCALPARAMETERI4IEXTPROC glad_glNamedProgramLocalParameterI4iEXT;
#define glNamedProgramLocalParameterI4iEXT glad_glNamedProgramLocalParameterI4iEXT
GLAPI PFNGLDISABLEVERTEXARRAYEXTPROC glad_glDisableVertexArrayEXT;
#define glDisableVertexArrayEXT glad_glDisableVertexArrayEXT
GLAPI PFNGLGETGRAPHICSRESETSTATUSARBPROC glad_glGetGraphicsResetStatusARB;
#define glGetGraphicsResetStatusARB glad_glGetGraphicsResetStatusARB
GLAPI PFNGLUNIFORM4IPROC glad_glUniform4i;
#define glUniform4i glad_glUniform4i
GLAPI PFNGLACTIVETEXTUREPROC glad_glActiveTexture;
#define glActiveTexture glad_glActiveTexture
GLAPI PFNGLENABLEVERTEXATTRIBARRAYPROC glad_glEnableVertexAttribArray;
#define glEnableVertexAttribArray glad_glEnableVertexAttribArray
GLAPI PFNGLUNIFORM4DPROC glad_glUniform4d;
#define glUniform4d glad_glUniform4d
GLAPI PFNGLUNIFORM4FPROC glad_glUniform4f;
#define glUniform4f glad_glUniform4f
GLAPI PFNGLCREATEPROGRAMPIPELINESPROC glad_glCreateProgramPipelines;
#define glCreateProgramPipelines glad_glCreateProgramPipelines
GLAPI PFNGLVERTEXATTRIBI4SVPROC glad_glVertexAttribI4sv;
#define glVertexAttribI4sv glad_glVertexAttribI4sv
GLAPI PFNGLVERTEXATTRIBLPOINTERPROC glad_glVertexAttribLPointer;
#define glVertexAttribLPointer glad_glVertexAttribLPointer
GLAPI PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glad_glDrawElementsInstancedBaseVertex;
#define glDrawElementsInstancedBaseVertex glad_glDrawElementsInstancedBaseVertex
GLAPI PFNGLDRAWTRANSFORMFEEDBACKNVPROC glad_glDrawTransformFeedbackNV;
#define glDrawTransformFeedbackNV glad_glDrawTransformFeedbackNV
GLAPI PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLECOVERAGEEXTPROC glad_glNamedRenderbufferStorageMultisampleCoverageEXT;
#define glNamedRenderbufferStorageMultisampleCoverageEXT glad_glNamedRenderbufferStorageMultisampleCoverageEXT
GLAPI PFNGLPOPDEBUGGROUPPROC glad_glPopDebugGroup;
#define glPopDebugGroup glad_glPopDebugGroup
GLAPI PFNGLUNIFORMBLOCKBINDINGPROC glad_glUniformBlockBinding;
#define glUniformBlockBinding glad_glUniformBlockBinding
GLAPI PFNGLGENERATETEXTUREMIPMAPEXTPROC glad_glGenerateTextureMipmapEXT;
#define glGenerateTextureMipmapEXT glad_glGenerateTextureMipmapEXT
GLAPI PFNGLVERTEXATTRIBIFORMATNVPROC glad_glVertexAttribIFormatNV;
#define glVertexAttribIFormatNV glad_glVertexAttribIFormatNV
GLAPI PFNGLPATHGLYPHRANGENVPROC glad_glPathGlyphRangeNV;
#define glPathGlyphRangeNV glad_glPathGlyphRangeNV
GLAPI PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC glad_glProgramUniformMatrix3x2fv;
#define glProgramUniformMatrix3x2fv glad_glProgramUniformMatrix3x2fv
GLAPI PFNGLUNIFORM2FVARBPROC glad_glUniform2fvARB;
#define glUniform2fvARB glad_glUniform2fvARB
GLAPI PFNGLPROGRAMUNIFORM4I64ARBPROC glad_glProgramUniform4i64ARB;
#define glProgramUniform4i64ARB glad_glProgramUniform4i64ARB
GLAPI PFNGLGETQUERYINDEXEDIVPROC glad_glGetQueryIndexediv;
#define glGetQueryIndexediv glad_glGetQueryIndexediv
GLAPI PFNGLFRAMEBUFFERTEXTURE3DEXTPROC glad_glFramebufferTexture3DEXT;
#define glFramebufferTexture3DEXT glad_glFramebufferTexture3DEXT
GLAPI PFNGLISCOMMANDLISTNVPROC glad_glIsCommandListNV;
#define glIsCommandListNV glad_glIsCommandListNV
GLAPI PFNGLOBJECTLABELPROC glad_glObjectLabel;
#define glObjectLabel glad_glObjectLabel
GLAPI PFNGLDELETETRANSFORMFEEDBACKSNVPROC glad_glDeleteTransformFeedbacksNV;
#define glDeleteTransformFeedbacksNV glad_glDeleteTransformFeedbacksNV
GLAPI PFNGLVERTEXATTRIBLPOINTEREXTPROC glad_glVertexAttribLPointerEXT;
#define glVertexAttribLPointerEXT glad_glVertexAttribLPointerEXT
GLAPI PFNGLSTENCILFUNCSEPARATEPROC glad_glStencilFuncSeparate;
#define glStencilFuncSeparate glad_glStencilFuncSeparate
GLAPI PFNGLCOMPILESHADERARBPROC glad_glCompileShaderARB;
#define glCompileShaderARB glad_glCompileShaderARB
GLAPI PFNGLNAMEDBUFFERSUBDATAEXTPROC glad_glNamedBufferSubDataEXT;
#define glNamedBufferSubDataEXT glad_glNamedBufferSubDataEXT
GLAPI PFNGLTEXBUFFEREXTPROC glad_glTexBufferEXT;
#define glTexBufferEXT glad_glTexBufferEXT
GLAPI PFNGLCREATEPROGRAMPROC glad_glCreateProgram;
#define glCreateProgram glad_glCreateProgram
GLAPI PFNGLBLENDEQUATIONSEPARATEEXTPROC glad_glBlendEquationSeparateEXT;
#define glBlendEquationSeparateEXT glad_glBlendEquationSeparateEXT
GLAPI PFNGLPRIMITIVERESTARTINDEXPROC glad_glPrimitiveRestartIndex;
#define glPrimitiveRestartIndex glad_glPrimitiveRestartIndex
GLAPI PFNGLVERTEXATTRIB2DNVPROC glad_glVertexAttrib2dNV;
#define glVertexAttrib2dNV glad_glVertexAttrib2dNV
GLAPI PFNGLBINDIMAGETEXTURESPROC glad_glBindImageTextures;
#define glBindImageTextures glad_glBindImageTextures
GLAPI PFNGLMULTITEXCOORD2DARBPROC glad_glMultiTexCoord2dARB;
#define glMultiTexCoord2dARB glad_glMultiTexCoord2dARB
GLAPI PFNGLPATHSTRINGNVPROC glad_glPathStringNV;
#define glPathStringNV glad_glPathStringNV
GLAPI PFNGLGETMULTITEXPARAMETERIIVEXTPROC glad_glGetMultiTexParameterIivEXT;
#define glGetMultiTexParameterIivEXT glad_glGetMultiTexParameterIivEXT
GLAPI PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVEXTPROC glad_glGetNamedFramebufferParameterivEXT;
#define glGetNamedFramebufferParameterivEXT glad_glGetNamedFramebufferParameterivEXT
GLAPI PFNGLARRAYELEMENTEXTPROC glad_glArrayElementEXT;
#define glArrayElementEXT glad_glArrayElementEXT
GLAPI PFNGLUNIFORM4IVPROC glad_glUniform4iv;
#define glUniform4iv glad_glUniform4iv
GLAPI PFNGLPATHDASHARRAYNVPROC glad_glPathDashArrayNV;
#define glPathDashArrayNV glad_glPathDashArrayNV
GLAPI PFNGLUNIFORM3UIVPROC glad_glUniform3uiv;
#define glUniform3uiv glad_glUniform3uiv
GLAPI PFNGLCLEARCOLORPROC glad_glClearColor;
#define glClearColor glad_glClearColor
GLAPI PFNGLGETIMAGEHANDLEARBPROC glad_glGetImageHandleARB;
#define glGetImageHandleARB glad_glGetImageHandleARB
GLAPI PFNGLPROGRAMUNIFORMMATRIX2X4DVEXTPROC glad_glProgramUniformMatrix2x4dvEXT;
#define glProgramUniformMatrix2x4dvEXT glad_glProgramUniformMatrix2x4dvEXT
GLAPI PFNGLGETQUERYOBJECTIVARBPROC glad_glGetQueryObjectivARB;
#define glGetQueryObjectivARB glad_glGetQueryObjectivARB
GLAPI PFNGLVERTEXATTRIB3SNVPROC glad_glVertexAttrib3sNV;
#define glVertexAttrib3sNV glad_glVertexAttrib3sNV
GLAPI PFNGLGETNUNIFORMIVPROC glad_glGetnUniformiv;
#define glGetnUniformiv glad_glGetnUniformiv
GLAPI PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC glad_glGetNamedFramebufferParameteriv;
#define glGetNamedFramebufferParameteriv glad_glGetNamedFramebufferParameteriv
GLAPI PFNGLUNIFORM3UIPROC glad_glUniform3ui;
#define glUniform3ui glad_glUniform3ui
GLAPI PFNGLCLEARCOLORIUIEXTPROC glad_glClearColorIuiEXT;
#define glClearColorIuiEXT glad_glClearColorIuiEXT
GLAPI PFNGLVERTEXATTRIBI4UIVPROC glad_glVertexAttribI4uiv;
#define glVertexAttribI4uiv glad_glVertexAttribI4uiv
GLAPI PFNGLVERTEXATTRIB4BVPROC glad_glVertexAttrib4bv;
#define glVertexAttrib4bv glad_glVertexAttrib4bv
GLAPI PFNGLGETPROGRAMRESOURCEINDEXPROC glad_glGetProgramResourceIndex;
#define glGetProgramResourceIndex glad_glGetProgramResourceIndex
GLAPI PFNGLDRAWELEMENTSINDIRECTPROC glad_glDrawElementsIndirect;
#define glDrawElementsIndirect glad_glDrawElementsIndirect
GLAPI PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC glad_glGetSubroutineUniformLocation;
#define glGetSubroutineUniformLocation glad_glGetSubroutineUniformLocation
GLAPI PFNGLVERTEXATTRIB1DARBPROC glad_glVertexAttrib1dARB;
#define glVertexAttrib1dARB glad_glVertexAttrib1dARB
GLAPI PFNGLCOMPILECOMMANDLISTNVPROC glad_glCompileCommandListNV;
#define glCompileCommandListNV glad_glCompileCommandListNV
GLAPI PFNGLPROGRAMUNIFORM1FEXTPROC glad_glProgramUniform1fEXT;
#define glProgramUniform1fEXT glad_glProgramUniform1fEXT
GLAPI PFNGLGETQUERYBUFFEROBJECTUI64VPROC glad_glGetQueryBufferObjectui64v;
#define glGetQueryBufferObjectui64v glad_glGetQueryBufferObjectui64v
GLAPI PFNGLVERTEXATTRIB3SARBPROC glad_glVertexAttrib3sARB;
#define glVertexAttrib3sARB glad_glVertexAttrib3sARB
GLAPI PFNGLUNIFORMMATRIX2X3FVPROC glad_glUniformMatrix2x3fv;
#define glUniformMatrix2x3fv glad_glUniformMatrix2x3fv
GLAPI PFNGLPROGRAMUNIFORM2I64VARBPROC glad_glProgramUniform2i64vARB;
#define glProgramUniform2i64vARB glad_glProgramUniform2i64vARB
GLAPI PFNGLGETVERTEXATTRIBIUIVPROC glad_glGetVertexAttribIuiv;
#define glGetVertexAttribIuiv glad_glGetVertexAttribIuiv
GLAPI PFNGLGETTEXPARAMETERIUIVEXTPROC glad_glGetTexParameterIuivEXT;
#define glGetTexParameterIuivEXT glad_glGetTexParameterIuivEXT
GLAPI PFNGLVERTEXARRAYSECONDARYCOLOROFFSETEXTPROC glad_glVertexArraySecondaryColorOffsetEXT;
#define glVertexArraySecondaryColorOffsetEXT glad_glVertexArraySecondaryColorOffsetEXT
GLAPI PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glad_glCompressedTexSubImage2D;
#define glCompressedTexSubImage2D glad_glCompressedTexSubImage2D
GLAPI PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC glad_glCompressedTexSubImage3DARB;
#define glCompressedTexSubImage3DARB glad_glCompressedTexSubImage3DARB
GLAPI PFNGLPROGRAMBINARYPROC glad_glProgramBinary;
#define glProgramBinary glad_glProgramBinary
GLAPI PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC glad_glTextureStorage2DMultisample;
#define glTextureStorage2DMultisample glad_glTextureStorage2DMultisample
GLAPI PFNGLGETTEXPARAMETERFVPROC glad_glGetTexParameterfv;
#define glGetTexParameterfv glad_glGetTexParameterfv
GLAPI PFNGLTRANSFORMFEEDBACKSTREAMATTRIBSNVPROC glad_glTransformFeedbackStreamAttribsNV;
#define glTransformFeedbackStreamAttribsNV glad_glTransformFeedbackStreamAttribsNV
GLAPI PFNGLVERTEXATTRIB1FPROC glad_glVertexAttrib1f;
#define glVertexAttrib1f glad_glVertexAttrib1f
GLAPI PFNGLMULTITEXCOORD4SARBPROC glad_glMultiTexCoord4sARB;
#define glMultiTexCoord4sARB glad_glMultiTexCoord4sARB
GLAPI PFNGLGETVERTEXARRAYINDEXED64IVPROC glad_glGetVertexArrayIndexed64iv;
#define glGetVertexArrayIndexed64iv glad_glGetVertexArrayIndexed64iv
GLAPI PFNGLMATRIXMULTTRANSPOSE3X3FNVPROC glad_glMatrixMultTranspose3x3fNV;
#define glMatrixMultTranspose3x3fNV glad_glMatrixMultTranspose3x3fNV
GLAPI PFNGLTEXPARAMETERIIVPROC glad_glTexParameterIiv;
#define glTexParameterIiv glad_glTexParameterIiv
GLAPI PFNGLGETSAMPLERPARAMETERIVPROC glad_glGetSamplerParameteriv;
#define glGetSamplerParameteriv glad_glGetSamplerParameteriv
GLAPI PFNGLVERTEXARRAYATTRIBFORMATPROC glad_glVertexArrayAttribFormat;
#define glVertexArrayAttribFormat glad_glVertexArrayAttribFormat
GLAPI PFNGLENDTRANSFORMFEEDBACKPROC glad_glEndTransformFeedback;
#define glEndTransformFeedback glad_glEndTransformFeedback
GLAPI PFNGLDRAWCOMMANDSSTATESADDRESSNVPROC glad_glDrawCommandsStatesAddressNV;
#define glDrawCommandsStatesAddressNV glad_glDrawCommandsStatesAddressNV
GLAPI PFNGLUNIFORM4UI64NVPROC glad_glUniform4ui64NV;
#define glUniform4ui64NV glad_glUniform4ui64NV
GLAPI PFNGLTEXIMAGE2DMULTISAMPLEPROC glad_glTexImage2DMultisample;
#define glTexImage2DMultisample glad_glTexImage2DMultisample
GLAPI PFNGLBINDTEXTUREEXTPROC glad_glBindTextureEXT;
#define glBindTextureEXT glad_glBindTextureEXT
GLAPI PFNGLPROGRAMUNIFORM2DVPROC glad_glProgramUniform2dv;
#define glProgramUniform2dv glad_glProgramUniform2dv
GLAPI PFNGLVERTEXATTRIBI1IVPROC glad_glVertexAttribI1iv;
#define glVertexAttribI1iv glad_glVertexAttribI1iv
GLAPI PFNGLVERTEXATTRIBDIVISORPROC glad_glVertexAttribDivisor;
#define glVertexAttribDivisor glad_glVertexAttribDivisor
GLAPI PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC glad_glGetCompressedTextureImage;
#define glGetCompressedTextureImage glad_glGetCompressedTextureImage
GLAPI PFNGLMATRIXROTATEFEXTPROC glad_glMatrixRotatefEXT;
#define glMatrixRotatefEXT glad_glMatrixRotatefEXT
GLAPI PFNGLACTIVETEXTUREARBPROC glad_glActiveTextureARB;
#define glActiveTextureARB glad_glActiveTextureARB
GLAPI PFNGLGETUNIFORMUI64VNVPROC glad_glGetUniformui64vNV;
#define glGetUniformui64vNV glad_glGetUniformui64vNV
GLAPI PFNGLTEXPARAMETERIIVEXTPROC glad_glTexParameterIivEXT;
#define glTexParameterIivEXT glad_glTexParameterIivEXT
GLAPI PFNGLUNIFORMMATRIX4X2FVPROC glad_glUniformMatrix4x2fv;
#define glUniformMatrix4x2fv glad_glUniformMatrix4x2fv
GLAPI PFNGLGETUNIFORMI64VARBPROC glad_glGetUniformi64vARB;
#define glGetUniformi64vARB glad_glGetUniformi64vARB
GLAPI PFNGLBINDVERTEXBUFFERSPROC glad_glBindVertexBuffers;
#define glBindVertexBuffers glad_glBindVertexBuffers
GLAPI PFNGLVERTEXATTRIB2FPROC glad_glVertexAttrib2f;
#define glVertexAttrib2f glad_glVertexAttrib2f
GLAPI PFNGLVERTEXATTRIB4NSVPROC glad_glVertexAttrib4Nsv;
#define glVertexAttrib4Nsv glad_glVertexAttrib4Nsv
GLAPI PFNGLINVALIDATEBUFFERSUBDATAPROC glad_glInvalidateBufferSubData;
#define glInvalidateBufferSubData glad_glInvalidateBufferSubData
GLAPI PFNGLVERTEXATTRIB3DVPROC glad_glVertexAttrib3dv;
#define glVertexAttrib3dv glad_glVertexAttrib3dv
GLAPI PFNGLNAMEDFRAMEBUFFERTEXTURE1DEXTPROC glad_glNamedFramebufferTexture1DEXT;
#define glNamedFramebufferTexture1DEXT glad_glNamedFramebufferTexture1DEXT
GLAPI PFNGLMAPBUFFERRANGEPROC glad_glMapBufferRange;
#define glMapBufferRange glad_glMapBufferRange
GLAPI PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC glad_glProgramUniformMatrix4x2fv;
#define glProgramUniformMatrix4x2fv glad_glProgramUniformMatrix4x2fv
GLAPI PFNGLGETTEXTUREPARAMETERIIVEXTPROC glad_glGetTextureParameterIivEXT;
#define glGetTextureParameterIivEXT glad_glGetTextureParameterIivEXT
GLAPI PFNGLVERTEXATTRIB3DVARBPROC glad_glVertexAttrib3dvARB;
#define glVertexAttrib3dvARB glad_glVertexAttrib3dvARB
GLAPI PFNGLNAMEDPROGRAMLOCALPARAMETERSI4UIVEXTPROC glad_glNamedProgramLocalParametersI4uivEXT;
#define glNamedProgramLocalParametersI4uivEXT glad_glNamedProgramLocalParametersI4uivEXT
GLAPI PFNGLGENPROGRAMSARBPROC glad_glGenProgramsARB;
#define glGenProgramsARB glad_glGenProgramsARB
GLAPI PFNGLGENTRANSFORMFEEDBACKSNVPROC glad_glGenTransformFeedbacksNV;
#define glGenTransformFeedbacksNV glad_glGenTransformFeedbacksNV
GLAPI PFNGLVERTEXATTRIBI2IVPROC glad_glVertexAttribI2iv;
#define glVertexAttribI2iv glad_glVertexAttribI2iv
GLAPI PFNGLCOPYMULTITEXIMAGE2DEXTPROC glad_glCopyMultiTexImage2DEXT;
#define glCopyMultiTexImage2DEXT glad_glCopyMultiTexImage2DEXT
GLAPI PFNGLVERTEXATTRIB4UBNVPROC glad_glVertexAttrib4ubNV;
#define glVertexAttrib4ubNV glad_glVertexAttrib4ubNV
GLAPI PFNGLGETSHADERPRECISIONFORMATPROC glad_glGetShaderPrecisionFormat;
#define glGetShaderPrecisionFormat glad_glGetShaderPrecisionFormat
GLAPI PFNGLTEXTUREVIEWPROC glad_glTextureView;
#define glTextureView glad_glTextureView
GLAPI PFNGLGETFRAGDATALOCATIONEXTPROC glad_glGetFragDataLocationEXT;
#define glGetFragDataLocationEXT glad_glGetFragDataLocationEXT
GLAPI PFNGLGETCOMPRESSEDMULTITEXIMAGEEXTPROC glad_glGetCompressedMultiTexImageEXT;
#define glGetCompressedMultiTexImageEXT glad_glGetCompressedMultiTexImageEXT
GLAPI PFNGLSHADERSOURCEPROC glad_glShaderSource;
#define glShaderSource glad_glShaderSource
GLAPI PFNGLDELETERENDERBUFFERSPROC glad_glDeleteRenderbuffers;
#define glDeleteRenderbuffers glad_glDeleteRenderbuffers
GLAPI PFNGLGENTEXTURESEXTPROC glad_glGenTexturesEXT;
#define glGenTexturesEXT glad_glGenTexturesEXT
GLAPI PFNGLCOPYTEXIMAGE2DEXTPROC glad_glCopyTexImage2DEXT;
#define glCopyTexImage2DEXT glad_glCopyTexImage2DEXT
GLAPI PFNGLGETNAMEDBUFFERPARAMETERI64VPROC glad_glGetNamedBufferParameteri64v;
#define glGetNamedBufferParameteri64v glad_glGetNamedBufferParameteri64v
GLAPI PFNGLTEXTUREPARAMETERIIVEXTPROC glad_glTextureParameterIivEXT;
#define glTextureParameterIivEXT glad_glTextureParameterIivEXT
GLAPI PFNGLPROGRAMUNIFORMUI64VNVPROC glad_glProgramUniformui64vNV;
#define glProgramUniformui64vNV glad_glProgramUniformui64vNV
GLAPI PFNGLPROGRAMUNIFORM2UI64ARBPROC glad_glProgramUniform2ui64ARB;
#define glProgramUniform2ui64ARB glad_glProgramUniform2ui64ARB
GLAPI PFNGLUNIFORM2UI64NVPROC glad_glUniform2ui64NV;
#define glUniform2ui64NV glad_glUniform2ui64NV
GLAPI PFNGLBUFFERDATAPROC glad_glBufferData;
#define glBufferData glad_glBufferData
GLAPI PFNGLTEXTUREIMAGE1DEXTPROC glad_glTextureImage1DEXT;
#define glTextureImage1DEXT glad_glTextureImage1DEXT
GLAPI PFNGLATTACHOBJECTARBPROC glad_glAttachObjectARB;
#define glAttachObjectARB glad_glAttachObjectARB
GLAPI PFNGLCOPYTEXSUBIMAGE1DEXTPROC glad_glCopyTexSubImage1DEXT;
#define glCopyTexSubImage1DEXT glad_glCopyTexSubImage1DEXT
GLAPI PFNGLUNIFORMMATRIX3FVARBPROC glad_glUniformMatrix3fvARB;
#define glUniformMatrix3fvARB glad_glUniformMatrix3fvARB
GLAPI PFNGLMAKEBUFFERRESIDENTNVPROC glad_glMakeBufferResidentNV;
#define glMakeBufferResidentNV glad_glMakeBufferResidentNV
GLAPI PFNGLPOINTPARAMETERFVEXTPROC glad_glPointParameterfvEXT;
#define glPointParameterfvEXT glad_glPointParameterfvEXT
GLAPI PFNGLUNIFORM4FARBPROC glad_glUniform4fARB;
#define glUniform4fARB glad_glUniform4fARB
GLAPI PFNGLCOPYTEXTURESUBIMAGE3DPROC glad_glCopyTextureSubImage3D;
#define glCopyTextureSubImage3D glad_glCopyTextureSubImage3D
GLAPI PFNGLMULTITEXENVFVEXTPROC glad_glMultiTexEnvfvEXT;
#define glMultiTexEnvfvEXT glad_glMultiTexEnvfvEXT
GLAPI PFNGLVERTEXATTRIBI1IPROC glad_glVertexAttribI1i;
#define glVertexAttribI1i glad_glVertexAttribI1i
GLAPI PFNGLGETDEBUGMESSAGELOGARBPROC glad_glGetDebugMessageLogARB;
#define glGetDebugMessageLogARB glad_glGetDebugMessageLogARB
GLAPI PFNGLNAMEDPROGRAMLOCALPARAMETERS4FVEXTPROC glad_glNamedProgramLocalParameters4fvEXT;
#define glNamedProgramLocalParameters4fvEXT glad_glNamedProgramLocalParameters4fvEXT
GLAPI PFNGLGETBUFFERPOINTERVPROC glad_glGetBufferPointerv;
#define glGetBufferPointerv glad_glGetBufferPointerv
GLAPI PFNGLFRAMEBUFFERTEXTURE3DPROC glad_glFramebufferTexture3D;
#define glFramebufferTexture3D glad_glFramebufferTexture3D
GLAPI PFNGLSAMPLERPARAMETERFVPROC glad_glSamplerParameterfv;
#define glSamplerParameterfv glad_glSamplerParameterfv
GLAPI PFNGLBINDBUFFERARBPROC glad_glBindBufferARB;
#define glBindBufferARB glad_glBindBufferARB
GLAPI PFNGLVERTEXATTRIB1SNVPROC glad_glVertexAttrib1sNV;
#define glVertexAttrib1sNV glad_glVertexAttrib1sNV
GLAPI PFNGLUNIFORM1FVPROC glad_glUniform1fv;
#define glUniform1fv glad_glUniform1fv
GLAPI PFNGLSCISSORINDEXEDVPROC glad_glScissorIndexedv;
#define glScissorIndexedv glad_glScissorIndexedv
GLAPI PFNGLUNIFORM1IVARBPROC glad_glUniform1ivARB;
#define glUniform1ivARB glad_glUniform1ivARB
GLAPI PFNGLMULTITEXCOORD4DVARBPROC glad_glMultiTexCoord4dvARB;
#define glMultiTexCoord4dvARB glad_glMultiTexCoord4dvARB
GLAPI PFNGLVERTEXATTRIB2DPROC glad_glVertexAttrib2d;
#define glVertexAttrib2d glad_glVertexAttrib2d
GLAPI PFNGLMULTIDRAWELEMENTSEXTPROC glad_glMultiDrawElementsEXT;
#define glMultiDrawElementsEXT glad_glMultiDrawElementsEXT
GLAPI PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC glad_glGetTransformFeedbackVaryingEXT;
#define glGetTransformFeedbackVaryingEXT glad_glGetTransformFeedbackVaryingEXT
GLAPI PFNGLVERTEXATTRIB2SPROC glad_glVertexAttrib2s;
#define glVertexAttrib2s glad_glVertexAttrib2s
GLAPI PFNGLTEXIMAGE3DMULTISAMPLEPROC glad_glTexImage3DMultisample;
#define glTexImage3DMultisample glad_glTexImage3DMultisample
GLAPI PFNGLGETUNIFORMBLOCKINDEXPROC glad_glGetUniformBlockIndex;
#define glGetUniformBlockIndex glad_glGetUniformBlockIndex
GLAPI PFNGLFRONTFACEPROC glad_glFrontFace;
#define glFrontFace glad_glFrontFace
GLAPI PFNGLVERTEXATTRIB4DARBPROC glad_glVertexAttrib4dARB;
#define glVertexAttrib4dARB glad_glVertexAttrib4dARB
GLAPI PFNGLISPOINTINSTROKEPATHNVPROC glad_glIsPointInStrokePathNV;
#define glIsPointInStrokePathNV glad_glIsPointInStrokePathNV



typedef void* (* GLADloadproc)(const char *name, void* userptr);
typedef void* (* GLADsimpleloadproc)(const char *name);
GLAPI int gladLoadGL(GLADloadproc load, void* userptr);
GLAPI int gladLoadGLSimple(GLADsimpleloadproc load);



#ifdef GLAD_GL

GLAPI int gladLoadGLInternalLoader();

#endif
#ifdef __cplusplus
}
#endif
#endif
/* Source */
#ifdef GLAD_GL_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int GLAD_GL_VERSION_1_0;
int GLAD_GL_VERSION_1_1;
int GLAD_GL_VERSION_1_2;
int GLAD_GL_VERSION_1_3;
int GLAD_GL_VERSION_1_4;
int GLAD_GL_VERSION_1_5;
int GLAD_GL_VERSION_2_0;
int GLAD_GL_VERSION_2_1;
int GLAD_GL_VERSION_3_0;
int GLAD_GL_VERSION_3_1;
int GLAD_GL_VERSION_3_2;
int GLAD_GL_VERSION_3_3;
int GLAD_GL_VERSION_4_0;
int GLAD_GL_VERSION_4_1;
int GLAD_GL_VERSION_4_2;
int GLAD_GL_VERSION_4_3;
int GLAD_GL_VERSION_4_4;
int GLAD_GL_VERSION_4_5;
int GLAD_GL_NV_transform_feedback2;
int GLAD_GL_ARB_texture_compression;
int GLAD_GL_NV_point_sprite;
int GLAD_GL_ARB_get_texture_sub_image;
int GLAD_GL_AMD_multi_draw_indirect;
int GLAD_GL_EXT_blend_color;
int GLAD_GL_ARB_texture_storage_multisample;
int GLAD_GL_ARB_clip_control;
int GLAD_GL_ARB_instanced_arrays;
int GLAD_GL_EXT_vertex_attrib_64bit;
int GLAD_GL_ARB_vertex_attrib_binding;
int GLAD_GL_ARB_framebuffer_no_attachments;
int GLAD_GL_ARB_texture_view;
int GLAD_GL_ATI_separate_stencil;
int GLAD_GL_EXT_texture_buffer_object;
int GLAD_GL_NV_shader_buffer_store;
int GLAD_GL_ARB_shader_subroutine;
int GLAD_GL_EXT_direct_state_access;
int GLAD_GL_ARB_program_interface_query;
int GLAD_GL_ARB_copy_image;
int GLAD_GL_EXT_texture_object;
int GLAD_GL_NVX_conditional_render;
int GLAD_GL_ARB_copy_buffer;
int GLAD_GL_EXT_blend_func_separate;
int GLAD_GL_ARB_buffer_storage;
int GLAD_GL_ARB_occlusion_query;
int GLAD_GL_ARB_multi_bind;
int GLAD_GL_KHR_robustness;
int GLAD_GL_ARB_provoking_vertex;
int GLAD_GL_EXT_gpu_shader4;
int GLAD_GL_NV_geometry_program4;
int GLAD_GL_NV_path_rendering_shared_edge;
int GLAD_GL_NV_uniform_buffer_unified_memory;
int GLAD_GL_ARB_internalformat_query2;
int GLAD_GL_NV_shader_buffer_load;
int GLAD_GL_EXT_vertex_array;
int GLAD_GL_NV_vertex_buffer_unified_memory;
int GLAD_GL_EXT_blend_equation_separate;
int GLAD_GL_EXT_provoking_vertex;
int GLAD_GL_ARB_point_parameters;
int GLAD_GL_ARB_clear_texture;
int GLAD_GL_ARB_shader_image_load_store;
int GLAD_GL_EXT_multi_draw_arrays;
int GLAD_GL_ARB_shader_atomic_counters;
int GLAD_GL_ARB_texture_barrier;
int GLAD_GL_NV_bindless_multi_draw_indirect;
int GLAD_GL_EXT_transform_feedback;
int GLAD_GL_ARB_transform_feedback2;
int GLAD_GL_ARB_transform_feedback3;
int GLAD_GL_ARB_vertex_program;
int GLAD_GL_ARB_robustness;
int GLAD_GL_ARB_timer_query;
int GLAD_GL_ARB_vertex_array_object;
int GLAD_GL_ARB_vertex_buffer_object;
int GLAD_GL_ARB_map_buffer_range;
int GLAD_GL_ARB_geometry_shader4;
int GLAD_GL_ARB_texture_buffer_object;
int GLAD_GL_EXT_timer_query;
int GLAD_GL_KHR_debug;
int GLAD_GL_ARB_get_program_binary;
int GLAD_GL_ARB_transform_feedback_instanced;
int GLAD_GL_SGIS_point_parameters;
int GLAD_GL_EXT_geometry_shader4;
int GLAD_GL_EXT_framebuffer_object;
int GLAD_GL_ARB_draw_indirect;
int GLAD_GL_NV_bindless_multi_draw_indirect_count;
int GLAD_GL_ARB_ES2_compatibility;
int GLAD_GL_NV_command_list;
int GLAD_GL_ARB_draw_buffers;
int GLAD_GL_ARB_multi_draw_indirect;
int GLAD_GL_ARB_debug_output;
int GLAD_GL_AMD_draw_buffers_blend;
int GLAD_GL_OES_single_precision;
int GLAD_GL_ARB_texture_buffer_range;
int GLAD_GL_NV_explicit_multisample;
int GLAD_GL_EXT_texture_array;
int GLAD_GL_EXT_point_parameters;
int GLAD_GL_APPLE_vertex_array_object;
int GLAD_GL_ARB_vertex_type_2_10_10_10_rev;
int GLAD_GL_ARB_vertex_shader;
int GLAD_GL_APPLE_flush_buffer_range;
int GLAD_GL_ARB_draw_instanced;
int GLAD_GL_ARB_shader_objects;
int GLAD_GL_ARB_tessellation_shader;
int GLAD_GL_EXT_draw_buffers2;
int GLAD_GL_ARB_vertex_attrib_64bit;
int GLAD_GL_ARB_imaging;
int GLAD_GL_INGR_blend_func_separate;
int GLAD_GL_NV_path_rendering;
int GLAD_GL_ARB_framebuffer_object;
int GLAD_GL_ARB_draw_buffers_blend;
int GLAD_GL_ARB_direct_state_access;
int GLAD_GL_ARB_gpu_shader_int64;
int GLAD_GL_ARB_texture_storage;
int GLAD_GL_NV_vertex_program;
int GLAD_GL_ARB_color_buffer_float;
int GLAD_GL_ARB_clear_buffer_object;
int GLAD_GL_ARB_multisample;
int GLAD_GL_NV_conditional_render;
int GLAD_GL_EXT_texture_filter_anisotropic;
int GLAD_GL_ARB_shader_storage_buffer_object;
int GLAD_GL_ARB_sample_shading;
int GLAD_GL_EXT_blend_minmax;
int GLAD_GL_EXT_framebuffer_blit;
int GLAD_GL_ARB_gpu_shader_fp64;
int GLAD_GL_NV_vertex_program4;
int GLAD_GL_ARB_base_instance;
int GLAD_GL_ARB_compute_shader;
int GLAD_GL_ARB_blend_func_extended;
int GLAD_GL_ARB_sync;
int GLAD_GL_EXT_subtexture;
int GLAD_GL_ARB_ES3_1_compatibility;
int GLAD_GL_ARB_bindless_texture;
int GLAD_GL_ARB_internalformat_query;
int GLAD_GL_EXT_texture_integer;
int GLAD_GL_ARB_compute_variable_group_size;
int GLAD_GL_ARB_texture_multisample;
int GLAD_GL_AMD_gpu_shader_int64;
int GLAD_GL_ARB_invalidate_subdata;
int GLAD_GL_EXT_framebuffer_multisample;
int GLAD_GL_EXT_shader_image_load_store;
int GLAD_GL_EXT_texture3D;
int GLAD_GL_ARB_multitexture;
int GLAD_GL_EXT_draw_instanced;
int GLAD_GL_ARB_draw_elements_base_vertex;
int GLAD_GL_ARB_viewport_array;
int GLAD_GL_ARB_separate_shader_objects;
int GLAD_GL_ATI_draw_buffers;
int GLAD_GL_NV_transform_feedback;
int GLAD_GL_NVX_gpu_memory_info;
int GLAD_GL_ARB_sampler_objects;
int GLAD_GL_EXT_copy_texture;
int GLAD_GL_EXT_draw_range_elements;
int GLAD_GL_ARB_uniform_buffer_object;



PFNGLMULTIDRAWARRAYSINDIRECTBINDLESSCOUNTNVPROC glad_glMultiDrawArraysIndirectBindlessCountNV;
PFNGLCOPYTEXIMAGE1DPROC glad_glCopyTexImage1D;
PFNGLPROGRAMUNIFORM1I64ARBPROC glad_glProgramUniform1i64ARB;
PFNGLSTENCILMASKSEPARATEPROC glad_glStencilMaskSeparate;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glad_glGetFramebufferAttachmentParameterivEXT;
PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC glad_glTextureStorage3DMultisample;
PFNGLGETINTEGERINDEXEDVEXTPROC glad_glGetIntegerIndexedvEXT;
PFNGLTEXTUREPARAMETERFVPROC glad_glTextureParameterfv;
PFNGLPROGRAMUNIFORM4UI64ARBPROC glad_glProgramUniform4ui64ARB;
PFNGLMULTITEXCOORD2SARBPROC glad_glMultiTexCoord2sARB;
PFNGLPROGRAMENVPARAMETER4FARBPROC glad_glProgramEnvParameter4fARB;
PFNGLCOPYTEXTUREIMAGE1DEXTPROC glad_glCopyTextureImage1DEXT;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glad_glCompressedTexSubImage3D;
PFNGLTEXTURESTORAGE3DEXTPROC glad_glTextureStorage3DEXT;
PFNGLUNIFORM4UIEXTPROC glad_glUniform4uiEXT;
PFNGLPROGRAMUNIFORM3UI64ARBPROC glad_glProgramUniform3ui64ARB;
PFNGLVERTEXARRAYBINDVERTEXBUFFEREXTPROC glad_glVertexArrayBindVertexBufferEXT;
PFNGLVERTEXARRAYELEMENTBUFFERPROC glad_glVertexArrayElementBuffer;
PFNGLVERTEXATTRIBI3UIVEXTPROC glad_glVertexAttribI3uivEXT;
PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC glad_glTransformFeedbackBufferRange;
PFNGLPATHGLYPHSNVPROC glad_glPathGlyphsNV;
PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC glad_glDrawElementsInstancedBaseInstance;
PFNGLGETTEXTUREPARAMETERIVEXTPROC glad_glGetTextureParameterivEXT;
PFNGLVERTEXATTRIBP4UIPROC glad_glVertexAttribP4ui;
PFNGLISBUFFERPROC glad_glIsBuffer;
PFNGLGETMULTISAMPLEFVPROC glad_glGetMultisamplefv;
PFNGLCOMPRESSEDMULTITEXSUBIMAGE3DEXTPROC glad_glCompressedMultiTexSubImage3DEXT;
PFNGLMINSAMPLESHADINGPROC glad_glMinSampleShading;
PFNGLCOVERSTROKEPATHNVPROC glad_glCoverStrokePathNV;
PFNGLDEBUGMESSAGECONTROLPROC glad_glDebugMessageControl;
PFNGLPROGRAMUNIFORM4IEXTPROC glad_glProgramUniform4iEXT;
PFNGLVERTEXATTRIB2SVARBPROC glad_glVertexAttrib2svARB;
PFNGLVERTEXATTRIB4UBVPROC glad_glVertexAttrib4ubv;
PFNGLVERTEXATTRIB4NUBVARBPROC glad_glVertexAttrib4NubvARB;
PFNGLMATRIXPUSHEXTPROC glad_glMatrixPushEXT;
PFNGLPROGRAMUNIFORM2FEXTPROC glad_glProgramUniform2fEXT;
PFNGLMULTIDRAWELEMENTSINDIRECTBINDLESSNVPROC glad_glMultiDrawElementsIndirectBindlessNV;
PFNGLUNIFORMMATRIX3DVPROC glad_glUniformMatrix3dv;
PFNGLGETVERTEXATTRIBLDVEXTPROC glad_glGetVertexAttribLdvEXT;
PFNGLGETVERTEXARRAYINDEXEDIVPROC glad_glGetVertexArrayIndexediv;
PFNGLRESUMETRANSFORMFEEDBACKPROC glad_glResumeTransformFeedback;
PFNGLGETMULTITEXPARAMETERIVEXTPROC glad_glGetMultiTexParameterivEXT;
PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC glad_glProgramUniformMatrix3x4fvEXT;
PFNGLVERTEXATTRIBI2IPROC glad_glVertexAttribI2i;
PFNGLBINDPROGRAMNVPROC glad_glBindProgramNV;
PFNGLLOADPROGRAMNVPROC glad_glLoadProgramNV;
PFNGLGETGRAPHICSRESETSTATUSPROC glad_glGetGraphicsResetStatus;
PFNGLVERTEXATTRIB1FVPROC glad_glVertexAttrib1fv;
PFNGLISENABLEDPROC glad_glIsEnabled;
PFNGLSTENCILOPPROC glad_glStencilOp;
PFNGLFRAMEBUFFERTEXTURE2DPROC glad_glFramebufferTexture2D;
PFNGLVERTEXARRAYATTRIBIFORMATPROC glad_glVertexArrayAttribIFormat;
PFNGLGENFRAMEBUFFERSPROC glad_glGenFramebuffers;
PFNGLTEXSUBIMAGE3DEXTPROC glad_glTexSubImage3DEXT;
PFNGLGETATTACHEDSHADERSPROC glad_glGetAttachedShaders;
PFNGLDELETEVERTEXARRAYSPROC glad_glDeleteVertexArrays;
PFNGLVIEWPORTARRAYVPROC glad_glViewportArrayv;
PFNGLMULTITEXCOORD3DVARBPROC glad_glMultiTexCoord3dvARB;
PFNGLUNIFORM3UI64ARBPROC glad_glUniform3ui64ARB;
PFNGLVERTEXATTRIB2FVNVPROC glad_glVertexAttrib2fvNV;
PFNGLGETNAMEDBUFFERSUBDATAEXTPROC glad_glGetNamedBufferSubDataEXT;
PFNGLMEMORYBARRIERBYREGIONPROC glad_glMemoryBarrierByRegion;
PFNGLMATRIXROTATEDEXTPROC glad_glMatrixRotatedEXT;
PFNGLISPROGRAMNVPROC glad_glIsProgramNV;
PFNGLUNIFORM2DVPROC glad_glUniform2dv;
PFNGLMULTITEXPARAMETERIEXTPROC glad_glMultiTexParameteriEXT;
PFNGLGETPROGRAMSTRINGNVPROC glad_glGetProgramStringNV;
PFNGLGETUNIFORMFVPROC glad_glGetUniformfv;
PFNGLGETUNIFORMUIVPROC glad_glGetUniformuiv;
PFNGLGETNUNIFORMFVARBPROC glad_glGetnUniformfvARB;
PFNGLVERTEXARRAYBINDINGDIVISORPROC glad_glVertexArrayBindingDivisor;
PFNGLPROGRAMUNIFORM1DVEXTPROC glad_glProgramUniform1dvEXT;
PFNGLMULTITEXCOORD4IARBPROC glad_glMultiTexCoord4iARB;
PFNGLGETRENDERBUFFERPARAMETERIVPROC glad_glGetRenderbufferParameteriv;
PFNGLPROGRAMUNIFORM3FEXTPROC glad_glProgramUniform3fEXT;
PFNGLPROGRAMPATHFRAGMENTINPUTGENNVPROC glad_glProgramPathFragmentInputGenNV;
PFNGLFENCESYNCPROC glad_glFenceSync;
PFNGLGETVERTEXATTRIBIIVEXTPROC glad_glGetVertexAttribIivEXT;
PFNGLBINDRENDERBUFFEREXTPROC glad_glBindRenderbufferEXT;
PFNGLFRAMEBUFFERDRAWBUFFERSEXTPROC glad_glFramebufferDrawBuffersEXT;
PFNGLVERTEXATTRIBS4FVNVPROC glad_glVertexAttribs4fvNV;
PFNGLPAUSETRANSFORMFEEDBACKNVPROC glad_glPauseTransformFeedbackNV;
PFNGLGETQUERYOBJECTI64VEXTPROC glad_glGetQueryObjecti64vEXT;
PFNGLPROGRAMUNIFORM4UIEXTPROC glad_glProgramUniform4uiEXT;
PFNGLVALIDATEPROGRAMPIPELINEPROC glad_glValidateProgramPipeline;
PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC glad_glStencilStrokePathInstancedNV;
PFNGLGENQUERIESARBPROC glad_glGenQueriesARB;
PFNGLPROGRAMUNIFORM2I64NVPROC glad_glProgramUniform2i64NV;
PFNGLGENSAMPLERSPROC glad_glGenSamplers;
PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC glad_glDrawTransformFeedbackInstanced;
PFNGLSTENCILTHENCOVERSTROKEPATHNVPROC glad_glStencilThenCoverStrokePathNV;
PFNGLDELETEVERTEXARRAYSAPPLEPROC glad_glDeleteVertexArraysAPPLE;
PFNGLGETNAMEDBUFFERPARAMETERUI64VNVPROC glad_glGetNamedBufferParameterui64vNV;
PFNGLDRAWTRANSFORMFEEDBACKPROC glad_glDrawTransformFeedback;
PFNGLGETTEXPARAMETERIUIVPROC glad_glGetTexParameterIuiv;
PFNGLVERTEXATTRIB4NBVPROC glad_glVertexAttrib4Nbv;
PFNGLVERTEXATTRIBL4DEXTPROC glad_glVertexAttribL4dEXT;
PFNGLUNIFORMUI64VNVPROC glad_glUniformui64vNV;
PFNGLISSYNCPROC glad_glIsSync;
PFNGLGETOBJECTPTRLABELPROC glad_glGetObjectPtrLabel;
PFNGLTEXTUREPARAMETERIPROC glad_glTextureParameteri;
PFNGLTEXTUREPARAMETERFPROC glad_glTextureParameterf;
PFNGLUNIFORMMATRIX2X3DVPROC glad_glUniformMatrix2x3dv;
PFNGLFRAMEBUFFERDRAWBUFFEREXTPROC glad_glFramebufferDrawBufferEXT;
PFNGLVERTEXATTRIBL3DPROC glad_glVertexAttribL3d;
PFNGLCOMPRESSEDTEXTUREIMAGE1DEXTPROC glad_glCompressedTextureImage1DEXT;
PFNGLMATRIXLOADTRANSPOSE3X3FNVPROC glad_glMatrixLoadTranspose3x3fNV;
PFNGLPROGRAMUNIFORM1UIPROC glad_glProgramUniform1ui;
PFNGLCOPYTEXIMAGE2DPROC glad_glCopyTexImage2D;
PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC glad_glProgramUniformMatrix2x4fvEXT;
PFNGLCOPYTEXSUBIMAGE3DEXTPROC glad_glCopyTexSubImage3DEXT;
PFNGLRENDERBUFFERSTORAGEEXTPROC glad_glRenderbufferStorageEXT;
PFNGLPATHPARAMETERIVNVPROC glad_glPathParameterivNV;
PFNGLUNIFORM4UIVPROC glad_glUniform4uiv;
PFNGLSCISSORARRAYVPROC glad_glScissorArrayv;
PFNGLPOINTPARAMETERFVARBPROC glad_glPointParameterfvARB;
PFNGLINVALIDATESUBFRAMEBUFFERPROC glad_glInvalidateSubFramebuffer;
PFNGLMAPBUFFERPROC glad_glMapBuffer;
PFNGLVERTEXATTRIBLFORMATPROC glad_glVertexAttribLFormat;
PFNGLDELETESYNCPROC glad_glDeleteSync;
PFNGLGETPROGRAMPARAMETERFVNVPROC glad_glGetProgramParameterfvNV;
PFNGLVERTEXARRAYVERTEXATTRIBLOFFSETEXTPROC glad_glVertexArrayVertexAttribLOffsetEXT;
PFNGLUNIFORM3IVPROC glad_glUniform3iv;
PFNGLPROGRAMUNIFORM4IVPROC glad_glProgramUniform4iv;
PFNGLVERTEXATTRIB1DVARBPROC glad_glVertexAttrib1dvARB;
PFNGLTEXTURESUBIMAGE2DEXTPROC glad_glTextureSubImage2DEXT;
PFNGLISVERTEXARRAYAPPLEPROC glad_glIsVertexArrayAPPLE;
PFNGLUSEPROGRAMPROC glad_glUseProgram;
PFNGLMULTITEXCOORD4FVARBPROC glad_glMultiTexCoord4fvARB;
PFNGLUNIFORM3FVARBPROC glad_glUniform3fvARB;
PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC glad_glProgramUniformMatrix3x2fvEXT;
PFNGLDELETEBUFFERSPROC glad_glDeleteBuffers;
PFNGLMULTITEXIMAGE1DEXTPROC glad_glMultiTexImage1DEXT;
PFNGLDELETEOBJECTARBPROC glad_glDeleteObjectARB;
PFNGLMULTITEXCOORD3DARBPROC glad_glMultiTexCoord3dARB;
PFNGLSCISSORPROC glad_glScissor;
PFNGLNAMEDFRAMEBUFFERTEXTUREEXTPROC glad_glNamedFramebufferTextureEXT;
PFNGLTEXTUREPARAMETERIEXTPROC glad_glTextureParameteriEXT;
PFNGLMULTITEXCOORD2DVARBPROC glad_glMultiTexCoord2dvARB;
PFNGLDELETESHADERPROC glad_glDeleteShader;
PFNGLCOPYTEXTURESUBIMAGE1DPROC glad_glCopyTextureSubImage1D;
PFNGLTEXTUREPARAMETERIUIVPROC glad_glTextureParameterIuiv;
PFNGLTRANSFORMFEEDBACKATTRIBSNVPROC glad_glTransformFeedbackAttribsNV;
PFNGLUNIFORM2UIPROC glad_glUniform2ui;
PFNGLVERTEXATTRIBIPOINTEREXTPROC glad_glVertexAttribIPointerEXT;
PFNGLVERTEXATTRIBI3IPROC glad_glVertexAttribI3i;
PFNGLVERTEXATTRIBFORMATPROC glad_glVertexAttribFormat;
PFNGLGETPROGRAMIVARBPROC glad_glGetProgramivARB;
PFNGLVERTEXATTRIBI4USVPROC glad_glVertexAttribI4usv;
PFNGLTEXPARAMETERFPROC glad_glTexParameterf;
PFNGLVERTEXATTRIBBINDINGPROC glad_glVertexAttribBinding;
PFNGLTEXPARAMETERIPROC glad_glTexParameteri;
PFNGLGETSHADERSOURCEPROC glad_glGetShaderSource;
PFNGLTEXTUREBUFFEREXTPROC glad_glTextureBufferEXT;
PFNGLGETMULTITEXIMAGEEXTPROC glad_glGetMultiTexImageEXT;
PFNGLGETPOINTERINDEXEDVEXTPROC glad_glGetPointerIndexedvEXT;
PFNGLBUFFERPARAMETERIAPPLEPROC glad_glBufferParameteriAPPLE;
PFNGLVERTEXATTRIB3FVPROC glad_glVertexAttrib3fv;
PFNGLVERTEXATTRIBP1UIVPROC glad_glVertexAttribP1uiv;
PFNGLLINKPROGRAMPROC glad_glLinkProgram;
PFNGLGETOBJECTLABELPROC glad_glGetObjectLabel;
PFNGLVERTEXPOINTEREXTPROC glad_glVertexPointerEXT;
PFNGLDEBUGMESSAGECALLBACKARBPROC glad_glDebugMessageCallbackARB;
PFNGLGETSTRINGPROC glad_glGetString;
PFNGLGETPATHPARAMETERFVNVPROC glad_glGetPathParameterfvNV;
PFNGLPROGRAMUNIFORM3IPROC glad_glProgramUniform3i;
PFNGLENDQUERYPROC glad_glEndQuery;
PFNGLVERTEXATTRIBI4IVEXTPROC glad_glVertexAttribI4ivEXT;
PFNGLFRAMEBUFFERPARAMETERIPROC glad_glFramebufferParameteri;
PFNGLVERTEXATTRIBI2UIPROC glad_glVertexAttribI2ui;
PFNGLUNIFORM2IARBPROC glad_glUniform2iARB;
PFNGLDELETETEXTURESPROC glad_glDeleteTextures;
PFNGLCREATEVERTEXARRAYSPROC glad_glCreateVertexArrays;
PFNGLBEGINCONDITIONALRENDERPROC glad_glBeginConditionalRender;
PFNGLMULTITEXCOORD3FARBPROC glad_glMultiTexCoord3fARB;
PFNGLGETQUERYOBJECTUIVARBPROC glad_glGetQueryObjectuivARB;
PFNGLSAMPLERPARAMETERIPROC glad_glSamplerParameteri;
PFNGLSAMPLERPARAMETERFPROC glad_glSamplerParameterf;
PFNGLUNIFORM1FPROC glad_glUniform1f;
PFNGLUNIFORM1DPROC glad_glUniform1d;
PFNGLCLEARCOLORIIEXTPROC glad_glClearColorIiEXT;
PFNGLGETCOMPRESSEDTEXIMAGEPROC glad_glGetCompressedTexImage;
PFNGLMINSAMPLESHADINGARBPROC glad_glMinSampleShadingARB;
PFNGLUNIFORM1IPROC glad_glUniform1i;
PFNGLFRAMEBUFFERTEXTUREEXTPROC glad_glFramebufferTextureEXT;
PFNGLCULLFACEPROC glad_glCullFace;
PFNGLPROGRAMUNIFORM4IPROC glad_glProgramUniform4i;
PFNGLUNIFORMHANDLEUI64ARBPROC glad_glUniformHandleui64ARB;
PFNGLPROGRAMUNIFORM4FPROC glad_glProgramUniform4f;
PFNGLVIEWPORTINDEXEDFPROC glad_glViewportIndexedf;
PFNGLPROGRAMUNIFORM4DPROC glad_glProgramUniform4d;
PFNGLPOINTALONGPATHNVPROC glad_glPointAlongPathNV;
PFNGLENDQUERYARBPROC glad_glEndQueryARB;
PFNGLVERTEXATTRIB4UIVARBPROC glad_glVertexAttrib4uivARB;
PFNGLRESUMETRANSFORMFEEDBACKNVPROC glad_glResumeTransformFeedbackNV;
PFNGLQUERYCOUNTERPROC glad_glQueryCounter;
PFNGLVERTEXATTRIB3FARBPROC glad_glVertexAttrib3fARB;
PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC glad_glDrawTransformFeedbackStreamInstanced;
PFNGLGETBUFFERPARAMETERIVPROC glad_glGetBufferParameteriv;
PFNGLNAMEDFRAMEBUFFERTEXTURE3DEXTPROC glad_glNamedFramebufferTexture3DEXT;
PFNGLTEXPARAMETERIUIVPROC glad_glTexParameterIuiv;
PFNGLGETCOMPRESSEDTEXIMAGEARBPROC glad_glGetCompressedTexImageARB;
PFNGLDELETEPATHSNVPROC glad_glDeletePathsNV;
PFNGLVERTEXATTRIBI4UBVEXTPROC glad_glVertexAttribI4ubvEXT;
PFNGLVERTEXATTRIB1DNVPROC glad_glVertexAttrib1dNV;
PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIEXTPROC glad_glNamedProgramLocalParameterI4uiEXT;
PFNGLPROGRAMUNIFORM1IEXTPROC glad_glProgramUniform1iEXT;
PFNGLDELETEQUERIESARBPROC glad_glDeleteQueriesARB;
PFNGLTEXTURESTORAGE3DPROC glad_glTextureStorage3D;
PFNGLISTRANSFORMFEEDBACKPROC glad_glIsTransformFeedback;
PFNGLISPROGRAMPIPELINEPROC glad_glIsProgramPipeline;
PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC glad_glGetActiveSubroutineUniformName;
PFNGLVERTEXATTRIB4SNVPROC glad_glVertexAttrib4sNV;
PFNGLPROGRAMLOCALPARAMETER4DVARBPROC glad_glProgramLocalParameter4dvARB;
PFNGLTEXTUREPARAMETERFVEXTPROC glad_glTextureParameterfvEXT;
PFNGLGETNUNIFORMFVPROC glad_glGetnUniformfv;
PFNGLVERTEXATTRIBL2DVPROC glad_glVertexAttribL2dv;
PFNGLTEXRENDERBUFFERNVPROC glad_glTexRenderbufferNV;
PFNGLBINDVERTEXARRAYPROC glad_glBindVertexArray;
PFNGLVERTEXATTRIBL2DPROC glad_glVertexAttribL2d;
PFNGLSTENCILFUNCPROC glad_glStencilFunc;
PFNGLGETVERTEXATTRIBDVNVPROC glad_glGetVertexAttribdvNV;
PFNGLGETQUERYBUFFEROBJECTIVPROC glad_glGetQueryBufferObjectiv;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glad_glGetFramebufferAttachmentParameteriv;
PFNGLGETPROGRAMPIPELINEIVPROC glad_glGetProgramPipelineiv;
PFNGLVERTEXATTRIBL4DPROC glad_glVertexAttribL4d;
PFNGLGETSHADERINFOLOGPROC glad_glGetShaderInfoLog;
PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXTPROC glad_glCompressedTextureSubImage2DEXT;
PFNGLCLEARBUFFERSUBDATAPROC glad_glClearBufferSubData;
PFNGLVERTEXATTRIBI4IPROC glad_glVertexAttribI4i;
PFNGLGETINTEGERUI64I_VNVPROC glad_glGetIntegerui64i_vNV;
PFNGLBINDATTRIBLOCATIONPROC glad_glBindAttribLocation;
PFNGLBLENDEQUATIONSEPARATEPROC glad_glBlendEquationSeparate;
PFNGLDRAWELEMENTSINSTANCEDARBPROC glad_glDrawElementsInstancedARB;
PFNGLBEGINQUERYARBPROC glad_glBeginQueryARB;
PFNGLGETSUBROUTINEINDEXPROC glad_glGetSubroutineIndex;
PFNGLVERTEXATTRIB2SVPROC glad_glVertexAttrib2sv;
PFNGLPATHPARAMETERFNVPROC glad_glPathParameterfNV;
PFNGLMULTITEXCOORD3FVARBPROC glad_glMultiTexCoord3fvARB;
PFNGLVERTEXATTRIB1FVARBPROC glad_glVertexAttrib1fvARB;
PFNGLNAMEDPROGRAMLOCALPARAMETER4FEXTPROC glad_glNamedProgramLocalParameter4fEXT;
PFNGLVERTEXATTRIBL1DEXTPROC glad_glVertexAttribL1dEXT;
PFNGLGETNUNIFORMDVARBPROC glad_glGetnUniformdvARB;
PFNGLPROGRAMUNIFORM1UIVEXTPROC glad_glProgramUniform1uivEXT;
PFNGLVERTEXARRAYVERTEXATTRIBLFORMATEXTPROC glad_glVertexArrayVertexAttribLFormatEXT;
PFNGLBINDPROGRAMPIPELINEPROC glad_glBindProgramPipeline;
PFNGLPROGRAMUNIFORM4FVPROC glad_glProgramUniform4fv;
PFNGLGETBOOLEANVPROC glad_glGetBooleanv;
PFNGLVERTEXATTRIB1DPROC glad_glVertexAttrib1d;
PFNGLMULTIDRAWARRAYSINDIRECTAMDPROC glad_glMultiDrawArraysIndirectAMD;
PFNGLGENTEXTURESPROC glad_glGenTextures;
PFNGLPROGRAMUNIFORM4UIVEXTPROC glad_glProgramUniform4uivEXT;
PFNGLGETTEXTURELEVELPARAMETERIVPROC glad_glGetTextureLevelParameteriv;
PFNGLGETSTRINGIPROC glad_glGetStringi;
PFNGLPOINTPARAMETERFVPROC glad_glPointParameterfv;
PFNGLUNIFORMMATRIX2FVARBPROC glad_glUniformMatrix2fvARB;
PFNGLUNIFORM2FVPROC glad_glUniform2fv;
PFNGLFOGCOORDFORMATNVPROC glad_glFogCoordFormatNV;
PFNGLGETMULTITEXGENFVEXTPROC glad_glGetMultiTexGenfvEXT;
PFNGLMATRIXFRUSTUMEXTPROC glad_glMatrixFrustumEXT;
PFNGLDISPATCHCOMPUTEINDIRECTPROC glad_glDispatchComputeIndirect;
PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC glad_glInvalidateNamedFramebufferSubData;
PFNGLPROGRAMENVPARAMETER4DARBPROC glad_glProgramEnvParameter4dARB;
PFNGLBINDBUFFERRANGEPROC glad_glBindBufferRange;
PFNGLGETUNIFORMI64VNVPROC glad_glGetUniformi64vNV;
PFNGLGETUNIFORMDVPROC glad_glGetUniformdv;
PFNGLGETMULTITEXLEVELPARAMETERFVEXTPROC glad_glGetMultiTexLevelParameterfvEXT;
PFNGLPROGRAMUNIFORM3FVEXTPROC glad_glProgramUniform3fvEXT;
PFNGLISVERTEXARRAYPROC glad_glIsVertexArray;
PFNGLGETVERTEXARRAYPOINTERVEXTPROC glad_glGetVertexArrayPointervEXT;
PFNGLPOINTPARAMETERFSGISPROC glad_glPointParameterfSGIS;
PFNGLUNIFORM1FARBPROC glad_glUniform1fARB;
PFNGLCLIENTWAITSYNCPROC glad_glClientWaitSync;
PFNGLVERTEXATTRIBS1FVNVPROC glad_glVertexAttribs1fvNV;
PFNGLVERTEXATTRIB4NUSVARBPROC glad_glVertexAttrib4NusvARB;
PFNGLBLENDFUNCIARBPROC glad_glBlendFunciARB;
PFNGLINVALIDATETEXIMAGEPROC glad_glInvalidateTexImage;
PFNGLCOPYIMAGESUBDATAPROC glad_glCopyImageSubData;
PFNGLGETUNIFORMSUBROUTINEUIVPROC glad_glGetUniformSubroutineuiv;
PFNGLBINDVERTEXBUFFERPROC glad_glBindVertexBuffer;
PFNGLMULTITEXCOORD1IARBPROC glad_glMultiTexCoord1iARB;
PFNGLDEBUGMESSAGEINSERTPROC glad_glDebugMessageInsert;
PFNGLISSAMPLERPROC glad_glIsSampler;
PFNGLMULTITEXGENIVEXTPROC glad_glMultiTexGenivEXT;
PFNGLNAMEDFRAMEBUFFERTEXTURE2DEXTPROC glad_glNamedFramebufferTexture2DEXT;
PFNGLCOPYTEXSUBIMAGE1DPROC glad_glCopyTexSubImage1D;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glad_glCheckFramebufferStatus;
PFNGLBINDIMAGETEXTUREPROC glad_glBindImageTexture;
PFNGLISPATHNVPROC glad_glIsPathNV;
PFNGLDRAWRANGEELEMENTSPROC glad_glDrawRangeElements;
PFNGLMULTITEXCOORDPOINTEREXTPROC glad_glMultiTexCoordPointerEXT;
PFNGLBINDBUFFERBASEPROC glad_glBindBufferBase;
PFNGLMULTIDRAWARRAYSPROC glad_glMultiDrawArrays;
PFNGLNAMEDRENDERBUFFERSTORAGEEXTPROC glad_glNamedRenderbufferStorageEXT;
PFNGLVERTEXATTRIBL1DPROC glad_glVertexAttribL1d;
PFNGLMULTITEXSUBIMAGE1DEXTPROC glad_glMultiTexSubImage1DEXT;
PFNGLVERTEXATTRIBI3UIVPROC glad_glVertexAttribI3uiv;
PFNGLBUFFERDATAARBPROC glad_glBufferDataARB;
PFNGLVERTEXATTRIBIFORMATPROC glad_glVertexAttribIFormat;
PFNGLCREATEFRAMEBUFFERSPROC glad_glCreateFramebuffers;
PFNGLGETACTIVEUNIFORMNAMEPROC glad_glGetActiveUniformName;
PFNGLUNIFORM2UIVEXTPROC glad_glUniform2uivEXT;
PFNGLVERTEXATTRIBI4UIPROC glad_glVertexAttribI4ui;
PFNGLBEGINQUERYPROC glad_glBeginQuery;
PFNGLBINDBUFFERPROC glad_glBindBuffer;
PFNGLUNIFORMMATRIX2X4FVPROC glad_glUniformMatrix2x4fv;
PFNGLGETMULTITEXPARAMETERFVEXTPROC glad_glGetMultiTexParameterfvEXT;
PFNGLDEPTHRANGEINDEXEDPROC glad_glDepthRangeIndexed;
PFNGLGETVERTEXATTRIBIUIVEXTPROC glad_glGetVertexAttribIuivEXT;
PFNGLMATRIXSCALEFEXTPROC glad_glMatrixScalefEXT;
PFNGLGETERRORPROC glad_glGetError;
PFNGLGETUNIFORMIVARBPROC glad_glGetUniformivARB;
PFNGLGETACTIVEUNIFORMARBPROC glad_glGetActiveUniformARB;
PFNGLVERTEXATTRIBI3IVEXTPROC glad_glVertexAttribI3ivEXT;
PFNGLGETTEXLEVELPARAMETERFVPROC glad_glGetTexLevelParameterfv;
PFNGLVERTEXATTRIBI1IVEXTPROC glad_glVertexAttribI1ivEXT;
PFNGLVERTEXATTRIB3SVNVPROC glad_glVertexAttrib3svNV;
PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC glad_glFlushMappedNamedBufferRange;
PFNGLGETNUNIFORMUI64VARBPROC glad_glGetnUniformui64vARB;
PFNGLGETACTIVEUNIFORMSIVPROC glad_glGetActiveUniformsiv;
PFNGLVERTEXATTRIBI1UIEXTPROC glad_glVertexAttribI1uiEXT;
PFNGLPROGRAMUNIFORM3I64VARBPROC glad_glProgramUniform3i64vARB;
PFNGLPROGRAMUNIFORM4UIPROC glad_glProgramUniform4ui;
PFNGLCOPYTEXSUBIMAGE2DEXTPROC glad_glCopyTexSubImage2DEXT;
PFNGLSTENCILMASKPROC glad_glStencilMask;
PFNGLPROGRAMUNIFORM1DVPROC glad_glProgramUniform1dv;
PFNGLNAMEDBUFFERDATAPROC glad_glNamedBufferData;
PFNGLVERTEXATTRIB3FVARBPROC glad_glVertexAttrib3fvARB;
PFNGLCLEARNAMEDBUFFERSUBDATAPROC glad_glClearNamedBufferSubData;
PFNGLMAPNAMEDBUFFERRANGEPROC glad_glMapNamedBufferRange;
PFNGLUNIFORM3IARBPROC glad_glUniform3iARB;
PFNGLCREATEPROGRAMOBJECTARBPROC glad_glCreateProgramObjectARB;
PFNGLMULTITEXCOORD1DVARBPROC glad_glMultiTexCoord1dvARB;
PFNGLPROGRAMUNIFORM3DVEXTPROC glad_glProgramUniform3dvEXT;
PFNGLMULTITEXIMAGE2DEXTPROC glad_glMultiTexImage2DEXT;
PFNGLPROGRAMUNIFORM1I64NVPROC glad_glProgramUniform1i64NV;
PFNGLMULTITEXCOORD1FVARBPROC glad_glMultiTexCoord1fvARB;
PFNGLBLENDFUNCSEPARATEINGRPROC glad_glBlendFuncSeparateINGR;
PFNGLVERTEXATTRIB2FARBPROC glad_glVertexAttrib2fARB;
PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC glad_glGetCompressedTextureSubImage;
PFNGLPATHGLYPHINDEXARRAYNVPROC glad_glPathGlyphIndexArrayNV;
PFNGLINDEXPOINTEREXTPROC glad_glIndexPointerEXT;
PFNGLTEXTUREIMAGE2DEXTPROC glad_glTextureImage2DEXT;
PFNGLACTIVESHADERPROGRAMPROC glad_glActiveShaderProgram;
PFNGLPATCHPARAMETERFVPROC glad_glPatchParameterfv;
PFNGLISFRAMEBUFFEREXTPROC glad_glIsFramebufferEXT;
PFNGLTEXTURESTORAGE2DPROC glad_glTextureStorage2D;
PFNGLPROGRAMUNIFORM1UI64ARBPROC glad_glProgramUniform1ui64ARB;
PFNGLTEXBUFFERRANGEPROC glad_glTexBufferRange;
PFNGLTEXTURESUBIMAGE3DEXTPROC glad_glTextureSubImage3DEXT;
PFNGLCLIPCONTROLPROC glad_glClipControl;
PFNGLUNIFORM2I64NVPROC glad_glUniform2i64NV;
PFNGLVERTEXATTRIBI3UIPROC glad_glVertexAttribI3ui;
PFNGLVERTEXATTRIB4NIVARBPROC glad_glVertexAttrib4NivARB;
PFNGLBLENDEQUATIONSEPARATEINDEXEDAMDPROC glad_glBlendEquationSeparateIndexedAMD;
PFNGLVERTEXARRAYVERTEXBUFFERSPROC glad_glVertexArrayVertexBuffers;
PFNGLUNIFORM1UI64NVPROC glad_glUniform1ui64NV;
PFNGLPROGRAMPARAMETERIPROC glad_glProgramParameteri;
PFNGLPROGRAMUNIFORM2I64VNVPROC glad_glProgramUniform2i64vNV;
PFNGLISSTATENVPROC glad_glIsStateNV;
PFNGLVERTEXATTRIBL2DEXTPROC glad_glVertexAttribL2dEXT;
PFNGLPROGRAMUNIFORMMATRIX3X2DVEXTPROC glad_glProgramUniformMatrix3x2dvEXT;
PFNGLENABLEVERTEXARRAYATTRIBEXTPROC glad_glEnableVertexArrayAttribEXT;
PFNGLPROGRAMUNIFORM2UIVEXTPROC glad_glProgramUniform2uivEXT;
PFNGLGETDOUBLEINDEXEDVEXTPROC glad_glGetDoubleIndexedvEXT;
PFNGLGETVARYINGLOCATIONNVPROC glad_glGetVaryingLocationNV;
PFNGLBINDFRAMEBUFFEREXTPROC glad_glBindFramebufferEXT;
PFNGLCREATESHADERPROC glad_glCreateShader;
PFNGLGENPATHSNVPROC glad_glGenPathsNV;
PFNGLGENRENDERBUFFERSPROC glad_glGenRenderbuffers;
PFNGLCOPYTEXSUBIMAGE2DPROC glad_glCopyTexSubImage2D;
PFNGLVERTEXATTRIB4UBVARBPROC glad_glVertexAttrib4ubvARB;
PFNGLVERTEXATTRIBS1SVNVPROC glad_glVertexAttribs1svNV;
PFNGLBINDBUFFERBASEEXTPROC glad_glBindBufferBaseEXT;
PFNGLCOMPRESSEDTEXIMAGE1DPROC glad_glCompressedTexImage1D;
PFNGLBLENDFUNCSEPARATEPROC glad_glBlendFuncSeparate;
PFNGLPOINTPARAMETERFARBPROC glad_glPointParameterfARB;
PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC glad_glCompressedTexSubImage1DARB;
PFNGLPOINTSIZEPROC glad_glPointSize;
PFNGLBINDTEXTUREUNITPROC glad_glBindTextureUnit;
PFNGLPROGRAMUNIFORM4I64VARBPROC glad_glProgramUniform4i64vARB;
PFNGLVERTEXATTRIB4NUIVPROC glad_glVertexAttrib4Nuiv;
PFNGLGETUNIFORMFVARBPROC glad_glGetUniformfvARB;
PFNGLPROGRAMLOCALPARAMETER4FARBPROC glad_glProgramLocalParameter4fARB;
PFNGLUNIFORM3IPROC glad_glUniform3i;
PFNGLBLENDEQUATIONSEPARATEIPROC glad_glBlendEquationSeparatei;
PFNGLTEXTUREPARAMETERIVEXTPROC glad_glTextureParameterivEXT;
PFNGLUNIFORM3DPROC glad_glUniform3d;
PFNGLUNIFORM3FPROC glad_glUniform3f;
PFNGLVERTEXATTRIBS4DVNVPROC glad_glVertexAttribs4dvNV;
PFNGLUNIFORM3UI64VARBPROC glad_glUniform3ui64vARB;
PFNGLPROGRAMUNIFORM1UI64VARBPROC glad_glProgramUniform1ui64vARB;
PFNGLTEXTUREPARAMETERFEXTPROC glad_glTextureParameterfEXT;
PFNGLVERTEXATTRIB4SARBPROC glad_glVertexAttrib4sARB;
PFNGLGETFRAGDATAINDEXPROC glad_glGetFragDataIndex;
PFNGLORTHOFOESPROC glad_glOrthofOES;
PFNGLWEIGHTPATHSNVPROC glad_glWeightPathsNV;
PFNGLCOPYTEXIMAGE1DEXTPROC glad_glCopyTexImage1DEXT;
PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXTPROC glad_glCompressedTextureSubImage3DEXT;
PFNGLVERTEXATTRIBL3DVEXTPROC glad_glVertexAttribL3dvEXT;
PFNGLDELETEFRAMEBUFFERSPROC glad_glDeleteFramebuffers;
PFNGLDRAWARRAYSPROC glad_glDrawArrays;
PFNGLGETNTEXIMAGEARBPROC glad_glGetnTexImageARB;
PFNGLCLEARPROC glad_glClear;
PFNGLUNIFORM1I64ARBPROC glad_glUniform1i64ARB;
PFNGLBLENDFUNCSEPARATEIARBPROC glad_glBlendFuncSeparateiARB;
PFNGLCREATEQUERIESPROC glad_glCreateQueries;
PFNGLPROGRAMUNIFORM3IEXTPROC glad_glProgramUniform3iEXT;
PFNGLVERTEXATTRIB4NUBPROC glad_glVertexAttrib4Nub;
PFNGLDEPTHRANGEARRAYVPROC glad_glDepthRangeArrayv;
PFNGLSAMPLERPARAMETERIIVPROC glad_glSamplerParameterIiv;
PFNGLTEXIMAGE1DPROC glad_glTexImage1D;
PFNGLISRENDERBUFFEREXTPROC glad_glIsRenderbufferEXT;
PFNGLDRAWCOMMANDSNVPROC glad_glDrawCommandsNV;
PFNGLUNIFORM3IVARBPROC glad_glUniform3ivARB;
PFNGLMATRIXMULTTRANSPOSEDEXTPROC glad_glMatrixMultTransposedEXT;
PFNGLREQUESTRESIDENTPROGRAMSNVPROC glad_glRequestResidentProgramsNV;
PFNGLMATRIXMULT3X3FNVPROC glad_glMatrixMult3x3fNV;
PFNGLPATHSUBCOMMANDSNVPROC glad_glPathSubCommandsNV;
PFNGLISRENDERBUFFERPROC glad_glIsRenderbuffer;
PFNGLDRAWBUFFERSARBPROC glad_glDrawBuffersARB;
PFNGLFRUSTUMFOESPROC glad_glFrustumfOES;
PFNGLCREATESAMPLERSPROC glad_glCreateSamplers;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glad_glDisableVertexAttribArray;
PFNGLTEXCOORDFORMATNVPROC glad_glTexCoordFormatNV;
PFNGLBLITNAMEDFRAMEBUFFERPROC glad_glBlitNamedFramebuffer;
PFNGLMATRIXLOADTRANSPOSEFEXTPROC glad_glMatrixLoadTransposefEXT;
PFNGLMULTITEXGENFEXTPROC glad_glMultiTexGenfEXT;
PFNGLSHADERSTORAGEBLOCKBINDINGPROC glad_glShaderStorageBlockBinding;
PFNGLSTENCILOPSEPARATEPROC glad_glStencilOpSeparate;
PFNGLVERTEXATTRIBI4UBVPROC glad_glVertexAttribI4ubv;
PFNGLVERTEXARRAYVERTEXATTRIBFORMATEXTPROC glad_glVertexArrayVertexAttribFormatEXT;
PFNGLTEXIMAGE3DEXTPROC glad_glTexImage3DEXT;
PFNGLTEXTURESTORAGE1DPROC glad_glTextureStorage1D;
PFNGLGETPROGRAMINTERFACEIVPROC glad_glGetProgramInterfaceiv;
PFNGLTEXCOORDPOINTEREXTPROC glad_glTexCoordPointerEXT;
PFNGLMULTITEXGENIEXTPROC glad_glMultiTexGeniEXT;
PFNGLVERTEXATTRIB2FNVPROC glad_glVertexAttrib2fNV;
PFNGLNAMEDPROGRAMSTRINGEXTPROC glad_glNamedProgramStringEXT;
PFNGLGETNUNIFORMDVPROC glad_glGetnUniformdv;
PFNGLGETTEXTURESUBIMAGEPROC glad_glGetTextureSubImage;
PFNGLVIEWPORTINDEXEDFVPROC glad_glViewportIndexedfv;
PFNGLMULTITEXCOORD4SVARBPROC glad_glMultiTexCoord4svARB;
PFNGLGETVERTEXATTRIBPOINTERVARBPROC glad_glGetVertexAttribPointervARB;
PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEEXTPROC glad_glFlushMappedNamedBufferRangeEXT;
PFNGLGETVERTEXATTRIBIIVPROC glad_glGetVertexAttribIiv;
PFNGLUNIFORMUI64NVPROC glad_glUniformui64NV;
PFNGLPROGRAMPARAMETERS4FVNVPROC glad_glProgramParameters4fvNV;
PFNGLPATCHPARAMETERIPROC glad_glPatchParameteri;
PFNGLVERTEXARRAYVERTEXBINDINGDIVISOREXTPROC glad_glVertexArrayVertexBindingDivisorEXT;
PFNGLMULTITEXCOORD3SVARBPROC glad_glMultiTexCoord3svARB;
PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC glad_glGetProgramLocalParameterfvARB;
PFNGLVERTEXATTRIB4FNVPROC glad_glVertexAttrib4fNV;
PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC glad_glGetNamedFramebufferAttachmentParameteriv;
PFNGLGENVERTEXARRAYSAPPLEPROC glad_glGenVertexArraysAPPLE;
PFNGLFRAMEBUFFERTEXTUREFACEARBPROC glad_glFramebufferTextureFaceARB;
PFNGLTEXTURESTORAGE2DEXTPROC glad_glTextureStorage2DEXT;
PFNGLVERTEXARRAYFOGCOORDOFFSETEXTPROC glad_glVertexArrayFogCoordOffsetEXT;
PFNGLBLENDEQUATIONSEPARATEIARBPROC glad_glBlendEquationSeparateiARB;
PFNGLCLIENTATTRIBDEFAULTEXTPROC glad_glClientAttribDefaultEXT;
PFNGLPROGRAMUNIFORM2I64ARBPROC glad_glProgramUniform2i64ARB;
PFNGLGETVERTEXATTRIBFVARBPROC glad_glGetVertexAttribfvARB;
PFNGLPROGRAMUNIFORM2IEXTPROC glad_glProgramUniform2iEXT;
PFNGLUNMAPNAMEDBUFFEREXTPROC glad_glUnmapNamedBufferEXT;
PFNGLUNIFORM4FVARBPROC glad_glUniform4fvARB;
PFNGLGETFLOATI_VEXTPROC glad_glGetFloati_vEXT;
PFNGLBINDATTRIBLOCATIONARBPROC glad_glBindAttribLocationARB;
PFNGLBUFFERADDRESSRANGENVPROC glad_glBufferAddressRangeNV;
PFNGLMULTITEXENVIVEXTPROC glad_glMultiTexEnvivEXT;
PFNGLPROGRAMUNIFORM1UI64VNVPROC glad_glProgramUniform1ui64vNV;
PFNGLGETVERTEXATTRIBFVNVPROC glad_glGetVertexAttribfvNV;
PFNGLUNIFORM2IPROC glad_glUniform2i;
PFNGLUNIFORM2FPROC glad_glUniform2f;
PFNGLUNIFORM2DPROC glad_glUniform2d;
PFNGLFRAMEBUFFERTEXTUREFACEEXTPROC glad_glFramebufferTextureFaceEXT;
PFNGLFRAMEBUFFERTEXTURELAYERPROC glad_glFramebufferTextureLayer;
PFNGLGETVERTEXARRAYPOINTERI_VEXTPROC glad_glGetVertexArrayPointeri_vEXT;
PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC glad_glProgramUniformMatrix2x3fvEXT;
PFNGLPROGRAMUNIFORM2FVPROC glad_glProgramUniform2fv;
PFNGLINTERPOLATEPATHSNVPROC glad_glInterpolatePathsNV;
PFNGLCOPYMULTITEXSUBIMAGE3DEXTPROC glad_glCopyMultiTexSubImage3DEXT;
PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC glad_glProgramUniformMatrix2x4dv;
PFNGLPATHMEMORYGLYPHINDEXARRAYNVPROC glad_glPathMemoryGlyphIndexArrayNV;
PFNGLDRAWARRAYSEXTPROC glad_glDrawArraysEXT;
PFNGLBLENDEQUATIONEXTPROC glad_glBlendEquationEXT;
PFNGLCOPYTEXTURESUBIMAGE2DEXTPROC glad_glCopyTextureSubImage2DEXT;
PFNGLVERTEXATTRIB2DVNVPROC glad_glVertexAttrib2dvNV;
PFNGLGETINTEGER64I_VPROC glad_glGetInteger64i_v;
PFNGLBLENDFUNCINDEXEDAMDPROC glad_glBlendFuncIndexedAMD;
PFNGLBLITFRAMEBUFFERPROC glad_glBlitFramebuffer;
PFNGLISENABLEDIPROC glad_glIsEnabledi;
PFNGLMATRIXTRANSLATEDEXTPROC glad_glMatrixTranslatedEXT;
PFNGLMAKETEXTUREHANDLERESIDENTARBPROC glad_glMakeTextureHandleResidentARB;
PFNGLUNIFORM3I64VNVPROC glad_glUniform3i64vNV;
PFNGLPATHCOMMANDSNVPROC glad_glPathCommandsNV;
PFNGLPROGRAMUNIFORMMATRIX3FVPROC glad_glProgramUniformMatrix3fv;
PFNGLBINDVERTEXARRAYAPPLEPROC glad_glBindVertexArrayAPPLE;
PFNGLOBJECTPTRLABELPROC glad_glObjectPtrLabel;
PFNGLGETDEBUGMESSAGELOGPROC glad_glGetDebugMessageLog;
PFNGLTRACKMATRIXNVPROC glad_glTrackMatrixNV;
PFNGLBINDFRAGDATALOCATIONPROC glad_glBindFragDataLocation;
PFNGLVERTEXATTRIBS4UBVNVPROC glad_glVertexAttribs4ubvNV;
PFNGLBINDBUFFERBASENVPROC glad_glBindBufferBaseNV;
PFNGLUNIFORM2I64VNVPROC glad_glUniform2i64vNV;
PFNGLFRAMEBUFFERTEXTURELAYERARBPROC glad_glFramebufferTextureLayerARB;
PFNGLMULTIDRAWELEMENTSINDIRECTPROC glad_glMultiDrawElementsIndirect;
PFNGLPROGRAMUNIFORM1DEXTPROC glad_glProgramUniform1dEXT;
PFNGLCOPYTEXSUBIMAGE3DPROC glad_glCopyTexSubImage3D;
PFNGLCREATESHADEROBJECTARBPROC glad_glCreateShaderObjectARB;
PFNGLGETSTAGEINDEXNVPROC glad_glGetStageIndexNV;
PFNGLGETNAMEDBUFFERPOINTERVEXTPROC glad_glGetNamedBufferPointervEXT;
PFNGLGETCOMMANDHEADERNVPROC glad_glGetCommandHeaderNV;
PFNGLVERTEXATTRIB1SVARBPROC glad_glVertexAttrib1svARB;
PFNGLMATRIXMULTTRANSPOSEFEXTPROC glad_glMatrixMultTransposefEXT;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC glad_glGetActiveUniformBlockiv;
PFNGLPROGRAMUNIFORM2DPROC glad_glProgramUniform2d;
PFNGLPROGRAMUNIFORM2FPROC glad_glProgramUniform2f;
PFNGLISFRAMEBUFFERPROC glad_glIsFramebuffer;
PFNGLCLIENTACTIVETEXTUREARBPROC glad_glClientActiveTextureARB;
PFNGLDRAWELEMENTSINSTANCEDPROC glad_glDrawElementsInstanced;
PFNGLBINDFRAMEBUFFERPROC glad_glBindFramebuffer;
PFNGLVERTEXATTRIBS2DVNVPROC glad_glVertexAttribs2dvNV;
PFNGLGETPROGRAMRESOURCEFVNVPROC glad_glGetProgramResourcefvNV;
PFNGLENDTRANSFORMFEEDBACKNVPROC glad_glEndTransformFeedbackNV;
PFNGLPROGRAMUNIFORM4IVEXTPROC glad_glProgramUniform4ivEXT;
PFNGLPROGRAMUNIFORM3UI64VNVPROC glad_glProgramUniform3ui64vNV;
PFNGLCOPYMULTITEXIMAGE1DEXTPROC glad_glCopyMultiTexImage1DEXT;
PFNGLCOLORMASKINDEXEDEXTPROC glad_glColorMaskIndexedEXT;
PFNGLUNIFORMMATRIX2X4DVPROC glad_glUniformMatrix2x4dv;
PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC glad_glGetProgramResourceLocationIndex;
PFNGLVIEWPORTPROC glad_glViewport;
PFNGLMATRIXLOADFEXTPROC glad_glMatrixLoadfEXT;
PFNGLTEXSTORAGE2DMULTISAMPLEPROC glad_glTexStorage2DMultisample;
PFNGLVERTEXATTRIBL1DVEXTPROC glad_glVertexAttribL1dvEXT;
PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC glad_glGetActiveSubroutineUniformiv;
PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC glad_glCompressedTexSubImage2DARB;
PFNGLTEXBUFFERPROC glad_glTexBuffer;
PFNGLPIXELSTOREIPROC glad_glPixelStorei;
PFNGLVALIDATEPROGRAMPROC glad_glValidateProgram;
PFNGLPIXELSTOREFPROC glad_glPixelStoref;
PFNGLPATHCOVERDEPTHFUNCNVPROC glad_glPathCoverDepthFuncNV;
PFNGLPROGRAMUNIFORM1UIEXTPROC glad_glProgramUniform1uiEXT;
PFNGLVERTEXATTRIBS2FVNVPROC glad_glVertexAttribs2fvNV;
PFNGLGETPROGRAMPARAMETERDVNVPROC glad_glGetProgramParameterdvNV;
PFNGLACTIVEVARYINGNVPROC glad_glActiveVaryingNV;
PFNGLGETMULTITEXGENDVEXTPROC glad_glGetMultiTexGendvEXT;
PFNGLCLEARNAMEDBUFFERSUBDATAEXTPROC glad_glClearNamedBufferSubDataEXT;
PFNGLGETTEXTURESAMPLERHANDLEARBPROC glad_glGetTextureSamplerHandleARB;
PFNGLBINDTEXTUREPROC glad_glBindTexture;
PFNGLGETACTIVEATTRIBARBPROC glad_glGetActiveAttribARB;
PFNGLDETACHSHADERPROC glad_glDetachShader;
PFNGLUNIFORMMATRIX3X4DVPROC glad_glUniformMatrix3x4dv;
PFNGLVERTEXATTRIB4FARBPROC glad_glVertexAttrib4fARB;
PFNGLGETPROGRAMRESOURCELOCATIONPROC glad_glGetProgramResourceLocation;
PFNGLVERTEXARRAYATTRIBLFORMATPROC glad_glVertexArrayAttribLFormat;
PFNGLNAMEDRENDERBUFFERSTORAGEPROC glad_glNamedRenderbufferStorage;
PFNGLVERTEXARRAYVERTEXATTRIBOFFSETEXTPROC glad_glVertexArrayVertexAttribOffsetEXT;
PFNGLINDEXFORMATNVPROC glad_glIndexFormatNV;
PFNGLSTENCILOPSEPARATEATIPROC glad_glStencilOpSeparateATI;
PFNGLDRAWELEMENTSBASEVERTEXPROC glad_glDrawElementsBaseVertex;
PFNGLVERTEXATTRIB1FNVPROC glad_glVertexAttrib1fNV;
PFNGLSAMPLECOVERAGEPROC glad_glSampleCoverage;
PFNGLTEXSTORAGE1DPROC glad_glTexStorage1D;
PFNGLVERTEXATTRIBI3IEXTPROC glad_glVertexAttribI3iEXT;
PFNGLMAKETEXTUREHANDLENONRESIDENTARBPROC glad_glMakeTextureHandleNonResidentARB;
PFNGLPROGRAMUNIFORMMATRIX2DVEXTPROC glad_glProgramUniformMatrix2dvEXT;
PFNGLGETUNIFORMIVPROC glad_glGetUniformiv;
PFNGLRENDERBUFFERSTORAGEPROC glad_glRenderbufferStorage;
PFNGLUNIFORM4UIPROC glad_glUniform4ui;
PFNGLDISABLEVERTEXARRAYATTRIBEXTPROC glad_glDisableVertexArrayAttribEXT;
PFNGLVALIDATEPROGRAMARBPROC glad_glValidateProgramARB;
PFNGLVERTEXATTRIBPOINTERARBPROC glad_glVertexAttribPointerARB;
PFNGLDEBUGMESSAGEINSERTARBPROC glad_glDebugMessageInsertARB;
PFNGLDELETEFRAMEBUFFERSEXTPROC glad_glDeleteFramebuffersEXT;
PFNGLGETTRANSFORMFEEDBACKI_VPROC glad_glGetTransformFeedbacki_v;
PFNGLVERTEXATTRIB1FVNVPROC glad_glVertexAttrib1fvNV;
PFNGLISQUERYARBPROC glad_glIsQueryARB;
PFNGLSCISSORINDEXEDPROC glad_glScissorIndexed;
PFNGLGENFRAMEBUFFERSEXTPROC glad_glGenFramebuffersEXT;
PFNGLVERTEXARRAYCOLOROFFSETEXTPROC glad_glVertexArrayColorOffsetEXT;
PFNGLMULTITEXCOORD3IVARBPROC glad_glMultiTexCoord3ivARB;
PFNGLISTEXTUREHANDLERESIDENTARBPROC glad_glIsTextureHandleResidentARB;
PFNGLMAPNAMEDBUFFERRANGEEXTPROC glad_glMapNamedBufferRangeEXT;
PFNGLCREATESHADERPROGRAMVPROC glad_glCreateShaderProgramv;
PFNGLGETQUERYOBJECTIVPROC glad_glGetQueryObjectiv;
PFNGLVERTEXATTRIB3DVNVPROC glad_glVertexAttrib3dvNV;
PFNGLMULTITEXCOORD3SARBPROC glad_glMultiTexCoord3sARB;
PFNGLGENERATEMIPMAPPROC glad_glGenerateMipmap;
PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC glad_glCompressedTextureSubImage2D;
PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glad_glNamedRenderbufferStorageMultisampleEXT;
PFNGLBLENDEQUATIONINDEXEDAMDPROC glad_glBlendEquationIndexedAMD;
PFNGLPOINTPARAMETERIPROC glad_glPointParameteri;
PFNGLUNMAPBUFFERPROC glad_glUnmapBuffer;
PFNGLVERTEXATTRIB3SVPROC glad_glVertexAttrib3sv;
PFNGLPOINTPARAMETERFPROC glad_glPointParameterf;
PFNGLVERTEXATTRIB1SARBPROC glad_glVertexAttrib1sARB;
PFNGLVERTEXATTRIB4SVPROC glad_glVertexAttrib4sv;
PFNGLPROGRAMPARAMETER4DVNVPROC glad_glProgramParameter4dvNV;
PFNGLTEXTUREPARAMETERIUIVEXTPROC glad_glTextureParameterIuivEXT;
PFNGLDELETERENDERBUFFERSEXTPROC glad_glDeleteRenderbuffersEXT;
PFNGLVERTEXATTRIB1SVNVPROC glad_glVertexAttrib1svNV;
PFNGLPROGRAMUNIFORM2UI64VNVPROC glad_glProgramUniform2ui64vNV;
PFNGLRELEASESHADERCOMPILERPROC glad_glReleaseShaderCompiler;
PFNGLREADPIXELSPROC glad_glReadPixels;
PFNGLSTATECAPTURENVPROC glad_glStateCaptureNV;
PFNGLGETDOUBLEI_VEXTPROC glad_glGetDoublei_vEXT;
PFNGLMULTITEXCOORD4DARBPROC glad_glMultiTexCoord4dARB;
PFNGLREADNPIXELSARBPROC glad_glReadnPixelsARB;
PFNGLMULTITEXBUFFEREXTPROC glad_glMultiTexBufferEXT;
PFNGLUSEPROGRAMSTAGESPROC glad_glUseProgramStages;
PFNGLFRAMEBUFFERTEXTUREARBPROC glad_glFramebufferTextureARB;
PFNGLUNIFORM3I64ARBPROC glad_glUniform3i64ARB;
PFNGLPROGRAMUNIFORM2DEXTPROC glad_glProgramUniform2dEXT;
PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC glad_glNamedFramebufferDrawBuffer;
PFNGLPROGRAMUNIFORMHANDLEUI64ARBPROC glad_glProgramUniformHandleui64ARB;
PFNGLNAMEDCOPYBUFFERSUBDATAEXTPROC glad_glNamedCopyBufferSubDataEXT;
PFNGLGETVERTEXATTRIBLDVPROC glad_glGetVertexAttribLdv;
PFNGLGETNUNIFORMUIVPROC glad_glGetnUniformuiv;
PFNGLTEXSTORAGE3DMULTISAMPLEPROC glad_glTexStorage3DMultisample;
PFNGLVERTEXATTRIB3DNVPROC glad_glVertexAttrib3dNV;
PFNGLTEXTURESTORAGE1DEXTPROC glad_glTextureStorage1DEXT;
PFNGLGENBUFFERSPROC glad_glGenBuffers;
PFNGLEXECUTEPROGRAMNVPROC glad_glExecuteProgramNV;
PFNGLDELETEPROGRAMSNVPROC glad_glDeleteProgramsNV;
PFNGLNORMALFORMATNVPROC glad_glNormalFormatNV;
PFNGLVERTEXATTRIB3FNVPROC glad_glVertexAttrib3fNV;
PFNGLCLEARNAMEDFRAMEBUFFERIVPROC glad_glClearNamedFramebufferiv;
PFNGLTEXIMAGE3DPROC glad_glTexImage3D;
PFNGLGETOBJECTPARAMETERFVARBPROC glad_glGetObjectParameterfvARB;
PFNGLCLEARNAMEDFRAMEBUFFERFVPROC glad_glClearNamedFramebufferfv;
PFNGLTEXTURESTORAGE2DMULTISAMPLEEXTPROC glad_glTextureStorage2DMultisampleEXT;
PFNGLMATRIXPOPEXTPROC glad_glMatrixPopEXT;
PFNGLMULTITEXENVIEXTPROC glad_glMultiTexEnviEXT;
PFNGLVERTEXATTRIBI2IEXTPROC glad_glVertexAttribI2iEXT;
PFNGLCLAMPCOLORPROC glad_glClampColor;
PFNGLVERTEXATTRIBI2IVEXTPROC glad_glVertexAttribI2ivEXT;
PFNGLDELETESTATESNVPROC glad_glDeleteStatesNV;
PFNGLGETBUFFERPARAMETERI64VPROC glad_glGetBufferParameteri64v;
PFNGLPROGRAMUNIFORM4DVPROC glad_glProgramUniform4dv;
PFNGLEDGEFLAGPOINTEREXTPROC glad_glEdgeFlagPointerEXT;
PFNGLMAKEBUFFERNONRESIDENTNVPROC glad_glMakeBufferNonResidentNV;
PFNGLUNIFORM1UI64ARBPROC glad_glUniform1ui64ARB;
PFNGLSTENCILSTROKEPATHNVPROC glad_glStencilStrokePathNV;
PFNGLPROGRAMUNIFORM3UIVPROC glad_glProgramUniform3uiv;
PFNGLMULTITEXCOORD1SARBPROC glad_glMultiTexCoord1sARB;
PFNGLUNIFORM4I64ARBPROC glad_glUniform4i64ARB;
PFNGLPROGRAMUNIFORM2UIEXTPROC glad_glProgramUniform2uiEXT;
PFNGLCOMPRESSEDMULTITEXIMAGE3DEXTPROC glad_glCompressedMultiTexImage3DEXT;
PFNGLNAMEDPROGRAMLOCALPARAMETER4DEXTPROC glad_glNamedProgramLocalParameter4dEXT;
PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC glad_glProgramUniformMatrix4x3fvEXT;
PFNGLPROGRAMUNIFORM3DEXTPROC glad_glProgramUniform3dEXT;
PFNGLDISPATCHCOMPUTEGROUPSIZEARBPROC glad_glDispatchComputeGroupSizeARB;
PFNGLVERTEXATTRIBL1UI64ARBPROC glad_glVertexAttribL1ui64ARB;
PFNGLCREATERENDERBUFFERSPROC glad_glCreateRenderbuffers;
PFNGLVERTEXATTRIB4NUSVPROC glad_glVertexAttrib4Nusv;
PFNGLDEPTHFUNCPROC glad_glDepthFunc;
PFNGLENABLECLIENTSTATEINDEXEDEXTPROC glad_glEnableClientStateIndexedEXT;
PFNGLBLENDFUNCIPROC glad_glBlendFunci;
PFNGLBUFFERSTORAGEPROC glad_glBufferStorage;
PFNGLMAKENAMEDBUFFERNONRESIDENTNVPROC glad_glMakeNamedBufferNonResidentNV;
PFNGLPROGRAMUNIFORM4FEXTPROC glad_glProgramUniform4fEXT;
PFNGLGETUNIFORMLOCATIONPROC glad_glGetUniformLocation;
PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC glad_glNamedFramebufferDrawBuffers;
PFNGLUNIFORM2UI64VNVPROC glad_glUniform2ui64vNV;
PFNGLUNIFORM4FVPROC glad_glUniform4fv;
PFNGLNAMEDFRAMEBUFFERTEXTUREFACEEXTPROC glad_glNamedFramebufferTextureFaceEXT;
PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC glad_glCompressedTextureSubImage3D;
PFNGLGETPROGRAMSTAGEIVPROC glad_glGetProgramStageiv;
PFNGLPRIORITIZETEXTURESEXTPROC glad_glPrioritizeTexturesEXT;
PFNGLENDCONDITIONALRENDERPROC glad_glEndConditionalRender;
PFNGLTEXSUBIMAGE2DEXTPROC glad_glTexSubImage2DEXT;
PFNGLPROGRAMUNIFORM2UIVPROC glad_glProgramUniform2uiv;
PFNGLGETQUERYOBJECTUIVPROC glad_glGetQueryObjectuiv;
PFNGLGENERATEMULTITEXMIPMAPEXTPROC glad_glGenerateMultiTexMipmapEXT;
PFNGLVERTEXATTRIB4IVPROC glad_glVertexAttrib4iv;
PFNGLPROGRAMUNIFORM1UIVPROC glad_glProgramUniform1uiv;
PFNGLFRAMEBUFFERTEXTUREPROC glad_glFramebufferTexture;
PFNGLMULTITEXENVFEXTPROC glad_glMultiTexEnvfEXT;
PFNGLVERTEXATTRIBL1DVPROC glad_glVertexAttribL1dv;
PFNGLNAMEDPROGRAMLOCALPARAMETER4FVEXTPROC glad_glNamedProgramLocalParameter4fvEXT;
PFNGLPOINTPARAMETERFVSGISPROC glad_glPointParameterfvSGIS;
PFNGLDEPTHMASKPROC glad_glDepthMask;
PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC glad_glProgramUniformMatrix2x4fv;
PFNGLAREPROGRAMSRESIDENTNVPROC glad_glAreProgramsResidentNV;
PFNGLFLUSHMAPPEDBUFFERRANGEAPPLEPROC glad_glFlushMappedBufferRangeAPPLE;
PFNGLVERTEXATTRIBS1DVNVPROC glad_glVertexAttribs1dvNV;
PFNGLPROGRAMUNIFORM4DEXTPROC glad_glProgramUniform4dEXT;
PFNGLUNIFORMMATRIX2FVPROC glad_glUniformMatrix2fv;
PFNGLSAMPLEMASKINDEXEDNVPROC glad_glSampleMaskIndexedNV;
PFNGLPROGRAMUNIFORM3I64ARBPROC glad_glProgramUniform3i64ARB;
PFNGLGETTEXTUREPARAMETERFVEXTPROC glad_glGetTextureParameterfvEXT;
PFNGLGETFLOATVPROC glad_glGetFloatv;
PFNGLMATRIXLOAD3X3FNVPROC glad_glMatrixLoad3x3fNV;
PFNGLMULTITEXCOORD2FARBPROC glad_glMultiTexCoord2fARB;
PFNGLISTRANSFORMFEEDBACKNVPROC glad_glIsTransformFeedbackNV;
PFNGLVERTEXATTRIBDIVISORARBPROC glad_glVertexAttribDivisorARB;
PFNGLVERTEXATTRIBI3IVPROC glad_glVertexAttribI3iv;
PFNGLGETNCOMPRESSEDTEXIMAGEARBPROC glad_glGetnCompressedTexImageARB;
PFNGLGETINTEGERVPROC glad_glGetIntegerv;
PFNGLMATRIXLOADDEXTPROC glad_glMatrixLoaddEXT;
PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC glad_glFramebufferTextureLayerEXT;
PFNGLPROGRAMUNIFORMMATRIX3DVPROC glad_glProgramUniformMatrix3dv;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glad_glRenderbufferStorageMultisample;
PFNGLISQUERYPROC glad_glIsQuery;
PFNGLTEXIMAGE2DPROC glad_glTexImage2D;
PFNGLENABLEINDEXEDEXTPROC glad_glEnableIndexedEXT;
PFNGLUNIFORM2UIEXTPROC glad_glUniform2uiEXT;
PFNGLMULTITEXCOORD1FARBPROC glad_glMultiTexCoord1fARB;
PFNGLVERTEXATTRIB4NUBVPROC glad_glVertexAttrib4Nubv;
PFNGLVERTEXARRAYVERTEXATTRIBIOFFSETEXTPROC glad_glVertexArrayVertexAttribIOffsetEXT;
PFNGLUNIFORM4UI64ARBPROC glad_glUniform4ui64ARB;
PFNGLTEXTURERENDERBUFFEREXTPROC glad_glTextureRenderbufferEXT;
PFNGLGETFRAMEBUFFERPARAMETERIVPROC glad_glGetFramebufferParameteriv;
PFNGLGETPOINTERVEXTPROC glad_glGetPointervEXT;
PFNGLPROGRAMUNIFORMMATRIX3DVEXTPROC glad_glProgramUniformMatrix3dvEXT;
PFNGLCOPYBUFFERSUBDATAPROC glad_glCopyBufferSubData;
PFNGLVERTEXATTRIBI1UIVPROC glad_glVertexAttribI1uiv;
PFNGLVERTEXATTRIBL4DVEXTPROC glad_glVertexAttribL4dvEXT;
PFNGLGETACTIVEUNIFORMPROC glad_glGetActiveUniform;
PFNGLUNIFORM2UI64VARBPROC glad_glUniform2ui64vARB;
PFNGLMAKENAMEDBUFFERRESIDENTNVPROC glad_glMakeNamedBufferResidentNV;
PFNGLPATHGLYPHINDEXRANGENVPROC glad_glPathGlyphIndexRangeNV;
PFNGLMULTITEXPARAMETERFEXTPROC glad_glMultiTexParameterfEXT;
PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC glad_glNamedFramebufferTextureLayer;
PFNGLGETVERTEXARRAYINTEGERVEXTPROC glad_glGetVertexArrayIntegervEXT;
PFNGLVERTEXATTRIB3DARBPROC glad_glVertexAttrib3dARB;
PFNGLUNIFORM3UIEXTPROC glad_glUniform3uiEXT;
PFNGLTEXPARAMETERIUIVEXTPROC glad_glTexParameterIuivEXT;
PFNGLPROGRAMUNIFORMMATRIX4FVPROC glad_glProgramUniformMatrix4fv;
PFNGLVERTEXATTRIBL3DEXTPROC glad_glVertexAttribL3dEXT;
PFNGLVERTEXATTRIBL3DVPROC glad_glVertexAttribL3dv;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glad_glFramebufferRenderbuffer;
PFNGLGETUNIFORMLOCATIONARBPROC glad_glGetUniformLocationARB;
PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC glad_glGetProgramLocalParameterdvARB;
PFNGLPATHPARAMETERFVNVPROC glad_glPathParameterfvNV;
PFNGLGETDOUBLEI_VPROC glad_glGetDoublei_v;
PFNGLVERTEXATTRIB1SVPROC glad_glVertexAttrib1sv;
PFNGLBINDSAMPLERPROC glad_glBindSampler;
PFNGLBINDBUFFEROFFSETEXTPROC glad_glBindBufferOffsetEXT;
PFNGLGETINTEGERI_VPROC glad_glGetIntegeri_v;
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glad_glGetTransformFeedbackVarying;
PFNGLGETTRANSFORMFEEDBACKVARYINGNVPROC glad_glGetTransformFeedbackVaryingNV;
PFNGLVERTEXATTRIB2DARBPROC glad_glVertexAttrib2dARB;
PFNGLDRAWRANGEELEMENTSEXTPROC glad_glDrawRangeElementsEXT;
PFNGLDEPTHRANGEFPROC glad_glDepthRangef;
PFNGLDELETEPROGRAMSARBPROC glad_glDeleteProgramsARB;
PFNGLCOMPRESSEDTEXTURESUBIMAGE1DEXTPROC glad_glCompressedTextureSubImage1DEXT;
PFNGLENABLEIPROC glad_glEnablei;
PFNGLGETACTIVESUBROUTINENAMEPROC glad_glGetActiveSubroutineName;
PFNGLBINDPROGRAMARBPROC glad_glBindProgramARB;
PFNGLPROGRAMUNIFORM3UI64VARBPROC glad_glProgramUniform3ui64vARB;
PFNGLPROGRAMUNIFORMMATRIX2DVPROC glad_glProgramUniformMatrix2dv;
PFNGLGETPATHMETRICRANGENVPROC glad_glGetPathMetricRangeNV;
PFNGLSAMPLECOVERAGEARBPROC glad_glSampleCoverageARB;
PFNGLPROGRAMUNIFORM2UI64VARBPROC glad_glProgramUniform2ui64vARB;
PFNGLEDGEFLAGFORMATNVPROC glad_glEdgeFlagFormatNV;
PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC glad_glInvalidateNamedFramebufferData;
PFNGLSAMPLEMASKIPROC glad_glSampleMaski;
PFNGLPOINTPARAMETERINVPROC glad_glPointParameteriNV;
PFNGLUNIFORMMATRIX3X2FVPROC glad_glUniformMatrix3x2fv;
PFNGLGETFRAMEBUFFERPARAMETERIVEXTPROC glad_glGetFramebufferParameterivEXT;
PFNGLUNIFORM2I64ARBPROC glad_glUniform2i64ARB;
PFNGLNAMEDBUFFERSTORAGEEXTPROC glad_glNamedBufferStorageEXT;
PFNGLVERTEXATTRIB2DVPROC glad_glVertexAttrib2dv;
PFNGLVERTEXATTRIB4DVARBPROC glad_glVertexAttrib4dvARB;
PFNGLTEXSTORAGE2DPROC glad_glTexStorage2D;
PFNGLUNIFORMMATRIX3X4FVPROC glad_glUniformMatrix3x4fv;
PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC glad_glProgramUniformMatrix3fvEXT;
PFNGLENABLEVERTEXARRAYEXTPROC glad_glEnableVertexArrayEXT;
PFNGLDELETEBUFFERSARBPROC glad_glDeleteBuffersARB;
PFNGLMULTITEXRENDERBUFFEREXTPROC glad_glMultiTexRenderbufferEXT;
PFNGLUNMAPBUFFERARBPROC glad_glUnmapBufferARB;
PFNGLMULTITEXCOORD4IVARBPROC glad_glMultiTexCoord4ivARB;
PFNGLNAMEDPROGRAMLOCALPARAMETER4DVEXTPROC glad_glNamedProgramLocalParameter4dvEXT;
PFNGLPROGRAMUNIFORM2FVEXTPROC glad_glProgramUniform2fvEXT;
PFNGLCOLORMASKIPROC glad_glColorMaski;
PFNGLCOPYNAMEDBUFFERSUBDATAPROC glad_glCopyNamedBufferSubData;
PFNGLGENPROGRAMSNVPROC glad_glGenProgramsNV;
PFNGLTEXSTORAGE3DPROC glad_glTexStorage3D;
PFNGLTEXTUREPARAMETERIVPROC glad_glTextureParameteriv;
PFNGLNAMEDBUFFERDATAEXTPROC glad_glNamedBufferDataEXT;
PFNGLVERTEXATTRIBI3UIEXTPROC glad_glVertexAttribI3uiEXT;
PFNGLNAMEDFRAMEBUFFERTEXTUREPROC glad_glNamedFramebufferTexture;
PFNGLVERTEXATTRIB4NSVARBPROC glad_glVertexAttrib4NsvARB;
PFNGLISIMAGEHANDLERESIDENTARBPROC glad_glIsImageHandleResidentARB;
PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC glad_glProgramUniformMatrix4x3fv;
PFNGLGETCOMPRESSEDTEXTUREIMAGEEXTPROC glad_glGetCompressedTextureImageEXT;
PFNGLDRAWCOMMANDSSTATESNVPROC glad_glDrawCommandsStatesNV;
PFNGLUNIFORM4UI64VNVPROC glad_glUniform4ui64vNV;
PFNGLPROGRAMUNIFORM2UIPROC glad_glProgramUniform2ui;
PFNGLGENRENDERBUFFERSEXTPROC glad_glGenRenderbuffersEXT;
PFNGLNAMEDPROGRAMLOCALPARAMETERI4IVEXTPROC glad_glNamedProgramLocalParameterI4ivEXT;
PFNGLGETMULTITEXPARAMETERIUIVEXTPROC glad_glGetMultiTexParameterIuivEXT;
PFNGLBLENDEQUATIONIARBPROC glad_glBlendEquationiARB;
PFNGLTEXPARAMETERIVPROC glad_glTexParameteriv;
PFNGLUNIFORM4IVARBPROC glad_glUniform4ivARB;
PFNGLMATRIXORTHOEXTPROC glad_glMatrixOrthoEXT;
PFNGLVERTEXARRAYVERTEXBUFFERPROC glad_glVertexArrayVertexBuffer;
PFNGLPROGRAMUNIFORM4UI64VNVPROC glad_glProgramUniform4ui64vNV;
PFNGLUNIFORM4UI64VARBPROC glad_glUniform4ui64vARB;
PFNGLPOINTPARAMETERIVNVPROC glad_glPointParameterivNV;
PFNGLNAMEDFRAMEBUFFERTEXTURELAYEREXTPROC glad_glNamedFramebufferTextureLayerEXT;
PFNGLMULTITEXGENFVEXTPROC glad_glMultiTexGenfvEXT;
PFNGLGETNAMEDBUFFERSUBDATAPROC glad_glGetNamedBufferSubData;
PFNGLSTENCILFUNCSEPARATEATIPROC glad_glStencilFuncSeparateATI;
PFNGLPROGRAMUNIFORM2IVPROC glad_glProgramUniform2iv;
PFNGLGETQUERYIVPROC glad_glGetQueryiv;
PFNGLGETTRANSFORMFEEDBACKIVPROC glad_glGetTransformFeedbackiv;
PFNGLGETSAMPLERPARAMETERFVPROC glad_glGetSamplerParameterfv;
PFNGLPROGRAMUNIFORM3I64NVPROC glad_glProgramUniform3i64NV;
PFNGLUNIFORM4IARBPROC glad_glUniform4iARB;
PFNGLGETUNIFORMINDICESPROC glad_glGetUniformIndices;
PFNGLMULTITEXIMAGE3DEXTPROC glad_glMultiTexImage3DEXT;
PFNGLPOINTPARAMETERIVPROC glad_glPointParameteriv;
PFNGLPROGRAMUNIFORMMATRIX3X4DVEXTPROC glad_glProgramUniformMatrix3x4dvEXT;
PFNGLPROGRAMUNIFORMMATRIX4X3DVEXTPROC glad_glProgramUniformMatrix4x3dvEXT;
PFNGLGETACTIVEVARYINGNVPROC glad_glGetActiveVaryingNV;
PFNGLUNIFORM1I64VARBPROC glad_glUniform1i64vARB;
PFNGLGETBUFFERSUBDATAPROC glad_glGetBufferSubData;
PFNGLMATRIXTRANSLATEFEXTPROC glad_glMatrixTranslatefEXT;
PFNGLVERTEXATTRIBP3UIPROC glad_glVertexAttribP3ui;
PFNGLTEXTUREBUFFERRANGEPROC glad_glTextureBufferRange;
PFNGLGETINTEGERUI64VNVPROC glad_glGetIntegerui64vNV;
PFNGLPROGRAMUNIFORM4UI64VARBPROC glad_glProgramUniform4ui64vARB;
PFNGLDEPTHRANGEPROC glad_glDepthRange;
PFNGLGETVERTEXATTRIBDVARBPROC glad_glGetVertexAttribdvARB;
PFNGLPROGRAMUNIFORM4I64VNVPROC glad_glProgramUniform4i64vNV;
PFNGLDRAWARRAYSINSTANCEDEXTPROC glad_glDrawArraysInstancedEXT;
PFNGLDISABLECLIENTSTATEINDEXEDEXTPROC glad_glDisableClientStateIndexedEXT;
PFNGLDRAWBUFFERPROC glad_glDrawBuffer;
PFNGLMULTIDRAWARRAYSINDIRECTBINDLESSNVPROC glad_glMultiDrawArraysIndirectBindlessNV;
PFNGLCLEARBUFFERUIVPROC glad_glClearBufferuiv;
PFNGLPATHSTENCILFUNCNVPROC glad_glPathStencilFuncNV;
PFNGLGETINTERNALFORMATI64VPROC glad_glGetInternalformati64v;
PFNGLSHADERSOURCEARBPROC glad_glShaderSourceARB;
PFNGLPROVOKINGVERTEXEXTPROC glad_glProvokingVertexEXT;
PFNGLFLUSHPROC glad_glFlush;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glad_glFramebufferTexture2DEXT;
PFNGLCOVERSTROKEPATHINSTANCEDNVPROC glad_glCoverStrokePathInstancedNV;
PFNGLATTACHSHADERPROC glad_glAttachShader;
PFNGLPROGRAMUNIFORMHANDLEUI64VARBPROC glad_glProgramUniformHandleui64vARB;
PFNGLPUSHDEBUGGROUPPROC glad_glPushDebugGroup;
PFNGLGETTEXLEVELPARAMETERIVPROC glad_glGetTexLevelParameteriv;
PFNGLSTENCILFILLPATHNVPROC glad_glStencilFillPathNV;
PFNGLPROGRAMVERTEXLIMITNVPROC glad_glProgramVertexLimitNV;
PFNGLUNIFORM3I64NVPROC glad_glUniform3i64NV;
PFNGLPROGRAMUNIFORM3UIVEXTPROC glad_glProgramUniform3uivEXT;
PFNGLCLEARSTENCILPROC glad_glClearStencil;
PFNGLVERTEXATTRIBS3SVNVPROC glad_glVertexAttribs3svNV;
PFNGLGETBUFFERPARAMETERUI64VNVPROC glad_glGetBufferParameterui64vNV;
PFNGLVERTEXATTRIB4BVARBPROC glad_glVertexAttrib4bvARB;
PFNGLGETDOUBLEVPROC glad_glGetDoublev;
PFNGLBLENDFUNCPROC glad_glBlendFunc;
PFNGLCOPYTEXTUREIMAGE2DEXTPROC glad_glCopyTextureImage2DEXT;
PFNGLBEGINQUERYINDEXEDPROC glad_glBeginQueryIndexed;
PFNGLMULTITEXCOORD4FARBPROC glad_glMultiTexCoord4fARB;
PFNGLGETPROGRAMIVPROC glad_glGetProgramiv;
PFNGLGETPROGRAMSTRINGARBPROC glad_glGetProgramStringARB;
PFNGLFLUSHMAPPEDBUFFERRANGEPROC glad_glFlushMappedBufferRange;
PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC glad_glProgramUniformMatrix2fvEXT;
PFNGLGETTEXTUREPARAMETERIUIVEXTPROC glad_glGetTextureParameterIuivEXT;
PFNGLGETMULTITEXENVIVEXTPROC glad_glGetMultiTexEnvivEXT;
PFNGLGENQUERIESPROC glad_glGenQueries;
PFNGLBLENDCOLOREXTPROC glad_glBlendColorEXT;
PFNGLGETNUNIFORMIVARBPROC glad_glGetnUniformivARB;
PFNGLTEXSUBIMAGE3DPROC glad_glTexSubImage3D;
PFNGLPATHSTENCILDEPTHOFFSETNVPROC glad_glPathStencilDepthOffsetNV;
PFNGLDELETESAMPLERSPROC glad_glDeleteSamplers;
PFNGLGETMULTITEXENVFVEXTPROC glad_glGetMultiTexEnvfvEXT;
PFNGLVERTEXARRAYNORMALOFFSETEXTPROC glad_glVertexArrayNormalOffsetEXT;
PFNGLGETBUFFERPARAMETERIVARBPROC glad_glGetBufferParameterivARB;
PFNGLGETTEXTUREPARAMETERFVPROC glad_glGetTextureParameterfv;
PFNGLVERTEXATTRIB4DNVPROC glad_glVertexAttrib4dNV;
PFNGLGETMULTISAMPLEFVNVPROC glad_glGetMultisamplefvNV;
PFNGLPROGRAMUNIFORMMATRIX4DVEXTPROC glad_glProgramUniformMatrix4dvEXT;
PFNGLGETPATHCOMMANDSNVPROC glad_glGetPathCommandsNV;
PFNGLVERTEXARRAYTEXCOORDOFFSETEXTPROC glad_glVertexArrayTexCoordOffsetEXT;
PFNGLCOLORFORMATNVPROC glad_glColorFormatNV;
PFNGLPROGRAMUNIFORMUI64NVPROC glad_glProgramUniformui64NV;
PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC glad_glProgramUniformMatrix4x2fvEXT;
PFNGLBINDTEXTURESPROC glad_glBindTextures;
PFNGLUNIFORM2FARBPROC glad_glUniform2fARB;
PFNGLBINDBUFFERRANGEEXTPROC glad_glBindBufferRangeEXT;
PFNGLCOPYTEXTURESUBIMAGE3DEXTPROC glad_glCopyTextureSubImage3DEXT;
PFNGLMAPBUFFERARBPROC glad_glMapBufferARB;
PFNGLGETPATHSPACINGNVPROC glad_glGetPathSpacingNV;
PFNGLUNIFORM4DVPROC glad_glUniform4dv;
PFNGLPROGRAMUNIFORM3DVPROC glad_glProgramUniform3dv;
PFNGLGETSHADERSOURCEARBPROC glad_glGetShaderSourceARB;
PFNGLINVALIDATEBUFFERDATAPROC glad_glInvalidateBufferData;
PFNGLMATRIXLOAD3X2FNVPROC glad_glMatrixLoad3x2fNV;
PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC glad_glCompressedTextureSubImage1D;
PFNGLUNIFORM3FVPROC glad_glUniform3fv;
PFNGLCLEARTEXIMAGEPROC glad_glClearTexImage;
PFNGLUNIFORMMATRIX4DVPROC glad_glUniformMatrix4dv;
PFNGLPROGRAMUNIFORMMATRIX2X3DVEXTPROC glad_glProgramUniformMatrix2x3dvEXT;
PFNGLGETTEXTURELEVELPARAMETERFVEXTPROC glad_glGetTextureLevelParameterfvEXT;
PFNGLCLEARNAMEDFRAMEBUFFERFIPROC glad_glClearNamedFramebufferfi;
PFNGLMULTIDRAWELEMENTSPROC glad_glMultiDrawElements;
PFNGLVERTEXATTRIBL2DVEXTPROC glad_glVertexAttribL2dvEXT;
PFNGLTEXSUBIMAGE1DEXTPROC glad_glTexSubImage1DEXT;
PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC glad_glNamedFramebufferReadBuffer;
PFNGLUNIFORM2I64VARBPROC glad_glUniform2i64vARB;
PFNGLPROGRAMPARAMETER4FVNVPROC glad_glProgramParameter4fvNV;
PFNGLUNIFORM1FVARBPROC glad_glUniform1fvARB;
PFNGLVERTEXFORMATNVPROC glad_glVertexFormatNV;
PFNGLBINDTRANSFORMFEEDBACKPROC glad_glBindTransformFeedback;
PFNGLUNIFORM1I64NVPROC glad_glUniform1i64NV;
PFNGLUNIFORM2UIVPROC glad_glUniform2uiv;
PFNGLFINISHPROC glad_glFinish;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glad_glCompressedTexSubImage1D;
PFNGLVERTEXARRAYVERTEXOFFSETEXTPROC glad_glVertexArrayVertexOffsetEXT;
PFNGLUNIFORM1UIVPROC glad_glUniform1uiv;
PFNGLCOPYPATHNVPROC glad_glCopyPathNV;
PFNGLCLEARNAMEDBUFFERDATAEXTPROC glad_glClearNamedBufferDataEXT;
PFNGLCLEARDEPTHPROC glad_glClearDepth;
PFNGLDISABLEINDEXEDEXTPROC glad_glDisableIndexedEXT;
PFNGLCOMPRESSEDTEXIMAGE3DARBPROC glad_glCompressedTexImage3DARB;
PFNGLUNIFORMMATRIX4X2DVPROC glad_glUniformMatrix4x2dv;
PFNGLCREATEBUFFERSPROC glad_glCreateBuffers;
PFNGLBEGINTRANSFORMFEEDBACKEXTPROC glad_glBeginTransformFeedbackEXT;
PFNGLUNIFORM4I64VNVPROC glad_glUniform4i64vNV;
PFNGLGETNUNIFORMUIVARBPROC glad_glGetnUniformuivARB;
PFNGLGETPROGRAMPIPELINEINFOLOGPROC glad_glGetProgramPipelineInfoLog;
PFNGLBLENDFUNCSEPARATEEXTPROC glad_glBlendFuncSeparateEXT;
PFNGLUNIFORMMATRIX4X3DVPROC glad_glUniformMatrix4x3dv;
PFNGLMAKEIMAGEHANDLENONRESIDENTARBPROC glad_glMakeImageHandleNonResidentARB;
PFNGLPOLYGONMODEPROC glad_glPolygonMode;
PFNGLVERTEXARRAYVERTEXATTRIBDIVISOREXTPROC glad_glVertexArrayVertexAttribDivisorEXT;
PFNGLWAITSYNCPROC glad_glWaitSync;
PFNGLMAPNAMEDBUFFEREXTPROC glad_glMapNamedBufferEXT;
PFNGLUNIFORM3UIVEXTPROC glad_glUniform3uivEXT;
PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC glad_glClearNamedFramebufferuiv;
PFNGLPROGRAMUNIFORM1UI64NVPROC glad_glProgramUniform1ui64NV;
PFNGLGETVERTEXATTRIBFVPROC glad_glGetVertexAttribfv;
PFNGLDISPATCHCOMPUTEPROC glad_glDispatchCompute;
PFNGLGETACTIVEATTRIBPROC glad_glGetActiveAttrib;
PFNGLTEXSUBIMAGE2DPROC glad_glTexSubImage2D;
PFNGLCLEARBUFFERFVPROC glad_glClearBufferfv;
PFNGLLOGICOPPROC glad_glLogicOp;
PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC glad_glProgramUniformMatrix3x4fv;
PFNGLGETTEXTUREPARAMETERIUIVPROC glad_glGetTextureParameterIuiv;
PFNGLPROGRAMUNIFORMMATRIX4DVPROC glad_glProgramUniformMatrix4dv;
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC glad_glFramebufferTexture1DEXT;
PFNGLMULTITEXCOORD2FVARBPROC glad_glMultiTexCoord2fvARB;
PFNGLNAMEDPROGRAMLOCALPARAMETERSI4IVEXTPROC glad_glNamedProgramLocalParametersI4ivEXT;
PFNGLVERTEXATTRIBI1UIVEXTPROC glad_glVertexAttribI1uivEXT;
PFNGLMULTITEXCOORD3IARBPROC glad_glMultiTexCoord3iARB;
PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC glad_glDrawTransformFeedbackStream;
PFNGLVERTEXARRAYMULTITEXCOORDOFFSETEXTPROC glad_glVertexArrayMultiTexCoordOffsetEXT;
PFNGLPROVOKINGVERTEXPROC glad_glProvokingVertex;
PFNGLSHADERBINARYPROC glad_glShaderBinary;
PFNGLGETMULTITEXGENIVEXTPROC glad_glGetMultiTexGenivEXT;
PFNGLGETHANDLEARBPROC glad_glGetHandleARB;
PFNGLTRANSFORMPATHNVPROC glad_glTransformPathNV;
PFNGLCOMPRESSEDTEXTUREIMAGE2DEXTPROC glad_glCompressedTextureImage2DEXT;
PFNGLGETNAMEDPROGRAMLOCALPARAMETERDVEXTPROC glad_glGetNamedProgramLocalParameterdvEXT;
PFNGLDRAWELEMENTSPROC glad_glDrawElements;
PFNGLPROGRAMUNIFORM3UI64NVPROC glad_glProgramUniform3ui64NV;
PFNGLUNIFORM1IVPROC glad_glUniform1iv;
PFNGLMATRIXMULTFEXTPROC glad_glMatrixMultfEXT;
PFNGLDRAWARRAYSINSTANCEDPROC glad_glDrawArraysInstanced;
PFNGLVERTEXATTRIB2SNVPROC glad_glVertexAttrib2sNV;
PFNGLPROGRAMUNIFORMMATRIX2FVPROC glad_glProgramUniformMatrix2fv;
PFNGLSAMPLERPARAMETERIVPROC glad_glSamplerParameteriv;
PFNGLVERTEXATTRIB4UIVPROC glad_glVertexAttrib4uiv;
PFNGLENDQUERYINDEXEDPROC glad_glEndQueryIndexed;
PFNGLPROGRAMPARAMETERIARBPROC glad_glProgramParameteriARB;
PFNGLPROGRAMUNIFORM1IVPROC glad_glProgramUniform1iv;
PFNGLBINDRENDERBUFFERPROC glad_glBindRenderbuffer;
PFNGLMULTITEXSUBIMAGE3DEXTPROC glad_glMultiTexSubImage3DEXT;
PFNGLISPROGRAMPROC glad_glIsProgram;
PFNGLMULTIDRAWELEMENTSINDIRECTAMDPROC glad_glMultiDrawElementsIndirectAMD;
PFNGLGETPROGRAMINFOLOGPROC glad_glGetProgramInfoLog;
PFNGLUNIFORMMATRIX3X2DVPROC glad_glUniformMatrix3x2dv;
PFNGLVERTEXATTRIB4FVPROC glad_glVertexAttrib4fv;
PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC glad_glProgramUniformMatrix2x3dv;
PFNGLGETNCOMPRESSEDTEXIMAGEPROC glad_glGetnCompressedTexImage;
PFNGLVERTEXATTRIBS3FVNVPROC glad_glVertexAttribs3fvNV;
PFNGLVERTEXATTRIB2FVPROC glad_glVertexAttrib2fv;
PFNGLDRAWARRAYSINDIRECTPROC glad_glDrawArraysIndirect;
PFNGLUNIFORM3UI64NVPROC glad_glUniform3ui64NV;
PFNGLBEGINCONDITIONALRENDERNVPROC glad_glBeginConditionalRenderNV;
PFNGLSTENCILTHENCOVERFILLPATHNVPROC glad_glStencilThenCoverFillPathNV;
PFNGLVERTEXARRAYINDEXOFFSETEXTPROC glad_glVertexArrayIndexOffsetEXT;
PFNGLDISABLECLIENTSTATEIEXTPROC glad_glDisableClientStateiEXT;
PFNGLCOPYMULTITEXSUBIMAGE1DEXTPROC glad_glCopyMultiTexSubImage1DEXT;
PFNGLTEXBUFFERARBPROC glad_glTexBufferARB;
PFNGLMULTIDRAWELEMENTSINDIRECTBINDLESSCOUNTNVPROC glad_glMultiDrawElementsIndirectBindlessCountNV;
PFNGLGETBUFFERPOINTERVARBPROC glad_glGetBufferPointervARB;
PFNGLMULTITEXPARAMETERIUIVEXTPROC glad_glMultiTexParameterIuivEXT;
PFNGLPROGRAMUNIFORM3FPROC glad_glProgramUniform3f;
PFNGLPROGRAMUNIFORM3DPROC glad_glProgramUniform3d;
PFNGLVERTEXATTRIBPOINTERNVPROC glad_glVertexAttribPointerNV;
PFNGLENDCONDITIONALRENDERNVPROC glad_glEndConditionalRenderNV;
PFNGLUNIFORM3I64VARBPROC glad_glUniform3i64vARB;
PFNGLGETQUERYIVARBPROC glad_glGetQueryivARB;
PFNGLVERTEXATTRIBI1UIPROC glad_glVertexAttribI1ui;
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC glad_glGetRenderbufferParameterivEXT;
PFNGLISBUFFERRESIDENTNVPROC glad_glIsBufferResidentNV;
PFNGLGETNAMEDRENDERBUFFERPARAMETERIVEXTPROC glad_glGetNamedRenderbufferParameterivEXT;
PFNGLVERTEXATTRIBI4BVEXTPROC glad_glVertexAttribI4bvEXT;
PFNGLMULTITEXGENDVEXTPROC glad_glMultiTexGendvEXT;
PFNGLGETPROGRAMENVPARAMETERDVARBPROC glad_glGetProgramEnvParameterdvARB;
PFNGLSECONDARYCOLORFORMATNVPROC glad_glSecondaryColorFormatNV;
PFNGLVERTEXATTRIB4UBVNVPROC glad_glVertexAttrib4ubvNV;
PFNGLINVALIDATETEXSUBIMAGEPROC glad_glInvalidateTexSubImage;
PFNGLGETINTEGER64VPROC glad_glGetInteger64v;
PFNGLGETPROGRAMENVPARAMETERFVARBPROC glad_glGetProgramEnvParameterfvARB;
PFNGLVERTEXATTRIB4USVARBPROC glad_glVertexAttrib4usvARB;
PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC glad_glNamedFramebufferRenderbuffer;
PFNGLPOINTPARAMETERFEXTPROC glad_glPointParameterfEXT;
PFNGLVERTEXATTRIB4NIVPROC glad_glVertexAttrib4Niv;
PFNGLCLEARBUFFERIVPROC glad_glClearBufferiv;
PFNGLVERTEXATTRIB4IVARBPROC glad_glVertexAttrib4ivARB;
PFNGLMATRIXMULT3X2FNVPROC glad_glMatrixMult3x2fNV;
PFNGLBEGINCONDITIONALRENDERNVXPROC glad_glBeginConditionalRenderNVX;
PFNGLGETNAMEDPROGRAMSTRINGEXTPROC glad_glGetNamedProgramStringEXT;
PFNGLGETTEXTURELEVELPARAMETERFVPROC glad_glGetTextureLevelParameterfv;
PFNGLGETATTRIBLOCATIONARBPROC glad_glGetAttribLocationARB;
PFNGLGETSAMPLERPARAMETERIUIVPROC glad_glGetSamplerParameterIuiv;
PFNGLCOPYTEXTURESUBIMAGE1DEXTPROC glad_glCopyTextureSubImage1DEXT;
PFNGLTEXTURESUBIMAGE1DPROC glad_glTextureSubImage1D;
PFNGLGENTRANSFORMFEEDBACKSPROC glad_glGenTransformFeedbacks;
PFNGLVERTEXATTRIBS2SVNVPROC glad_glVertexAttribs2svNV;
PFNGLTEXTUREIMAGE3DEXTPROC glad_glTextureImage3DEXT;
PFNGLGETNAMEDBUFFERPOINTERVPROC glad_glGetNamedBufferPointerv;
PFNGLISENABLEDINDEXEDEXTPROC glad_glIsEnabledIndexedEXT;
PFNGLGETNAMEDPROGRAMIVEXTPROC glad_glGetNamedProgramivEXT;
PFNGLGETTEXTURELEVELPARAMETERIVEXTPROC glad_glGetTextureLevelParameterivEXT;
PFNGLGETOBJECTPARAMETERIVARBPROC glad_glGetObjectParameterivARB;
PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC glad_glProgramUniformMatrix4x3dv;
PFNGLGETNAMEDPROGRAMLOCALPARAMETERIUIVEXTPROC glad_glGetNamedProgramLocalParameterIuivEXT;
PFNGLREADNPIXELSPROC glad_glReadnPixels;
PFNGLREADBUFFERPROC glad_glReadBuffer;
PFNGLPROGRAMUNIFORM4FVEXTPROC glad_glProgramUniform4fvEXT;
PFNGLVERTEXATTRIB2SARBPROC glad_glVertexAttrib2sARB;
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glad_glCompressedTexImage2DARB;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glad_glCheckFramebufferStatusEXT;
PFNGLVERTEXATTRIB1FARBPROC glad_glVertexAttrib1fARB;
PFNGLMULTITEXCOORD1IVARBPROC glad_glMultiTexCoord1ivARB;
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC glad_glDisableVertexAttribArrayARB;
PFNGLVERTEXATTRIBI1IEXTPROC glad_glVertexAttribI1iEXT;
PFNGLGETPATHDASHARRAYNVPROC glad_glGetPathDashArrayNV;
PFNGLSTENCILTHENCOVERFILLPATHINSTANCEDNVPROC glad_glStencilThenCoverFillPathInstancedNV;
PFNGLCOLORMASKPROC glad_glColorMask;
PFNGLGETNTEXIMAGEPROC glad_glGetnTexImage;
PFNGLPROGRAMLOCALPARAMETER4DARBPROC glad_glProgramLocalParameter4dARB;
PFNGLBLENDEQUATIONPROC glad_glBlendEquation;
PFNGLGETQUERYOBJECTUI64VEXTPROC glad_glGetQueryObjectui64vEXT;
PFNGLVERTEXATTRIBI4UIVEXTPROC glad_glVertexAttribI4uivEXT;
PFNGLGETPROGRAMRESOURCEIVPROC glad_glGetProgramResourceiv;
PFNGLCLEARBUFFERDATAPROC glad_glClearBufferData;
PFNGLGETPROGRAMIVNVPROC glad_glGetProgramivNV;
PFNGLDELETETRANSFORMFEEDBACKSPROC glad_glDeleteTransformFeedbacks;
PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glad_glDrawRangeElementsBaseVertex;
PFNGLUNIFORM4I64VARBPROC glad_glUniform4i64vARB;
PFNGLTEXTURESTORAGE3DMULTISAMPLEEXTPROC glad_glTextureStorage3DMultisampleEXT;
PFNGLCOMPRESSEDMULTITEXIMAGE1DEXTPROC glad_glCompressedMultiTexImage1DEXT;
PFNGLUNIFORMMATRIX2DVPROC glad_glUniformMatrix2dv;
PFNGLPROGRAMUNIFORM2IVEXTPROC glad_glProgramUniform2ivEXT;
PFNGLVERTEXATTRIB1DVPROC glad_glVertexAttrib1dv;
PFNGLVERTEXATTRIBS3DVNVPROC glad_glVertexAttribs3dvNV;
PFNGLBUFFERSUBDATAPROC glad_glBufferSubData;
PFNGLBLENDFUNCSEPARATEIPROC glad_glBlendFuncSeparatei;
PFNGLGETBOOLEANINDEXEDVEXTPROC glad_glGetBooleanIndexedvEXT;
PFNGLDISABLEIPROC glad_glDisablei;
PFNGLGETUNIFORMUI64VARBPROC glad_glGetUniformui64vARB;
PFNGLGETSYNCIVPROC glad_glGetSynciv;
PFNGLMAKEIMAGEHANDLERESIDENTARBPROC glad_glMakeImageHandleResidentARB;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glad_glRenderbufferStorageMultisampleEXT;
PFNGLPROGRAMUNIFORM2IPROC glad_glProgramUniform2i;
PFNGLLISTDRAWCOMMANDSSTATESCLIENTNVPROC glad_glListDrawCommandsStatesClientNV;
PFNGLBEGINTRANSFORMFEEDBACKNVPROC glad_glBeginTransformFeedbackNV;
PFNGLGETPROGRAMBINARYPROC glad_glGetProgramBinary;
PFNGLPROGRAMENVPARAMETER4FVARBPROC glad_glProgramEnvParameter4fvARB;
PFNGLPAUSETRANSFORMFEEDBACKPROC glad_glPauseTransformFeedback;
PFNGLUNIFORM2UI64ARBPROC glad_glUniform2ui64ARB;
PFNGLVERTEXATTRIBP2UIPROC glad_glVertexAttribP2ui;
PFNGLTRANSFORMFEEDBACKVARYINGSNVPROC glad_glTransformFeedbackVaryingsNV;
PFNGLTEXSUBIMAGE1DPROC glad_glTexSubImage1D;
PFNGLBLITFRAMEBUFFEREXTPROC glad_glBlitFramebufferEXT;
PFNGLUNIFORMMATRIX4FVARBPROC glad_glUniformMatrix4fvARB;
PFNGLVERTEXATTRIBP3UIVPROC glad_glVertexAttribP3uiv;
PFNGLVERTEXATTRIBI4IVPROC glad_glVertexAttribI4iv;
PFNGLGETMULTITEXLEVELPARAMETERIVEXTPROC glad_glGetMultiTexLevelParameterivEXT;
PFNGLGETVERTEXARRAYIVPROC glad_glGetVertexArrayiv;
PFNGLVERTEXATTRIB4FVARBPROC glad_glVertexAttrib4fvARB;
PFNGLGETNAMEDPROGRAMLOCALPARAMETERIIVEXTPROC glad_glGetNamedProgramLocalParameterIivEXT;
PFNGLPROGRAMUNIFORM3UIEXTPROC glad_glProgramUniform3uiEXT;
PFNGLPROGRAMUNIFORMMATRIX4X2DVEXTPROC glad_glProgramUniformMatrix4x2dvEXT;
PFNGLTEXPARAMETERFVPROC glad_glTexParameterfv;
PFNGLCALLCOMMANDLISTNVPROC glad_glCallCommandListNV;
PFNGLUNIFORM3DVPROC glad_glUniform3dv;
PFNGLPROGRAMUNIFORM3FVPROC glad_glProgramUniform3fv;
PFNGLISBUFFERARBPROC glad_glIsBufferARB;
PFNGLVERTEXATTRIB4DVNVPROC glad_glVertexAttrib4dvNV;
PFNGLPROGRAMUNIFORM1FVEXTPROC glad_glProgramUniform1fvEXT;
PFNGLVERTEXATTRIB1DVNVPROC glad_glVertexAttrib1dvNV;
PFNGLCOMPRESSEDTEXIMAGE1DARBPROC glad_glCompressedTexImage1DARB;
PFNGLBINDSAMPLERSPROC glad_glBindSamplers;
PFNGLVERTEXATTRIB4USVPROC glad_glVertexAttrib4usv;
PFNGLGETQUERYOBJECTUI64VPROC glad_glGetQueryObjectui64v;
PFNGLCREATETEXTURESPROC glad_glCreateTextures;
PFNGLGETTEXTUREIMAGEPROC glad_glGetTextureImage;
PFNGLPROGRAMUNIFORM1FVPROC glad_glProgramUniform1fv;
PFNGLUNIFORMMATRIX4FVPROC glad_glUniformMatrix4fv;
PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC glad_glProgramUniformMatrix4fvEXT;
PFNGLNORMALPOINTEREXTPROC glad_glNormalPointerEXT;
PFNGLMULTIDRAWARRAYSEXTPROC glad_glMultiDrawArraysEXT;
PFNGLPATHPARAMETERINVPROC glad_glPathParameteriNV;
PFNGLCREATECOMMANDLISTSNVPROC glad_glCreateCommandListsNV;
PFNGLCREATETRANSFORMFEEDBACKSPROC glad_glCreateTransformFeedbacks;
PFNGLDELETEPROGRAMPIPELINESPROC glad_glDeleteProgramPipelines;
PFNGLGENPROGRAMPIPELINESPROC glad_glGenProgramPipelines;
PFNGLVERTEXARRAYEDGEFLAGOFFSETEXTPROC glad_glVertexArrayEdgeFlagOffsetEXT;
PFNGLUNIFORM4UIVEXTPROC glad_glUniform4uivEXT;
PFNGLUNIFORMSUBROUTINESUIVPROC glad_glUniformSubroutinesuiv;
PFNGLPROGRAMUNIFORM1I64VNVPROC glad_glProgramUniform1i64vNV;
PFNGLVERTEXATTRIBI4UIEXTPROC glad_glVertexAttribI4uiEXT;
PFNGLCOMPRESSEDMULTITEXSUBIMAGE1DEXTPROC glad_glCompressedMultiTexSubImage1DEXT;
PFNGLMULTITEXGENDEXTPROC glad_glMultiTexGendEXT;
PFNGLCOMPILESHADERPROC glad_glCompileShader;
PFNGLUNIFORM1UIVEXTPROC glad_glUniform1uivEXT;
PFNGLUNIFORMMATRIX3FVPROC glad_glUniformMatrix3fv;
PFNGLGETUNIFORMUIVEXTPROC glad_glGetUniformuivEXT;
PFNGLGETQUERYOBJECTI64VPROC glad_glGetQueryObjecti64v;
PFNGLVERTEXATTRIBIPOINTERPROC glad_glVertexAttribIPointer;
PFNGLDEPTHRANGEFOESPROC glad_glDepthRangefOES;
PFNGLGETNAMEDBUFFERPARAMETERIVPROC glad_glGetNamedBufferParameteriv;
PFNGLGETNAMEDPROGRAMLOCALPARAMETERFVEXTPROC glad_glGetNamedProgramLocalParameterfvEXT;
PFNGLLINEWIDTHPROC glad_glLineWidth;
PFNGLINVALIDATEFRAMEBUFFERPROC glad_glInvalidateFramebuffer;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glad_glCompressedTexImage2D;
PFNGLGETQUERYBUFFEROBJECTUIVPROC glad_glGetQueryBufferObjectuiv;
PFNGLDRAWBUFFERSPROC glad_glDrawBuffers;
PFNGLVERTEXATTRIB1SPROC glad_glVertexAttrib1s;
PFNGLUNIFORM1IARBPROC glad_glUniform1iARB;
PFNGLUNIFORM1UI64VNVPROC glad_glUniform1ui64vNV;
PFNGLGETCLIPPLANEFOESPROC glad_glGetClipPlanefOES;
PFNGLPROGRAMUNIFORM3IVEXTPROC glad_glProgramUniform3ivEXT;
PFNGLPROGRAMPARAMETER4FNVPROC glad_glProgramParameter4fNV;
PFNGLDELETEPROGRAMPROC glad_glDeleteProgram;
PFNGLPROGRAMPARAMETER4DNVPROC glad_glProgramParameter4dNV;
PFNGLUNIFORMMATRIX4X3FVPROC glad_glUniformMatrix4x3fv;
PFNGLTEXTUREBARRIERPROC glad_glTextureBarrier;
PFNGLMULTITEXPARAMETERIIVEXTPROC glad_glMultiTexParameterIivEXT;
PFNGLGETINTERNALFORMATIVPROC glad_glGetInternalformativ;
PFNGLMULTITEXCOORD1DARBPROC glad_glMultiTexCoord1dARB;
PFNGLCLEARBUFFERFIPROC glad_glClearBufferfi;
PFNGLGENVERTEXARRAYSPROC glad_glGenVertexArrays;
PFNGLGETTRANSFORMFEEDBACKI64_VPROC glad_glGetTransformFeedbacki64_v;
PFNGLENABLEVERTEXARRAYATTRIBPROC glad_glEnableVertexArrayAttrib;
PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC glad_glProgramUniformMatrix3x2dv;
PFNGLVERTEXBINDINGDIVISORPROC glad_glVertexBindingDivisor;
PFNGLGETSAMPLERPARAMETERIIVPROC glad_glGetSamplerParameterIiv;
PFNGLVERTEXATTRIBFORMATNVPROC glad_glVertexAttribFormatNV;
PFNGLVERTEXATTRIBP4UIVPROC glad_glVertexAttribP4uiv;
PFNGLGETQUERYBUFFEROBJECTI64VPROC glad_glGetQueryBufferObjecti64v;
PFNGLGETVERTEXATTRIBDVPROC glad_glGetVertexAttribdv;
PFNGLARETEXTURESRESIDENTEXTPROC glad_glAreTexturesResidentEXT;
PFNGLGETPROGRAMRESOURCENAMEPROC glad_glGetProgramResourceName;
PFNGLGETFLOATINDEXEDVEXTPROC glad_glGetFloatIndexedvEXT;
PFNGLMULTITEXCOORD2SVARBPROC glad_glMultiTexCoord2svARB;
PFNGLGETTEXPARAMETERIIVEXTPROC glad_glGetTexParameterIivEXT;
PFNGLUNIFORM1UIEXTPROC glad_glUniform1uiEXT;
PFNGLVERTEXATTRIB4NUBARBPROC glad_glVertexAttrib4NubARB;
PFNGLVERTEXATTRIBL1UI64VARBPROC glad_glVertexAttribL1ui64vARB;
PFNGLUNIFORM1I64VNVPROC glad_glUniform1i64vNV;
PFNGLVERTEXATTRIBS4SVNVPROC glad_glVertexAttribs4svNV;
PFNGLUNIFORM1UIPROC glad_glUniform1ui;
PFNGLVERTEXATTRIB2FVARBPROC glad_glVertexAttrib2fvARB;
PFNGLCLEARDEPTHFPROC glad_glClearDepthf;
PFNGLVERTEXATTRIBI4SVEXTPROC glad_glVertexAttribI4svEXT;
PFNGLMEMORYBARRIERPROC glad_glMemoryBarrier;
PFNGLMULTITEXCOORD2IARBPROC glad_glMultiTexCoord2iARB;
PFNGLGETTEXTUREHANDLEARBPROC glad_glGetTextureHandleARB;
PFNGLISPROGRAMARBPROC glad_glIsProgramARB;
PFNGLBINDIMAGETEXTUREEXTPROC glad_glBindImageTextureEXT;
PFNGLGETFRAGDATALOCATIONPROC glad_glGetFragDataLocation;
PFNGLMULTITEXCOORD1SVARBPROC glad_glMultiTexCoord1svARB;
PFNGLVERTEXATTRIBI4IEXTPROC glad_glVertexAttribI4iEXT;
PFNGLVERTEXATTRIB4NBVARBPROC glad_glVertexAttrib4NbvARB;
PFNGLCOLORPOINTEREXTPROC glad_glColorPointerEXT;
PFNGLENABLECLIENTSTATEIEXTPROC glad_glEnableClientStateiEXT;
PFNGLCLEARTEXSUBIMAGEPROC glad_glClearTexSubImage;
PFNGLDETACHOBJECTARBPROC glad_glDetachObjectARB;
PFNGLGETTEXTUREPARAMETERIIVPROC glad_glGetTextureParameterIiv;
PFNGLCOMPRESSEDTEXTUREIMAGE3DEXTPROC glad_glCompressedTextureImage3DEXT;
PFNGLGETVERTEXARRAYINTEGERI_VEXTPROC glad_glGetVertexArrayIntegeri_vEXT;
PFNGLCHECKNAMEDFRAMEBUFFERSTATUSEXTPROC glad_glCheckNamedFramebufferStatusEXT;
PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC glad_glProgramUniformMatrix4x2dv;
PFNGLUNIFORM3FARBPROC glad_glUniform3fARB;
PFNGLISSHADERPROC glad_glIsShader;
PFNGLCOPYMULTITEXSUBIMAGE2DEXTPROC glad_glCopyMultiTexSubImage2DEXT;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC glad_glEnableVertexAttribArrayARB;
PFNGLENABLEPROC glad_glEnable;
PFNGLGETVERTEXATTRIBIVARBPROC glad_glGetVertexAttribivARB;
PFNGLDELETEQUERIESPROC glad_glDeleteQueries;
PFNGLUSEPROGRAMOBJECTARBPROC glad_glUseProgramObjectARB;
PFNGLVERTEXATTRIB4DVPROC glad_glVertexAttrib4dv;
PFNGLVERTEXATTRIB4FPROC glad_glVertexAttrib4f;
PFNGLUNIFORM3UI64VNVPROC glad_glUniform3ui64vNV;
PFNGLVERTEXATTRIB4DPROC glad_glVertexAttrib4d;
PFNGLMATRIXLOADIDENTITYEXTPROC glad_glMatrixLoadIdentityEXT;
PFNGLPROGRAMUNIFORM3UIPROC glad_glProgramUniform3ui;
PFNGLVERTEXATTRIB3SVARBPROC glad_glVertexAttrib3svARB;
PFNGLMATRIXSCALEDEXTPROC glad_glMatrixScaledEXT;
PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC glad_glProgramUniformMatrix2x3fv;
PFNGLGETVERTEXATTRIBPOINTERVNVPROC glad_glGetVertexAttribPointervNV;
PFNGLGENERATEMIPMAPEXTPROC glad_glGenerateMipmapEXT;
PFNGLVERTEXATTRIBI2UIVEXTPROC glad_glVertexAttribI2uivEXT;
PFNGLPATHCOORDSNVPROC glad_glPathCoordsNV;
PFNGLPROGRAMUNIFORM1IPROC glad_glProgramUniform1i;
PFNGLPROGRAMUNIFORM1DPROC glad_glProgramUniform1d;
PFNGLPROGRAMUNIFORM1FPROC glad_glProgramUniform1f;
PFNGLPROGRAMPARAMETERIEXTPROC glad_glProgramParameteriEXT;
PFNGLBEGINTRANSFORMFEEDBACKPROC glad_glBeginTransformFeedback;
PFNGLCOPYTEXTURESUBIMAGE2DPROC glad_glCopyTextureSubImage2D;
PFNGLPROGRAMUNIFORM3IVPROC glad_glProgramUniform3iv;
PFNGLBINDFRAGDATALOCATIONEXTPROC glad_glBindFragDataLocationEXT;
PFNGLFRAMEBUFFERREADBUFFEREXTPROC glad_glFramebufferReadBufferEXT;
PFNGLNAMEDBUFFERSUBDATAPROC glad_glNamedBufferSubData;
PFNGLGENBUFFERSARBPROC glad_glGenBuffersARB;
PFNGLBLENDEQUATIONIPROC glad_glBlendEquationi;
PFNGLVERTEXATTRIB4SPROC glad_glVertexAttrib4s;
PFNGLVERTEXATTRIB4FVNVPROC glad_glVertexAttrib4fvNV;
PFNGLGETBUFFERSUBDATAARBPROC glad_glGetBufferSubDataARB;
PFNGLGETPATHPARAMETERIVNVPROC glad_glGetPathParameterivNV;
PFNGLGETATTRIBLOCATIONPROC glad_glGetAttribLocation;
PFNGLDISABLEVERTEXARRAYATTRIBPROC glad_glDisableVertexArrayAttrib;
PFNGLGETTEXTUREPARAMETERIVPROC glad_glGetTextureParameteriv;
PFNGLGETATTACHEDOBJECTSARBPROC glad_glGetAttachedObjectsARB;
PFNGLGETTEXIMAGEPROC glad_glGetTexImage;
PFNGLCLEARDEPTHFOESPROC glad_glClearDepthfOES;
PFNGLDELETECOMMANDLISTSNVPROC glad_glDeleteCommandListsNV;
PFNGLPATHSUBCOORDSNVPROC glad_glPathSubCoordsNV;
PFNGLBLENDFUNCSEPARATEINDEXEDAMDPROC glad_glBlendFuncSeparateIndexedAMD;
PFNGLPROGRAMSTRINGARBPROC glad_glProgramStringARB;
PFNGLGETVERTEXATTRIBIVNVPROC glad_glGetVertexAttribivNV;
PFNGLISTEXTUREPROC glad_glIsTexture;
PFNGLTEXTURESUBIMAGE1DEXTPROC glad_glTextureSubImage1DEXT;
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glad_glGetActiveUniformBlockName;
PFNGLVERTEXATTRIB2DVARBPROC glad_glVertexAttrib2dvARB;
PFNGLMAPNAMEDBUFFERPROC glad_glMapNamedBuffer;
PFNGLVERTEXATTRIBPOINTERPROC glad_glVertexAttribPointer;
PFNGLDELETETEXTURESEXTPROC glad_glDeleteTexturesEXT;
PFNGLDRAWBUFFERSATIPROC glad_glDrawBuffersATI;
PFNGLGETFLOATI_VPROC glad_glGetFloati_v;
PFNGLPROGRAMUNIFORM1IVEXTPROC glad_glProgramUniform1ivEXT;
PFNGLVERTEXATTRIBI4BVPROC glad_glVertexAttribI4bv;
PFNGLPROGRAMUNIFORM4UI64NVPROC glad_glProgramUniform4ui64NV;
PFNGLVERTEXATTRIBP1UIPROC glad_glVertexAttribP1ui;
PFNGLPROGRAMUNIFORM4DVEXTPROC glad_glProgramUniform4dvEXT;
PFNGLENDTRANSFORMFEEDBACKEXTPROC glad_glEndTransformFeedbackEXT;
PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC glad_glProgramUniformMatrix3x4dv;
PFNGLENDCONDITIONALRENDERNVXPROC glad_glEndConditionalRenderNVX;
PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC glad_glGetNamedRenderbufferParameteriv;
PFNGLBINDBUFFERSRANGEPROC glad_glBindBuffersRange;
PFNGLDRAWCOMMANDSADDRESSNVPROC glad_glDrawCommandsAddressNV;
PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glad_glBindFragDataLocationIndexed;
PFNGLUNIFORM2IVPROC glad_glUniform2iv;
PFNGLNAMEDFRAMEBUFFERRENDERBUFFEREXTPROC glad_glNamedFramebufferRenderbufferEXT;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC glad_glDrawElementsInstancedBaseVertexBaseInstance;
PFNGLGETNAMEDBUFFERPARAMETERIVEXTPROC glad_glGetNamedBufferParameterivEXT;
PFNGLTEXTURESUBIMAGE3DPROC glad_glTextureSubImage3D;
PFNGLFRAMEBUFFERTEXTURE1DPROC glad_glFramebufferTexture1D;
PFNGLGETSHADERIVPROC glad_glGetShaderiv;
PFNGLMATRIXLOADTRANSPOSEDEXTPROC glad_glMatrixLoadTransposedEXT;
PFNGLGETPOINTERI_VEXTPROC glad_glGetPointeri_vEXT;
PFNGLNAMEDBUFFERSTORAGEPROC glad_glNamedBufferStorage;
PFNGLPOLYGONOFFSETPROC glad_glPolygonOffset;
PFNGLPROGRAMUNIFORM2UI64NVPROC glad_glProgramUniform2ui64NV;
PFNGLUNIFORM1DVPROC glad_glUniform1dv;
PFNGLPROGRAMENVPARAMETER4DVARBPROC glad_glProgramEnvParameter4dvARB;
PFNGLMULTITEXSUBIMAGE2DEXTPROC glad_glMultiTexSubImage2DEXT;
PFNGLDEBUGMESSAGECONTROLARBPROC glad_glDebugMessageControlARB;
PFNGLISTEXTUREEXTPROC glad_glIsTextureEXT;
PFNGLLINKPROGRAMARBPROC glad_glLinkProgramARB;
PFNGLVERTEXARRAYVERTEXATTRIBBINDINGEXTPROC glad_glVertexArrayVertexAttribBindingEXT;
PFNGLUNIFORM1UI64VARBPROC glad_glUniform1ui64vARB;
PFNGLCOMPRESSEDTEXIMAGE3DPROC glad_glCompressedTexImage3D;
PFNGLGETPATHCOORDSNVPROC glad_glGetPathCoordsNV;
PFNGLCOMMANDLISTSEGMENTSNVPROC glad_glCommandListSegmentsNV;
PFNGLCLAMPCOLORARBPROC glad_glClampColorARB;
PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC glad_glTransformFeedbackVaryingsEXT;
PFNGLVERTEXATTRIB2SVNVPROC glad_glVertexAttrib2svNV;
PFNGLMULTITEXPARAMETERFVEXTPROC glad_glMultiTexParameterfvEXT;
PFNGLMULTITEXCOORD2IVARBPROC glad_glMultiTexCoord2ivARB;
PFNGLGETINFOLOGARBPROC glad_glGetInfoLogARB;
PFNGLGETTRACKMATRIXIVNVPROC glad_glGetTrackMatrixivNV;
PFNGLISNAMEDBUFFERRESIDENTNVPROC glad_glIsNamedBufferResidentNV;
PFNGLMATRIXMULTDEXTPROC glad_glMatrixMultdEXT;
PFNGLCOVERFILLPATHNVPROC glad_glCoverFillPathNV;
PFNGLGENERATETEXTUREMIPMAPPROC glad_glGenerateTextureMipmap;
PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIVEXTPROC glad_glNamedProgramLocalParameterI4uivEXT;
PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glad_glMultiDrawElementsBaseVertex;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC glad_glProgramLocalParameter4fvARB;
PFNGLMEMORYBARRIEREXTPROC glad_glMemoryBarrierEXT;
PFNGLGETTEXPARAMETERIIVPROC glad_glGetTexParameterIiv;
PFNGLNAMEDFRAMEBUFFERPARAMETERIEXTPROC glad_glNamedFramebufferParameteriEXT;
PFNGLVERTEXATTRIBI4USVEXTPROC glad_glVertexAttribI4usvEXT;
PFNGLTEXTUREBUFFERRANGEEXTPROC glad_glTextureBufferRangeEXT;
PFNGLTRANSFORMFEEDBACKVARYINGSPROC glad_glTransformFeedbackVaryings;
PFNGLGETNUNIFORMI64VARBPROC glad_glGetnUniformi64vARB;
PFNGLCOMPRESSEDMULTITEXSUBIMAGE2DEXTPROC glad_glCompressedMultiTexSubImage2DEXT;
PFNGLVERTEXATTRIB4SVARBPROC glad_glVertexAttrib4svARB;
PFNGLDEBUGMESSAGECALLBACKPROC glad_glDebugMessageCallback;
PFNGLSTENCILTHENCOVERSTROKEPATHINSTANCEDNVPROC glad_glStencilThenCoverStrokePathInstancedNV;
PFNGLBUFFERSUBDATAARBPROC glad_glBufferSubDataARB;
PFNGLGETVERTEXATTRIBLUI64VARBPROC glad_glGetVertexAttribLui64vARB;
PFNGLGETPATHMETRICSNVPROC glad_glGetPathMetricsNV;
PFNGLGETBOOLEANI_VPROC glad_glGetBooleani_v;
PFNGLCOMPRESSEDMULTITEXIMAGE2DEXTPROC glad_glCompressedMultiTexImage2DEXT;
PFNGLTEXTUREPARAMETERIIVPROC glad_glTextureParameterIiv;
PFNGLBINDBUFFERRANGENVPROC glad_glBindBufferRangeNV;
PFNGLPROGRAMUNIFORM2DVEXTPROC glad_glProgramUniform2dvEXT;
PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glad_glGetNamedFramebufferAttachmentParameterivEXT;
PFNGLVERTEXARRAYVERTEXATTRIBIFORMATEXTPROC glad_glVertexArrayVertexAttribIFormatEXT;
PFNGLHINTPROC glad_glHint;
PFNGLGETTEXTUREIMAGEEXTPROC glad_glGetTextureImageEXT;
PFNGLMULTIDRAWARRAYSINDIRECTPROC glad_glMultiDrawArraysIndirect;
PFNGLVERTEXATTRIBP2UIVPROC glad_glVertexAttribP2uiv;
PFNGLPROGRAMUNIFORM1I64VARBPROC glad_glProgramUniform1i64vARB;
PFNGLPUSHCLIENTATTRIBDEFAULTEXTPROC glad_glPushClientAttribDefaultEXT;
PFNGLVERTEXATTRIB4SVNVPROC glad_glVertexAttrib4svNV;
PFNGLCOVERFILLPATHINSTANCEDNVPROC glad_glCoverFillPathInstancedNV;
PFNGLMULTITEXPARAMETERIVEXTPROC glad_glMultiTexParameterivEXT;
PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC glad_glGetActiveAtomicCounterBufferiv;
PFNGLVERTEXARRAYATTRIBBINDINGPROC glad_glVertexArrayAttribBinding;
PFNGLVERTEXATTRIB3FVNVPROC glad_glVertexAttrib3fvNV;
PFNGLPROGRAMPARAMETERS4DVNVPROC glad_glProgramParameters4dvNV;
PFNGLVERTEXATTRIBL4DVPROC glad_glVertexAttribL4dv;
PFNGLGETTEXPARAMETERIVPROC glad_glGetTexParameteriv;
PFNGLGETVERTEXATTRIBPOINTERVPROC glad_glGetVertexAttribPointerv;
PFNGLDRAWARRAYSINSTANCEDARBPROC glad_glDrawArraysInstancedARB;
PFNGLPROGRAMUNIFORM3I64VNVPROC glad_glProgramUniform3i64vNV;
PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC glad_glTransformFeedbackBufferBase;
PFNGLDISABLEPROC glad_glDisable;
PFNGLISPOINTINFILLPATHNVPROC glad_glIsPointInFillPathNV;
PFNGLVERTEXATTRIBI2UIEXTPROC glad_glVertexAttribI2uiEXT;
PFNGLPROGRAMUNIFORM4UIVPROC glad_glProgramUniform4uiv;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glad_glFramebufferRenderbufferEXT;
PFNGLBINDBUFFEROFFSETNVPROC glad_glBindBufferOffsetNV;
PFNGLGETVERTEXATTRIBIVPROC glad_glGetVertexAttribiv;
PFNGLVERTEXATTRIBI2UIVPROC glad_glVertexAttribI2uiv;
PFNGLUNIFORM2IVARBPROC glad_glUniform2ivARB;
PFNGLBINDBUFFERSBASEPROC glad_glBindBuffersBase;
PFNGLTEXTURESUBIMAGE2DPROC glad_glTextureSubImage2D;
PFNGLGETPATHLENGTHNVPROC glad_glGetPathLengthNV;
PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC glad_glNamedFramebufferParameteri;
PFNGLTEXTUREBUFFERPROC glad_glTextureBuffer;
PFNGLCLIPPLANEFOESPROC glad_glClipPlanefOES;
PFNGLUNIFORM4I64NVPROC glad_glUniform4i64NV;
PFNGLUNIFORMHANDLEUI64VARBPROC glad_glUniformHandleui64vARB;
PFNGLUNMAPNAMEDBUFFERPROC glad_glUnmapNamedBuffer;
PFNGLCREATESTATESNVPROC glad_glCreateStatesNV;
PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC glad_glNamedRenderbufferStorageMultisample;
PFNGLCLEARNAMEDBUFFERDATAPROC glad_glClearNamedBufferData;
PFNGLSTENCILFILLPATHINSTANCEDNVPROC glad_glStencilFillPathInstancedNV;
PFNGLPROGRAMUNIFORM4I64NVPROC glad_glProgramUniform4i64NV;
PFNGLTEXTUREPAGECOMMITMENTEXTPROC glad_glTexturePageCommitmentEXT;
PFNGLVERTEXATTRIB4NUIVARBPROC glad_glVertexAttrib4NuivARB;
PFNGLBINDMULTITEXTUREEXTPROC glad_glBindMultiTextureEXT;
PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC glad_glDrawArraysInstancedBaseInstance;
PFNGLVERTEXATTRIB3FPROC glad_glVertexAttrib3f;
PFNGLVERTEXATTRIB3DPROC glad_glVertexAttrib3d;
PFNGLBLENDCOLORPROC glad_glBlendColor;
PFNGLBINDTRANSFORMFEEDBACKNVPROC glad_glBindTransformFeedbackNV;
PFNGLSAMPLERPARAMETERIUIVPROC glad_glSamplerParameterIuiv;
PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC glad_glCheckNamedFramebufferStatus;
PFNGLVERTEXATTRIB3SPROC glad_glVertexAttrib3s;
PFNGLDRAWELEMENTSINSTANCEDEXTPROC glad_glDrawElementsInstancedEXT;
PFNGLNAMEDPROGRAMLOCALPARAMETERI4IEXTPROC glad_glNamedProgramLocalParameterI4iEXT;
PFNGLDISABLEVERTEXARRAYEXTPROC glad_glDisableVertexArrayEXT;
PFNGLGETGRAPHICSRESETSTATUSARBPROC glad_glGetGraphicsResetStatusARB;
PFNGLUNIFORM4IPROC glad_glUniform4i;
PFNGLACTIVETEXTUREPROC glad_glActiveTexture;
PFNGLENABLEVERTEXATTRIBARRAYPROC glad_glEnableVertexAttribArray;
PFNGLUNIFORM4DPROC glad_glUniform4d;
PFNGLUNIFORM4FPROC glad_glUniform4f;
PFNGLCREATEPROGRAMPIPELINESPROC glad_glCreateProgramPipelines;
PFNGLVERTEXATTRIBI4SVPROC glad_glVertexAttribI4sv;
PFNGLVERTEXATTRIBLPOINTERPROC glad_glVertexAttribLPointer;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glad_glDrawElementsInstancedBaseVertex;
PFNGLDRAWTRANSFORMFEEDBACKNVPROC glad_glDrawTransformFeedbackNV;
PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLECOVERAGEEXTPROC glad_glNamedRenderbufferStorageMultisampleCoverageEXT;
PFNGLPOPDEBUGGROUPPROC glad_glPopDebugGroup;
PFNGLUNIFORMBLOCKBINDINGPROC glad_glUniformBlockBinding;
PFNGLGENERATETEXTUREMIPMAPEXTPROC glad_glGenerateTextureMipmapEXT;
PFNGLVERTEXATTRIBIFORMATNVPROC glad_glVertexAttribIFormatNV;
PFNGLPATHGLYPHRANGENVPROC glad_glPathGlyphRangeNV;
PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC glad_glProgramUniformMatrix3x2fv;
PFNGLUNIFORM2FVARBPROC glad_glUniform2fvARB;
PFNGLPROGRAMUNIFORM4I64ARBPROC glad_glProgramUniform4i64ARB;
PFNGLGETQUERYINDEXEDIVPROC glad_glGetQueryIndexediv;
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC glad_glFramebufferTexture3DEXT;
PFNGLISCOMMANDLISTNVPROC glad_glIsCommandListNV;
PFNGLOBJECTLABELPROC glad_glObjectLabel;
PFNGLDELETETRANSFORMFEEDBACKSNVPROC glad_glDeleteTransformFeedbacksNV;
PFNGLVERTEXATTRIBLPOINTEREXTPROC glad_glVertexAttribLPointerEXT;
PFNGLSTENCILFUNCSEPARATEPROC glad_glStencilFuncSeparate;
PFNGLCOMPILESHADERARBPROC glad_glCompileShaderARB;
PFNGLNAMEDBUFFERSUBDATAEXTPROC glad_glNamedBufferSubDataEXT;
PFNGLTEXBUFFEREXTPROC glad_glTexBufferEXT;
PFNGLCREATEPROGRAMPROC glad_glCreateProgram;
PFNGLBLENDEQUATIONSEPARATEEXTPROC glad_glBlendEquationSeparateEXT;
PFNGLPRIMITIVERESTARTINDEXPROC glad_glPrimitiveRestartIndex;
PFNGLVERTEXATTRIB2DNVPROC glad_glVertexAttrib2dNV;
PFNGLBINDIMAGETEXTURESPROC glad_glBindImageTextures;
PFNGLMULTITEXCOORD2DARBPROC glad_glMultiTexCoord2dARB;
PFNGLPATHSTRINGNVPROC glad_glPathStringNV;
PFNGLGETMULTITEXPARAMETERIIVEXTPROC glad_glGetMultiTexParameterIivEXT;
PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVEXTPROC glad_glGetNamedFramebufferParameterivEXT;
PFNGLARRAYELEMENTEXTPROC glad_glArrayElementEXT;
PFNGLUNIFORM4IVPROC glad_glUniform4iv;
PFNGLPATHDASHARRAYNVPROC glad_glPathDashArrayNV;
PFNGLUNIFORM3UIVPROC glad_glUniform3uiv;
PFNGLCLEARCOLORPROC glad_glClearColor;
PFNGLGETIMAGEHANDLEARBPROC glad_glGetImageHandleARB;
PFNGLPROGRAMUNIFORMMATRIX2X4DVEXTPROC glad_glProgramUniformMatrix2x4dvEXT;
PFNGLGETQUERYOBJECTIVARBPROC glad_glGetQueryObjectivARB;
PFNGLVERTEXATTRIB3SNVPROC glad_glVertexAttrib3sNV;
PFNGLGETNUNIFORMIVPROC glad_glGetnUniformiv;
PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC glad_glGetNamedFramebufferParameteriv;
PFNGLUNIFORM3UIPROC glad_glUniform3ui;
PFNGLCLEARCOLORIUIEXTPROC glad_glClearColorIuiEXT;
PFNGLVERTEXATTRIBI4UIVPROC glad_glVertexAttribI4uiv;
PFNGLVERTEXATTRIB4BVPROC glad_glVertexAttrib4bv;
PFNGLGETPROGRAMRESOURCEINDEXPROC glad_glGetProgramResourceIndex;
PFNGLDRAWELEMENTSINDIRECTPROC glad_glDrawElementsIndirect;
PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC glad_glGetSubroutineUniformLocation;
PFNGLVERTEXATTRIB1DARBPROC glad_glVertexAttrib1dARB;
PFNGLCOMPILECOMMANDLISTNVPROC glad_glCompileCommandListNV;
PFNGLPROGRAMUNIFORM1FEXTPROC glad_glProgramUniform1fEXT;
PFNGLGETQUERYBUFFEROBJECTUI64VPROC glad_glGetQueryBufferObjectui64v;
PFNGLVERTEXATTRIB3SARBPROC glad_glVertexAttrib3sARB;
PFNGLUNIFORMMATRIX2X3FVPROC glad_glUniformMatrix2x3fv;
PFNGLPROGRAMUNIFORM2I64VARBPROC glad_glProgramUniform2i64vARB;
PFNGLGETVERTEXATTRIBIUIVPROC glad_glGetVertexAttribIuiv;
PFNGLGETTEXPARAMETERIUIVEXTPROC glad_glGetTexParameterIuivEXT;
PFNGLVERTEXARRAYSECONDARYCOLOROFFSETEXTPROC glad_glVertexArraySecondaryColorOffsetEXT;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glad_glCompressedTexSubImage2D;
PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC glad_glCompressedTexSubImage3DARB;
PFNGLPROGRAMBINARYPROC glad_glProgramBinary;
PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC glad_glTextureStorage2DMultisample;
PFNGLGETTEXPARAMETERFVPROC glad_glGetTexParameterfv;
PFNGLTRANSFORMFEEDBACKSTREAMATTRIBSNVPROC glad_glTransformFeedbackStreamAttribsNV;
PFNGLVERTEXATTRIB1FPROC glad_glVertexAttrib1f;
PFNGLMULTITEXCOORD4SARBPROC glad_glMultiTexCoord4sARB;
PFNGLGETVERTEXARRAYINDEXED64IVPROC glad_glGetVertexArrayIndexed64iv;
PFNGLMATRIXMULTTRANSPOSE3X3FNVPROC glad_glMatrixMultTranspose3x3fNV;
PFNGLTEXPARAMETERIIVPROC glad_glTexParameterIiv;
PFNGLGETSAMPLERPARAMETERIVPROC glad_glGetSamplerParameteriv;
PFNGLVERTEXARRAYATTRIBFORMATPROC glad_glVertexArrayAttribFormat;
PFNGLENDTRANSFORMFEEDBACKPROC glad_glEndTransformFeedback;
PFNGLDRAWCOMMANDSSTATESADDRESSNVPROC glad_glDrawCommandsStatesAddressNV;
PFNGLUNIFORM4UI64NVPROC glad_glUniform4ui64NV;
PFNGLTEXIMAGE2DMULTISAMPLEPROC glad_glTexImage2DMultisample;
PFNGLBINDTEXTUREEXTPROC glad_glBindTextureEXT;
PFNGLPROGRAMUNIFORM2DVPROC glad_glProgramUniform2dv;
PFNGLVERTEXATTRIBI1IVPROC glad_glVertexAttribI1iv;
PFNGLVERTEXATTRIBDIVISORPROC glad_glVertexAttribDivisor;
PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC glad_glGetCompressedTextureImage;
PFNGLMATRIXROTATEFEXTPROC glad_glMatrixRotatefEXT;
PFNGLACTIVETEXTUREARBPROC glad_glActiveTextureARB;
PFNGLGETUNIFORMUI64VNVPROC glad_glGetUniformui64vNV;
PFNGLTEXPARAMETERIIVEXTPROC glad_glTexParameterIivEXT;
PFNGLUNIFORMMATRIX4X2FVPROC glad_glUniformMatrix4x2fv;
PFNGLGETUNIFORMI64VARBPROC glad_glGetUniformi64vARB;
PFNGLBINDVERTEXBUFFERSPROC glad_glBindVertexBuffers;
PFNGLVERTEXATTRIB2FPROC glad_glVertexAttrib2f;
PFNGLVERTEXATTRIB4NSVPROC glad_glVertexAttrib4Nsv;
PFNGLINVALIDATEBUFFERSUBDATAPROC glad_glInvalidateBufferSubData;
PFNGLVERTEXATTRIB3DVPROC glad_glVertexAttrib3dv;
PFNGLNAMEDFRAMEBUFFERTEXTURE1DEXTPROC glad_glNamedFramebufferTexture1DEXT;
PFNGLMAPBUFFERRANGEPROC glad_glMapBufferRange;
PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC glad_glProgramUniformMatrix4x2fv;
PFNGLGETTEXTUREPARAMETERIIVEXTPROC glad_glGetTextureParameterIivEXT;
PFNGLVERTEXATTRIB3DVARBPROC glad_glVertexAttrib3dvARB;
PFNGLNAMEDPROGRAMLOCALPARAMETERSI4UIVEXTPROC glad_glNamedProgramLocalParametersI4uivEXT;
PFNGLGENPROGRAMSARBPROC glad_glGenProgramsARB;
PFNGLGENTRANSFORMFEEDBACKSNVPROC glad_glGenTransformFeedbacksNV;
PFNGLVERTEXATTRIBI2IVPROC glad_glVertexAttribI2iv;
PFNGLCOPYMULTITEXIMAGE2DEXTPROC glad_glCopyMultiTexImage2DEXT;
PFNGLVERTEXATTRIB4UBNVPROC glad_glVertexAttrib4ubNV;
PFNGLGETSHADERPRECISIONFORMATPROC glad_glGetShaderPrecisionFormat;
PFNGLTEXTUREVIEWPROC glad_glTextureView;
PFNGLGETFRAGDATALOCATIONEXTPROC glad_glGetFragDataLocationEXT;
PFNGLGETCOMPRESSEDMULTITEXIMAGEEXTPROC glad_glGetCompressedMultiTexImageEXT;
PFNGLSHADERSOURCEPROC glad_glShaderSource;
PFNGLDELETERENDERBUFFERSPROC glad_glDeleteRenderbuffers;
PFNGLGENTEXTURESEXTPROC glad_glGenTexturesEXT;
PFNGLCOPYTEXIMAGE2DEXTPROC glad_glCopyTexImage2DEXT;
PFNGLGETNAMEDBUFFERPARAMETERI64VPROC glad_glGetNamedBufferParameteri64v;
PFNGLTEXTUREPARAMETERIIVEXTPROC glad_glTextureParameterIivEXT;
PFNGLPROGRAMUNIFORMUI64VNVPROC glad_glProgramUniformui64vNV;
PFNGLPROGRAMUNIFORM2UI64ARBPROC glad_glProgramUniform2ui64ARB;
PFNGLUNIFORM2UI64NVPROC glad_glUniform2ui64NV;
PFNGLBUFFERDATAPROC glad_glBufferData;
PFNGLTEXTUREIMAGE1DEXTPROC glad_glTextureImage1DEXT;
PFNGLATTACHOBJECTARBPROC glad_glAttachObjectARB;
PFNGLCOPYTEXSUBIMAGE1DEXTPROC glad_glCopyTexSubImage1DEXT;
PFNGLUNIFORMMATRIX3FVARBPROC glad_glUniformMatrix3fvARB;
PFNGLMAKEBUFFERRESIDENTNVPROC glad_glMakeBufferResidentNV;
PFNGLPOINTPARAMETERFVEXTPROC glad_glPointParameterfvEXT;
PFNGLUNIFORM4FARBPROC glad_glUniform4fARB;
PFNGLCOPYTEXTURESUBIMAGE3DPROC glad_glCopyTextureSubImage3D;
PFNGLMULTITEXENVFVEXTPROC glad_glMultiTexEnvfvEXT;
PFNGLVERTEXATTRIBI1IPROC glad_glVertexAttribI1i;
PFNGLGETDEBUGMESSAGELOGARBPROC glad_glGetDebugMessageLogARB;
PFNGLNAMEDPROGRAMLOCALPARAMETERS4FVEXTPROC glad_glNamedProgramLocalParameters4fvEXT;
PFNGLGETBUFFERPOINTERVPROC glad_glGetBufferPointerv;
PFNGLFRAMEBUFFERTEXTURE3DPROC glad_glFramebufferTexture3D;
PFNGLSAMPLERPARAMETERFVPROC glad_glSamplerParameterfv;
PFNGLBINDBUFFERARBPROC glad_glBindBufferARB;
PFNGLVERTEXATTRIB1SNVPROC glad_glVertexAttrib1sNV;
PFNGLUNIFORM1FVPROC glad_glUniform1fv;
PFNGLSCISSORINDEXEDVPROC glad_glScissorIndexedv;
PFNGLUNIFORM1IVARBPROC glad_glUniform1ivARB;
PFNGLMULTITEXCOORD4DVARBPROC glad_glMultiTexCoord4dvARB;
PFNGLVERTEXATTRIB2DPROC glad_glVertexAttrib2d;
PFNGLMULTIDRAWELEMENTSEXTPROC glad_glMultiDrawElementsEXT;
PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC glad_glGetTransformFeedbackVaryingEXT;
PFNGLVERTEXATTRIB2SPROC glad_glVertexAttrib2s;
PFNGLTEXIMAGE3DMULTISAMPLEPROC glad_glTexImage3DMultisample;
PFNGLGETUNIFORMBLOCKINDEXPROC glad_glGetUniformBlockIndex;
PFNGLFRONTFACEPROC glad_glFrontFace;
PFNGLVERTEXATTRIB4DARBPROC glad_glVertexAttrib4dARB;
PFNGLISPOINTINSTROKEPATHNVPROC glad_glIsPointInStrokePathNV;


static void load_GL_VERSION_1_0(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_VERSION_1_0) return;
    glad_glFlush = (PFNGLFLUSHPROC)load("glFlush", userptr);
    glad_glClearColor = (PFNGLCLEARCOLORPROC)load("glClearColor", userptr);
    glad_glGetFloatv = (PFNGLGETFLOATVPROC)load("glGetFloatv", userptr);
    glad_glHint = (PFNGLHINTPROC)load("glHint", userptr);
    glad_glGetString = (PFNGLGETSTRINGPROC)load("glGetString", userptr);
    glad_glGetTexLevelParameteriv = (PFNGLGETTEXLEVELPARAMETERIVPROC)load("glGetTexLevelParameteriv", userptr);
    glad_glLineWidth = (PFNGLLINEWIDTHPROC)load("glLineWidth", userptr);
    glad_glPolygonMode = (PFNGLPOLYGONMODEPROC)load("glPolygonMode", userptr);
    glad_glGetTexParameteriv = (PFNGLGETTEXPARAMETERIVPROC)load("glGetTexParameteriv", userptr);
    glad_glClearStencil = (PFNGLCLEARSTENCILPROC)load("glClearStencil", userptr);
    glad_glDepthFunc = (PFNGLDEPTHFUNCPROC)load("glDepthFunc", userptr);
    glad_glGetTexParameterfv = (PFNGLGETTEXPARAMETERFVPROC)load("glGetTexParameterfv", userptr);
    glad_glDisable = (PFNGLDISABLEPROC)load("glDisable", userptr);
    glad_glLogicOp = (PFNGLLOGICOPPROC)load("glLogicOp", userptr);
    glad_glColorMask = (PFNGLCOLORMASKPROC)load("glColorMask", userptr);
    glad_glCullFace = (PFNGLCULLFACEPROC)load("glCullFace", userptr);
    glad_glPointSize = (PFNGLPOINTSIZEPROC)load("glPointSize", userptr);
    glad_glReadBuffer = (PFNGLREADBUFFERPROC)load("glReadBuffer", userptr);
    glad_glGetDoublev = (PFNGLGETDOUBLEVPROC)load("glGetDoublev", userptr);
    glad_glClear = (PFNGLCLEARPROC)load("glClear", userptr);
    glad_glIsEnabled = (PFNGLISENABLEDPROC)load("glIsEnabled", userptr);
    glad_glStencilOp = (PFNGLSTENCILOPPROC)load("glStencilOp", userptr);
    glad_glReadPixels = (PFNGLREADPIXELSPROC)load("glReadPixels", userptr);
    glad_glClearDepth = (PFNGLCLEARDEPTHPROC)load("glClearDepth", userptr);
    glad_glTexImage1D = (PFNGLTEXIMAGE1DPROC)load("glTexImage1D", userptr);
    glad_glGetError = (PFNGLGETERRORPROC)load("glGetError", userptr);
    glad_glTexParameteriv = (PFNGLTEXPARAMETERIVPROC)load("glTexParameteriv", userptr);
    glad_glGetTexImage = (PFNGLGETTEXIMAGEPROC)load("glGetTexImage", userptr);
    glad_glGetTexLevelParameterfv = (PFNGLGETTEXLEVELPARAMETERFVPROC)load("glGetTexLevelParameterfv", userptr);
    glad_glStencilFunc = (PFNGLSTENCILFUNCPROC)load("glStencilFunc", userptr);
    glad_glGetIntegerv = (PFNGLGETINTEGERVPROC)load("glGetIntegerv", userptr);
    glad_glDrawBuffer = (PFNGLDRAWBUFFERPROC)load("glDrawBuffer", userptr);
    glad_glTexImage2D = (PFNGLTEXIMAGE2DPROC)load("glTexImage2D", userptr);
    glad_glStencilMask = (PFNGLSTENCILMASKPROC)load("glStencilMask", userptr);
    glad_glTexParameterfv = (PFNGLTEXPARAMETERFVPROC)load("glTexParameterfv", userptr);
    glad_glEnable = (PFNGLENABLEPROC)load("glEnable", userptr);
    glad_glFinish = (PFNGLFINISHPROC)load("glFinish", userptr);
    glad_glBlendFunc = (PFNGLBLENDFUNCPROC)load("glBlendFunc", userptr);
    glad_glViewport = (PFNGLVIEWPORTPROC)load("glViewport", userptr);
    glad_glDepthMask = (PFNGLDEPTHMASKPROC)load("glDepthMask", userptr);
    glad_glDepthRange = (PFNGLDEPTHRANGEPROC)load("glDepthRange", userptr);
    glad_glTexParameterf = (PFNGLTEXPARAMETERFPROC)load("glTexParameterf", userptr);
    glad_glTexParameteri = (PFNGLTEXPARAMETERIPROC)load("glTexParameteri", userptr);
    glad_glFrontFace = (PFNGLFRONTFACEPROC)load("glFrontFace", userptr);
    glad_glScissor = (PFNGLSCISSORPROC)load("glScissor", userptr);
    glad_glGetBooleanv = (PFNGLGETBOOLEANVPROC)load("glGetBooleanv", userptr);
    glad_glPixelStorei = (PFNGLPIXELSTOREIPROC)load("glPixelStorei", userptr);
    glad_glPixelStoref = (PFNGLPIXELSTOREFPROC)load("glPixelStoref", userptr);
}
static void load_GL_VERSION_1_1(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_VERSION_1_1) return;
    glad_glCopyTexImage1D = (PFNGLCOPYTEXIMAGE1DPROC)load("glCopyTexImage1D", userptr);
    glad_glDrawArrays = (PFNGLDRAWARRAYSPROC)load("glDrawArrays", userptr);
    glad_glBindTexture = (PFNGLBINDTEXTUREPROC)load("glBindTexture", userptr);
    glad_glDeleteTextures = (PFNGLDELETETEXTURESPROC)load("glDeleteTextures", userptr);
    glad_glPolygonOffset = (PFNGLPOLYGONOFFSETPROC)load("glPolygonOffset", userptr);
    glad_glGenTextures = (PFNGLGENTEXTURESPROC)load("glGenTextures", userptr);
    glad_glCopyTexImage2D = (PFNGLCOPYTEXIMAGE2DPROC)load("glCopyTexImage2D", userptr);
    glad_glCopyTexSubImage2D = (PFNGLCOPYTEXSUBIMAGE2DPROC)load("glCopyTexSubImage2D", userptr);
    glad_glTexSubImage1D = (PFNGLTEXSUBIMAGE1DPROC)load("glTexSubImage1D", userptr);
    glad_glTexSubImage2D = (PFNGLTEXSUBIMAGE2DPROC)load("glTexSubImage2D", userptr);
    glad_glDrawElements = (PFNGLDRAWELEMENTSPROC)load("glDrawElements", userptr);
    glad_glCopyTexSubImage1D = (PFNGLCOPYTEXSUBIMAGE1DPROC)load("glCopyTexSubImage1D", userptr);
    glad_glIsTexture = (PFNGLISTEXTUREPROC)load("glIsTexture", userptr);
}
static void load_GL_VERSION_1_2(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_VERSION_1_2) return;
    glad_glTexImage3D = (PFNGLTEXIMAGE3DPROC)load("glTexImage3D", userptr);
    glad_glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC)load("glTexSubImage3D", userptr);
    glad_glCopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3DPROC)load("glCopyTexSubImage3D", userptr);
    glad_glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC)load("glDrawRangeElements", userptr);
}
static void load_GL_VERSION_1_3(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_VERSION_1_3) return;
    glad_glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)load("glCompressedTexSubImage3D", userptr);
    glad_glSampleCoverage = (PFNGLSAMPLECOVERAGEPROC)load("glSampleCoverage", userptr);
    glad_glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC)load("glGetCompressedTexImage", userptr);
    glad_glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC)load("glCompressedTexImage1D", userptr);
    glad_glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)load("glCompressedTexImage2D", userptr);
    glad_glActiveTexture = (PFNGLACTIVETEXTUREPROC)load("glActiveTexture", userptr);
    glad_glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC)load("glCompressedTexImage3D", userptr);
    glad_glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)load("glCompressedTexSubImage1D", userptr);
    glad_glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)load("glCompressedTexSubImage2D", userptr);
}
static void load_GL_VERSION_1_4(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_VERSION_1_4) return;
    glad_glPointParameteri = (PFNGLPOINTPARAMETERIPROC)load("glPointParameteri", userptr);
    glad_glBlendColor = (PFNGLBLENDCOLORPROC)load("glBlendColor", userptr);
    glad_glPointParameterf = (PFNGLPOINTPARAMETERFPROC)load("glPointParameterf", userptr);
    glad_glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC)load("glPointParameterfv", userptr);
    glad_glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC)load("glBlendFuncSeparate", userptr);
    glad_glMultiDrawElements = (PFNGLMULTIDRAWELEMENTSPROC)load("glMultiDrawElements", userptr);
    glad_glBlendEquation = (PFNGLBLENDEQUATIONPROC)load("glBlendEquation", userptr);
    glad_glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC)load("glPointParameteriv", userptr);
    glad_glMultiDrawArrays = (PFNGLMULTIDRAWARRAYSPROC)load("glMultiDrawArrays", userptr);
}
static void load_GL_VERSION_1_5(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_VERSION_1_5) return;
    glad_glMapBuffer = (PFNGLMAPBUFFERPROC)load("glMapBuffer", userptr);
    glad_glGetQueryiv = (PFNGLGETQUERYIVPROC)load("glGetQueryiv", userptr);
    glad_glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC)load("glGetBufferParameteriv", userptr);
    glad_glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)load("glUnmapBuffer", userptr);
    glad_glGetBufferSubData = (PFNGLGETBUFFERSUBDATAPROC)load("glGetBufferSubData", userptr);
    glad_glEndQuery = (PFNGLENDQUERYPROC)load("glEndQuery", userptr);
    glad_glGenBuffers = (PFNGLGENBUFFERSPROC)load("glGenBuffers", userptr);
    glad_glBindBuffer = (PFNGLBINDBUFFERPROC)load("glBindBuffer", userptr);
    glad_glGenQueries = (PFNGLGENQUERIESPROC)load("glGenQueries", userptr);
    glad_glBeginQuery = (PFNGLBEGINQUERYPROC)load("glBeginQuery", userptr);
    glad_glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)load("glGetQueryObjectuiv", userptr);
    glad_glBufferData = (PFNGLBUFFERDATAPROC)load("glBufferData", userptr);
    glad_glDeleteQueries = (PFNGLDELETEQUERIESPROC)load("glDeleteQueries", userptr);
    glad_glGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC)load("glGetBufferPointerv", userptr);
    glad_glBufferSubData = (PFNGLBUFFERSUBDATAPROC)load("glBufferSubData", userptr);
    glad_glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)load("glGetQueryObjectiv", userptr);
    glad_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)load("glDeleteBuffers", userptr);
    glad_glIsBuffer = (PFNGLISBUFFERPROC)load("glIsBuffer", userptr);
    glad_glIsQuery = (PFNGLISQUERYPROC)load("glIsQuery", userptr);
}
static void load_GL_VERSION_2_0(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_VERSION_2_0) return;
    glad_glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC)load("glVertexAttrib4Niv", userptr);
    glad_glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC)load("glStencilMaskSeparate", userptr);
    glad_glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC)load("glGetVertexAttribPointerv", userptr);
    glad_glLinkProgram = (PFNGLLINKPROGRAMPROC)load("glLinkProgram", userptr);
    glad_glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC)load("glVertexAttrib4bv", userptr);
    glad_glDeleteProgram = (PFNGLDELETEPROGRAMPROC)load("glDeleteProgram", userptr);
    glad_glCompileShader = (PFNGLCOMPILESHADERPROC)load("glCompileShader", userptr);
    glad_glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC)load("glStencilOpSeparate", userptr);
    glad_glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)load("glUniformMatrix3fv", userptr);
    glad_glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC)load("glStencilFuncSeparate", userptr);
    glad_glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC)load("glVertexAttrib4f", userptr);
    glad_glUniform4iv = (PFNGLUNIFORM4IVPROC)load("glUniform4iv", userptr);
    glad_glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC)load("glVertexAttrib4s", userptr);
    glad_glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC)load("glVertexAttrib4Nusv", userptr);
    glad_glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC)load("glGetVertexAttribfv", userptr);
    glad_glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)load("glValidateProgram", userptr);
    glad_glCreateShader = (PFNGLCREATESHADERPROC)load("glCreateShader", userptr);
    glad_glUniform1i = (PFNGLUNIFORM1IPROC)load("glUniform1i", userptr);
    glad_glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC)load("glVertexAttrib3s", userptr);
    glad_glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC)load("glGetActiveAttrib", userptr);
    glad_glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC)load("glVertexAttrib3dv", userptr);
    glad_glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC)load("glVertexAttrib1d", userptr);
    glad_glGetUniformiv = (PFNGLGETUNIFORMIVPROC)load("glGetUniformiv", userptr);
    glad_glUniform2i = (PFNGLUNIFORM2IPROC)load("glUniform2i", userptr);
    glad_glUniform2f = (PFNGLUNIFORM2FPROC)load("glUniform2f", userptr);
    glad_glGetProgramiv = (PFNGLGETPROGRAMIVPROC)load("glGetProgramiv", userptr);
    glad_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)load("glVertexAttribPointer", userptr);
    glad_glDrawBuffers = (PFNGLDRAWBUFFERSPROC)load("glDrawBuffers", userptr);
    glad_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)load("glGetUniformLocation", userptr);
    glad_glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC)load("glVertexAttrib2dv", userptr);
    glad_glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC)load("glVertexAttrib4Nuiv", userptr);
    glad_glAttachShader = (PFNGLATTACHSHADERPROC)load("glAttachShader", userptr);
    glad_glUniform3i = (PFNGLUNIFORM3IPROC)load("glUniform3i", userptr);
    glad_glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC)load("glVertexAttrib4Nbv", userptr);
    glad_glUniform3f = (PFNGLUNIFORM3FPROC)load("glUniform3f", userptr);
    glad_glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC)load("glVertexAttrib4ubv", userptr);
    glad_glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC)load("glVertexAttrib1dv", userptr);
    glad_glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC)load("glGetVertexAttribdv", userptr);
    glad_glUniform2iv = (PFNGLUNIFORM2IVPROC)load("glUniform2iv", userptr);
    glad_glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)load("glVertexAttrib1fv", userptr);
    glad_glUniform1iv = (PFNGLUNIFORM1IVPROC)load("glUniform1iv", userptr);
    glad_glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)load("glBindAttribLocation", userptr);
    glad_glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC)load("glVertexAttrib3sv", userptr);
    glad_glDetachShader = (PFNGLDETACHSHADERPROC)load("glDetachShader", userptr);
    glad_glGetShaderiv = (PFNGLGETSHADERIVPROC)load("glGetShaderiv", userptr);
    glad_glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC)load("glVertexAttrib3f", userptr);
    glad_glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC)load("glVertexAttrib4uiv", userptr);
    glad_glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC)load("glVertexAttrib3d", userptr);
    glad_glUseProgram = (PFNGLUSEPROGRAMPROC)load("glUseProgram", userptr);
    glad_glShaderSource = (PFNGLSHADERSOURCEPROC)load("glShaderSource", userptr);
    glad_glIsProgram = (PFNGLISPROGRAMPROC)load("glIsProgram", userptr);
    glad_glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)load("glVertexAttrib1f", userptr);
    glad_glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)load("glVertexAttrib4fv", userptr);
    glad_glUniform4i = (PFNGLUNIFORM4IPROC)load("glUniform4i", userptr);
    glad_glUniform2fv = (PFNGLUNIFORM2FVPROC)load("glUniform2fv", userptr);
    glad_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)load("glEnableVertexAttribArray", userptr);
    glad_glUniform3fv = (PFNGLUNIFORM3FVPROC)load("glUniform3fv", userptr);
    glad_glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)load("glVertexAttrib2fv", userptr);
    glad_glUniform4f = (PFNGLUNIFORM4FPROC)load("glUniform4f", userptr);
    glad_glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC)load("glGetVertexAttribiv", userptr);
    glad_glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)load("glVertexAttrib3fv", userptr);
    glad_glUniform3iv = (PFNGLUNIFORM3IVPROC)load("glUniform3iv", userptr);
    glad_glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC)load("glVertexAttrib4iv", userptr);
    glad_glUniform4fv = (PFNGLUNIFORM4FVPROC)load("glUniform4fv", userptr);
    glad_glUniform1f = (PFNGLUNIFORM1FPROC)load("glUniform1f", userptr);
    glad_glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC)load("glGetAttachedShaders", userptr);
    glad_glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC)load("glVertexAttrib1s", userptr);
    glad_glUniform1fv = (PFNGLUNIFORM1FVPROC)load("glUniform1fv", userptr);
    glad_glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC)load("glVertexAttrib4Nubv", userptr);
    glad_glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC)load("glUniformMatrix2fv", userptr);
    glad_glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)load("glDisableVertexAttribArray", userptr);
    glad_glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC)load("glVertexAttrib4sv", userptr);
    glad_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)load("glGetProgramInfoLog", userptr);
    glad_glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC)load("glVertexAttrib4Nub", userptr);
    glad_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)load("glGetShaderInfoLog", userptr);
    glad_glIsShader = (PFNGLISSHADERPROC)load("glIsShader", userptr);
    glad_glGetShaderSource = (PFNGLGETSHADERSOURCEPROC)load("glGetShaderSource", userptr);
    glad_glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC)load("glBlendEquationSeparate", userptr);
    glad_glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC)load("glVertexAttrib1sv", userptr);
    glad_glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)load("glGetAttribLocation", userptr);
    glad_glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC)load("glVertexAttrib4dv", userptr);
    glad_glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC)load("glVertexAttrib2sv", userptr);
    glad_glDeleteShader = (PFNGLDELETESHADERPROC)load("glDeleteShader", userptr);
    glad_glCreateProgram = (PFNGLCREATEPROGRAMPROC)load("glCreateProgram", userptr);
    glad_glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC)load("glVertexAttrib4Nsv", userptr);
    glad_glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC)load("glVertexAttrib2d", userptr);
    glad_glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC)load("glVertexAttrib2f", userptr);
    glad_glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC)load("glVertexAttrib4usv", userptr);
    glad_glGetUniformfv = (PFNGLGETUNIFORMFVPROC)load("glGetUniformfv", userptr);
    glad_glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC)load("glVertexAttrib2s", userptr);
    glad_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)load("glUniformMatrix4fv", userptr);
    glad_glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)load("glGetActiveUniform", userptr);
    glad_glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC)load("glVertexAttrib4d", userptr);
}
static void load_GL_VERSION_2_1(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_VERSION_2_1) return;
    glad_glUniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FVPROC)load("glUniformMatrix3x2fv", userptr);
    glad_glUniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FVPROC)load("glUniformMatrix3x4fv", userptr);
    glad_glUniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FVPROC)load("glUniformMatrix2x4fv", userptr);
    glad_glUniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FVPROC)load("glUniformMatrix4x2fv", userptr);
    glad_glUniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC)load("glUniformMatrix2x3fv", userptr);
    glad_glUniformMatrix4x3fv = (PFNGLUNIFORMMATRIX4X3FVPROC)load("glUniformMatrix4x3fv", userptr);
}
static void load_GL_VERSION_3_0(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_VERSION_3_0) return;
    glad_glVertexAttribI3ui = (PFNGLVERTEXATTRIBI3UIPROC)load("glVertexAttribI3ui", userptr);
    glad_glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)load("glFramebufferRenderbuffer", userptr);
    glad_glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC)load("glGetIntegeri_v", userptr);
    glad_glVertexAttribI2ui = (PFNGLVERTEXATTRIBI2UIPROC)load("glVertexAttribI2ui", userptr);
    glad_glGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC)load("glGetTransformFeedbackVarying", userptr);
    glad_glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)load("glVertexAttribIPointer", userptr);
    glad_glBeginConditionalRender = (PFNGLBEGINCONDITIONALRENDERPROC)load("glBeginConditionalRender", userptr);
    glad_glEnablei = (PFNGLENABLEIPROC)load("glEnablei", userptr);
    glad_glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)load("glGenRenderbuffers", userptr);
    glad_glUniform4ui = (PFNGLUNIFORM4UIPROC)load("glUniform4ui", userptr);
    glad_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)load("glBindFramebuffer", userptr);
    glad_glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)load("glRenderbufferStorage", userptr);
    glad_glClearBufferfv = (PFNGLCLEARBUFFERFVPROC)load("glClearBufferfv", userptr);
    glad_glColorMaski = (PFNGLCOLORMASKIPROC)load("glColorMaski", userptr);
    glad_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)load("glGenVertexArrays", userptr);
    glad_glVertexAttribI4sv = (PFNGLVERTEXATTRIBI4SVPROC)load("glVertexAttribI4sv", userptr);
    glad_glTexParameterIuiv = (PFNGLTEXPARAMETERIUIVPROC)load("glTexParameterIuiv", userptr);
    glad_glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)load("glGenerateMipmap", userptr);
    glad_glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)load("glBindRenderbuffer", userptr);
    glad_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)load("glDeleteFramebuffers", userptr);
    glad_glUniform1ui = (PFNGLUNIFORM1UIPROC)load("glUniform1ui", userptr);
    glad_glVertexAttribI2i = (PFNGLVERTEXATTRIBI2IPROC)load("glVertexAttribI2i", userptr);
    glad_glClearBufferfi = (PFNGLCLEARBUFFERFIPROC)load("glClearBufferfi", userptr);
    glad_glVertexAttribI3iv = (PFNGLVERTEXATTRIBI3IVPROC)load("glVertexAttribI3iv", userptr);
    glad_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)load("glFramebufferTexture2D", userptr);
    glad_glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)load("glGetFramebufferAttachmentParameteriv", userptr);
    glad_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)load("glGenFramebuffers", userptr);
    glad_glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)load("glIsRenderbuffer", userptr);
    glad_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)load("glDeleteVertexArrays", userptr);
    glad_glUniform4uiv = (PFNGLUNIFORM4UIVPROC)load("glUniform4uiv", userptr);
    glad_glIsVertexArray = (PFNGLISVERTEXARRAYPROC)load("glIsVertexArray", userptr);
    glad_glVertexAttribI4iv = (PFNGLVERTEXATTRIBI4IVPROC)load("glVertexAttribI4iv", userptr);
    glad_glVertexAttribI4ubv = (PFNGLVERTEXATTRIBI4UBVPROC)load("glVertexAttribI4ubv", userptr);
    glad_glVertexAttribI4i = (PFNGLVERTEXATTRIBI4IPROC)load("glVertexAttribI4i", userptr);
    glad_glVertexAttribI1ui = (PFNGLVERTEXATTRIBI1UIPROC)load("glVertexAttribI1ui", userptr);
    glad_glGetUniformuiv = (PFNGLGETUNIFORMUIVPROC)load("glGetUniformuiv", userptr);
    glad_glGetStringi = (PFNGLGETSTRINGIPROC)load("glGetStringi", userptr);
    glad_glGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC)load("glGetVertexAttribIiv", userptr);
    glad_glClearBufferuiv = (PFNGLCLEARBUFFERUIVPROC)load("glClearBufferuiv", userptr);
    glad_glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)load("glGetRenderbufferParameteriv", userptr);
    glad_glClearBufferiv = (PFNGLCLEARBUFFERIVPROC)load("glClearBufferiv", userptr);
    glad_glVertexAttribI2uiv = (PFNGLVERTEXATTRIBI2UIVPROC)load("glVertexAttribI2uiv", userptr);
    glad_glUniform3ui = (PFNGLUNIFORM3UIPROC)load("glUniform3ui", userptr);
    glad_glVertexAttribI4uiv = (PFNGLVERTEXATTRIBI4UIVPROC)load("glVertexAttribI4uiv", userptr);
    glad_glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)load("glBindBufferRange", userptr);
    glad_glClampColor = (PFNGLCLAMPCOLORPROC)load("glClampColor", userptr);
    glad_glGetTexParameterIuiv = (PFNGLGETTEXPARAMETERIUIVPROC)load("glGetTexParameterIuiv", userptr);
    glad_glVertexAttribI4bv = (PFNGLVERTEXATTRIBI4BVPROC)load("glVertexAttribI4bv", userptr);
    glad_glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)load("glDeleteRenderbuffers", userptr);
    glad_glVertexAttribI4ui = (PFNGLVERTEXATTRIBI4UIPROC)load("glVertexAttribI4ui", userptr);
    glad_glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)load("glFramebufferTextureLayer", userptr);
    glad_glTexParameterIiv = (PFNGLTEXPARAMETERIIVPROC)load("glTexParameterIiv", userptr);
    glad_glEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC)load("glEndTransformFeedback", userptr);
    glad_glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC)load("glFlushMappedBufferRange", userptr);
    glad_glBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC)load("glBeginTransformFeedback", userptr);
    glad_glVertexAttribI1iv = (PFNGLVERTEXATTRIBI1IVPROC)load("glVertexAttribI1iv", userptr);
    glad_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)load("glCheckFramebufferStatus", userptr);
    glad_glEndConditionalRender = (PFNGLENDCONDITIONALRENDERPROC)load("glEndConditionalRender", userptr);
    glad_glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)load("glBlitFramebuffer", userptr);
    glad_glIsEnabledi = (PFNGLISENABLEDIPROC)load("glIsEnabledi", userptr);
    glad_glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)load("glBindBufferBase", userptr);
    glad_glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC)load("glMapBufferRange", userptr);
    glad_glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)load("glFramebufferTexture1D", userptr);
    glad_glVertexAttribI2iv = (PFNGLVERTEXATTRIBI2IVPROC)load("glVertexAttribI2iv", userptr);
    glad_glBindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONPROC)load("glBindFragDataLocation", userptr);
    glad_glDisablei = (PFNGLDISABLEIPROC)load("glDisablei", userptr);
    glad_glVertexAttribI3i = (PFNGLVERTEXATTRIBI3IPROC)load("glVertexAttribI3i", userptr);
    glad_glVertexAttribI3uiv = (PFNGLVERTEXATTRIBI3UIVPROC)load("glVertexAttribI3uiv", userptr);
    glad_glGetTexParameterIiv = (PFNGLGETTEXPARAMETERIIVPROC)load("glGetTexParameterIiv", userptr);
    glad_glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)load("glRenderbufferStorageMultisample", userptr);
    glad_glVertexAttribI4usv = (PFNGLVERTEXATTRIBI4USVPROC)load("glVertexAttribI4usv", userptr);
    glad_glVertexAttribI1i = (PFNGLVERTEXATTRIBI1IPROC)load("glVertexAttribI1i", userptr);
    glad_glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)load("glFramebufferTexture3D", userptr);
    glad_glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)load("glIsFramebuffer", userptr);
    glad_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)load("glBindVertexArray", userptr);
    glad_glGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC)load("glGetVertexAttribIuiv", userptr);
    glad_glGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC)load("glGetFragDataLocation", userptr);
    glad_glUniform2uiv = (PFNGLUNIFORM2UIVPROC)load("glUniform2uiv", userptr);
    glad_glVertexAttribI1uiv = (PFNGLVERTEXATTRIBI1UIVPROC)load("glVertexAttribI1uiv", userptr);
    glad_glUniform1uiv = (PFNGLUNIFORM1UIVPROC)load("glUniform1uiv", userptr);
    glad_glTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC)load("glTransformFeedbackVaryings", userptr);
    glad_glUniform2ui = (PFNGLUNIFORM2UIPROC)load("glUniform2ui", userptr);
    glad_glUniform3uiv = (PFNGLUNIFORM3UIVPROC)load("glUniform3uiv", userptr);
    glad_glGetBooleani_v = (PFNGLGETBOOLEANI_VPROC)load("glGetBooleani_v", userptr);
}
static void load_GL_VERSION_3_1(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_VERSION_3_1) return;
    glad_glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)load("glBindBufferRange", userptr);
    glad_glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC)load("glUniformBlockBinding", userptr);
    glad_glGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC)load("glGetActiveUniformsiv", userptr);
    glad_glGetActiveUniformName = (PFNGLGETACTIVEUNIFORMNAMEPROC)load("glGetActiveUniformName", userptr);
    glad_glCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC)load("glCopyBufferSubData", userptr);
    glad_glGetUniformIndices = (PFNGLGETUNIFORMINDICESPROC)load("glGetUniformIndices", userptr);
    glad_glGetActiveUniformBlockName = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC)load("glGetActiveUniformBlockName", userptr);
    glad_glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC)load("glGetUniformBlockIndex", userptr);
    glad_glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)load("glBindBufferBase", userptr);
    glad_glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC)load("glGetIntegeri_v", userptr);
    glad_glGetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC)load("glGetActiveUniformBlockiv", userptr);
    glad_glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC)load("glDrawElementsInstanced", userptr);
    glad_glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC)load("glDrawArraysInstanced", userptr);
    glad_glTexBuffer = (PFNGLTEXBUFFERPROC)load("glTexBuffer", userptr);
    glad_glPrimitiveRestartIndex = (PFNGLPRIMITIVERESTARTINDEXPROC)load("glPrimitiveRestartIndex", userptr);
}
static void load_GL_VERSION_3_2(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_VERSION_3_2) return;
    glad_glGetBufferParameteri64v = (PFNGLGETBUFFERPARAMETERI64VPROC)load("glGetBufferParameteri64v", userptr);
    glad_glFenceSync = (PFNGLFENCESYNCPROC)load("glFenceSync", userptr);
    glad_glGetSynciv = (PFNGLGETSYNCIVPROC)load("glGetSynciv", userptr);
    glad_glDeleteSync = (PFNGLDELETESYNCPROC)load("glDeleteSync", userptr);
    glad_glDrawElementsInstancedBaseVertex = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC)load("glDrawElementsInstancedBaseVertex", userptr);
    glad_glDrawElementsBaseVertex = (PFNGLDRAWELEMENTSBASEVERTEXPROC)load("glDrawElementsBaseVertex", userptr);
    glad_glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC)load("glGetMultisamplefv", userptr);
    glad_glIsSync = (PFNGLISSYNCPROC)load("glIsSync", userptr);
    glad_glClientWaitSync = (PFNGLCLIENTWAITSYNCPROC)load("glClientWaitSync", userptr);
    glad_glSampleMaski = (PFNGLSAMPLEMASKIPROC)load("glSampleMaski", userptr);
    glad_glMultiDrawElementsBaseVertex = (PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC)load("glMultiDrawElementsBaseVertex", userptr);
    glad_glDrawRangeElementsBaseVertex = (PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC)load("glDrawRangeElementsBaseVertex", userptr);
    glad_glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)load("glTexImage2DMultisample", userptr);
    glad_glWaitSync = (PFNGLWAITSYNCPROC)load("glWaitSync", userptr);
    glad_glGetInteger64i_v = (PFNGLGETINTEGER64I_VPROC)load("glGetInteger64i_v", userptr);
    glad_glProvokingVertex = (PFNGLPROVOKINGVERTEXPROC)load("glProvokingVertex", userptr);
    glad_glTexImage3DMultisample = (PFNGLTEXIMAGE3DMULTISAMPLEPROC)load("glTexImage3DMultisample", userptr);
    glad_glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)load("glFramebufferTexture", userptr);
    glad_glGetInteger64v = (PFNGLGETINTEGER64VPROC)load("glGetInteger64v", userptr);
}
static void load_GL_VERSION_3_3(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_VERSION_3_3) return;
    glad_glSamplerParameterIuiv = (PFNGLSAMPLERPARAMETERIUIVPROC)load("glSamplerParameterIuiv", userptr);
    glad_glGetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFVPROC)load("glGetSamplerParameterfv", userptr);
    glad_glSamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC)load("glSamplerParameterfv", userptr);
    glad_glVertexAttribP1uiv = (PFNGLVERTEXATTRIBP1UIVPROC)load("glVertexAttribP1uiv", userptr);
    glad_glVertexAttribP4uiv = (PFNGLVERTEXATTRIBP4UIVPROC)load("glVertexAttribP4uiv", userptr);
    glad_glVertexAttribP2uiv = (PFNGLVERTEXATTRIBP2UIVPROC)load("glVertexAttribP2uiv", userptr);
    glad_glBindSampler = (PFNGLBINDSAMPLERPROC)load("glBindSampler", userptr);
    glad_glIsSampler = (PFNGLISSAMPLERPROC)load("glIsSampler", userptr);
    glad_glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)load("glVertexAttribDivisor", userptr);
    glad_glVertexAttribP2ui = (PFNGLVERTEXATTRIBP2UIPROC)load("glVertexAttribP2ui", userptr);
    glad_glVertexAttribP1ui = (PFNGLVERTEXATTRIBP1UIPROC)load("glVertexAttribP1ui", userptr);
    glad_glQueryCounter = (PFNGLQUERYCOUNTERPROC)load("glQueryCounter", userptr);
    glad_glGetSamplerParameterIuiv = (PFNGLGETSAMPLERPARAMETERIUIVPROC)load("glGetSamplerParameterIuiv", userptr);
    glad_glGenSamplers = (PFNGLGENSAMPLERSPROC)load("glGenSamplers", userptr);
    glad_glGetQueryObjectui64v = (PFNGLGETQUERYOBJECTUI64VPROC)load("glGetQueryObjectui64v", userptr);
    glad_glBindFragDataLocationIndexed = (PFNGLBINDFRAGDATALOCATIONINDEXEDPROC)load("glBindFragDataLocationIndexed", userptr);
    glad_glSamplerParameterIiv = (PFNGLSAMPLERPARAMETERIIVPROC)load("glSamplerParameterIiv", userptr);
    glad_glGetQueryObjecti64v = (PFNGLGETQUERYOBJECTI64VPROC)load("glGetQueryObjecti64v", userptr);
    glad_glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC)load("glSamplerParameteri", userptr);
    glad_glDeleteSamplers = (PFNGLDELETESAMPLERSPROC)load("glDeleteSamplers", userptr);
    glad_glVertexAttribP3uiv = (PFNGLVERTEXATTRIBP3UIVPROC)load("glVertexAttribP3uiv", userptr);
    glad_glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC)load("glSamplerParameterf", userptr);
    glad_glGetSamplerParameterIiv = (PFNGLGETSAMPLERPARAMETERIIVPROC)load("glGetSamplerParameterIiv", userptr);
    glad_glGetFragDataIndex = (PFNGLGETFRAGDATAINDEXPROC)load("glGetFragDataIndex", userptr);
    glad_glVertexAttribP3ui = (PFNGLVERTEXATTRIBP3UIPROC)load("glVertexAttribP3ui", userptr);
    glad_glVertexAttribP4ui = (PFNGLVERTEXATTRIBP4UIPROC)load("glVertexAttribP4ui", userptr);
    glad_glGetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIVPROC)load("glGetSamplerParameteriv", userptr);
    glad_glSamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC)load("glSamplerParameteriv", userptr);
}
static void load_GL_VERSION_4_0(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_VERSION_4_0) return;
    glad_glResumeTransformFeedback = (PFNGLRESUMETRANSFORMFEEDBACKPROC)load("glResumeTransformFeedback", userptr);
    glad_glEndQueryIndexed = (PFNGLENDQUERYINDEXEDPROC)load("glEndQueryIndexed", userptr);
    glad_glUniformMatrix3x4dv = (PFNGLUNIFORMMATRIX3X4DVPROC)load("glUniformMatrix3x4dv", userptr);
    glad_glUniform1dv = (PFNGLUNIFORM1DVPROC)load("glUniform1dv", userptr);
    glad_glGetProgramStageiv = (PFNGLGETPROGRAMSTAGEIVPROC)load("glGetProgramStageiv", userptr);
    glad_glUniformSubroutinesuiv = (PFNGLUNIFORMSUBROUTINESUIVPROC)load("glUniformSubroutinesuiv", userptr);
    glad_glUniformMatrix3x2dv = (PFNGLUNIFORMMATRIX3X2DVPROC)load("glUniformMatrix3x2dv", userptr);
    glad_glIsTransformFeedback = (PFNGLISTRANSFORMFEEDBACKPROC)load("glIsTransformFeedback", userptr);
    glad_glGetActiveSubroutineUniformName = (PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC)load("glGetActiveSubroutineUniformName", userptr);
    glad_glUniformMatrix4x3dv = (PFNGLUNIFORMMATRIX4X3DVPROC)load("glUniformMatrix4x3dv", userptr);
    glad_glUniform4d = (PFNGLUNIFORM4DPROC)load("glUniform4d", userptr);
    glad_glUniform2d = (PFNGLUNIFORM2DPROC)load("glUniform2d", userptr);
    glad_glDrawElementsIndirect = (PFNGLDRAWELEMENTSINDIRECTPROC)load("glDrawElementsIndirect", userptr);
    glad_glGetUniformdv = (PFNGLGETUNIFORMDVPROC)load("glGetUniformdv", userptr);
    glad_glPauseTransformFeedback = (PFNGLPAUSETRANSFORMFEEDBACKPROC)load("glPauseTransformFeedback", userptr);
    glad_glUniformMatrix4dv = (PFNGLUNIFORMMATRIX4DVPROC)load("glUniformMatrix4dv", userptr);
    glad_glUniformMatrix4x2dv = (PFNGLUNIFORMMATRIX4X2DVPROC)load("glUniformMatrix4x2dv", userptr);
    glad_glGetQueryIndexediv = (PFNGLGETQUERYINDEXEDIVPROC)load("glGetQueryIndexediv", userptr);
    glad_glDrawTransformFeedbackStream = (PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC)load("glDrawTransformFeedbackStream", userptr);
    glad_glGenTransformFeedbacks = (PFNGLGENTRANSFORMFEEDBACKSPROC)load("glGenTransformFeedbacks", userptr);
    glad_glGetActiveSubroutineName = (PFNGLGETACTIVESUBROUTINENAMEPROC)load("glGetActiveSubroutineName", userptr);
    glad_glDrawTransformFeedback = (PFNGLDRAWTRANSFORMFEEDBACKPROC)load("glDrawTransformFeedback", userptr);
    glad_glUniform1d = (PFNGLUNIFORM1DPROC)load("glUniform1d", userptr);
    glad_glBlendFunci = (PFNGLBLENDFUNCIPROC)load("glBlendFunci", userptr);
    glad_glBeginQueryIndexed = (PFNGLBEGINQUERYINDEXEDPROC)load("glBeginQueryIndexed", userptr);
    glad_glBindTransformFeedback = (PFNGLBINDTRANSFORMFEEDBACKPROC)load("glBindTransformFeedback", userptr);
    glad_glMinSampleShading = (PFNGLMINSAMPLESHADINGPROC)load("glMinSampleShading", userptr);
    glad_glGetSubroutineUniformLocation = (PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC)load("glGetSubroutineUniformLocation", userptr);
    glad_glUniformMatrix2x4dv = (PFNGLUNIFORMMATRIX2X4DVPROC)load("glUniformMatrix2x4dv", userptr);
    glad_glUniformMatrix2x3dv = (PFNGLUNIFORMMATRIX2X3DVPROC)load("glUniformMatrix2x3dv", userptr);
    glad_glUniform3dv = (PFNGLUNIFORM3DVPROC)load("glUniform3dv", userptr);
    glad_glBlendEquationi = (PFNGLBLENDEQUATIONIPROC)load("glBlendEquationi", userptr);
    glad_glGetSubroutineIndex = (PFNGLGETSUBROUTINEINDEXPROC)load("glGetSubroutineIndex", userptr);
    glad_glUniform2dv = (PFNGLUNIFORM2DVPROC)load("glUniform2dv", userptr);
    glad_glUniform4dv = (PFNGLUNIFORM4DVPROC)load("glUniform4dv", userptr);
    glad_glDeleteTransformFeedbacks = (PFNGLDELETETRANSFORMFEEDBACKSPROC)load("glDeleteTransformFeedbacks", userptr);
    glad_glBlendEquationSeparatei = (PFNGLBLENDEQUATIONSEPARATEIPROC)load("glBlendEquationSeparatei", userptr);
    glad_glUniform3d = (PFNGLUNIFORM3DPROC)load("glUniform3d", userptr);
    glad_glUniformMatrix2dv = (PFNGLUNIFORMMATRIX2DVPROC)load("glUniformMatrix2dv", userptr);
    glad_glGetUniformSubroutineuiv = (PFNGLGETUNIFORMSUBROUTINEUIVPROC)load("glGetUniformSubroutineuiv", userptr);
    glad_glPatchParameterfv = (PFNGLPATCHPARAMETERFVPROC)load("glPatchParameterfv", userptr);
    glad_glDrawArraysIndirect = (PFNGLDRAWARRAYSINDIRECTPROC)load("glDrawArraysIndirect", userptr);
    glad_glGetActiveSubroutineUniformiv = (PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC)load("glGetActiveSubroutineUniformiv", userptr);
    glad_glBlendFuncSeparatei = (PFNGLBLENDFUNCSEPARATEIPROC)load("glBlendFuncSeparatei", userptr);
    glad_glPatchParameteri = (PFNGLPATCHPARAMETERIPROC)load("glPatchParameteri", userptr);
    glad_glUniformMatrix3dv = (PFNGLUNIFORMMATRIX3DVPROC)load("glUniformMatrix3dv", userptr);
}
static void load_GL_VERSION_4_1(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_VERSION_4_1) return;
    glad_glProgramUniformMatrix3x4dv = (PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC)load("glProgramUniformMatrix3x4dv", userptr);
    glad_glGenProgramPipelines = (PFNGLGENPROGRAMPIPELINESPROC)load("glGenProgramPipelines", userptr);
    glad_glProgramUniform3iv = (PFNGLPROGRAMUNIFORM3IVPROC)load("glProgramUniform3iv", userptr);
    glad_glGetShaderPrecisionFormat = (PFNGLGETSHADERPRECISIONFORMATPROC)load("glGetShaderPrecisionFormat", userptr);
    glad_glScissorArrayv = (PFNGLSCISSORARRAYVPROC)load("glScissorArrayv", userptr);
    glad_glProgramUniform4dv = (PFNGLPROGRAMUNIFORM4DVPROC)load("glProgramUniform4dv", userptr);
    glad_glProgramUniformMatrix2x4fv = (PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC)load("glProgramUniformMatrix2x4fv", userptr);
    glad_glProgramUniform1iv = (PFNGLPROGRAMUNIFORM1IVPROC)load("glProgramUniform1iv", userptr);
    glad_glProgramUniformMatrix2x3dv = (PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC)load("glProgramUniformMatrix2x3dv", userptr);
    glad_glProgramUniformMatrix4fv = (PFNGLPROGRAMUNIFORMMATRIX4FVPROC)load("glProgramUniformMatrix4fv", userptr);
    glad_glGetProgramPipelineiv = (PFNGLGETPROGRAMPIPELINEIVPROC)load("glGetProgramPipelineiv", userptr);
    glad_glProgramUniform3dv = (PFNGLPROGRAMUNIFORM3DVPROC)load("glProgramUniform3dv", userptr);
    glad_glProgramUniform2dv = (PFNGLPROGRAMUNIFORM2DVPROC)load("glProgramUniform2dv", userptr);
    glad_glValidateProgramPipeline = (PFNGLVALIDATEPROGRAMPIPELINEPROC)load("glValidateProgramPipeline", userptr);
    glad_glReleaseShaderCompiler = (PFNGLRELEASESHADERCOMPILERPROC)load("glReleaseShaderCompiler", userptr);
    glad_glProgramUniform2i = (PFNGLPROGRAMUNIFORM2IPROC)load("glProgramUniform2i", userptr);
    glad_glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC)load("glProgramParameteri", userptr);
    glad_glIsProgramPipeline = (PFNGLISPROGRAMPIPELINEPROC)load("glIsProgramPipeline", userptr);
    glad_glProgramUniform2d = (PFNGLPROGRAMUNIFORM2DPROC)load("glProgramUniform2d", userptr);
    glad_glProgramUniform2f = (PFNGLPROGRAMUNIFORM2FPROC)load("glProgramUniform2f", userptr);
    glad_glGetProgramBinary = (PFNGLGETPROGRAMBINARYPROC)load("glGetProgramBinary", userptr);
    glad_glDepthRangeArrayv = (PFNGLDEPTHRANGEARRAYVPROC)load("glDepthRangeArrayv", userptr);
    glad_glDepthRangeIndexed = (PFNGLDEPTHRANGEINDEXEDPROC)load("glDepthRangeIndexed", userptr);
    glad_glVertexAttribLPointer = (PFNGLVERTEXATTRIBLPOINTERPROC)load("glVertexAttribLPointer", userptr);
    glad_glVertexAttribL3dv = (PFNGLVERTEXATTRIBL3DVPROC)load("glVertexAttribL3dv", userptr);
    glad_glViewportIndexedfv = (PFNGLVIEWPORTINDEXEDFVPROC)load("glViewportIndexedfv", userptr);
    glad_glScissorIndexed = (PFNGLSCISSORINDEXEDPROC)load("glScissorIndexed", userptr);
    glad_glDepthRangef = (PFNGLDEPTHRANGEFPROC)load("glDepthRangef", userptr);
    glad_glVertexAttribL4dv = (PFNGLVERTEXATTRIBL4DVPROC)load("glVertexAttribL4dv", userptr);
    glad_glVertexAttribL2dv = (PFNGLVERTEXATTRIBL2DVPROC)load("glVertexAttribL2dv", userptr);
    glad_glViewportArrayv = (PFNGLVIEWPORTARRAYVPROC)load("glViewportArrayv", userptr);
    glad_glProgramUniform1dv = (PFNGLPROGRAMUNIFORM1DVPROC)load("glProgramUniform1dv", userptr);
    glad_glProgramUniformMatrix3x2dv = (PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC)load("glProgramUniformMatrix3x2dv", userptr);
    glad_glProgramUniformMatrix4dv = (PFNGLPROGRAMUNIFORMMATRIX4DVPROC)load("glProgramUniformMatrix4dv", userptr);
    glad_glProgramUniformMatrix4x3dv = (PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC)load("glProgramUniformMatrix4x3dv", userptr);
    glad_glVertexAttribL2d = (PFNGLVERTEXATTRIBL2DPROC)load("glVertexAttribL2d", userptr);
    glad_glProgramUniformMatrix4x3fv = (PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC)load("glProgramUniformMatrix4x3fv", userptr);
    glad_glGetFloati_v = (PFNGLGETFLOATI_VPROC)load("glGetFloati_v", userptr);
    glad_glProgramUniform3i = (PFNGLPROGRAMUNIFORM3IPROC)load("glProgramUniform3i", userptr);
    glad_glUseProgramStages = (PFNGLUSEPROGRAMSTAGESPROC)load("glUseProgramStages", userptr);
    glad_glProgramUniformMatrix3dv = (PFNGLPROGRAMUNIFORMMATRIX3DVPROC)load("glProgramUniformMatrix3dv", userptr);
    glad_glProgramUniform3uiv = (PFNGLPROGRAMUNIFORM3UIVPROC)load("glProgramUniform3uiv", userptr);
    glad_glProgramUniform4iv = (PFNGLPROGRAMUNIFORM4IVPROC)load("glProgramUniform4iv", userptr);
    glad_glProgramUniformMatrix3x2fv = (PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC)load("glProgramUniformMatrix3x2fv", userptr);
    glad_glProgramUniform3d = (PFNGLPROGRAMUNIFORM3DPROC)load("glProgramUniform3d", userptr);
    glad_glProgramUniformMatrix2dv = (PFNGLPROGRAMUNIFORMMATRIX2DVPROC)load("glProgramUniformMatrix2dv", userptr);
    glad_glProgramUniform2ui = (PFNGLPROGRAMUNIFORM2UIPROC)load("glProgramUniform2ui", userptr);
    glad_glProgramUniform2iv = (PFNGLPROGRAMUNIFORM2IVPROC)load("glProgramUniform2iv", userptr);
    glad_glProgramUniformMatrix3x4fv = (PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC)load("glProgramUniformMatrix3x4fv", userptr);
    glad_glVertexAttribL1d = (PFNGLVERTEXATTRIBL1DPROC)load("glVertexAttribL1d", userptr);
    glad_glProgramUniform4uiv = (PFNGLPROGRAMUNIFORM4UIVPROC)load("glProgramUniform4uiv", userptr);
    glad_glActiveShaderProgram = (PFNGLACTIVESHADERPROGRAMPROC)load("glActiveShaderProgram", userptr);
    glad_glProgramUniform4fv = (PFNGLPROGRAMUNIFORM4FVPROC)load("glProgramUniform4fv", userptr);
    glad_glBindProgramPipeline = (PFNGLBINDPROGRAMPIPELINEPROC)load("glBindProgramPipeline", userptr);
    glad_glScissorIndexedv = (PFNGLSCISSORINDEXEDVPROC)load("glScissorIndexedv", userptr);
    glad_glGetVertexAttribLdv = (PFNGLGETVERTEXATTRIBLDVPROC)load("glGetVertexAttribLdv", userptr);
    glad_glProgramUniform2fv = (PFNGLPROGRAMUNIFORM2FVPROC)load("glProgramUniform2fv", userptr);
    glad_glProgramUniformMatrix2x3fv = (PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC)load("glProgramUniformMatrix2x3fv", userptr);
    glad_glProgramUniform4i = (PFNGLPROGRAMUNIFORM4IPROC)load("glProgramUniform4i", userptr);
    glad_glProgramUniform4f = (PFNGLPROGRAMUNIFORM4FPROC)load("glProgramUniform4f", userptr);
    glad_glViewportIndexedf = (PFNGLVIEWPORTINDEXEDFPROC)load("glViewportIndexedf", userptr);
    glad_glProgramUniform4d = (PFNGLPROGRAMUNIFORM4DPROC)load("glProgramUniform4d", userptr);
    glad_glProgramUniform3fv = (PFNGLPROGRAMUNIFORM3FVPROC)load("glProgramUniform3fv", userptr);
    glad_glGetProgramPipelineInfoLog = (PFNGLGETPROGRAMPIPELINEINFOLOGPROC)load("glGetProgramPipelineInfoLog", userptr);
    glad_glProgramUniformMatrix2x4dv = (PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC)load("glProgramUniformMatrix2x4dv", userptr);
    glad_glProgramBinary = (PFNGLPROGRAMBINARYPROC)load("glProgramBinary", userptr);
    glad_glProgramUniform4ui = (PFNGLPROGRAMUNIFORM4UIPROC)load("glProgramUniform4ui", userptr);
    glad_glProgramUniform1ui = (PFNGLPROGRAMUNIFORM1UIPROC)load("glProgramUniform1ui", userptr);
    glad_glGetDoublei_v = (PFNGLGETDOUBLEI_VPROC)load("glGetDoublei_v", userptr);
    glad_glShaderBinary = (PFNGLSHADERBINARYPROC)load("glShaderBinary", userptr);
    glad_glProgramUniformMatrix4x2dv = (PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC)load("glProgramUniformMatrix4x2dv", userptr);
    glad_glDeleteProgramPipelines = (PFNGLDELETEPROGRAMPIPELINESPROC)load("glDeleteProgramPipelines", userptr);
    glad_glProgramUniform3ui = (PFNGLPROGRAMUNIFORM3UIPROC)load("glProgramUniform3ui", userptr);
    glad_glProgramUniform2uiv = (PFNGLPROGRAMUNIFORM2UIVPROC)load("glProgramUniform2uiv", userptr);
    glad_glProgramUniform1fv = (PFNGLPROGRAMUNIFORM1FVPROC)load("glProgramUniform1fv", userptr);
    glad_glCreateShaderProgramv = (PFNGLCREATESHADERPROGRAMVPROC)load("glCreateShaderProgramv", userptr);
    glad_glProgramUniform3f = (PFNGLPROGRAMUNIFORM3FPROC)load("glProgramUniform3f", userptr);
    glad_glProgramUniform1uiv = (PFNGLPROGRAMUNIFORM1UIVPROC)load("glProgramUniform1uiv", userptr);
    glad_glVertexAttribL4d = (PFNGLVERTEXATTRIBL4DPROC)load("glVertexAttribL4d", userptr);
    glad_glClearDepthf = (PFNGLCLEARDEPTHFPROC)load("glClearDepthf", userptr);
    glad_glProgramUniformMatrix4x2fv = (PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC)load("glProgramUniformMatrix4x2fv", userptr);
    glad_glProgramUniform1i = (PFNGLPROGRAMUNIFORM1IPROC)load("glProgramUniform1i", userptr);
    glad_glVertexAttribL1dv = (PFNGLVERTEXATTRIBL1DVPROC)load("glVertexAttribL1dv", userptr);
    glad_glVertexAttribL3d = (PFNGLVERTEXATTRIBL3DPROC)load("glVertexAttribL3d", userptr);
    glad_glProgramUniform1d = (PFNGLPROGRAMUNIFORM1DPROC)load("glProgramUniform1d", userptr);
    glad_glProgramUniformMatrix2fv = (PFNGLPROGRAMUNIFORMMATRIX2FVPROC)load("glProgramUniformMatrix2fv", userptr);
    glad_glProgramUniformMatrix3fv = (PFNGLPROGRAMUNIFORMMATRIX3FVPROC)load("glProgramUniformMatrix3fv", userptr);
    glad_glProgramUniform1f = (PFNGLPROGRAMUNIFORM1FPROC)load("glProgramUniform1f", userptr);
}
static void load_GL_VERSION_4_2(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_VERSION_4_2) return;
    glad_glDrawTransformFeedbackInstanced = (PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC)load("glDrawTransformFeedbackInstanced", userptr);
    glad_glDrawElementsInstancedBaseVertexBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC)load("glDrawElementsInstancedBaseVertexBaseInstance", userptr);
    glad_glDrawTransformFeedbackStreamInstanced = (PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC)load("glDrawTransformFeedbackStreamInstanced", userptr);
    glad_glDrawElementsInstancedBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC)load("glDrawElementsInstancedBaseInstance", userptr);
    glad_glGetActiveAtomicCounterBufferiv = (PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC)load("glGetActiveAtomicCounterBufferiv", userptr);
    glad_glMemoryBarrier = (PFNGLMEMORYBARRIERPROC)load("glMemoryBarrier", userptr);
    glad_glDrawArraysInstancedBaseInstance = (PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)load("glDrawArraysInstancedBaseInstance", userptr);
    glad_glGetInternalformativ = (PFNGLGETINTERNALFORMATIVPROC)load("glGetInternalformativ", userptr);
    glad_glBindImageTexture = (PFNGLBINDIMAGETEXTUREPROC)load("glBindImageTexture", userptr);
    glad_glTexStorage2D = (PFNGLTEXSTORAGE2DPROC)load("glTexStorage2D", userptr);
    glad_glTexStorage1D = (PFNGLTEXSTORAGE1DPROC)load("glTexStorage1D", userptr);
    glad_glTexStorage3D = (PFNGLTEXSTORAGE3DPROC)load("glTexStorage3D", userptr);
}
static void load_GL_VERSION_4_3(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_VERSION_4_3) return;
    glad_glMultiDrawArraysIndirect = (PFNGLMULTIDRAWARRAYSINDIRECTPROC)load("glMultiDrawArraysIndirect", userptr);
    glad_glFramebufferParameteri = (PFNGLFRAMEBUFFERPARAMETERIPROC)load("glFramebufferParameteri", userptr);
    glad_glGetProgramResourceName = (PFNGLGETPROGRAMRESOURCENAMEPROC)load("glGetProgramResourceName", userptr);
    glad_glGetProgramResourceLocation = (PFNGLGETPROGRAMRESOURCELOCATIONPROC)load("glGetProgramResourceLocation", userptr);
    glad_glClearBufferSubData = (PFNGLCLEARBUFFERSUBDATAPROC)load("glClearBufferSubData", userptr);
    glad_glInvalidateFramebuffer = (PFNGLINVALIDATEFRAMEBUFFERPROC)load("glInvalidateFramebuffer", userptr);
    glad_glPopDebugGroup = (PFNGLPOPDEBUGGROUPPROC)load("glPopDebugGroup", userptr);
    glad_glGetObjectLabel = (PFNGLGETOBJECTLABELPROC)load("glGetObjectLabel", userptr);
    glad_glGetInternalformati64v = (PFNGLGETINTERNALFORMATI64VPROC)load("glGetInternalformati64v", userptr);
    glad_glObjectLabel = (PFNGLOBJECTLABELPROC)load("glObjectLabel", userptr);
    glad_glShaderStorageBlockBinding = (PFNGLSHADERSTORAGEBLOCKBINDINGPROC)load("glShaderStorageBlockBinding", userptr);
    glad_glGetProgramInterfaceiv = (PFNGLGETPROGRAMINTERFACEIVPROC)load("glGetProgramInterfaceiv", userptr);
    glad_glVertexBindingDivisor = (PFNGLVERTEXBINDINGDIVISORPROC)load("glVertexBindingDivisor", userptr);
    glad_glInvalidateTexSubImage = (PFNGLINVALIDATETEXSUBIMAGEPROC)load("glInvalidateTexSubImage", userptr);
    glad_glInvalidateBufferSubData = (PFNGLINVALIDATEBUFFERSUBDATAPROC)load("glInvalidateBufferSubData", userptr);
    glad_glGetProgramResourceIndex = (PFNGLGETPROGRAMRESOURCEINDEXPROC)load("glGetProgramResourceIndex", userptr);
    glad_glDispatchComputeIndirect = (PFNGLDISPATCHCOMPUTEINDIRECTPROC)load("glDispatchComputeIndirect", userptr);
    glad_glTexStorage3DMultisample = (PFNGLTEXSTORAGE3DMULTISAMPLEPROC)load("glTexStorage3DMultisample", userptr);
    glad_glTextureView = (PFNGLTEXTUREVIEWPROC)load("glTextureView", userptr);
    glad_glGetObjectPtrLabel = (PFNGLGETOBJECTPTRLABELPROC)load("glGetObjectPtrLabel", userptr);
    glad_glCopyImageSubData = (PFNGLCOPYIMAGESUBDATAPROC)load("glCopyImageSubData", userptr);
    glad_glBindVertexBuffer = (PFNGLBINDVERTEXBUFFERPROC)load("glBindVertexBuffer", userptr);
    glad_glClearBufferData = (PFNGLCLEARBUFFERDATAPROC)load("glClearBufferData", userptr);
    glad_glDispatchCompute = (PFNGLDISPATCHCOMPUTEPROC)load("glDispatchCompute", userptr);
    glad_glInvalidateBufferData = (PFNGLINVALIDATEBUFFERDATAPROC)load("glInvalidateBufferData", userptr);
    glad_glObjectPtrLabel = (PFNGLOBJECTPTRLABELPROC)load("glObjectPtrLabel", userptr);
    glad_glGetDebugMessageLog = (PFNGLGETDEBUGMESSAGELOGPROC)load("glGetDebugMessageLog", userptr);
    glad_glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)load("glDebugMessageCallback", userptr);
    glad_glVertexAttribIFormat = (PFNGLVERTEXATTRIBIFORMATPROC)load("glVertexAttribIFormat", userptr);
    glad_glMultiDrawElementsIndirect = (PFNGLMULTIDRAWELEMENTSINDIRECTPROC)load("glMultiDrawElementsIndirect", userptr);
    glad_glInvalidateSubFramebuffer = (PFNGLINVALIDATESUBFRAMEBUFFERPROC)load("glInvalidateSubFramebuffer", userptr);
    glad_glVertexAttribLFormat = (PFNGLVERTEXATTRIBLFORMATPROC)load("glVertexAttribLFormat", userptr);
    glad_glPushDebugGroup = (PFNGLPUSHDEBUGGROUPPROC)load("glPushDebugGroup", userptr);
    glad_glGetFramebufferParameteriv = (PFNGLGETFRAMEBUFFERPARAMETERIVPROC)load("glGetFramebufferParameteriv", userptr);
    glad_glInvalidateTexImage = (PFNGLINVALIDATETEXIMAGEPROC)load("glInvalidateTexImage", userptr);
    glad_glGetProgramResourceLocationIndex = (PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC)load("glGetProgramResourceLocationIndex", userptr);
    glad_glDebugMessageControl = (PFNGLDEBUGMESSAGECONTROLPROC)load("glDebugMessageControl", userptr);
    glad_glVertexAttribFormat = (PFNGLVERTEXATTRIBFORMATPROC)load("glVertexAttribFormat", userptr);
    glad_glTexStorage2DMultisample = (PFNGLTEXSTORAGE2DMULTISAMPLEPROC)load("glTexStorage2DMultisample", userptr);
    glad_glVertexAttribBinding = (PFNGLVERTEXATTRIBBINDINGPROC)load("glVertexAttribBinding", userptr);
    glad_glDebugMessageInsert = (PFNGLDEBUGMESSAGEINSERTPROC)load("glDebugMessageInsert", userptr);
    glad_glTexBufferRange = (PFNGLTEXBUFFERRANGEPROC)load("glTexBufferRange", userptr);
    glad_glGetProgramResourceiv = (PFNGLGETPROGRAMRESOURCEIVPROC)load("glGetProgramResourceiv", userptr);
}
static void load_GL_VERSION_4_4(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_VERSION_4_4) return;
    glad_glBufferStorage = (PFNGLBUFFERSTORAGEPROC)load("glBufferStorage", userptr);
    glad_glBindTextures = (PFNGLBINDTEXTURESPROC)load("glBindTextures", userptr);
    glad_glClearTexImage = (PFNGLCLEARTEXIMAGEPROC)load("glClearTexImage", userptr);
    glad_glBindImageTextures = (PFNGLBINDIMAGETEXTURESPROC)load("glBindImageTextures", userptr);
    glad_glBindSamplers = (PFNGLBINDSAMPLERSPROC)load("glBindSamplers", userptr);
    glad_glBindBuffersBase = (PFNGLBINDBUFFERSBASEPROC)load("glBindBuffersBase", userptr);
    glad_glBindBuffersRange = (PFNGLBINDBUFFERSRANGEPROC)load("glBindBuffersRange", userptr);
    glad_glBindVertexBuffers = (PFNGLBINDVERTEXBUFFERSPROC)load("glBindVertexBuffers", userptr);
    glad_glClearTexSubImage = (PFNGLCLEARTEXSUBIMAGEPROC)load("glClearTexSubImage", userptr);
}
static void load_GL_VERSION_4_5(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_VERSION_4_5) return;
    glad_glTextureParameterfv = (PFNGLTEXTUREPARAMETERFVPROC)load("glTextureParameterfv", userptr);
    glad_glVertexArrayElementBuffer = (PFNGLVERTEXARRAYELEMENTBUFFERPROC)load("glVertexArrayElementBuffer", userptr);
    glad_glNamedRenderbufferStorage = (PFNGLNAMEDRENDERBUFFERSTORAGEPROC)load("glNamedRenderbufferStorage", userptr);
    glad_glGetNamedFramebufferAttachmentParameteriv = (PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC)load("glGetNamedFramebufferAttachmentParameteriv", userptr);
    glad_glGetNamedBufferParameteriv = (PFNGLGETNAMEDBUFFERPARAMETERIVPROC)load("glGetNamedBufferParameteriv", userptr);
    glad_glTextureStorage3DMultisample = (PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC)load("glTextureStorage3DMultisample", userptr);
    glad_glVertexArrayAttribIFormat = (PFNGLVERTEXARRAYATTRIBIFORMATPROC)load("glVertexArrayAttribIFormat", userptr);
    glad_glBindTextureUnit = (PFNGLBINDTEXTUREUNITPROC)load("glBindTextureUnit", userptr);
    glad_glGetTextureLevelParameteriv = (PFNGLGETTEXTURELEVELPARAMETERIVPROC)load("glGetTextureLevelParameteriv", userptr);
    glad_glGetnUniformiv = (PFNGLGETNUNIFORMIVPROC)load("glGetnUniformiv", userptr);
    glad_glVertexArrayVertexBuffer = (PFNGLVERTEXARRAYVERTEXBUFFERPROC)load("glVertexArrayVertexBuffer", userptr);
    glad_glVertexArrayVertexBuffers = (PFNGLVERTEXARRAYVERTEXBUFFERSPROC)load("glVertexArrayVertexBuffers", userptr);
    glad_glGetTextureLevelParameterfv = (PFNGLGETTEXTURELEVELPARAMETERFVPROC)load("glGetTextureLevelParameterfv", userptr);
    glad_glGetNamedBufferPointerv = (PFNGLGETNAMEDBUFFERPOINTERVPROC)load("glGetNamedBufferPointerv", userptr);
    glad_glVertexArrayAttribLFormat = (PFNGLVERTEXARRAYATTRIBLFORMATPROC)load("glVertexArrayAttribLFormat", userptr);
    glad_glInvalidateNamedFramebufferSubData = (PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC)load("glInvalidateNamedFramebufferSubData", userptr);
    glad_glVertexArrayAttribBinding = (PFNGLVERTEXARRAYATTRIBBINDINGPROC)load("glVertexArrayAttribBinding", userptr);
    glad_glDisableVertexArrayAttrib = (PFNGLDISABLEVERTEXARRAYATTRIBPROC)load("glDisableVertexArrayAttrib", userptr);
    glad_glTextureSubImage1D = (PFNGLTEXTURESUBIMAGE1DPROC)load("glTextureSubImage1D", userptr);
    glad_glTransformFeedbackBufferRange = (PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC)load("glTransformFeedbackBufferRange", userptr);
    glad_glGetQueryBufferObjectui64v = (PFNGLGETQUERYBUFFEROBJECTUI64VPROC)load("glGetQueryBufferObjectui64v", userptr);
    glad_glGetNamedFramebufferParameteriv = (PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC)load("glGetNamedFramebufferParameteriv", userptr);
    glad_glCreateVertexArrays = (PFNGLCREATEVERTEXARRAYSPROC)load("glCreateVertexArrays", userptr);
    glad_glCreateRenderbuffers = (PFNGLCREATERENDERBUFFERSPROC)load("glCreateRenderbuffers", userptr);
    glad_glTextureStorage2DMultisample = (PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC)load("glTextureStorage2DMultisample", userptr);
    glad_glClearNamedFramebufferuiv = (PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC)load("glClearNamedFramebufferuiv", userptr);
    glad_glReadnPixels = (PFNGLREADNPIXELSPROC)load("glReadnPixels", userptr);
    glad_glGetTransformFeedbacki_v = (PFNGLGETTRANSFORMFEEDBACKI_VPROC)load("glGetTransformFeedbacki_v", userptr);
    glad_glGetCompressedTextureSubImage = (PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC)load("glGetCompressedTextureSubImage", userptr);
    glad_glGetnCompressedTexImage = (PFNGLGETNCOMPRESSEDTEXIMAGEPROC)load("glGetnCompressedTexImage", userptr);
    glad_glClearNamedFramebufferfi = (PFNGLCLEARNAMEDFRAMEBUFFERFIPROC)load("glClearNamedFramebufferfi", userptr);
    glad_glGetQueryBufferObjectuiv = (PFNGLGETQUERYBUFFEROBJECTUIVPROC)load("glGetQueryBufferObjectuiv", userptr);
    glad_glClearNamedFramebufferfv = (PFNGLCLEARNAMEDFRAMEBUFFERFVPROC)load("glClearNamedFramebufferfv", userptr);
    glad_glGetVertexArrayIndexed64iv = (PFNGLGETVERTEXARRAYINDEXED64IVPROC)load("glGetVertexArrayIndexed64iv", userptr);
    glad_glTextureParameterf = (PFNGLTEXTUREPARAMETERFPROC)load("glTextureParameterf", userptr);
    glad_glVertexArrayAttribFormat = (PFNGLVERTEXARRAYATTRIBFORMATPROC)load("glVertexArrayAttribFormat", userptr);
    glad_glCopyTextureSubImage2D = (PFNGLCOPYTEXTURESUBIMAGE2DPROC)load("glCopyTextureSubImage2D", userptr);
    glad_glNamedFramebufferDrawBuffers = (PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC)load("glNamedFramebufferDrawBuffers", userptr);
    glad_glTextureParameterIiv = (PFNGLTEXTUREPARAMETERIIVPROC)load("glTextureParameterIiv", userptr);
    glad_glTextureSubImage2D = (PFNGLTEXTURESUBIMAGE2DPROC)load("glTextureSubImage2D", userptr);
    glad_glGetQueryBufferObjectiv = (PFNGLGETQUERYBUFFEROBJECTIVPROC)load("glGetQueryBufferObjectiv", userptr);
    glad_glCompressedTextureSubImage3D = (PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC)load("glCompressedTextureSubImage3D", userptr);
    glad_glGetCompressedTextureImage = (PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC)load("glGetCompressedTextureImage", userptr);
    glad_glTextureBarrier = (PFNGLTEXTUREBARRIERPROC)load("glTextureBarrier", userptr);
    glad_glUnmapNamedBuffer = (PFNGLUNMAPNAMEDBUFFERPROC)load("glUnmapNamedBuffer", userptr);
    glad_glGetNamedRenderbufferParameteriv = (PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC)load("glGetNamedRenderbufferParameteriv", userptr);
    glad_glGetTextureParameterfv = (PFNGLGETTEXTUREPARAMETERFVPROC)load("glGetTextureParameterfv", userptr);
    glad_glNamedRenderbufferStorageMultisample = (PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC)load("glNamedRenderbufferStorageMultisample", userptr);
    glad_glClearNamedBufferData = (PFNGLCLEARNAMEDBUFFERDATAPROC)load("glClearNamedBufferData", userptr);
    glad_glEnableVertexArrayAttrib = (PFNGLENABLEVERTEXARRAYATTRIBPROC)load("glEnableVertexArrayAttrib", userptr);
    glad_glCopyNamedBufferSubData = (PFNGLCOPYNAMEDBUFFERSUBDATAPROC)load("glCopyNamedBufferSubData", userptr);
    glad_glCreateSamplers = (PFNGLCREATESAMPLERSPROC)load("glCreateSamplers", userptr);
    glad_glTextureParameteriv = (PFNGLTEXTUREPARAMETERIVPROC)load("glTextureParameteriv", userptr);
    glad_glGetQueryBufferObjecti64v = (PFNGLGETQUERYBUFFEROBJECTI64VPROC)load("glGetQueryBufferObjecti64v", userptr);
    glad_glTextureSubImage3D = (PFNGLTEXTURESUBIMAGE3DPROC)load("glTextureSubImage3D", userptr);
    glad_glFlushMappedNamedBufferRange = (PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC)load("glFlushMappedNamedBufferRange", userptr);
    glad_glNamedFramebufferTexture = (PFNGLNAMEDFRAMEBUFFERTEXTUREPROC)load("glNamedFramebufferTexture", userptr);
    glad_glNamedBufferStorage = (PFNGLNAMEDBUFFERSTORAGEPROC)load("glNamedBufferStorage", userptr);
    glad_glGetVertexArrayIndexediv = (PFNGLGETVERTEXARRAYINDEXEDIVPROC)load("glGetVertexArrayIndexediv", userptr);
    glad_glCheckNamedFramebufferStatus = (PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC)load("glCheckNamedFramebufferStatus", userptr);
    glad_glCreateFramebuffers = (PFNGLCREATEFRAMEBUFFERSPROC)load("glCreateFramebuffers", userptr);
    glad_glGetTransformFeedbackiv = (PFNGLGETTRANSFORMFEEDBACKIVPROC)load("glGetTransformFeedbackiv", userptr);
    glad_glTextureStorage3D = (PFNGLTEXTURESTORAGE3DPROC)load("glTextureStorage3D", userptr);
    glad_glCompressedTextureSubImage1D = (PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC)load("glCompressedTextureSubImage1D", userptr);
    glad_glGetNamedBufferParameteri64v = (PFNGLGETNAMEDBUFFERPARAMETERI64VPROC)load("glGetNamedBufferParameteri64v", userptr);
    glad_glCreateProgramPipelines = (PFNGLCREATEPROGRAMPIPELINESPROC)load("glCreateProgramPipelines", userptr);
    glad_glMapNamedBufferRange = (PFNGLMAPNAMEDBUFFERRANGEPROC)load("glMapNamedBufferRange", userptr);
    glad_glCreateBuffers = (PFNGLCREATEBUFFERSPROC)load("glCreateBuffers", userptr);
    glad_glGetnUniformfv = (PFNGLGETNUNIFORMFVPROC)load("glGetnUniformfv", userptr);
    glad_glCreateTransformFeedbacks = (PFNGLCREATETRANSFORMFEEDBACKSPROC)load("glCreateTransformFeedbacks", userptr);
    glad_glCopyTextureSubImage3D = (PFNGLCOPYTEXTURESUBIMAGE3DPROC)load("glCopyTextureSubImage3D", userptr);
    glad_glTransformFeedbackBufferBase = (PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC)load("glTransformFeedbackBufferBase", userptr);
    glad_glGetTextureParameterIuiv = (PFNGLGETTEXTUREPARAMETERIUIVPROC)load("glGetTextureParameterIuiv", userptr);
    glad_glInvalidateNamedFramebufferData = (PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC)load("glInvalidateNamedFramebufferData", userptr);
    glad_glNamedFramebufferReadBuffer = (PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC)load("glNamedFramebufferReadBuffer", userptr);
    glad_glGetTextureParameterIiv = (PFNGLGETTEXTUREPARAMETERIIVPROC)load("glGetTextureParameterIiv", userptr);
    glad_glGetGraphicsResetStatus = (PFNGLGETGRAPHICSRESETSTATUSPROC)load("glGetGraphicsResetStatus", userptr);
    glad_glGetVertexArrayiv = (PFNGLGETVERTEXARRAYIVPROC)load("glGetVertexArrayiv", userptr);
    glad_glBlitNamedFramebuffer = (PFNGLBLITNAMEDFRAMEBUFFERPROC)load("glBlitNamedFramebuffer", userptr);
    glad_glCreateQueries = (PFNGLCREATEQUERIESPROC)load("glCreateQueries", userptr);
    glad_glTextureParameteri = (PFNGLTEXTUREPARAMETERIPROC)load("glTextureParameteri", userptr);
    glad_glNamedFramebufferDrawBuffer = (PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC)load("glNamedFramebufferDrawBuffer", userptr);
    glad_glNamedBufferData = (PFNGLNAMEDBUFFERDATAPROC)load("glNamedBufferData", userptr);
    glad_glClearNamedBufferSubData = (PFNGLCLEARNAMEDBUFFERSUBDATAPROC)load("glClearNamedBufferSubData", userptr);
    glad_glGenerateTextureMipmap = (PFNGLGENERATETEXTUREMIPMAPPROC)load("glGenerateTextureMipmap", userptr);
    glad_glNamedBufferSubData = (PFNGLNAMEDBUFFERSUBDATAPROC)load("glNamedBufferSubData", userptr);
    glad_glGetnUniformuiv = (PFNGLGETNUNIFORMUIVPROC)load("glGetnUniformuiv", userptr);
    glad_glTextureStorage1D = (PFNGLTEXTURESTORAGE1DPROC)load("glTextureStorage1D", userptr);
    glad_glMemoryBarrierByRegion = (PFNGLMEMORYBARRIERBYREGIONPROC)load("glMemoryBarrierByRegion", userptr);
    glad_glGetNamedBufferSubData = (PFNGLGETNAMEDBUFFERSUBDATAPROC)load("glGetNamedBufferSubData", userptr);
    glad_glGetTextureParameteriv = (PFNGLGETTEXTUREPARAMETERIVPROC)load("glGetTextureParameteriv", userptr);
    glad_glCompressedTextureSubImage2D = (PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC)load("glCompressedTextureSubImage2D", userptr);
    glad_glClearNamedFramebufferiv = (PFNGLCLEARNAMEDFRAMEBUFFERIVPROC)load("glClearNamedFramebufferiv", userptr);
    glad_glMapNamedBuffer = (PFNGLMAPNAMEDBUFFERPROC)load("glMapNamedBuffer", userptr);
    glad_glTextureBuffer = (PFNGLTEXTUREBUFFERPROC)load("glTextureBuffer", userptr);
    glad_glTextureParameterIuiv = (PFNGLTEXTUREPARAMETERIUIVPROC)load("glTextureParameterIuiv", userptr);
    glad_glTextureBufferRange = (PFNGLTEXTUREBUFFERRANGEPROC)load("glTextureBufferRange", userptr);
    glad_glGetnUniformdv = (PFNGLGETNUNIFORMDVPROC)load("glGetnUniformdv", userptr);
    glad_glGetTextureSubImage = (PFNGLGETTEXTURESUBIMAGEPROC)load("glGetTextureSubImage", userptr);
    glad_glGetTextureImage = (PFNGLGETTEXTUREIMAGEPROC)load("glGetTextureImage", userptr);
    glad_glGetTransformFeedbacki64_v = (PFNGLGETTRANSFORMFEEDBACKI64_VPROC)load("glGetTransformFeedbacki64_v", userptr);
    glad_glCopyTextureSubImage1D = (PFNGLCOPYTEXTURESUBIMAGE1DPROC)load("glCopyTextureSubImage1D", userptr);
    glad_glTextureStorage2D = (PFNGLTEXTURESTORAGE2DPROC)load("glTextureStorage2D", userptr);
    glad_glVertexArrayBindingDivisor = (PFNGLVERTEXARRAYBINDINGDIVISORPROC)load("glVertexArrayBindingDivisor", userptr);
    glad_glNamedFramebufferTextureLayer = (PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC)load("glNamedFramebufferTextureLayer", userptr);
    glad_glNamedFramebufferParameteri = (PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC)load("glNamedFramebufferParameteri", userptr);
    glad_glGetnTexImage = (PFNGLGETNTEXIMAGEPROC)load("glGetnTexImage", userptr);
    glad_glCreateTextures = (PFNGLCREATETEXTURESPROC)load("glCreateTextures", userptr);
    glad_glNamedFramebufferRenderbuffer = (PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC)load("glNamedFramebufferRenderbuffer", userptr);
    glad_glClipControl = (PFNGLCLIPCONTROLPROC)load("glClipControl", userptr);
}
static void load_GL_NV_transform_feedback2(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_NV_transform_feedback2) return;
    glad_glIsTransformFeedbackNV = (PFNGLISTRANSFORMFEEDBACKNVPROC)load("glIsTransformFeedbackNV", userptr);
    glad_glBindTransformFeedbackNV = (PFNGLBINDTRANSFORMFEEDBACKNVPROC)load("glBindTransformFeedbackNV", userptr);
    glad_glPauseTransformFeedbackNV = (PFNGLPAUSETRANSFORMFEEDBACKNVPROC)load("glPauseTransformFeedbackNV", userptr);
    glad_glDrawTransformFeedbackNV = (PFNGLDRAWTRANSFORMFEEDBACKNVPROC)load("glDrawTransformFeedbackNV", userptr);
    glad_glDeleteTransformFeedbacksNV = (PFNGLDELETETRANSFORMFEEDBACKSNVPROC)load("glDeleteTransformFeedbacksNV", userptr);
    glad_glResumeTransformFeedbackNV = (PFNGLRESUMETRANSFORMFEEDBACKNVPROC)load("glResumeTransformFeedbackNV", userptr);
    glad_glGenTransformFeedbacksNV = (PFNGLGENTRANSFORMFEEDBACKSNVPROC)load("glGenTransformFeedbacksNV", userptr);
}
static void load_GL_ARB_texture_compression(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_texture_compression) return;
    glad_glCompressedTexImage1DARB = (PFNGLCOMPRESSEDTEXIMAGE1DARBPROC)load("glCompressedTexImage1DARB", userptr);
    glad_glGetCompressedTexImageARB = (PFNGLGETCOMPRESSEDTEXIMAGEARBPROC)load("glGetCompressedTexImageARB", userptr);
    glad_glCompressedTexSubImage1DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC)load("glCompressedTexSubImage1DARB", userptr);
    glad_glCompressedTexImage3DARB = (PFNGLCOMPRESSEDTEXIMAGE3DARBPROC)load("glCompressedTexImage3DARB", userptr);
    glad_glCompressedTexSubImage2DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC)load("glCompressedTexSubImage2DARB", userptr);
    glad_glCompressedTexSubImage3DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC)load("glCompressedTexSubImage3DARB", userptr);
    glad_glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)load("glCompressedTexImage2DARB", userptr);
}
static void load_GL_NV_point_sprite(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_NV_point_sprite) return;
    glad_glPointParameteriNV = (PFNGLPOINTPARAMETERINVPROC)load("glPointParameteriNV", userptr);
    glad_glPointParameterivNV = (PFNGLPOINTPARAMETERIVNVPROC)load("glPointParameterivNV", userptr);
}
static void load_GL_ARB_get_texture_sub_image(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_get_texture_sub_image) return;
    glad_glGetTextureSubImage = (PFNGLGETTEXTURESUBIMAGEPROC)load("glGetTextureSubImage", userptr);
    glad_glGetCompressedTextureSubImage = (PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC)load("glGetCompressedTextureSubImage", userptr);
}
static void load_GL_AMD_multi_draw_indirect(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_AMD_multi_draw_indirect) return;
    glad_glMultiDrawElementsIndirectAMD = (PFNGLMULTIDRAWELEMENTSINDIRECTAMDPROC)load("glMultiDrawElementsIndirectAMD", userptr);
    glad_glMultiDrawArraysIndirectAMD = (PFNGLMULTIDRAWARRAYSINDIRECTAMDPROC)load("glMultiDrawArraysIndirectAMD", userptr);
}
static void load_GL_EXT_blend_color(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_blend_color) return;
    glad_glBlendColorEXT = (PFNGLBLENDCOLOREXTPROC)load("glBlendColorEXT", userptr);
}
static void load_GL_ARB_texture_storage_multisample(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_texture_storage_multisample) return;
    glad_glTexStorage3DMultisample = (PFNGLTEXSTORAGE3DMULTISAMPLEPROC)load("glTexStorage3DMultisample", userptr);
    glad_glTexStorage2DMultisample = (PFNGLTEXSTORAGE2DMULTISAMPLEPROC)load("glTexStorage2DMultisample", userptr);
}
static void load_GL_ARB_clip_control(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_clip_control) return;
    glad_glClipControl = (PFNGLCLIPCONTROLPROC)load("glClipControl", userptr);
}
static void load_GL_ARB_instanced_arrays(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_instanced_arrays) return;
    glad_glVertexAttribDivisorARB = (PFNGLVERTEXATTRIBDIVISORARBPROC)load("glVertexAttribDivisorARB", userptr);
}
static void load_GL_EXT_vertex_attrib_64bit(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_vertex_attrib_64bit) return;
    glad_glGetVertexAttribLdvEXT = (PFNGLGETVERTEXATTRIBLDVEXTPROC)load("glGetVertexAttribLdvEXT", userptr);
    glad_glVertexAttribL3dvEXT = (PFNGLVERTEXATTRIBL3DVEXTPROC)load("glVertexAttribL3dvEXT", userptr);
    glad_glVertexAttribL3dEXT = (PFNGLVERTEXATTRIBL3DEXTPROC)load("glVertexAttribL3dEXT", userptr);
    glad_glVertexAttribL2dvEXT = (PFNGLVERTEXATTRIBL2DVEXTPROC)load("glVertexAttribL2dvEXT", userptr);
    glad_glVertexAttribL2dEXT = (PFNGLVERTEXATTRIBL2DEXTPROC)load("glVertexAttribL2dEXT", userptr);
    glad_glVertexAttribLPointerEXT = (PFNGLVERTEXATTRIBLPOINTEREXTPROC)load("glVertexAttribLPointerEXT", userptr);
    glad_glVertexAttribL1dEXT = (PFNGLVERTEXATTRIBL1DEXTPROC)load("glVertexAttribL1dEXT", userptr);
    glad_glVertexAttribL4dvEXT = (PFNGLVERTEXATTRIBL4DVEXTPROC)load("glVertexAttribL4dvEXT", userptr);
    glad_glVertexAttribL1dvEXT = (PFNGLVERTEXATTRIBL1DVEXTPROC)load("glVertexAttribL1dvEXT", userptr);
    glad_glVertexAttribL4dEXT = (PFNGLVERTEXATTRIBL4DEXTPROC)load("glVertexAttribL4dEXT", userptr);
}
static void load_GL_ARB_vertex_attrib_binding(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_vertex_attrib_binding) return;
    glad_glVertexAttribLFormat = (PFNGLVERTEXATTRIBLFORMATPROC)load("glVertexAttribLFormat", userptr);
    glad_glBindVertexBuffer = (PFNGLBINDVERTEXBUFFERPROC)load("glBindVertexBuffer", userptr);
    glad_glVertexAttribBinding = (PFNGLVERTEXATTRIBBINDINGPROC)load("glVertexAttribBinding", userptr);
    glad_glVertexBindingDivisor = (PFNGLVERTEXBINDINGDIVISORPROC)load("glVertexBindingDivisor", userptr);
    glad_glVertexAttribFormat = (PFNGLVERTEXATTRIBFORMATPROC)load("glVertexAttribFormat", userptr);
    glad_glVertexAttribIFormat = (PFNGLVERTEXATTRIBIFORMATPROC)load("glVertexAttribIFormat", userptr);
}
static void load_GL_ARB_framebuffer_no_attachments(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_framebuffer_no_attachments) return;
    glad_glFramebufferParameteri = (PFNGLFRAMEBUFFERPARAMETERIPROC)load("glFramebufferParameteri", userptr);
    glad_glGetFramebufferParameteriv = (PFNGLGETFRAMEBUFFERPARAMETERIVPROC)load("glGetFramebufferParameteriv", userptr);
}
static void load_GL_ARB_texture_view(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_texture_view) return;
    glad_glTextureView = (PFNGLTEXTUREVIEWPROC)load("glTextureView", userptr);
}
static void load_GL_ATI_separate_stencil(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ATI_separate_stencil) return;
    glad_glStencilFuncSeparateATI = (PFNGLSTENCILFUNCSEPARATEATIPROC)load("glStencilFuncSeparateATI", userptr);
    glad_glStencilOpSeparateATI = (PFNGLSTENCILOPSEPARATEATIPROC)load("glStencilOpSeparateATI", userptr);
}
static void load_GL_EXT_texture_buffer_object(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_texture_buffer_object) return;
    glad_glTexBufferEXT = (PFNGLTEXBUFFEREXTPROC)load("glTexBufferEXT", userptr);
}
static void load_GL_NV_shader_buffer_store(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_NV_shader_buffer_store) return;
}
static void load_GL_ARB_shader_subroutine(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_shader_subroutine) return;
    glad_glGetUniformSubroutineuiv = (PFNGLGETUNIFORMSUBROUTINEUIVPROC)load("glGetUniformSubroutineuiv", userptr);
    glad_glGetActiveSubroutineName = (PFNGLGETACTIVESUBROUTINENAMEPROC)load("glGetActiveSubroutineName", userptr);
    glad_glGetSubroutineUniformLocation = (PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC)load("glGetSubroutineUniformLocation", userptr);
    glad_glGetActiveSubroutineUniformiv = (PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC)load("glGetActiveSubroutineUniformiv", userptr);
    glad_glGetActiveSubroutineUniformName = (PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC)load("glGetActiveSubroutineUniformName", userptr);
    glad_glGetProgramStageiv = (PFNGLGETPROGRAMSTAGEIVPROC)load("glGetProgramStageiv", userptr);
    glad_glGetSubroutineIndex = (PFNGLGETSUBROUTINEINDEXPROC)load("glGetSubroutineIndex", userptr);
    glad_glUniformSubroutinesuiv = (PFNGLUNIFORMSUBROUTINESUIVPROC)load("glUniformSubroutinesuiv", userptr);
}
static void load_GL_EXT_direct_state_access(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_direct_state_access) return;
    glad_glCompressedMultiTexImage2DEXT = (PFNGLCOMPRESSEDMULTITEXIMAGE2DEXTPROC)load("glCompressedMultiTexImage2DEXT", userptr);
    glad_glVertexArrayEdgeFlagOffsetEXT = (PFNGLVERTEXARRAYEDGEFLAGOFFSETEXTPROC)load("glVertexArrayEdgeFlagOffsetEXT", userptr);
    glad_glGetNamedBufferPointervEXT = (PFNGLGETNAMEDBUFFERPOINTERVEXTPROC)load("glGetNamedBufferPointervEXT", userptr);
    glad_glProgramUniform2dvEXT = (PFNGLPROGRAMUNIFORM2DVEXTPROC)load("glProgramUniform2dvEXT", userptr);
    glad_glGetNamedFramebufferAttachmentParameterivEXT = (PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)load("glGetNamedFramebufferAttachmentParameterivEXT", userptr);
    glad_glGetIntegerIndexedvEXT = (PFNGLGETINTEGERINDEXEDVEXTPROC)load("glGetIntegerIndexedvEXT", userptr);
    glad_glGetMultiTexGendvEXT = (PFNGLGETMULTITEXGENDVEXTPROC)load("glGetMultiTexGendvEXT", userptr);
    glad_glClearNamedBufferSubDataEXT = (PFNGLCLEARNAMEDBUFFERSUBDATAEXTPROC)load("glClearNamedBufferSubDataEXT", userptr);
    glad_glGetTextureImageEXT = (PFNGLGETTEXTUREIMAGEEXTPROC)load("glGetTextureImageEXT", userptr);
    glad_glCopyTextureImage1DEXT = (PFNGLCOPYTEXTUREIMAGE1DEXTPROC)load("glCopyTextureImage1DEXT", userptr);
    glad_glPushClientAttribDefaultEXT = (PFNGLPUSHCLIENTATTRIBDEFAULTEXTPROC)load("glPushClientAttribDefaultEXT", userptr);
    glad_glCompressedMultiTexSubImage1DEXT = (PFNGLCOMPRESSEDMULTITEXSUBIMAGE1DEXTPROC)load("glCompressedMultiTexSubImage1DEXT", userptr);
    glad_glTextureRenderbufferEXT = (PFNGLTEXTURERENDERBUFFEREXTPROC)load("glTextureRenderbufferEXT", userptr);
    glad_glMultiTexGendEXT = (PFNGLMULTITEXGENDEXTPROC)load("glMultiTexGendEXT", userptr);
    glad_glTextureStorage3DEXT = (PFNGLTEXTURESTORAGE3DEXTPROC)load("glTextureStorage3DEXT", userptr);
    glad_glVertexArrayBindVertexBufferEXT = (PFNGLVERTEXARRAYBINDVERTEXBUFFEREXTPROC)load("glVertexArrayBindVertexBufferEXT", userptr);
    glad_glMultiTexParameterivEXT = (PFNGLMULTITEXPARAMETERIVEXTPROC)load("glMultiTexParameterivEXT", userptr);
    glad_glMatrixLoadTransposefEXT = (PFNGLMATRIXLOADTRANSPOSEFEXTPROC)load("glMatrixLoadTransposefEXT", userptr);
    glad_glVertexArrayVertexAttribOffsetEXT = (PFNGLVERTEXARRAYVERTEXATTRIBOFFSETEXTPROC)load("glVertexArrayVertexAttribOffsetEXT", userptr);
    glad_glMatrixTranslatefEXT = (PFNGLMATRIXTRANSLATEFEXTPROC)load("glMatrixTranslatefEXT", userptr);
    glad_glMultiTexParameteriEXT = (PFNGLMULTITEXPARAMETERIEXTPROC)load("glMultiTexParameteriEXT", userptr);
    glad_glVertexArrayVertexAttribDivisorEXT = (PFNGLVERTEXARRAYVERTEXATTRIBDIVISOREXTPROC)load("glVertexArrayVertexAttribDivisorEXT", userptr);
    glad_glProgramUniformMatrix3x2dvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X2DVEXTPROC)load("glProgramUniformMatrix3x2dvEXT", userptr);
    glad_glGetNamedProgramLocalParameterfvEXT = (PFNGLGETNAMEDPROGRAMLOCALPARAMETERFVEXTPROC)load("glGetNamedProgramLocalParameterfvEXT", userptr);
    glad_glEnableVertexArrayAttribEXT = (PFNGLENABLEVERTEXARRAYATTRIBEXTPROC)load("glEnableVertexArrayAttribEXT", userptr);
    glad_glProgramUniform2uivEXT = (PFNGLPROGRAMUNIFORM2UIVEXTPROC)load("glProgramUniform2uivEXT", userptr);
    glad_glGetTextureParameterivEXT = (PFNGLGETTEXTUREPARAMETERIVEXTPROC)load("glGetTextureParameterivEXT", userptr);
    glad_glEnableClientStateiEXT = (PFNGLENABLECLIENTSTATEIEXTPROC)load("glEnableClientStateiEXT", userptr);
    glad_glCopyTextureSubImage2DEXT = (PFNGLCOPYTEXTURESUBIMAGE2DEXTPROC)load("glCopyTextureSubImage2DEXT", userptr);
    glad_glProgramUniformMatrix2dvEXT = (PFNGLPROGRAMUNIFORMMATRIX2DVEXTPROC)load("glProgramUniformMatrix2dvEXT", userptr);
    glad_glCompressedMultiTexSubImage3DEXT = (PFNGLCOMPRESSEDMULTITEXSUBIMAGE3DEXTPROC)load("glCompressedMultiTexSubImage3DEXT", userptr);
    glad_glVertexArrayVertexAttribIFormatEXT = (PFNGLVERTEXARRAYVERTEXATTRIBIFORMATEXTPROC)load("glVertexArrayVertexAttribIFormatEXT", userptr);
    glad_glProgramUniform4fEXT = (PFNGLPROGRAMUNIFORM4FEXTPROC)load("glProgramUniform4fEXT", userptr);
    glad_glNamedProgramLocalParameters4fvEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERS4FVEXTPROC)load("glNamedProgramLocalParameters4fvEXT", userptr);
    glad_glDisableVertexArrayAttribEXT = (PFNGLDISABLEVERTEXARRAYATTRIBEXTPROC)load("glDisableVertexArrayAttribEXT", userptr);
    glad_glProgramUniform3ivEXT = (PFNGLPROGRAMUNIFORM3IVEXTPROC)load("glProgramUniform3ivEXT", userptr);
    glad_glGetFramebufferParameterivEXT = (PFNGLGETFRAMEBUFFERPARAMETERIVEXTPROC)load("glGetFramebufferParameterivEXT", userptr);
    glad_glNamedProgramLocalParametersI4ivEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERSI4IVEXTPROC)load("glNamedProgramLocalParametersI4ivEXT", userptr);
    glad_glNamedBufferStorageEXT = (PFNGLNAMEDBUFFERSTORAGEEXTPROC)load("glNamedBufferStorageEXT", userptr);
    glad_glNamedFramebufferTexture3DEXT = (PFNGLNAMEDFRAMEBUFFERTEXTURE3DEXTPROC)load("glNamedFramebufferTexture3DEXT", userptr);
    glad_glVertexArrayMultiTexCoordOffsetEXT = (PFNGLVERTEXARRAYMULTITEXCOORDOFFSETEXTPROC)load("glVertexArrayMultiTexCoordOffsetEXT", userptr);
    glad_glProgramUniform4iEXT = (PFNGLPROGRAMUNIFORM4IEXTPROC)load("glProgramUniform4iEXT", userptr);
    glad_glProgramUniformMatrix3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC)load("glProgramUniformMatrix3fvEXT", userptr);
    glad_glEnableVertexArrayEXT = (PFNGLENABLEVERTEXARRAYEXTPROC)load("glEnableVertexArrayEXT", userptr);
    glad_glVertexArrayColorOffsetEXT = (PFNGLVERTEXARRAYCOLOROFFSETEXTPROC)load("glVertexArrayColorOffsetEXT", userptr);
    glad_glTextureParameterivEXT = (PFNGLTEXTUREPARAMETERIVEXTPROC)load("glTextureParameterivEXT", userptr);
    glad_glMultiTexRenderbufferEXT = (PFNGLMULTITEXRENDERBUFFEREXTPROC)load("glMultiTexRenderbufferEXT", userptr);
    glad_glGetMultiTexGenivEXT = (PFNGLGETMULTITEXGENIVEXTPROC)load("glGetMultiTexGenivEXT", userptr);
    glad_glMultiTexParameterIivEXT = (PFNGLMULTITEXPARAMETERIIVEXTPROC)load("glMultiTexParameterIivEXT", userptr);
    glad_glBindMultiTextureEXT = (PFNGLBINDMULTITEXTUREEXTPROC)load("glBindMultiTextureEXT", userptr);
    glad_glGetPointerIndexedvEXT = (PFNGLGETPOINTERINDEXEDVEXTPROC)load("glGetPointerIndexedvEXT", userptr);
    glad_glMatrixPushEXT = (PFNGLMATRIXPUSHEXTPROC)load("glMatrixPushEXT", userptr);
    glad_glDisableVertexArrayEXT = (PFNGLDISABLEVERTEXARRAYEXTPROC)load("glDisableVertexArrayEXT", userptr);
    glad_glProgramUniform2fEXT = (PFNGLPROGRAMUNIFORM2FEXTPROC)load("glProgramUniform2fEXT", userptr);
    glad_glTextureParameterfEXT = (PFNGLTEXTUREPARAMETERFEXTPROC)load("glTextureParameterfEXT", userptr);
    glad_glFramebufferDrawBuffersEXT = (PFNGLFRAMEBUFFERDRAWBUFFERSEXTPROC)load("glFramebufferDrawBuffersEXT", userptr);
    glad_glMapNamedBufferRangeEXT = (PFNGLMAPNAMEDBUFFERRANGEEXTPROC)load("glMapNamedBufferRangeEXT", userptr);
    glad_glNamedBufferDataEXT = (PFNGLNAMEDBUFFERDATAEXTPROC)load("glNamedBufferDataEXT", userptr);
    glad_glNamedRenderbufferStorageMultisampleEXT = (PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)load("glNamedRenderbufferStorageMultisampleEXT", userptr);
    glad_glGetFloatIndexedvEXT = (PFNGLGETFLOATINDEXEDVEXTPROC)load("glGetFloatIndexedvEXT", userptr);
    glad_glMultiTexSubImage1DEXT = (PFNGLMULTITEXSUBIMAGE1DEXTPROC)load("glMultiTexSubImage1DEXT", userptr);
    glad_glMatrixMultfEXT = (PFNGLMATRIXMULTFEXTPROC)load("glMatrixMultfEXT", userptr);
    glad_glCompressedTextureSubImage3DEXT = (PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXTPROC)load("glCompressedTextureSubImage3DEXT", userptr);
    glad_glProgramUniform2fvEXT = (PFNGLPROGRAMUNIFORM2FVEXTPROC)load("glProgramUniform2fvEXT", userptr);
    glad_glGetTextureLevelParameterivEXT = (PFNGLGETTEXTURELEVELPARAMETERIVEXTPROC)load("glGetTextureLevelParameterivEXT", userptr);
    glad_glNamedProgramLocalParameterI4uiEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIEXTPROC)load("glNamedProgramLocalParameterI4uiEXT", userptr);
    glad_glTextureParameterIuivEXT = (PFNGLTEXTUREPARAMETERIUIVEXTPROC)load("glTextureParameterIuivEXT", userptr);
    glad_glProgramUniform1iEXT = (PFNGLPROGRAMUNIFORM1IEXTPROC)load("glProgramUniform1iEXT", userptr);
    glad_glMultiTexSubImage3DEXT = (PFNGLMULTITEXSUBIMAGE3DEXTPROC)load("glMultiTexSubImage3DEXT", userptr);
    glad_glNamedFramebufferTextureFaceEXT = (PFNGLNAMEDFRAMEBUFFERTEXTUREFACEEXTPROC)load("glNamedFramebufferTextureFaceEXT", userptr);
    glad_glGetMultiTexParameterivEXT = (PFNGLGETMULTITEXPARAMETERIVEXTPROC)load("glGetMultiTexParameterivEXT", userptr);
    glad_glProgramUniformMatrix3x4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC)load("glProgramUniformMatrix3x4fvEXT", userptr);
    glad_glProgramUniform3iEXT = (PFNGLPROGRAMUNIFORM3IEXTPROC)load("glProgramUniform3iEXT", userptr);
    glad_glNamedProgramLocalParameterI4ivEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERI4IVEXTPROC)load("glNamedProgramLocalParameterI4ivEXT", userptr);
    glad_glGetMultiTexParameterIuivEXT = (PFNGLGETMULTITEXPARAMETERIUIVEXTPROC)load("glGetMultiTexParameterIuivEXT", userptr);
    glad_glTextureStorage2DMultisampleEXT = (PFNGLTEXTURESTORAGE2DMULTISAMPLEEXTPROC)load("glTextureStorage2DMultisampleEXT", userptr);
    glad_glGetDoublei_vEXT = (PFNGLGETDOUBLEI_VEXTPROC)load("glGetDoublei_vEXT", userptr);
    glad_glVertexArrayIndexOffsetEXT = (PFNGLVERTEXARRAYINDEXOFFSETEXTPROC)load("glVertexArrayIndexOffsetEXT", userptr);
    glad_glMatrixOrthoEXT = (PFNGLMATRIXORTHOEXTPROC)load("glMatrixOrthoEXT", userptr);
    glad_glMatrixMultTransposedEXT = (PFNGLMATRIXMULTTRANSPOSEDEXTPROC)load("glMatrixMultTransposedEXT", userptr);
    glad_glDisableClientStateiEXT = (PFNGLDISABLECLIENTSTATEIEXTPROC)load("glDisableClientStateiEXT", userptr);
    glad_glNamedRenderbufferStorageMultisampleCoverageEXT = (PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLECOVERAGEEXTPROC)load("glNamedRenderbufferStorageMultisampleCoverageEXT", userptr);
    glad_glGetDoubleIndexedvEXT = (PFNGLGETDOUBLEINDEXEDVEXTPROC)load("glGetDoubleIndexedvEXT", userptr);
    glad_glMatrixMultdEXT = (PFNGLMATRIXMULTDEXTPROC)load("glMatrixMultdEXT", userptr);
    glad_glMultiTexBufferEXT = (PFNGLMULTITEXBUFFEREXTPROC)load("glMultiTexBufferEXT", userptr);
    glad_glGenerateTextureMipmapEXT = (PFNGLGENERATETEXTUREMIPMAPEXTPROC)load("glGenerateTextureMipmapEXT", userptr);
    glad_glCompressedTextureImage3DEXT = (PFNGLCOMPRESSEDTEXTUREIMAGE3DEXTPROC)load("glCompressedTextureImage3DEXT", userptr);
    glad_glMultiTexParameterIuivEXT = (PFNGLMULTITEXPARAMETERIUIVEXTPROC)load("glMultiTexParameterIuivEXT", userptr);
    glad_glNamedFramebufferTextureLayerEXT = (PFNGLNAMEDFRAMEBUFFERTEXTURELAYEREXTPROC)load("glNamedFramebufferTextureLayerEXT", userptr);
    glad_glGetMultiTexParameterIivEXT = (PFNGLGETMULTITEXPARAMETERIIVEXTPROC)load("glGetMultiTexParameterIivEXT", userptr);
    glad_glMultiTexGenfvEXT = (PFNGLMULTITEXGENFVEXTPROC)load("glMultiTexGenfvEXT", userptr);
    glad_glProgramUniform2dEXT = (PFNGLPROGRAMUNIFORM2DEXTPROC)load("glProgramUniform2dEXT", userptr);
    glad_glGetVertexArrayIntegeri_vEXT = (PFNGLGETVERTEXARRAYINTEGERI_VEXTPROC)load("glGetVertexArrayIntegeri_vEXT", userptr);
    glad_glMultiTexGenfEXT = (PFNGLMULTITEXGENFEXTPROC)load("glMultiTexGenfEXT", userptr);
    glad_glMultiTexImage3DEXT = (PFNGLMULTITEXIMAGE3DEXTPROC)load("glMultiTexImage3DEXT", userptr);
    glad_glCheckNamedFramebufferStatusEXT = (PFNGLCHECKNAMEDFRAMEBUFFERSTATUSEXTPROC)load("glCheckNamedFramebufferStatusEXT", userptr);
    glad_glVertexArrayVertexAttribFormatEXT = (PFNGLVERTEXARRAYVERTEXATTRIBFORMATEXTPROC)load("glVertexArrayVertexAttribFormatEXT", userptr);
    glad_glGetCompressedTextureImageEXT = (PFNGLGETCOMPRESSEDTEXTUREIMAGEEXTPROC)load("glGetCompressedTextureImageEXT", userptr);
    glad_glMultiTexImage1DEXT = (PFNGLMULTITEXIMAGE1DEXTPROC)load("glMultiTexImage1DEXT", userptr);
    glad_glCopyMultiTexSubImage2DEXT = (PFNGLCOPYMULTITEXSUBIMAGE2DEXTPROC)load("glCopyMultiTexSubImage2DEXT", userptr);
    glad_glProgramUniformMatrix3x4dvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X4DVEXTPROC)load("glProgramUniformMatrix3x4dvEXT", userptr);
    glad_glNamedRenderbufferStorageEXT = (PFNGLNAMEDRENDERBUFFERSTORAGEEXTPROC)load("glNamedRenderbufferStorageEXT", userptr);
    glad_glGetNamedBufferSubDataEXT = (PFNGLGETNAMEDBUFFERSUBDATAEXTPROC)load("glGetNamedBufferSubDataEXT", userptr);
    glad_glProgramUniformMatrix4x3dvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X3DVEXTPROC)load("glProgramUniformMatrix4x3dvEXT", userptr);
    glad_glMatrixRotatedEXT = (PFNGLMATRIXROTATEDEXTPROC)load("glMatrixRotatedEXT", userptr);
    glad_glGetTextureParameterIivEXT = (PFNGLGETTEXTUREPARAMETERIIVEXTPROC)load("glGetTextureParameterIivEXT", userptr);
    glad_glNamedBufferSubDataEXT = (PFNGLNAMEDBUFFERSUBDATAEXTPROC)load("glNamedBufferSubDataEXT", userptr);
    glad_glMultiTexGeniEXT = (PFNGLMULTITEXGENIEXTPROC)load("glMultiTexGeniEXT", userptr);
    glad_glNamedProgramStringEXT = (PFNGLNAMEDPROGRAMSTRINGEXTPROC)load("glNamedProgramStringEXT", userptr);
    glad_glGetNamedRenderbufferParameterivEXT = (PFNGLGETNAMEDRENDERBUFFERPARAMETERIVEXTPROC)load("glGetNamedRenderbufferParameterivEXT", userptr);
    glad_glCompressedTextureImage1DEXT = (PFNGLCOMPRESSEDTEXTUREIMAGE1DEXTPROC)load("glCompressedTextureImage1DEXT", userptr);
    glad_glProgramUniform1fEXT = (PFNGLPROGRAMUNIFORM1FEXTPROC)load("glProgramUniform1fEXT", userptr);
    glad_glProgramUniformMatrix4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC)load("glProgramUniformMatrix4fvEXT", userptr);
    glad_glMultiTexGendvEXT = (PFNGLMULTITEXGENDVEXTPROC)load("glMultiTexGendvEXT", userptr);
    glad_glGetNamedBufferParameterivEXT = (PFNGLGETNAMEDBUFFERPARAMETERIVEXTPROC)load("glGetNamedBufferParameterivEXT", userptr);
    glad_glNamedProgramLocalParameter4fEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETER4FEXTPROC)load("glNamedProgramLocalParameter4fEXT", userptr);
    glad_glMatrixScaledEXT = (PFNGLMATRIXSCALEDEXTPROC)load("glMatrixScaledEXT", userptr);
    glad_glMatrixPopEXT = (PFNGLMATRIXPOPEXTPROC)load("glMatrixPopEXT", userptr);
    glad_glFlushMappedNamedBufferRangeEXT = (PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEEXTPROC)load("glFlushMappedNamedBufferRangeEXT", userptr);
    glad_glDisableClientStateIndexedEXT = (PFNGLDISABLECLIENTSTATEINDEXEDEXTPROC)load("glDisableClientStateIndexedEXT", userptr);
    glad_glProgramUniform1uivEXT = (PFNGLPROGRAMUNIFORM1UIVEXTPROC)load("glProgramUniform1uivEXT", userptr);
    glad_glProgramUniform2iEXT = (PFNGLPROGRAMUNIFORM2IEXTPROC)load("glProgramUniform2iEXT", userptr);
    glad_glCopyTextureSubImage3DEXT = (PFNGLCOPYTEXTURESUBIMAGE3DEXTPROC)load("glCopyTextureSubImage3DEXT", userptr);
    glad_glProgramUniform1dvEXT = (PFNGLPROGRAMUNIFORM1DVEXTPROC)load("glProgramUniform1dvEXT", userptr);
    glad_glProgramUniformMatrix2x4dvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X4DVEXTPROC)load("glProgramUniformMatrix2x4dvEXT", userptr);
    glad_glGetNamedFramebufferParameterivEXT = (PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVEXTPROC)load("glGetNamedFramebufferParameterivEXT", userptr);
    glad_glMatrixLoadfEXT = (PFNGLMATRIXLOADFEXTPROC)load("glMatrixLoadfEXT", userptr);
    glad_glVertexArrayVertexBindingDivisorEXT = (PFNGLVERTEXARRAYVERTEXBINDINGDIVISOREXTPROC)load("glVertexArrayVertexBindingDivisorEXT", userptr);
    glad_glProgramUniform4uivEXT = (PFNGLPROGRAMUNIFORM4UIVEXTPROC)load("glProgramUniform4uivEXT", userptr);
    glad_glTextureStorage1DEXT = (PFNGLTEXTURESTORAGE1DEXTPROC)load("glTextureStorage1DEXT", userptr);
    glad_glFramebufferReadBufferEXT = (PFNGLFRAMEBUFFERREADBUFFEREXTPROC)load("glFramebufferReadBufferEXT", userptr);
    glad_glGetNamedProgramLocalParameterdvEXT = (PFNGLGETNAMEDPROGRAMLOCALPARAMETERDVEXTPROC)load("glGetNamedProgramLocalParameterdvEXT", userptr);
    glad_glTextureStorage2DEXT = (PFNGLTEXTURESTORAGE2DEXTPROC)load("glTextureStorage2DEXT", userptr);
    glad_glVertexArrayFogCoordOffsetEXT = (PFNGLVERTEXARRAYFOGCOORDOFFSETEXTPROC)load("glVertexArrayFogCoordOffsetEXT", userptr);
    glad_glProgramUniform4uiEXT = (PFNGLPROGRAMUNIFORM4UIEXTPROC)load("glProgramUniform4uiEXT", userptr);
    glad_glGetNamedProgramStringEXT = (PFNGLGETNAMEDPROGRAMSTRINGEXTPROC)load("glGetNamedProgramStringEXT", userptr);
    glad_glGetMultiTexGenfvEXT = (PFNGLGETMULTITEXGENFVEXTPROC)load("glGetMultiTexGenfvEXT", userptr);
    glad_glClientAttribDefaultEXT = (PFNGLCLIENTATTRIBDEFAULTEXTPROC)load("glClientAttribDefaultEXT", userptr);
    glad_glMatrixFrustumEXT = (PFNGLMATRIXFRUSTUMEXTPROC)load("glMatrixFrustumEXT", userptr);
    glad_glProgramUniform2uiEXT = (PFNGLPROGRAMUNIFORM2UIEXTPROC)load("glProgramUniform2uiEXT", userptr);
    glad_glCompressedMultiTexImage3DEXT = (PFNGLCOMPRESSEDMULTITEXIMAGE3DEXTPROC)load("glCompressedMultiTexImage3DEXT", userptr);
    glad_glProgramUniformMatrix4x3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC)load("glProgramUniformMatrix4x3fvEXT", userptr);
    glad_glProgramUniform3dEXT = (PFNGLPROGRAMUNIFORM3DEXTPROC)load("glProgramUniform3dEXT", userptr);
    glad_glProgramUniform3uivEXT = (PFNGLPROGRAMUNIFORM3UIVEXTPROC)load("glProgramUniform3uivEXT", userptr);
    glad_glGetMultiTexLevelParameterfvEXT = (PFNGLGETMULTITEXLEVELPARAMETERFVEXTPROC)load("glGetMultiTexLevelParameterfvEXT", userptr);
    glad_glTextureImage3DEXT = (PFNGLTEXTUREIMAGE3DEXTPROC)load("glTextureImage3DEXT", userptr);
    glad_glIsEnabledIndexedEXT = (PFNGLISENABLEDINDEXEDEXTPROC)load("glIsEnabledIndexedEXT", userptr);
    glad_glGetNamedProgramivEXT = (PFNGLGETNAMEDPROGRAMIVEXTPROC)load("glGetNamedProgramivEXT", userptr);
    glad_glProgramUniform3fvEXT = (PFNGLPROGRAMUNIFORM3FVEXTPROC)load("glProgramUniform3fvEXT", userptr);
    glad_glVertexArraySecondaryColorOffsetEXT = (PFNGLVERTEXARRAYSECONDARYCOLOROFFSETEXTPROC)load("glVertexArraySecondaryColorOffsetEXT", userptr);
    glad_glGetNamedProgramLocalParameterIuivEXT = (PFNGLGETNAMEDPROGRAMLOCALPARAMETERIUIVEXTPROC)load("glGetNamedProgramLocalParameterIuivEXT", userptr);
    glad_glUnmapNamedBufferEXT = (PFNGLUNMAPNAMEDBUFFEREXTPROC)load("glUnmapNamedBufferEXT", userptr);
    glad_glGetVertexArrayPointervEXT = (PFNGLGETVERTEXARRAYPOINTERVEXTPROC)load("glGetVertexArrayPointervEXT", userptr);
    glad_glEnableClientStateIndexedEXT = (PFNGLENABLECLIENTSTATEINDEXEDEXTPROC)load("glEnableClientStateIndexedEXT", userptr);
    glad_glCompressedMultiTexImage1DEXT = (PFNGLCOMPRESSEDMULTITEXIMAGE1DEXTPROC)load("glCompressedMultiTexImage1DEXT", userptr);
    glad_glMatrixLoadIdentityEXT = (PFNGLMATRIXLOADIDENTITYEXTPROC)load("glMatrixLoadIdentityEXT", userptr);
    glad_glGetFloati_vEXT = (PFNGLGETFLOATI_VEXTPROC)load("glGetFloati_vEXT", userptr);
    glad_glTextureSubImage1DEXT = (PFNGLTEXTURESUBIMAGE1DEXTPROC)load("glTextureSubImage1DEXT", userptr);
    glad_glMultiTexEnvivEXT = (PFNGLMULTITEXENVIVEXTPROC)load("glMultiTexEnvivEXT", userptr);
    glad_glCopyTextureImage2DEXT = (PFNGLCOPYTEXTUREIMAGE2DEXTPROC)load("glCopyTextureImage2DEXT", userptr);
    glad_glCompressedTextureImage2DEXT = (PFNGLCOMPRESSEDTEXTUREIMAGE2DEXTPROC)load("glCompressedTextureImage2DEXT", userptr);
    glad_glNamedProgramLocalParameterI4iEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERI4IEXTPROC)load("glNamedProgramLocalParameterI4iEXT", userptr);
    glad_glGetVertexArrayPointeri_vEXT = (PFNGLGETVERTEXARRAYPOINTERI_VEXTPROC)load("glGetVertexArrayPointeri_vEXT", userptr);
    glad_glNamedProgramLocalParameter4dEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETER4DEXTPROC)load("glNamedProgramLocalParameter4dEXT", userptr);
    glad_glFramebufferDrawBufferEXT = (PFNGLFRAMEBUFFERDRAWBUFFEREXTPROC)load("glFramebufferDrawBufferEXT", userptr);
    glad_glCopyMultiTexSubImage3DEXT = (PFNGLCOPYMULTITEXSUBIMAGE3DEXTPROC)load("glCopyMultiTexSubImage3DEXT", userptr);
    glad_glProgramUniformMatrix2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC)load("glProgramUniformMatrix2fvEXT", userptr);
    glad_glGetTextureParameterIuivEXT = (PFNGLGETTEXTUREPARAMETERIUIVEXTPROC)load("glGetTextureParameterIuivEXT", userptr);
    glad_glGetMultiTexEnvivEXT = (PFNGLGETMULTITEXENVIVEXTPROC)load("glGetMultiTexEnvivEXT", userptr);
    glad_glProgramUniform1ivEXT = (PFNGLPROGRAMUNIFORM1IVEXTPROC)load("glProgramUniform1ivEXT", userptr);
    glad_glCompressedTextureSubImage1DEXT = (PFNGLCOMPRESSEDTEXTURESUBIMAGE1DEXTPROC)load("glCompressedTextureSubImage1DEXT", userptr);
    glad_glMultiTexGenivEXT = (PFNGLMULTITEXGENIVEXTPROC)load("glMultiTexGenivEXT", userptr);
    glad_glNamedFramebufferTexture2DEXT = (PFNGLNAMEDFRAMEBUFFERTEXTURE2DEXTPROC)load("glNamedFramebufferTexture2DEXT", userptr);
    glad_glTextureImage2DEXT = (PFNGLTEXTUREIMAGE2DEXTPROC)load("glTextureImage2DEXT", userptr);
    glad_glProgramUniform4dvEXT = (PFNGLPROGRAMUNIFORM4DVEXTPROC)load("glProgramUniform4dvEXT", userptr);
    glad_glTextureStorage3DMultisampleEXT = (PFNGLTEXTURESTORAGE3DMULTISAMPLEEXTPROC)load("glTextureStorage3DMultisampleEXT", userptr);
    glad_glVertexArrayNormalOffsetEXT = (PFNGLVERTEXARRAYNORMALOFFSETEXTPROC)load("glVertexArrayNormalOffsetEXT", userptr);
    glad_glProgramUniformMatrix2x4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC)load("glProgramUniformMatrix2x4fvEXT", userptr);
    glad_glProgramUniform2ivEXT = (PFNGLPROGRAMUNIFORM2IVEXTPROC)load("glProgramUniform2ivEXT", userptr);
    glad_glMatrixTranslatedEXT = (PFNGLMATRIXTRANSLATEDEXTPROC)load("glMatrixTranslatedEXT", userptr);
    glad_glProgramUniform4fvEXT = (PFNGLPROGRAMUNIFORM4FVEXTPROC)load("glProgramUniform4fvEXT", userptr);
    glad_glMultiTexCoordPointerEXT = (PFNGLMULTITEXCOORDPOINTEREXTPROC)load("glMultiTexCoordPointerEXT", userptr);
    glad_glGenerateMultiTexMipmapEXT = (PFNGLGENERATEMULTITEXMIPMAPEXTPROC)load("glGenerateMultiTexMipmapEXT", userptr);
    glad_glNamedFramebufferRenderbufferEXT = (PFNGLNAMEDFRAMEBUFFERRENDERBUFFEREXTPROC)load("glNamedFramebufferRenderbufferEXT", userptr);
    glad_glVertexArrayTexCoordOffsetEXT = (PFNGLVERTEXARRAYTEXCOORDOFFSETEXTPROC)load("glVertexArrayTexCoordOffsetEXT", userptr);
    glad_glNamedFramebufferTexture1DEXT = (PFNGLNAMEDFRAMEBUFFERTEXTURE1DEXTPROC)load("glNamedFramebufferTexture1DEXT", userptr);
    glad_glMultiTexEnvfEXT = (PFNGLMULTITEXENVFEXTPROC)load("glMultiTexEnvfEXT", userptr);
    glad_glProgramUniformMatrix4x2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC)load("glProgramUniformMatrix4x2fvEXT", userptr);
    glad_glNamedProgramLocalParametersI4uivEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERSI4UIVEXTPROC)load("glNamedProgramLocalParametersI4uivEXT", userptr);
    glad_glGetBooleanIndexedvEXT = (PFNGLGETBOOLEANINDEXEDVEXTPROC)load("glGetBooleanIndexedvEXT", userptr);
    glad_glMatrixLoadTransposedEXT = (PFNGLMATRIXLOADTRANSPOSEDEXTPROC)load("glMatrixLoadTransposedEXT", userptr);
    glad_glGetPointeri_vEXT = (PFNGLGETPOINTERI_VEXTPROC)load("glGetPointeri_vEXT", userptr);
    glad_glCopyMultiTexImage2DEXT = (PFNGLCOPYMULTITEXIMAGE2DEXTPROC)load("glCopyMultiTexImage2DEXT", userptr);
    glad_glNamedProgramLocalParameter4fvEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETER4FVEXTPROC)load("glNamedProgramLocalParameter4fvEXT", userptr);
    glad_glMatrixRotatefEXT = (PFNGLMATRIXROTATEFEXTPROC)load("glMatrixRotatefEXT", userptr);
    glad_glGetCompressedMultiTexImageEXT = (PFNGLGETCOMPRESSEDMULTITEXIMAGEEXTPROC)load("glGetCompressedMultiTexImageEXT", userptr);
    glad_glMapNamedBufferEXT = (PFNGLMAPNAMEDBUFFEREXTPROC)load("glMapNamedBufferEXT", userptr);
    glad_glProgramUniformMatrix2x3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC)load("glProgramUniformMatrix2x3fvEXT", userptr);
    glad_glGetMultiTexEnvfvEXT = (PFNGLGETMULTITEXENVFVEXTPROC)load("glGetMultiTexEnvfvEXT", userptr);
    glad_glProgramUniformMatrix4dvEXT = (PFNGLPROGRAMUNIFORMMATRIX4DVEXTPROC)load("glProgramUniformMatrix4dvEXT", userptr);
    glad_glMultiTexSubImage2DEXT = (PFNGLMULTITEXSUBIMAGE2DEXTPROC)load("glMultiTexSubImage2DEXT", userptr);
    glad_glProgramUniform4dEXT = (PFNGLPROGRAMUNIFORM4DEXTPROC)load("glProgramUniform4dEXT", userptr);
    glad_glProgramUniform1dEXT = (PFNGLPROGRAMUNIFORM1DEXTPROC)load("glProgramUniform1dEXT", userptr);
    glad_glVertexArrayVertexAttribBindingEXT = (PFNGLVERTEXARRAYVERTEXATTRIBBINDINGEXTPROC)load("glVertexArrayVertexAttribBindingEXT", userptr);
    glad_glMultiTexParameterfvEXT = (PFNGLMULTITEXPARAMETERFVEXTPROC)load("glMultiTexParameterfvEXT", userptr);
    glad_glNamedProgramLocalParameter4dvEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETER4DVEXTPROC)load("glNamedProgramLocalParameter4dvEXT", userptr);
    glad_glTextureImage1DEXT = (PFNGLTEXTUREIMAGE1DEXTPROC)load("glTextureImage1DEXT", userptr);
    glad_glTextureParameteriEXT = (PFNGLTEXTUREPARAMETERIEXTPROC)load("glTextureParameteriEXT", userptr);
    glad_glMatrixScalefEXT = (PFNGLMATRIXSCALEFEXTPROC)load("glMatrixScalefEXT", userptr);
    glad_glClearNamedBufferDataEXT = (PFNGLCLEARNAMEDBUFFERDATAEXTPROC)load("glClearNamedBufferDataEXT", userptr);
    glad_glGetTextureParameterfvEXT = (PFNGLGETTEXTUREPARAMETERFVEXTPROC)load("glGetTextureParameterfvEXT", userptr);
    glad_glGetTextureLevelParameterfvEXT = (PFNGLGETTEXTURELEVELPARAMETERFVEXTPROC)load("glGetTextureLevelParameterfvEXT", userptr);
    glad_glVertexArrayVertexAttribLOffsetEXT = (PFNGLVERTEXARRAYVERTEXATTRIBLOFFSETEXTPROC)load("glVertexArrayVertexAttribLOffsetEXT", userptr);
    glad_glTexturePageCommitmentEXT = (PFNGLTEXTUREPAGECOMMITMENTEXTPROC)load("glTexturePageCommitmentEXT", userptr);
    glad_glMultiTexEnvfvEXT = (PFNGLMULTITEXENVFVEXTPROC)load("glMultiTexEnvfvEXT", userptr);
    glad_glTextureParameterIivEXT = (PFNGLTEXTUREPARAMETERIIVEXTPROC)load("glTextureParameterIivEXT", userptr);
    glad_glNamedCopyBufferSubDataEXT = (PFNGLNAMEDCOPYBUFFERSUBDATAEXTPROC)load("glNamedCopyBufferSubDataEXT", userptr);
    glad_glMatrixMultTransposefEXT = (PFNGLMATRIXMULTTRANSPOSEFEXTPROC)load("glMatrixMultTransposefEXT", userptr);
    glad_glMultiTexEnviEXT = (PFNGLMULTITEXENVIEXTPROC)load("glMultiTexEnviEXT", userptr);
    glad_glCopyTextureSubImage1DEXT = (PFNGLCOPYTEXTURESUBIMAGE1DEXTPROC)load("glCopyTextureSubImage1DEXT", userptr);
    glad_glMatrixLoaddEXT = (PFNGLMATRIXLOADDEXTPROC)load("glMatrixLoaddEXT", userptr);
    glad_glGetMultiTexLevelParameterivEXT = (PFNGLGETMULTITEXLEVELPARAMETERIVEXTPROC)load("glGetMultiTexLevelParameterivEXT", userptr);
    glad_glCompressedTextureSubImage2DEXT = (PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXTPROC)load("glCompressedTextureSubImage2DEXT", userptr);
    glad_glTextureSubImage2DEXT = (PFNGLTEXTURESUBIMAGE2DEXTPROC)load("glTextureSubImage2DEXT", userptr);
    glad_glProgramUniformMatrix2x3dvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X3DVEXTPROC)load("glProgramUniformMatrix2x3dvEXT", userptr);
    glad_glEnableIndexedEXT = (PFNGLENABLEINDEXEDEXTPROC)load("glEnableIndexedEXT", userptr);
    glad_glTextureParameterfvEXT = (PFNGLTEXTUREPARAMETERFVEXTPROC)load("glTextureParameterfvEXT", userptr);
    glad_glGetNamedProgramLocalParameterIivEXT = (PFNGLGETNAMEDPROGRAMLOCALPARAMETERIIVEXTPROC)load("glGetNamedProgramLocalParameterIivEXT", userptr);
    glad_glProgramUniform3uiEXT = (PFNGLPROGRAMUNIFORM3UIEXTPROC)load("glProgramUniform3uiEXT", userptr);
    glad_glVertexArrayVertexAttribLFormatEXT = (PFNGLVERTEXARRAYVERTEXATTRIBLFORMATEXTPROC)load("glVertexArrayVertexAttribLFormatEXT", userptr);
    glad_glCopyMultiTexSubImage1DEXT = (PFNGLCOPYMULTITEXSUBIMAGE1DEXTPROC)load("glCopyMultiTexSubImage1DEXT", userptr);
    glad_glProgramUniformMatrix4x2dvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X2DVEXTPROC)load("glProgramUniformMatrix4x2dvEXT", userptr);
    glad_glProgramUniformMatrix3x2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC)load("glProgramUniformMatrix3x2fvEXT", userptr);
    glad_glVertexArrayVertexAttribIOffsetEXT = (PFNGLVERTEXARRAYVERTEXATTRIBIOFFSETEXTPROC)load("glVertexArrayVertexAttribIOffsetEXT", userptr);
    glad_glNamedProgramLocalParameterI4uivEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIVEXTPROC)load("glNamedProgramLocalParameterI4uivEXT", userptr);
    glad_glProgramUniform3fEXT = (PFNGLPROGRAMUNIFORM3FEXTPROC)load("glProgramUniform3fEXT", userptr);
    glad_glProgramUniform3dvEXT = (PFNGLPROGRAMUNIFORM3DVEXTPROC)load("glProgramUniform3dvEXT", userptr);
    glad_glNamedFramebufferTextureEXT = (PFNGLNAMEDFRAMEBUFFERTEXTUREEXTPROC)load("glNamedFramebufferTextureEXT", userptr);
    glad_glMultiTexImage2DEXT = (PFNGLMULTITEXIMAGE2DEXTPROC)load("glMultiTexImage2DEXT", userptr);
    glad_glGetMultiTexParameterfvEXT = (PFNGLGETMULTITEXPARAMETERFVEXTPROC)load("glGetMultiTexParameterfvEXT", userptr);
    glad_glProgramUniform4ivEXT = (PFNGLPROGRAMUNIFORM4IVEXTPROC)load("glProgramUniform4ivEXT", userptr);
    glad_glProgramUniform1fvEXT = (PFNGLPROGRAMUNIFORM1FVEXTPROC)load("glProgramUniform1fvEXT", userptr);
    glad_glCopyMultiTexImage1DEXT = (PFNGLCOPYMULTITEXIMAGE1DEXTPROC)load("glCopyMultiTexImage1DEXT", userptr);
    glad_glVertexArrayVertexOffsetEXT = (PFNGLVERTEXARRAYVERTEXOFFSETEXTPROC)load("glVertexArrayVertexOffsetEXT", userptr);
    glad_glNamedFramebufferParameteriEXT = (PFNGLNAMEDFRAMEBUFFERPARAMETERIEXTPROC)load("glNamedFramebufferParameteriEXT", userptr);
    glad_glTextureBufferRangeEXT = (PFNGLTEXTUREBUFFERRANGEEXTPROC)load("glTextureBufferRangeEXT", userptr);
    glad_glProgramUniformMatrix3dvEXT = (PFNGLPROGRAMUNIFORMMATRIX3DVEXTPROC)load("glProgramUniformMatrix3dvEXT", userptr);
    glad_glCompressedMultiTexSubImage2DEXT = (PFNGLCOMPRESSEDMULTITEXSUBIMAGE2DEXTPROC)load("glCompressedMultiTexSubImage2DEXT", userptr);
    glad_glDisableIndexedEXT = (PFNGLDISABLEINDEXEDEXTPROC)load("glDisableIndexedEXT", userptr);
    glad_glMultiTexParameterfEXT = (PFNGLMULTITEXPARAMETERFEXTPROC)load("glMultiTexParameterfEXT", userptr);
    glad_glGetVertexArrayIntegervEXT = (PFNGLGETVERTEXARRAYINTEGERVEXTPROC)load("glGetVertexArrayIntegervEXT", userptr);
    glad_glTextureSubImage3DEXT = (PFNGLTEXTURESUBIMAGE3DEXTPROC)load("glTextureSubImage3DEXT", userptr);
    glad_glTextureBufferEXT = (PFNGLTEXTUREBUFFEREXTPROC)load("glTextureBufferEXT", userptr);
    glad_glGetMultiTexImageEXT = (PFNGLGETMULTITEXIMAGEEXTPROC)load("glGetMultiTexImageEXT", userptr);
    glad_glProgramUniform1uiEXT = (PFNGLPROGRAMUNIFORM1UIEXTPROC)load("glProgramUniform1uiEXT", userptr);
}
static void load_GL_ARB_program_interface_query(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_program_interface_query) return;
    glad_glGetProgramResourceiv = (PFNGLGETPROGRAMRESOURCEIVPROC)load("glGetProgramResourceiv", userptr);
    glad_glGetProgramResourceIndex = (PFNGLGETPROGRAMRESOURCEINDEXPROC)load("glGetProgramResourceIndex", userptr);
    glad_glGetProgramResourceName = (PFNGLGETPROGRAMRESOURCENAMEPROC)load("glGetProgramResourceName", userptr);
    glad_glGetProgramResourceLocation = (PFNGLGETPROGRAMRESOURCELOCATIONPROC)load("glGetProgramResourceLocation", userptr);
    glad_glGetProgramResourceLocationIndex = (PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC)load("glGetProgramResourceLocationIndex", userptr);
    glad_glGetProgramInterfaceiv = (PFNGLGETPROGRAMINTERFACEIVPROC)load("glGetProgramInterfaceiv", userptr);
}
static void load_GL_ARB_copy_image(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_copy_image) return;
    glad_glCopyImageSubData = (PFNGLCOPYIMAGESUBDATAPROC)load("glCopyImageSubData", userptr);
}
static void load_GL_EXT_texture_object(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_texture_object) return;
    glad_glBindTextureEXT = (PFNGLBINDTEXTUREEXTPROC)load("glBindTextureEXT", userptr);
    glad_glPrioritizeTexturesEXT = (PFNGLPRIORITIZETEXTURESEXTPROC)load("glPrioritizeTexturesEXT", userptr);
    glad_glDeleteTexturesEXT = (PFNGLDELETETEXTURESEXTPROC)load("glDeleteTexturesEXT", userptr);
    glad_glIsTextureEXT = (PFNGLISTEXTUREEXTPROC)load("glIsTextureEXT", userptr);
    glad_glGenTexturesEXT = (PFNGLGENTEXTURESEXTPROC)load("glGenTexturesEXT", userptr);
    glad_glAreTexturesResidentEXT = (PFNGLARETEXTURESRESIDENTEXTPROC)load("glAreTexturesResidentEXT", userptr);
}
static void load_GL_NVX_conditional_render(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_NVX_conditional_render) return;
    glad_glBeginConditionalRenderNVX = (PFNGLBEGINCONDITIONALRENDERNVXPROC)load("glBeginConditionalRenderNVX", userptr);
    glad_glEndConditionalRenderNVX = (PFNGLENDCONDITIONALRENDERNVXPROC)load("glEndConditionalRenderNVX", userptr);
}
static void load_GL_ARB_copy_buffer(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_copy_buffer) return;
    glad_glCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC)load("glCopyBufferSubData", userptr);
}
static void load_GL_EXT_blend_func_separate(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_blend_func_separate) return;
    glad_glBlendFuncSeparateEXT = (PFNGLBLENDFUNCSEPARATEEXTPROC)load("glBlendFuncSeparateEXT", userptr);
}
static void load_GL_ARB_buffer_storage(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_buffer_storage) return;
    glad_glBufferStorage = (PFNGLBUFFERSTORAGEPROC)load("glBufferStorage", userptr);
}
static void load_GL_ARB_occlusion_query(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_occlusion_query) return;
    glad_glGetQueryObjectuivARB = (PFNGLGETQUERYOBJECTUIVARBPROC)load("glGetQueryObjectuivARB", userptr);
    glad_glGetQueryivARB = (PFNGLGETQUERYIVARBPROC)load("glGetQueryivARB", userptr);
    glad_glGenQueriesARB = (PFNGLGENQUERIESARBPROC)load("glGenQueriesARB", userptr);
    glad_glGetQueryObjectivARB = (PFNGLGETQUERYOBJECTIVARBPROC)load("glGetQueryObjectivARB", userptr);
    glad_glEndQueryARB = (PFNGLENDQUERYARBPROC)load("glEndQueryARB", userptr);
    glad_glIsQueryARB = (PFNGLISQUERYARBPROC)load("glIsQueryARB", userptr);
    glad_glBeginQueryARB = (PFNGLBEGINQUERYARBPROC)load("glBeginQueryARB", userptr);
    glad_glDeleteQueriesARB = (PFNGLDELETEQUERIESARBPROC)load("glDeleteQueriesARB", userptr);
}
static void load_GL_ARB_multi_bind(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_multi_bind) return;
    glad_glBindImageTextures = (PFNGLBINDIMAGETEXTURESPROC)load("glBindImageTextures", userptr);
    glad_glBindTextures = (PFNGLBINDTEXTURESPROC)load("glBindTextures", userptr);
    glad_glBindSamplers = (PFNGLBINDSAMPLERSPROC)load("glBindSamplers", userptr);
    glad_glBindBuffersBase = (PFNGLBINDBUFFERSBASEPROC)load("glBindBuffersBase", userptr);
    glad_glBindBuffersRange = (PFNGLBINDBUFFERSRANGEPROC)load("glBindBuffersRange", userptr);
    glad_glBindVertexBuffers = (PFNGLBINDVERTEXBUFFERSPROC)load("glBindVertexBuffers", userptr);
}
static void load_GL_KHR_robustness(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_KHR_robustness) return;
    glad_glGetnUniformuiv = (PFNGLGETNUNIFORMUIVPROC)load("glGetnUniformuiv", userptr);
    glad_glGetnUniformfv = (PFNGLGETNUNIFORMFVPROC)load("glGetnUniformfv", userptr);
    glad_glGetGraphicsResetStatus = (PFNGLGETGRAPHICSRESETSTATUSPROC)load("glGetGraphicsResetStatus", userptr);
    glad_glGetnUniformiv = (PFNGLGETNUNIFORMIVPROC)load("glGetnUniformiv", userptr);
    glad_glReadnPixels = (PFNGLREADNPIXELSPROC)load("glReadnPixels", userptr);
}
static void load_GL_ARB_provoking_vertex(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_provoking_vertex) return;
    glad_glProvokingVertex = (PFNGLPROVOKINGVERTEXPROC)load("glProvokingVertex", userptr);
}
static void load_GL_EXT_gpu_shader4(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_gpu_shader4) return;
    glad_glUniform3uiEXT = (PFNGLUNIFORM3UIEXTPROC)load("glUniform3uiEXT", userptr);
    glad_glUniform2uiEXT = (PFNGLUNIFORM2UIEXTPROC)load("glUniform2uiEXT", userptr);
    glad_glGetFragDataLocationEXT = (PFNGLGETFRAGDATALOCATIONEXTPROC)load("glGetFragDataLocationEXT", userptr);
    glad_glUniform2uivEXT = (PFNGLUNIFORM2UIVEXTPROC)load("glUniform2uivEXT", userptr);
    glad_glUniform4uivEXT = (PFNGLUNIFORM4UIVEXTPROC)load("glUniform4uivEXT", userptr);
    glad_glBindFragDataLocationEXT = (PFNGLBINDFRAGDATALOCATIONEXTPROC)load("glBindFragDataLocationEXT", userptr);
    glad_glUniform4uiEXT = (PFNGLUNIFORM4UIEXTPROC)load("glUniform4uiEXT", userptr);
    glad_glUniform1uivEXT = (PFNGLUNIFORM1UIVEXTPROC)load("glUniform1uivEXT", userptr);
    glad_glGetUniformuivEXT = (PFNGLGETUNIFORMUIVEXTPROC)load("glGetUniformuivEXT", userptr);
    glad_glUniform1uiEXT = (PFNGLUNIFORM1UIEXTPROC)load("glUniform1uiEXT", userptr);
    glad_glUniform3uivEXT = (PFNGLUNIFORM3UIVEXTPROC)load("glUniform3uivEXT", userptr);
}
static void load_GL_NV_geometry_program4(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_NV_geometry_program4) return;
    glad_glFramebufferTextureEXT = (PFNGLFRAMEBUFFERTEXTUREEXTPROC)load("glFramebufferTextureEXT", userptr);
    glad_glProgramVertexLimitNV = (PFNGLPROGRAMVERTEXLIMITNVPROC)load("glProgramVertexLimitNV", userptr);
    glad_glFramebufferTextureFaceEXT = (PFNGLFRAMEBUFFERTEXTUREFACEEXTPROC)load("glFramebufferTextureFaceEXT", userptr);
    glad_glFramebufferTextureLayerEXT = (PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC)load("glFramebufferTextureLayerEXT", userptr);
}
static void load_GL_NV_path_rendering_shared_edge(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_NV_path_rendering_shared_edge) return;
}
static void load_GL_NV_uniform_buffer_unified_memory(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_NV_uniform_buffer_unified_memory) return;
}
static void load_GL_ARB_internalformat_query2(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_internalformat_query2) return;
    glad_glGetInternalformati64v = (PFNGLGETINTERNALFORMATI64VPROC)load("glGetInternalformati64v", userptr);
}
static void load_GL_NV_shader_buffer_load(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_NV_shader_buffer_load) return;
    glad_glGetNamedBufferParameterui64vNV = (PFNGLGETNAMEDBUFFERPARAMETERUI64VNVPROC)load("glGetNamedBufferParameterui64vNV", userptr);
    glad_glMakeNamedBufferResidentNV = (PFNGLMAKENAMEDBUFFERRESIDENTNVPROC)load("glMakeNamedBufferResidentNV", userptr);
    glad_glGetBufferParameterui64vNV = (PFNGLGETBUFFERPARAMETERUI64VNVPROC)load("glGetBufferParameterui64vNV", userptr);
    glad_glGetUniformui64vNV = (PFNGLGETUNIFORMUI64VNVPROC)load("glGetUniformui64vNV", userptr);
    glad_glUniformui64vNV = (PFNGLUNIFORMUI64VNVPROC)load("glUniformui64vNV", userptr);
    glad_glUniformui64NV = (PFNGLUNIFORMUI64NVPROC)load("glUniformui64NV", userptr);
    glad_glProgramUniformui64vNV = (PFNGLPROGRAMUNIFORMUI64VNVPROC)load("glProgramUniformui64vNV", userptr);
    glad_glGetIntegerui64vNV = (PFNGLGETINTEGERUI64VNVPROC)load("glGetIntegerui64vNV", userptr);
    glad_glIsNamedBufferResidentNV = (PFNGLISNAMEDBUFFERRESIDENTNVPROC)load("glIsNamedBufferResidentNV", userptr);
    glad_glMakeBufferResidentNV = (PFNGLMAKEBUFFERRESIDENTNVPROC)load("glMakeBufferResidentNV", userptr);
    glad_glMakeNamedBufferNonResidentNV = (PFNGLMAKENAMEDBUFFERNONRESIDENTNVPROC)load("glMakeNamedBufferNonResidentNV", userptr);
    glad_glMakeBufferNonResidentNV = (PFNGLMAKEBUFFERNONRESIDENTNVPROC)load("glMakeBufferNonResidentNV", userptr);
    glad_glProgramUniformui64NV = (PFNGLPROGRAMUNIFORMUI64NVPROC)load("glProgramUniformui64NV", userptr);
    glad_glIsBufferResidentNV = (PFNGLISBUFFERRESIDENTNVPROC)load("glIsBufferResidentNV", userptr);
}
static void load_GL_EXT_vertex_array(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_vertex_array) return;
    glad_glEdgeFlagPointerEXT = (PFNGLEDGEFLAGPOINTEREXTPROC)load("glEdgeFlagPointerEXT", userptr);
    glad_glVertexPointerEXT = (PFNGLVERTEXPOINTEREXTPROC)load("glVertexPointerEXT", userptr);
    glad_glNormalPointerEXT = (PFNGLNORMALPOINTEREXTPROC)load("glNormalPointerEXT", userptr);
    glad_glDrawArraysEXT = (PFNGLDRAWARRAYSEXTPROC)load("glDrawArraysEXT", userptr);
    glad_glTexCoordPointerEXT = (PFNGLTEXCOORDPOINTEREXTPROC)load("glTexCoordPointerEXT", userptr);
    glad_glGetPointervEXT = (PFNGLGETPOINTERVEXTPROC)load("glGetPointervEXT", userptr);
    glad_glIndexPointerEXT = (PFNGLINDEXPOINTEREXTPROC)load("glIndexPointerEXT", userptr);
    glad_glArrayElementEXT = (PFNGLARRAYELEMENTEXTPROC)load("glArrayElementEXT", userptr);
    glad_glColorPointerEXT = (PFNGLCOLORPOINTEREXTPROC)load("glColorPointerEXT", userptr);
}
static void load_GL_NV_vertex_buffer_unified_memory(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_NV_vertex_buffer_unified_memory) return;
    glad_glTexCoordFormatNV = (PFNGLTEXCOORDFORMATNVPROC)load("glTexCoordFormatNV", userptr);
    glad_glColorFormatNV = (PFNGLCOLORFORMATNVPROC)load("glColorFormatNV", userptr);
    glad_glGetIntegerui64i_vNV = (PFNGLGETINTEGERUI64I_VNVPROC)load("glGetIntegerui64i_vNV", userptr);
    glad_glEdgeFlagFormatNV = (PFNGLEDGEFLAGFORMATNVPROC)load("glEdgeFlagFormatNV", userptr);
    glad_glFogCoordFormatNV = (PFNGLFOGCOORDFORMATNVPROC)load("glFogCoordFormatNV", userptr);
    glad_glVertexFormatNV = (PFNGLVERTEXFORMATNVPROC)load("glVertexFormatNV", userptr);
    glad_glIndexFormatNV = (PFNGLINDEXFORMATNVPROC)load("glIndexFormatNV", userptr);
    glad_glSecondaryColorFormatNV = (PFNGLSECONDARYCOLORFORMATNVPROC)load("glSecondaryColorFormatNV", userptr);
    glad_glNormalFormatNV = (PFNGLNORMALFORMATNVPROC)load("glNormalFormatNV", userptr);
    glad_glVertexAttribFormatNV = (PFNGLVERTEXATTRIBFORMATNVPROC)load("glVertexAttribFormatNV", userptr);
    glad_glVertexAttribIFormatNV = (PFNGLVERTEXATTRIBIFORMATNVPROC)load("glVertexAttribIFormatNV", userptr);
    glad_glBufferAddressRangeNV = (PFNGLBUFFERADDRESSRANGENVPROC)load("glBufferAddressRangeNV", userptr);
}
static void load_GL_EXT_blend_equation_separate(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_blend_equation_separate) return;
    glad_glBlendEquationSeparateEXT = (PFNGLBLENDEQUATIONSEPARATEEXTPROC)load("glBlendEquationSeparateEXT", userptr);
}
static void load_GL_EXT_provoking_vertex(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_provoking_vertex) return;
    glad_glProvokingVertexEXT = (PFNGLPROVOKINGVERTEXEXTPROC)load("glProvokingVertexEXT", userptr);
}
static void load_GL_ARB_point_parameters(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_point_parameters) return;
    glad_glPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC)load("glPointParameterfvARB", userptr);
    glad_glPointParameterfARB = (PFNGLPOINTPARAMETERFARBPROC)load("glPointParameterfARB", userptr);
}
static void load_GL_ARB_clear_texture(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_clear_texture) return;
    glad_glClearTexImage = (PFNGLCLEARTEXIMAGEPROC)load("glClearTexImage", userptr);
    glad_glClearTexSubImage = (PFNGLCLEARTEXSUBIMAGEPROC)load("glClearTexSubImage", userptr);
}
static void load_GL_ARB_shader_image_load_store(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_shader_image_load_store) return;
    glad_glMemoryBarrier = (PFNGLMEMORYBARRIERPROC)load("glMemoryBarrier", userptr);
    glad_glBindImageTexture = (PFNGLBINDIMAGETEXTUREPROC)load("glBindImageTexture", userptr);
}
static void load_GL_EXT_multi_draw_arrays(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_multi_draw_arrays) return;
    glad_glMultiDrawElementsEXT = (PFNGLMULTIDRAWELEMENTSEXTPROC)load("glMultiDrawElementsEXT", userptr);
    glad_glMultiDrawArraysEXT = (PFNGLMULTIDRAWARRAYSEXTPROC)load("glMultiDrawArraysEXT", userptr);
}
static void load_GL_ARB_shader_atomic_counters(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_shader_atomic_counters) return;
    glad_glGetActiveAtomicCounterBufferiv = (PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC)load("glGetActiveAtomicCounterBufferiv", userptr);
}
static void load_GL_ARB_texture_barrier(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_texture_barrier) return;
    glad_glTextureBarrier = (PFNGLTEXTUREBARRIERPROC)load("glTextureBarrier", userptr);
}
static void load_GL_NV_bindless_multi_draw_indirect(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_NV_bindless_multi_draw_indirect) return;
    glad_glMultiDrawElementsIndirectBindlessNV = (PFNGLMULTIDRAWELEMENTSINDIRECTBINDLESSNVPROC)load("glMultiDrawElementsIndirectBindlessNV", userptr);
    glad_glMultiDrawArraysIndirectBindlessNV = (PFNGLMULTIDRAWARRAYSINDIRECTBINDLESSNVPROC)load("glMultiDrawArraysIndirectBindlessNV", userptr);
}
static void load_GL_EXT_transform_feedback(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_transform_feedback) return;
    glad_glBeginTransformFeedbackEXT = (PFNGLBEGINTRANSFORMFEEDBACKEXTPROC)load("glBeginTransformFeedbackEXT", userptr);
    glad_glBindBufferRangeEXT = (PFNGLBINDBUFFERRANGEEXTPROC)load("glBindBufferRangeEXT", userptr);
    glad_glBindBufferOffsetEXT = (PFNGLBINDBUFFEROFFSETEXTPROC)load("glBindBufferOffsetEXT", userptr);
    glad_glBindBufferBaseEXT = (PFNGLBINDBUFFERBASEEXTPROC)load("glBindBufferBaseEXT", userptr);
    glad_glEndTransformFeedbackEXT = (PFNGLENDTRANSFORMFEEDBACKEXTPROC)load("glEndTransformFeedbackEXT", userptr);
    glad_glGetTransformFeedbackVaryingEXT = (PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC)load("glGetTransformFeedbackVaryingEXT", userptr);
    glad_glTransformFeedbackVaryingsEXT = (PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC)load("glTransformFeedbackVaryingsEXT", userptr);
}
static void load_GL_ARB_transform_feedback2(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_transform_feedback2) return;
    glad_glDeleteTransformFeedbacks = (PFNGLDELETETRANSFORMFEEDBACKSPROC)load("glDeleteTransformFeedbacks", userptr);
    glad_glResumeTransformFeedback = (PFNGLRESUMETRANSFORMFEEDBACKPROC)load("glResumeTransformFeedback", userptr);
    glad_glBindTransformFeedback = (PFNGLBINDTRANSFORMFEEDBACKPROC)load("glBindTransformFeedback", userptr);
    glad_glDrawTransformFeedback = (PFNGLDRAWTRANSFORMFEEDBACKPROC)load("glDrawTransformFeedback", userptr);
    glad_glGenTransformFeedbacks = (PFNGLGENTRANSFORMFEEDBACKSPROC)load("glGenTransformFeedbacks", userptr);
    glad_glPauseTransformFeedback = (PFNGLPAUSETRANSFORMFEEDBACKPROC)load("glPauseTransformFeedback", userptr);
    glad_glIsTransformFeedback = (PFNGLISTRANSFORMFEEDBACKPROC)load("glIsTransformFeedback", userptr);
}
static void load_GL_ARB_transform_feedback3(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_transform_feedback3) return;
    glad_glEndQueryIndexed = (PFNGLENDQUERYINDEXEDPROC)load("glEndQueryIndexed", userptr);
    glad_glBeginQueryIndexed = (PFNGLBEGINQUERYINDEXEDPROC)load("glBeginQueryIndexed", userptr);
    glad_glGetQueryIndexediv = (PFNGLGETQUERYINDEXEDIVPROC)load("glGetQueryIndexediv", userptr);
    glad_glDrawTransformFeedbackStream = (PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC)load("glDrawTransformFeedbackStream", userptr);
}
static void load_GL_ARB_vertex_program(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_vertex_program) return;
    glad_glGetProgramLocalParameterfvARB = (PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC)load("glGetProgramLocalParameterfvARB", userptr);
    glad_glVertexAttrib4NivARB = (PFNGLVERTEXATTRIB4NIVARBPROC)load("glVertexAttrib4NivARB", userptr);
    glad_glDeleteProgramsARB = (PFNGLDELETEPROGRAMSARBPROC)load("glDeleteProgramsARB", userptr);
    glad_glGetProgramEnvParameterfvARB = (PFNGLGETPROGRAMENVPARAMETERFVARBPROC)load("glGetProgramEnvParameterfvARB", userptr);
    glad_glProgramEnvParameter4fARB = (PFNGLPROGRAMENVPARAMETER4FARBPROC)load("glProgramEnvParameter4fARB", userptr);
    glad_glVertexAttrib4fARB = (PFNGLVERTEXATTRIB4FARBPROC)load("glVertexAttrib4fARB", userptr);
    glad_glVertexAttrib4bvARB = (PFNGLVERTEXATTRIB4BVARBPROC)load("glVertexAttrib4bvARB", userptr);
    glad_glVertexAttrib1dARB = (PFNGLVERTEXATTRIB1DARBPROC)load("glVertexAttrib1dARB", userptr);
    glad_glProgramLocalParameter4fvARB = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)load("glProgramLocalParameter4fvARB", userptr);
    glad_glProgramEnvParameter4dARB = (PFNGLPROGRAMENVPARAMETER4DARBPROC)load("glProgramEnvParameter4dARB", userptr);
    glad_glVertexAttrib2dARB = (PFNGLVERTEXATTRIB2DARBPROC)load("glVertexAttrib2dARB", userptr);
    glad_glGetVertexAttribfvARB = (PFNGLGETVERTEXATTRIBFVARBPROC)load("glGetVertexAttribfvARB", userptr);
    glad_glGetVertexAttribPointervARB = (PFNGLGETVERTEXATTRIBPOINTERVARBPROC)load("glGetVertexAttribPointervARB", userptr);
    glad_glVertexAttrib4ivARB = (PFNGLVERTEXATTRIB4IVARBPROC)load("glVertexAttrib4ivARB", userptr);
    glad_glVertexAttrib1sARB = (PFNGLVERTEXATTRIB1SARBPROC)load("glVertexAttrib1sARB", userptr);
    glad_glVertexAttrib3svARB = (PFNGLVERTEXATTRIB3SVARBPROC)load("glVertexAttrib3svARB", userptr);
    glad_glBindProgramARB = (PFNGLBINDPROGRAMARBPROC)load("glBindProgramARB", userptr);
    glad_glGenProgramsARB = (PFNGLGENPROGRAMSARBPROC)load("glGenProgramsARB", userptr);
    glad_glVertexAttrib4ubvARB = (PFNGLVERTEXATTRIB4UBVARBPROC)load("glVertexAttrib4ubvARB", userptr);
    glad_glDisableVertexAttribArrayARB = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)load("glDisableVertexAttribArrayARB", userptr);
    glad_glVertexAttrib2dvARB = (PFNGLVERTEXATTRIB2DVARBPROC)load("glVertexAttrib2dvARB", userptr);
    glad_glProgramEnvParameter4fvARB = (PFNGLPROGRAMENVPARAMETER4FVARBPROC)load("glProgramEnvParameter4fvARB", userptr);
    glad_glVertexAttrib2fARB = (PFNGLVERTEXATTRIB2FARBPROC)load("glVertexAttrib2fARB", userptr);
    glad_glProgramLocalParameter4dARB = (PFNGLPROGRAMLOCALPARAMETER4DARBPROC)load("glProgramLocalParameter4dARB", userptr);
    glad_glGetProgramStringARB = (PFNGLGETPROGRAMSTRINGARBPROC)load("glGetProgramStringARB", userptr);
    glad_glProgramStringARB = (PFNGLPROGRAMSTRINGARBPROC)load("glProgramStringARB", userptr);
    glad_glVertexAttrib4uivARB = (PFNGLVERTEXATTRIB4UIVARBPROC)load("glVertexAttrib4uivARB", userptr);
    glad_glVertexAttrib2svARB = (PFNGLVERTEXATTRIB2SVARBPROC)load("glVertexAttrib2svARB", userptr);
    glad_glVertexAttrib4dvARB = (PFNGLVERTEXATTRIB4DVARBPROC)load("glVertexAttrib4dvARB", userptr);
    glad_glVertexAttrib3fARB = (PFNGLVERTEXATTRIB3FARBPROC)load("glVertexAttrib3fARB", userptr);
    glad_glVertexAttrib4NubvARB = (PFNGLVERTEXATTRIB4NUBVARBPROC)load("glVertexAttrib4NubvARB", userptr);
    glad_glVertexAttrib4sARB = (PFNGLVERTEXATTRIB4SARBPROC)load("glVertexAttrib4sARB", userptr);
    glad_glVertexAttrib2sARB = (PFNGLVERTEXATTRIB2SARBPROC)load("glVertexAttrib2sARB", userptr);
    glad_glVertexAttrib4NsvARB = (PFNGLVERTEXATTRIB4NSVARBPROC)load("glVertexAttrib4NsvARB", userptr);
    glad_glVertexAttrib4NubARB = (PFNGLVERTEXATTRIB4NUBARBPROC)load("glVertexAttrib4NubARB", userptr);
    glad_glGetProgramLocalParameterdvARB = (PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC)load("glGetProgramLocalParameterdvARB", userptr);
    glad_glProgramEnvParameter4dvARB = (PFNGLPROGRAMENVPARAMETER4DVARBPROC)load("glProgramEnvParameter4dvARB", userptr);
    glad_glVertexAttrib2fvARB = (PFNGLVERTEXATTRIB2FVARBPROC)load("glVertexAttrib2fvARB", userptr);
    glad_glVertexAttrib3dvARB = (PFNGLVERTEXATTRIB3DVARBPROC)load("glVertexAttrib3dvARB", userptr);
    glad_glIsProgramARB = (PFNGLISPROGRAMARBPROC)load("glIsProgramARB", userptr);
    glad_glProgramLocalParameter4dvARB = (PFNGLPROGRAMLOCALPARAMETER4DVARBPROC)load("glProgramLocalParameter4dvARB", userptr);
    glad_glVertexAttrib4NusvARB = (PFNGLVERTEXATTRIB4NUSVARBPROC)load("glVertexAttrib4NusvARB", userptr);
    glad_glVertexAttrib4NbvARB = (PFNGLVERTEXATTRIB4NBVARBPROC)load("glVertexAttrib4NbvARB", userptr);
    glad_glVertexAttrib1svARB = (PFNGLVERTEXATTRIB1SVARBPROC)load("glVertexAttrib1svARB", userptr);
    glad_glVertexAttrib4svARB = (PFNGLVERTEXATTRIB4SVARBPROC)load("glVertexAttrib4svARB", userptr);
    glad_glProgramLocalParameter4fARB = (PFNGLPROGRAMLOCALPARAMETER4FARBPROC)load("glProgramLocalParameter4fARB", userptr);
    glad_glVertexAttrib1dvARB = (PFNGLVERTEXATTRIB1DVARBPROC)load("glVertexAttrib1dvARB", userptr);
    glad_glVertexAttrib4NuivARB = (PFNGLVERTEXATTRIB4NUIVARBPROC)load("glVertexAttrib4NuivARB", userptr);
    glad_glVertexAttrib4fvARB = (PFNGLVERTEXATTRIB4FVARBPROC)load("glVertexAttrib4fvARB", userptr);
    glad_glVertexAttrib3fvARB = (PFNGLVERTEXATTRIB3FVARBPROC)load("glVertexAttrib3fvARB", userptr);
    glad_glEnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)load("glEnableVertexAttribArrayARB", userptr);
    glad_glGetVertexAttribivARB = (PFNGLGETVERTEXATTRIBIVARBPROC)load("glGetVertexAttribivARB", userptr);
    glad_glVertexAttrib3sARB = (PFNGLVERTEXATTRIB3SARBPROC)load("glVertexAttrib3sARB", userptr);
    glad_glVertexAttrib1fARB = (PFNGLVERTEXATTRIB1FARBPROC)load("glVertexAttrib1fARB", userptr);
    glad_glVertexAttrib1fvARB = (PFNGLVERTEXATTRIB1FVARBPROC)load("glVertexAttrib1fvARB", userptr);
    glad_glGetProgramEnvParameterdvARB = (PFNGLGETPROGRAMENVPARAMETERDVARBPROC)load("glGetProgramEnvParameterdvARB", userptr);
    glad_glGetProgramivARB = (PFNGLGETPROGRAMIVARBPROC)load("glGetProgramivARB", userptr);
    glad_glGetVertexAttribdvARB = (PFNGLGETVERTEXATTRIBDVARBPROC)load("glGetVertexAttribdvARB", userptr);
    glad_glVertexAttrib4usvARB = (PFNGLVERTEXATTRIB4USVARBPROC)load("glVertexAttrib4usvARB", userptr);
    glad_glVertexAttribPointerARB = (PFNGLVERTEXATTRIBPOINTERARBPROC)load("glVertexAttribPointerARB", userptr);
    glad_glVertexAttrib4dARB = (PFNGLVERTEXATTRIB4DARBPROC)load("glVertexAttrib4dARB", userptr);
    glad_glVertexAttrib3dARB = (PFNGLVERTEXATTRIB3DARBPROC)load("glVertexAttrib3dARB", userptr);
}
static void load_GL_ARB_robustness(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_robustness) return;
    glad_glGetnUniformdvARB = (PFNGLGETNUNIFORMDVARBPROC)load("glGetnUniformdvARB", userptr);
    glad_glGetnUniformuivARB = (PFNGLGETNUNIFORMUIVARBPROC)load("glGetnUniformuivARB", userptr);
    glad_glGetnUniformfvARB = (PFNGLGETNUNIFORMFVARBPROC)load("glGetnUniformfvARB", userptr);
    glad_glGetnTexImageARB = (PFNGLGETNTEXIMAGEARBPROC)load("glGetnTexImageARB", userptr);
    glad_glReadnPixelsARB = (PFNGLREADNPIXELSARBPROC)load("glReadnPixelsARB", userptr);
    glad_glGetnCompressedTexImageARB = (PFNGLGETNCOMPRESSEDTEXIMAGEARBPROC)load("glGetnCompressedTexImageARB", userptr);
    glad_glGetGraphicsResetStatusARB = (PFNGLGETGRAPHICSRESETSTATUSARBPROC)load("glGetGraphicsResetStatusARB", userptr);
    glad_glGetnUniformivARB = (PFNGLGETNUNIFORMIVARBPROC)load("glGetnUniformivARB", userptr);
}
static void load_GL_ARB_timer_query(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_timer_query) return;
    glad_glGetQueryObjecti64v = (PFNGLGETQUERYOBJECTI64VPROC)load("glGetQueryObjecti64v", userptr);
    glad_glQueryCounter = (PFNGLQUERYCOUNTERPROC)load("glQueryCounter", userptr);
    glad_glGetQueryObjectui64v = (PFNGLGETQUERYOBJECTUI64VPROC)load("glGetQueryObjectui64v", userptr);
}
static void load_GL_ARB_vertex_array_object(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_vertex_array_object) return;
    glad_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)load("glBindVertexArray", userptr);
    glad_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)load("glDeleteVertexArrays", userptr);
    glad_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)load("glGenVertexArrays", userptr);
    glad_glIsVertexArray = (PFNGLISVERTEXARRAYPROC)load("glIsVertexArray", userptr);
}
static void load_GL_ARB_vertex_buffer_object(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_vertex_buffer_object) return;
    glad_glBindBufferARB = (PFNGLBINDBUFFERARBPROC)load("glBindBufferARB", userptr);
    glad_glMapBufferARB = (PFNGLMAPBUFFERARBPROC)load("glMapBufferARB", userptr);
    glad_glBufferDataARB = (PFNGLBUFFERDATAARBPROC)load("glBufferDataARB", userptr);
    glad_glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)load("glGenBuffersARB", userptr);
    glad_glIsBufferARB = (PFNGLISBUFFERARBPROC)load("glIsBufferARB", userptr);
    glad_glGetBufferSubDataARB = (PFNGLGETBUFFERSUBDATAARBPROC)load("glGetBufferSubDataARB", userptr);
    glad_glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)load("glDeleteBuffersARB", userptr);
    glad_glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)load("glUnmapBufferARB", userptr);
    glad_glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)load("glGetBufferParameterivARB", userptr);
    glad_glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)load("glBufferSubDataARB", userptr);
    glad_glGetBufferPointervARB = (PFNGLGETBUFFERPOINTERVARBPROC)load("glGetBufferPointervARB", userptr);
}
static void load_GL_ARB_map_buffer_range(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_map_buffer_range) return;
    glad_glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC)load("glFlushMappedBufferRange", userptr);
    glad_glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC)load("glMapBufferRange", userptr);
}
static void load_GL_ARB_geometry_shader4(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_geometry_shader4) return;
    glad_glProgramParameteriARB = (PFNGLPROGRAMPARAMETERIARBPROC)load("glProgramParameteriARB", userptr);
    glad_glFramebufferTextureFaceARB = (PFNGLFRAMEBUFFERTEXTUREFACEARBPROC)load("glFramebufferTextureFaceARB", userptr);
    glad_glFramebufferTextureARB = (PFNGLFRAMEBUFFERTEXTUREARBPROC)load("glFramebufferTextureARB", userptr);
    glad_glFramebufferTextureLayerARB = (PFNGLFRAMEBUFFERTEXTURELAYERARBPROC)load("glFramebufferTextureLayerARB", userptr);
}
static void load_GL_ARB_texture_buffer_object(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_texture_buffer_object) return;
    glad_glTexBufferARB = (PFNGLTEXBUFFERARBPROC)load("glTexBufferARB", userptr);
}
static void load_GL_EXT_timer_query(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_timer_query) return;
    glad_glGetQueryObjecti64vEXT = (PFNGLGETQUERYOBJECTI64VEXTPROC)load("glGetQueryObjecti64vEXT", userptr);
    glad_glGetQueryObjectui64vEXT = (PFNGLGETQUERYOBJECTUI64VEXTPROC)load("glGetQueryObjectui64vEXT", userptr);
}
static void load_GL_KHR_debug(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_KHR_debug) return;
    glad_glObjectPtrLabel = (PFNGLOBJECTPTRLABELPROC)load("glObjectPtrLabel", userptr);
    glad_glGetDebugMessageLog = (PFNGLGETDEBUGMESSAGELOGPROC)load("glGetDebugMessageLog", userptr);
    glad_glGetObjectPtrLabel = (PFNGLGETOBJECTPTRLABELPROC)load("glGetObjectPtrLabel", userptr);
    glad_glObjectLabel = (PFNGLOBJECTLABELPROC)load("glObjectLabel", userptr);
    glad_glGetObjectLabel = (PFNGLGETOBJECTLABELPROC)load("glGetObjectLabel", userptr);
    glad_glDebugMessageControl = (PFNGLDEBUGMESSAGECONTROLPROC)load("glDebugMessageControl", userptr);
    glad_glDebugMessageInsert = (PFNGLDEBUGMESSAGEINSERTPROC)load("glDebugMessageInsert", userptr);
    glad_glPushDebugGroup = (PFNGLPUSHDEBUGGROUPPROC)load("glPushDebugGroup", userptr);
    glad_glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)load("glDebugMessageCallback", userptr);
    glad_glPopDebugGroup = (PFNGLPOPDEBUGGROUPPROC)load("glPopDebugGroup", userptr);
}
static void load_GL_ARB_get_program_binary(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_get_program_binary) return;
    glad_glGetProgramBinary = (PFNGLGETPROGRAMBINARYPROC)load("glGetProgramBinary", userptr);
    glad_glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC)load("glProgramParameteri", userptr);
    glad_glProgramBinary = (PFNGLPROGRAMBINARYPROC)load("glProgramBinary", userptr);
}
static void load_GL_ARB_transform_feedback_instanced(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_transform_feedback_instanced) return;
    glad_glDrawTransformFeedbackInstanced = (PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC)load("glDrawTransformFeedbackInstanced", userptr);
    glad_glDrawTransformFeedbackStreamInstanced = (PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC)load("glDrawTransformFeedbackStreamInstanced", userptr);
}
static void load_GL_SGIS_point_parameters(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_SGIS_point_parameters) return;
    glad_glPointParameterfvSGIS = (PFNGLPOINTPARAMETERFVSGISPROC)load("glPointParameterfvSGIS", userptr);
    glad_glPointParameterfSGIS = (PFNGLPOINTPARAMETERFSGISPROC)load("glPointParameterfSGIS", userptr);
}
static void load_GL_EXT_geometry_shader4(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_geometry_shader4) return;
    glad_glProgramParameteriEXT = (PFNGLPROGRAMPARAMETERIEXTPROC)load("glProgramParameteriEXT", userptr);
}
static void load_GL_EXT_framebuffer_object(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_framebuffer_object) return;
    glad_glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)load("glCheckFramebufferStatusEXT", userptr);
    glad_glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)load("glGenFramebuffersEXT", userptr);
    glad_glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)load("glBindRenderbufferEXT", userptr);
    glad_glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)load("glDeleteRenderbuffersEXT", userptr);
    glad_glFramebufferTexture1DEXT = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)load("glFramebufferTexture1DEXT", userptr);
    glad_glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)load("glGetFramebufferAttachmentParameterivEXT", userptr);
    glad_glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)load("glFramebufferTexture2DEXT", userptr);
    glad_glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)load("glDeleteFramebuffersEXT", userptr);
    glad_glIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC)load("glIsFramebufferEXT", userptr);
    glad_glGetRenderbufferParameterivEXT = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)load("glGetRenderbufferParameterivEXT", userptr);
    glad_glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC)load("glGenerateMipmapEXT", userptr);
    glad_glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)load("glGenRenderbuffersEXT", userptr);
    glad_glIsRenderbufferEXT = (PFNGLISRENDERBUFFEREXTPROC)load("glIsRenderbufferEXT", userptr);
    glad_glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)load("glRenderbufferStorageEXT", userptr);
    glad_glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)load("glFramebufferRenderbufferEXT", userptr);
    glad_glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)load("glBindFramebufferEXT", userptr);
    glad_glFramebufferTexture3DEXT = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)load("glFramebufferTexture3DEXT", userptr);
}
static void load_GL_ARB_draw_indirect(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_draw_indirect) return;
    glad_glDrawElementsIndirect = (PFNGLDRAWELEMENTSINDIRECTPROC)load("glDrawElementsIndirect", userptr);
    glad_glDrawArraysIndirect = (PFNGLDRAWARRAYSINDIRECTPROC)load("glDrawArraysIndirect", userptr);
}
static void load_GL_NV_bindless_multi_draw_indirect_count(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_NV_bindless_multi_draw_indirect_count) return;
    glad_glMultiDrawArraysIndirectBindlessCountNV = (PFNGLMULTIDRAWARRAYSINDIRECTBINDLESSCOUNTNVPROC)load("glMultiDrawArraysIndirectBindlessCountNV", userptr);
    glad_glMultiDrawElementsIndirectBindlessCountNV = (PFNGLMULTIDRAWELEMENTSINDIRECTBINDLESSCOUNTNVPROC)load("glMultiDrawElementsIndirectBindlessCountNV", userptr);
}
static void load_GL_ARB_ES2_compatibility(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_ES2_compatibility) return;
    glad_glGetShaderPrecisionFormat = (PFNGLGETSHADERPRECISIONFORMATPROC)load("glGetShaderPrecisionFormat", userptr);
    glad_glShaderBinary = (PFNGLSHADERBINARYPROC)load("glShaderBinary", userptr);
    glad_glDepthRangef = (PFNGLDEPTHRANGEFPROC)load("glDepthRangef", userptr);
    glad_glReleaseShaderCompiler = (PFNGLRELEASESHADERCOMPILERPROC)load("glReleaseShaderCompiler", userptr);
    glad_glClearDepthf = (PFNGLCLEARDEPTHFPROC)load("glClearDepthf", userptr);
}
static void load_GL_NV_command_list(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_NV_command_list) return;
    glad_glDeleteStatesNV = (PFNGLDELETESTATESNVPROC)load("glDeleteStatesNV", userptr);
    glad_glDrawCommandsStatesAddressNV = (PFNGLDRAWCOMMANDSSTATESADDRESSNVPROC)load("glDrawCommandsStatesAddressNV", userptr);
    glad_glCallCommandListNV = (PFNGLCALLCOMMANDLISTNVPROC)load("glCallCommandListNV", userptr);
    glad_glDrawCommandsStatesNV = (PFNGLDRAWCOMMANDSSTATESNVPROC)load("glDrawCommandsStatesNV", userptr);
    glad_glIsCommandListNV = (PFNGLISCOMMANDLISTNVPROC)load("glIsCommandListNV", userptr);
    glad_glCommandListSegmentsNV = (PFNGLCOMMANDLISTSEGMENTSNVPROC)load("glCommandListSegmentsNV", userptr);
    glad_glListDrawCommandsStatesClientNV = (PFNGLLISTDRAWCOMMANDSSTATESCLIENTNVPROC)load("glListDrawCommandsStatesClientNV", userptr);
    glad_glIsStateNV = (PFNGLISSTATENVPROC)load("glIsStateNV", userptr);
    glad_glCompileCommandListNV = (PFNGLCOMPILECOMMANDLISTNVPROC)load("glCompileCommandListNV", userptr);
    glad_glCreateStatesNV = (PFNGLCREATESTATESNVPROC)load("glCreateStatesNV", userptr);
    glad_glGetStageIndexNV = (PFNGLGETSTAGEINDEXNVPROC)load("glGetStageIndexNV", userptr);
    glad_glDrawCommandsNV = (PFNGLDRAWCOMMANDSNVPROC)load("glDrawCommandsNV", userptr);
    glad_glDrawCommandsAddressNV = (PFNGLDRAWCOMMANDSADDRESSNVPROC)load("glDrawCommandsAddressNV", userptr);
    glad_glDeleteCommandListsNV = (PFNGLDELETECOMMANDLISTSNVPROC)load("glDeleteCommandListsNV", userptr);
    glad_glGetCommandHeaderNV = (PFNGLGETCOMMANDHEADERNVPROC)load("glGetCommandHeaderNV", userptr);
    glad_glStateCaptureNV = (PFNGLSTATECAPTURENVPROC)load("glStateCaptureNV", userptr);
    glad_glCreateCommandListsNV = (PFNGLCREATECOMMANDLISTSNVPROC)load("glCreateCommandListsNV", userptr);
}
static void load_GL_ARB_draw_buffers(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_draw_buffers) return;
    glad_glDrawBuffersARB = (PFNGLDRAWBUFFERSARBPROC)load("glDrawBuffersARB", userptr);
}
static void load_GL_ARB_multi_draw_indirect(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_multi_draw_indirect) return;
    glad_glMultiDrawElementsIndirect = (PFNGLMULTIDRAWELEMENTSINDIRECTPROC)load("glMultiDrawElementsIndirect", userptr);
    glad_glMultiDrawArraysIndirect = (PFNGLMULTIDRAWARRAYSINDIRECTPROC)load("glMultiDrawArraysIndirect", userptr);
}
static void load_GL_ARB_debug_output(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_debug_output) return;
    glad_glDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKARBPROC)load("glDebugMessageCallbackARB", userptr);
    glad_glDebugMessageControlARB = (PFNGLDEBUGMESSAGECONTROLARBPROC)load("glDebugMessageControlARB", userptr);
    glad_glDebugMessageInsertARB = (PFNGLDEBUGMESSAGEINSERTARBPROC)load("glDebugMessageInsertARB", userptr);
    glad_glGetDebugMessageLogARB = (PFNGLGETDEBUGMESSAGELOGARBPROC)load("glGetDebugMessageLogARB", userptr);
}
static void load_GL_AMD_draw_buffers_blend(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_AMD_draw_buffers_blend) return;
    glad_glBlendFuncSeparateIndexedAMD = (PFNGLBLENDFUNCSEPARATEINDEXEDAMDPROC)load("glBlendFuncSeparateIndexedAMD", userptr);
    glad_glBlendEquationIndexedAMD = (PFNGLBLENDEQUATIONINDEXEDAMDPROC)load("glBlendEquationIndexedAMD", userptr);
    glad_glBlendFuncIndexedAMD = (PFNGLBLENDFUNCINDEXEDAMDPROC)load("glBlendFuncIndexedAMD", userptr);
    glad_glBlendEquationSeparateIndexedAMD = (PFNGLBLENDEQUATIONSEPARATEINDEXEDAMDPROC)load("glBlendEquationSeparateIndexedAMD", userptr);
}
static void load_GL_OES_single_precision(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_OES_single_precision) return;
    glad_glClipPlanefOES = (PFNGLCLIPPLANEFOESPROC)load("glClipPlanefOES", userptr);
    glad_glFrustumfOES = (PFNGLFRUSTUMFOESPROC)load("glFrustumfOES", userptr);
    glad_glClearDepthfOES = (PFNGLCLEARDEPTHFOESPROC)load("glClearDepthfOES", userptr);
    glad_glDepthRangefOES = (PFNGLDEPTHRANGEFOESPROC)load("glDepthRangefOES", userptr);
    glad_glGetClipPlanefOES = (PFNGLGETCLIPPLANEFOESPROC)load("glGetClipPlanefOES", userptr);
    glad_glOrthofOES = (PFNGLORTHOFOESPROC)load("glOrthofOES", userptr);
}
static void load_GL_ARB_texture_buffer_range(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_texture_buffer_range) return;
    glad_glTexBufferRange = (PFNGLTEXBUFFERRANGEPROC)load("glTexBufferRange", userptr);
}
static void load_GL_NV_explicit_multisample(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_NV_explicit_multisample) return;
    glad_glGetMultisamplefvNV = (PFNGLGETMULTISAMPLEFVNVPROC)load("glGetMultisamplefvNV", userptr);
    glad_glSampleMaskIndexedNV = (PFNGLSAMPLEMASKINDEXEDNVPROC)load("glSampleMaskIndexedNV", userptr);
    glad_glTexRenderbufferNV = (PFNGLTEXRENDERBUFFERNVPROC)load("glTexRenderbufferNV", userptr);
}
static void load_GL_EXT_texture_array(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_texture_array) return;
    glad_glFramebufferTextureLayerEXT = (PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC)load("glFramebufferTextureLayerEXT", userptr);
}
static void load_GL_EXT_point_parameters(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_point_parameters) return;
    glad_glPointParameterfEXT = (PFNGLPOINTPARAMETERFEXTPROC)load("glPointParameterfEXT", userptr);
    glad_glPointParameterfvEXT = (PFNGLPOINTPARAMETERFVEXTPROC)load("glPointParameterfvEXT", userptr);
}
static void load_GL_APPLE_vertex_array_object(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_APPLE_vertex_array_object) return;
    glad_glGenVertexArraysAPPLE = (PFNGLGENVERTEXARRAYSAPPLEPROC)load("glGenVertexArraysAPPLE", userptr);
    glad_glIsVertexArrayAPPLE = (PFNGLISVERTEXARRAYAPPLEPROC)load("glIsVertexArrayAPPLE", userptr);
    glad_glBindVertexArrayAPPLE = (PFNGLBINDVERTEXARRAYAPPLEPROC)load("glBindVertexArrayAPPLE", userptr);
    glad_glDeleteVertexArraysAPPLE = (PFNGLDELETEVERTEXARRAYSAPPLEPROC)load("glDeleteVertexArraysAPPLE", userptr);
}
static void load_GL_ARB_vertex_type_2_10_10_10_rev(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_vertex_type_2_10_10_10_rev) return;
    glad_glVertexAttribP1ui = (PFNGLVERTEXATTRIBP1UIPROC)load("glVertexAttribP1ui", userptr);
    glad_glVertexAttribP2uiv = (PFNGLVERTEXATTRIBP2UIVPROC)load("glVertexAttribP2uiv", userptr);
    glad_glVertexAttribP3uiv = (PFNGLVERTEXATTRIBP3UIVPROC)load("glVertexAttribP3uiv", userptr);
    glad_glVertexAttribP1uiv = (PFNGLVERTEXATTRIBP1UIVPROC)load("glVertexAttribP1uiv", userptr);
    glad_glVertexAttribP2ui = (PFNGLVERTEXATTRIBP2UIPROC)load("glVertexAttribP2ui", userptr);
    glad_glVertexAttribP3ui = (PFNGLVERTEXATTRIBP3UIPROC)load("glVertexAttribP3ui", userptr);
    glad_glVertexAttribP4ui = (PFNGLVERTEXATTRIBP4UIPROC)load("glVertexAttribP4ui", userptr);
    glad_glVertexAttribP4uiv = (PFNGLVERTEXATTRIBP4UIVPROC)load("glVertexAttribP4uiv", userptr);
}
static void load_GL_ARB_vertex_shader(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_vertex_shader) return;
    glad_glDisableVertexAttribArrayARB = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)load("glDisableVertexAttribArrayARB", userptr);
    glad_glVertexAttrib2dvARB = (PFNGLVERTEXATTRIB2DVARBPROC)load("glVertexAttrib2dvARB", userptr);
    glad_glVertexAttrib4NivARB = (PFNGLVERTEXATTRIB4NIVARBPROC)load("glVertexAttrib4NivARB", userptr);
    glad_glVertexAttrib4ubvARB = (PFNGLVERTEXATTRIB4UBVARBPROC)load("glVertexAttrib4ubvARB", userptr);
    glad_glVertexAttrib1sARB = (PFNGLVERTEXATTRIB1SARBPROC)load("glVertexAttrib1sARB", userptr);
    glad_glVertexAttrib4NubARB = (PFNGLVERTEXATTRIB4NUBARBPROC)load("glVertexAttrib4NubARB", userptr);
    glad_glVertexAttrib3fvARB = (PFNGLVERTEXATTRIB3FVARBPROC)load("glVertexAttrib3fvARB", userptr);
    glad_glVertexAttrib1fvARB = (PFNGLVERTEXATTRIB1FVARBPROC)load("glVertexAttrib1fvARB", userptr);
    glad_glGetActiveAttribARB = (PFNGLGETACTIVEATTRIBARBPROC)load("glGetActiveAttribARB", userptr);
    glad_glEnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)load("glEnableVertexAttribArrayARB", userptr);
    glad_glVertexAttrib3svARB = (PFNGLVERTEXATTRIB3SVARBPROC)load("glVertexAttrib3svARB", userptr);
    glad_glGetAttribLocationARB = (PFNGLGETATTRIBLOCATIONARBPROC)load("glGetAttribLocationARB", userptr);
    glad_glVertexAttrib4uivARB = (PFNGLVERTEXATTRIB4UIVARBPROC)load("glVertexAttrib4uivARB", userptr);
    glad_glGetVertexAttribfvARB = (PFNGLGETVERTEXATTRIBFVARBPROC)load("glGetVertexAttribfvARB", userptr);
    glad_glVertexAttrib3sARB = (PFNGLVERTEXATTRIB3SARBPROC)load("glVertexAttrib3sARB", userptr);
    glad_glVertexAttrib4NbvARB = (PFNGLVERTEXATTRIB4NBVARBPROC)load("glVertexAttrib4NbvARB", userptr);
    glad_glVertexAttrib1dARB = (PFNGLVERTEXATTRIB1DARBPROC)load("glVertexAttrib1dARB", userptr);
    glad_glVertexAttrib4fARB = (PFNGLVERTEXATTRIB4FARBPROC)load("glVertexAttrib4fARB", userptr);
    glad_glVertexAttrib1fARB = (PFNGLVERTEXATTRIB1FARBPROC)load("glVertexAttrib1fARB", userptr);
    glad_glVertexAttrib2fvARB = (PFNGLVERTEXATTRIB2FVARBPROC)load("glVertexAttrib2fvARB", userptr);
    glad_glVertexAttrib2fARB = (PFNGLVERTEXATTRIB2FARBPROC)load("glVertexAttrib2fARB", userptr);
    glad_glVertexAttrib4NsvARB = (PFNGLVERTEXATTRIB4NSVARBPROC)load("glVertexAttrib4NsvARB", userptr);
    glad_glVertexAttrib3fARB = (PFNGLVERTEXATTRIB3FARBPROC)load("glVertexAttrib3fARB", userptr);
    glad_glVertexAttrib4NusvARB = (PFNGLVERTEXATTRIB4NUSVARBPROC)load("glVertexAttrib4NusvARB", userptr);
    glad_glGetVertexAttribdvARB = (PFNGLGETVERTEXATTRIBDVARBPROC)load("glGetVertexAttribdvARB", userptr);
    glad_glGetVertexAttribPointervARB = (PFNGLGETVERTEXATTRIBPOINTERVARBPROC)load("glGetVertexAttribPointervARB", userptr);
    glad_glVertexAttrib4NubvARB = (PFNGLVERTEXATTRIB4NUBVARBPROC)load("glVertexAttrib4NubvARB", userptr);
    glad_glVertexAttrib4ivARB = (PFNGLVERTEXATTRIB4IVARBPROC)load("glVertexAttrib4ivARB", userptr);
    glad_glVertexAttrib3dvARB = (PFNGLVERTEXATTRIB3DVARBPROC)load("glVertexAttrib3dvARB", userptr);
    glad_glVertexAttrib2dARB = (PFNGLVERTEXATTRIB2DARBPROC)load("glVertexAttrib2dARB", userptr);
    glad_glVertexAttrib4usvARB = (PFNGLVERTEXATTRIB4USVARBPROC)load("glVertexAttrib4usvARB", userptr);
    glad_glVertexAttrib1svARB = (PFNGLVERTEXATTRIB1SVARBPROC)load("glVertexAttrib1svARB", userptr);
    glad_glGetVertexAttribivARB = (PFNGLGETVERTEXATTRIBIVARBPROC)load("glGetVertexAttribivARB", userptr);
    glad_glVertexAttrib4sARB = (PFNGLVERTEXATTRIB4SARBPROC)load("glVertexAttrib4sARB", userptr);
    glad_glVertexAttrib4dvARB = (PFNGLVERTEXATTRIB4DVARBPROC)load("glVertexAttrib4dvARB", userptr);
    glad_glVertexAttrib4bvARB = (PFNGLVERTEXATTRIB4BVARBPROC)load("glVertexAttrib4bvARB", userptr);
    glad_glVertexAttribPointerARB = (PFNGLVERTEXATTRIBPOINTERARBPROC)load("glVertexAttribPointerARB", userptr);
    glad_glVertexAttrib2svARB = (PFNGLVERTEXATTRIB2SVARBPROC)load("glVertexAttrib2svARB", userptr);
    glad_glVertexAttrib4svARB = (PFNGLVERTEXATTRIB4SVARBPROC)load("glVertexAttrib4svARB", userptr);
    glad_glVertexAttrib3dARB = (PFNGLVERTEXATTRIB3DARBPROC)load("glVertexAttrib3dARB", userptr);
    glad_glVertexAttrib4fvARB = (PFNGLVERTEXATTRIB4FVARBPROC)load("glVertexAttrib4fvARB", userptr);
    glad_glVertexAttrib4dARB = (PFNGLVERTEXATTRIB4DARBPROC)load("glVertexAttrib4dARB", userptr);
    glad_glVertexAttrib1dvARB = (PFNGLVERTEXATTRIB1DVARBPROC)load("glVertexAttrib1dvARB", userptr);
    glad_glVertexAttrib2sARB = (PFNGLVERTEXATTRIB2SARBPROC)load("glVertexAttrib2sARB", userptr);
    glad_glBindAttribLocationARB = (PFNGLBINDATTRIBLOCATIONARBPROC)load("glBindAttribLocationARB", userptr);
    glad_glVertexAttrib4NuivARB = (PFNGLVERTEXATTRIB4NUIVARBPROC)load("glVertexAttrib4NuivARB", userptr);
}
static void load_GL_APPLE_flush_buffer_range(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_APPLE_flush_buffer_range) return;
    glad_glBufferParameteriAPPLE = (PFNGLBUFFERPARAMETERIAPPLEPROC)load("glBufferParameteriAPPLE", userptr);
    glad_glFlushMappedBufferRangeAPPLE = (PFNGLFLUSHMAPPEDBUFFERRANGEAPPLEPROC)load("glFlushMappedBufferRangeAPPLE", userptr);
}
static void load_GL_ARB_draw_instanced(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_draw_instanced) return;
    glad_glDrawElementsInstancedARB = (PFNGLDRAWELEMENTSINSTANCEDARBPROC)load("glDrawElementsInstancedARB", userptr);
    glad_glDrawArraysInstancedARB = (PFNGLDRAWARRAYSINSTANCEDARBPROC)load("glDrawArraysInstancedARB", userptr);
}
static void load_GL_ARB_shader_objects(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_shader_objects) return;
    glad_glGetHandleARB = (PFNGLGETHANDLEARBPROC)load("glGetHandleARB", userptr);
    glad_glUniform3ivARB = (PFNGLUNIFORM3IVARBPROC)load("glUniform3ivARB", userptr);
    glad_glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)load("glGetUniformLocationARB", userptr);
    glad_glUniform4ivARB = (PFNGLUNIFORM4IVARBPROC)load("glUniform4ivARB", userptr);
    glad_glUniform3fvARB = (PFNGLUNIFORM3FVARBPROC)load("glUniform3fvARB", userptr);
    glad_glUniform4iARB = (PFNGLUNIFORM4IARBPROC)load("glUniform4iARB", userptr);
    glad_glUniform2fARB = (PFNGLUNIFORM2FARBPROC)load("glUniform2fARB", userptr);
    glad_glUniform3fARB = (PFNGLUNIFORM3FARBPROC)load("glUniform3fARB", userptr);
    glad_glUniform1iARB = (PFNGLUNIFORM1IARBPROC)load("glUniform1iARB", userptr);
    glad_glUniform1fvARB = (PFNGLUNIFORM1FVARBPROC)load("glUniform1fvARB", userptr);
    glad_glUniform3iARB = (PFNGLUNIFORM3IARBPROC)load("glUniform3iARB", userptr);
    glad_glDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)load("glDeleteObjectARB", userptr);
    glad_glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)load("glCompileShaderARB", userptr);
    glad_glValidateProgramARB = (PFNGLVALIDATEPROGRAMARBPROC)load("glValidateProgramARB", userptr);
    glad_glGetShaderSourceARB = (PFNGLGETSHADERSOURCEARBPROC)load("glGetShaderSourceARB", userptr);
    glad_glUniform2ivARB = (PFNGLUNIFORM2IVARBPROC)load("glUniform2ivARB", userptr);
    glad_glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)load("glUseProgramObjectARB", userptr);
    glad_glUniformMatrix2fvARB = (PFNGLUNIFORMMATRIX2FVARBPROC)load("glUniformMatrix2fvARB", userptr);
    glad_glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)load("glLinkProgramARB", userptr);
    glad_glGetUniformfvARB = (PFNGLGETUNIFORMFVARBPROC)load("glGetUniformfvARB", userptr);
    glad_glUniform1ivARB = (PFNGLUNIFORM1IVARBPROC)load("glUniform1ivARB", userptr);
    glad_glUniform2iARB = (PFNGLUNIFORM2IARBPROC)load("glUniform2iARB", userptr);
    glad_glGetUniformivARB = (PFNGLGETUNIFORMIVARBPROC)load("glGetUniformivARB", userptr);
    glad_glGetObjectParameterfvARB = (PFNGLGETOBJECTPARAMETERFVARBPROC)load("glGetObjectParameterfvARB", userptr);
    glad_glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)load("glAttachObjectARB", userptr);
    glad_glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)load("glShaderSourceARB", userptr);
    glad_glUniformMatrix3fvARB = (PFNGLUNIFORMMATRIX3FVARBPROC)load("glUniformMatrix3fvARB", userptr);
    glad_glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)load("glCreateShaderObjectARB", userptr);
    glad_glUniform1fARB = (PFNGLUNIFORM1FARBPROC)load("glUniform1fARB", userptr);
    glad_glUniform4fARB = (PFNGLUNIFORM4FARBPROC)load("glUniform4fARB", userptr);
    glad_glUniform4fvARB = (PFNGLUNIFORM4FVARBPROC)load("glUniform4fvARB", userptr);
    glad_glGetActiveUniformARB = (PFNGLGETACTIVEUNIFORMARBPROC)load("glGetActiveUniformARB", userptr);
    glad_glUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC)load("glUniformMatrix4fvARB", userptr);
    glad_glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)load("glGetInfoLogARB", userptr);
    glad_glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)load("glGetObjectParameterivARB", userptr);
    glad_glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)load("glCreateProgramObjectARB", userptr);
    glad_glDetachObjectARB = (PFNGLDETACHOBJECTARBPROC)load("glDetachObjectARB", userptr);
    glad_glGetAttachedObjectsARB = (PFNGLGETATTACHEDOBJECTSARBPROC)load("glGetAttachedObjectsARB", userptr);
    glad_glUniform2fvARB = (PFNGLUNIFORM2FVARBPROC)load("glUniform2fvARB", userptr);
}
static void load_GL_ARB_tessellation_shader(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_tessellation_shader) return;
    glad_glPatchParameteri = (PFNGLPATCHPARAMETERIPROC)load("glPatchParameteri", userptr);
    glad_glPatchParameterfv = (PFNGLPATCHPARAMETERFVPROC)load("glPatchParameterfv", userptr);
}
static void load_GL_EXT_draw_buffers2(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_draw_buffers2) return;
    glad_glEnableIndexedEXT = (PFNGLENABLEINDEXEDEXTPROC)load("glEnableIndexedEXT", userptr);
    glad_glColorMaskIndexedEXT = (PFNGLCOLORMASKINDEXEDEXTPROC)load("glColorMaskIndexedEXT", userptr);
    glad_glDisableIndexedEXT = (PFNGLDISABLEINDEXEDEXTPROC)load("glDisableIndexedEXT", userptr);
    glad_glGetIntegerIndexedvEXT = (PFNGLGETINTEGERINDEXEDVEXTPROC)load("glGetIntegerIndexedvEXT", userptr);
    glad_glGetBooleanIndexedvEXT = (PFNGLGETBOOLEANINDEXEDVEXTPROC)load("glGetBooleanIndexedvEXT", userptr);
    glad_glIsEnabledIndexedEXT = (PFNGLISENABLEDINDEXEDEXTPROC)load("glIsEnabledIndexedEXT", userptr);
}
static void load_GL_ARB_vertex_attrib_64bit(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_vertex_attrib_64bit) return;
    glad_glVertexAttribL1d = (PFNGLVERTEXATTRIBL1DPROC)load("glVertexAttribL1d", userptr);
    glad_glGetVertexAttribLdv = (PFNGLGETVERTEXATTRIBLDVPROC)load("glGetVertexAttribLdv", userptr);
    glad_glVertexAttribLPointer = (PFNGLVERTEXATTRIBLPOINTERPROC)load("glVertexAttribLPointer", userptr);
    glad_glVertexAttribL3dv = (PFNGLVERTEXATTRIBL3DVPROC)load("glVertexAttribL3dv", userptr);
    glad_glVertexAttribL4dv = (PFNGLVERTEXATTRIBL4DVPROC)load("glVertexAttribL4dv", userptr);
    glad_glVertexAttribL2dv = (PFNGLVERTEXATTRIBL2DVPROC)load("glVertexAttribL2dv", userptr);
    glad_glVertexAttribL2d = (PFNGLVERTEXATTRIBL2DPROC)load("glVertexAttribL2d", userptr);
    glad_glVertexAttribL4d = (PFNGLVERTEXATTRIBL4DPROC)load("glVertexAttribL4d", userptr);
    glad_glVertexAttribL1dv = (PFNGLVERTEXATTRIBL1DVPROC)load("glVertexAttribL1dv", userptr);
    glad_glVertexAttribL3d = (PFNGLVERTEXATTRIBL3DPROC)load("glVertexAttribL3d", userptr);
}
static void load_GL_ARB_imaging(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_imaging) return;
    glad_glBlendColor = (PFNGLBLENDCOLORPROC)load("glBlendColor", userptr);
    glad_glBlendEquation = (PFNGLBLENDEQUATIONPROC)load("glBlendEquation", userptr);
}
static void load_GL_INGR_blend_func_separate(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_INGR_blend_func_separate) return;
    glad_glBlendFuncSeparateINGR = (PFNGLBLENDFUNCSEPARATEINGRPROC)load("glBlendFuncSeparateINGR", userptr);
}
static void load_GL_NV_path_rendering(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_NV_path_rendering) return;
    glad_glStencilStrokePathNV = (PFNGLSTENCILSTROKEPATHNVPROC)load("glStencilStrokePathNV", userptr);
    glad_glGetPathParameterfvNV = (PFNGLGETPATHPARAMETERFVNVPROC)load("glGetPathParameterfvNV", userptr);
    glad_glMatrixMult3x2fNV = (PFNGLMATRIXMULT3X2FNVPROC)load("glMatrixMult3x2fNV", userptr);
    glad_glStencilFillPathNV = (PFNGLSTENCILFILLPATHNVPROC)load("glStencilFillPathNV", userptr);
    glad_glPathMemoryGlyphIndexArrayNV = (PFNGLPATHMEMORYGLYPHINDEXARRAYNVPROC)load("glPathMemoryGlyphIndexArrayNV", userptr);
    glad_glGetPathParameterivNV = (PFNGLGETPATHPARAMETERIVNVPROC)load("glGetPathParameterivNV", userptr);
    glad_glCoverFillPathInstancedNV = (PFNGLCOVERFILLPATHINSTANCEDNVPROC)load("glCoverFillPathInstancedNV", userptr);
    glad_glStencilStrokePathInstancedNV = (PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC)load("glStencilStrokePathInstancedNV", userptr);
    glad_glPathStencilDepthOffsetNV = (PFNGLPATHSTENCILDEPTHOFFSETNVPROC)load("glPathStencilDepthOffsetNV", userptr);
    glad_glPathGlyphsNV = (PFNGLPATHGLYPHSNVPROC)load("glPathGlyphsNV", userptr);
    glad_glStencilThenCoverStrokePathNV = (PFNGLSTENCILTHENCOVERSTROKEPATHNVPROC)load("glStencilThenCoverStrokePathNV", userptr);
    glad_glCoverStrokePathInstancedNV = (PFNGLCOVERSTROKEPATHINSTANCEDNVPROC)load("glCoverStrokePathInstancedNV", userptr);
    glad_glPathSubCoordsNV = (PFNGLPATHSUBCOORDSNVPROC)load("glPathSubCoordsNV", userptr);
    glad_glGetPathMetricRangeNV = (PFNGLGETPATHMETRICRANGENVPROC)load("glGetPathMetricRangeNV", userptr);
    glad_glCopyPathNV = (PFNGLCOPYPATHNVPROC)load("glCopyPathNV", userptr);
    glad_glPathCoverDepthFuncNV = (PFNGLPATHCOVERDEPTHFUNCNVPROC)load("glPathCoverDepthFuncNV", userptr);
    glad_glGenPathsNV = (PFNGLGENPATHSNVPROC)load("glGenPathsNV", userptr);
    glad_glPathSubCommandsNV = (PFNGLPATHSUBCOMMANDSNVPROC)load("glPathSubCommandsNV", userptr);
    glad_glGetPathDashArrayNV = (PFNGLGETPATHDASHARRAYNVPROC)load("glGetPathDashArrayNV", userptr);
    glad_glIsPointInFillPathNV = (PFNGLISPOINTINFILLPATHNVPROC)load("glIsPointInFillPathNV", userptr);
    glad_glCoverStrokePathNV = (PFNGLCOVERSTROKEPATHNVPROC)load("glCoverStrokePathNV", userptr);
    glad_glPathGlyphIndexArrayNV = (PFNGLPATHGLYPHINDEXARRAYNVPROC)load("glPathGlyphIndexArrayNV", userptr);
    glad_glPointAlongPathNV = (PFNGLPOINTALONGPATHNVPROC)load("glPointAlongPathNV", userptr);
    glad_glMatrixLoadTranspose3x3fNV = (PFNGLMATRIXLOADTRANSPOSE3X3FNVPROC)load("glMatrixLoadTranspose3x3fNV", userptr);
    glad_glDeletePathsNV = (PFNGLDELETEPATHSNVPROC)load("glDeletePathsNV", userptr);
    glad_glPathParameterivNV = (PFNGLPATHPARAMETERIVNVPROC)load("glPathParameterivNV", userptr);
    glad_glStencilFillPathInstancedNV = (PFNGLSTENCILFILLPATHINSTANCEDNVPROC)load("glStencilFillPathInstancedNV", userptr);
    glad_glWeightPathsNV = (PFNGLWEIGHTPATHSNVPROC)load("glWeightPathsNV", userptr);
    glad_glPathStencilFuncNV = (PFNGLPATHSTENCILFUNCNVPROC)load("glPathStencilFuncNV", userptr);
    glad_glProgramPathFragmentInputGenNV = (PFNGLPROGRAMPATHFRAGMENTINPUTGENNVPROC)load("glProgramPathFragmentInputGenNV", userptr);
    glad_glPathStringNV = (PFNGLPATHSTRINGNVPROC)load("glPathStringNV", userptr);
    glad_glGetPathSpacingNV = (PFNGLGETPATHSPACINGNVPROC)load("glGetPathSpacingNV", userptr);
    glad_glPathParameterfvNV = (PFNGLPATHPARAMETERFVNVPROC)load("glPathParameterfvNV", userptr);
    glad_glMatrixLoad3x2fNV = (PFNGLMATRIXLOAD3X2FNVPROC)load("glMatrixLoad3x2fNV", userptr);
    glad_glTransformPathNV = (PFNGLTRANSFORMPATHNVPROC)load("glTransformPathNV", userptr);
    glad_glGetPathCommandsNV = (PFNGLGETPATHCOMMANDSNVPROC)load("glGetPathCommandsNV", userptr);
    glad_glGetPathCoordsNV = (PFNGLGETPATHCOORDSNVPROC)load("glGetPathCoordsNV", userptr);
    glad_glMatrixLoad3x3fNV = (PFNGLMATRIXLOAD3X3FNVPROC)load("glMatrixLoad3x3fNV", userptr);
    glad_glMatrixMult3x3fNV = (PFNGLMATRIXMULT3X3FNVPROC)load("glMatrixMult3x3fNV", userptr);
    glad_glGetPathLengthNV = (PFNGLGETPATHLENGTHNVPROC)load("glGetPathLengthNV", userptr);
    glad_glMatrixMultTranspose3x3fNV = (PFNGLMATRIXMULTTRANSPOSE3X3FNVPROC)load("glMatrixMultTranspose3x3fNV", userptr);
    glad_glStencilThenCoverFillPathInstancedNV = (PFNGLSTENCILTHENCOVERFILLPATHINSTANCEDNVPROC)load("glStencilThenCoverFillPathInstancedNV", userptr);
    glad_glCoverFillPathNV = (PFNGLCOVERFILLPATHNVPROC)load("glCoverFillPathNV", userptr);
    glad_glGetProgramResourcefvNV = (PFNGLGETPROGRAMRESOURCEFVNVPROC)load("glGetProgramResourcefvNV", userptr);
    glad_glPathParameteriNV = (PFNGLPATHPARAMETERINVPROC)load("glPathParameteriNV", userptr);
    glad_glPathParameterfNV = (PFNGLPATHPARAMETERFNVPROC)load("glPathParameterfNV", userptr);
    glad_glInterpolatePathsNV = (PFNGLINTERPOLATEPATHSNVPROC)load("glInterpolatePathsNV", userptr);
    glad_glIsPathNV = (PFNGLISPATHNVPROC)load("glIsPathNV", userptr);
    glad_glPathCommandsNV = (PFNGLPATHCOMMANDSNVPROC)load("glPathCommandsNV", userptr);
    glad_glStencilThenCoverStrokePathInstancedNV = (PFNGLSTENCILTHENCOVERSTROKEPATHINSTANCEDNVPROC)load("glStencilThenCoverStrokePathInstancedNV", userptr);
    glad_glPathGlyphIndexRangeNV = (PFNGLPATHGLYPHINDEXRANGENVPROC)load("glPathGlyphIndexRangeNV", userptr);
    glad_glPathGlyphRangeNV = (PFNGLPATHGLYPHRANGENVPROC)load("glPathGlyphRangeNV", userptr);
    glad_glGetPathMetricsNV = (PFNGLGETPATHMETRICSNVPROC)load("glGetPathMetricsNV", userptr);
    glad_glPathCoordsNV = (PFNGLPATHCOORDSNVPROC)load("glPathCoordsNV", userptr);
    glad_glPathDashArrayNV = (PFNGLPATHDASHARRAYNVPROC)load("glPathDashArrayNV", userptr);
    glad_glIsPointInStrokePathNV = (PFNGLISPOINTINSTROKEPATHNVPROC)load("glIsPointInStrokePathNV", userptr);
    glad_glStencilThenCoverFillPathNV = (PFNGLSTENCILTHENCOVERFILLPATHNVPROC)load("glStencilThenCoverFillPathNV", userptr);
}
static void load_GL_ARB_framebuffer_object(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_framebuffer_object) return;
    glad_glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)load("glGetRenderbufferParameteriv", userptr);
    glad_glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)load("glBindRenderbuffer", userptr);
    glad_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)load("glDeleteFramebuffers", userptr);
    glad_glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)load("glFramebufferRenderbuffer", userptr);
    glad_glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)load("glRenderbufferStorageMultisample", userptr);
    glad_glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)load("glGetFramebufferAttachmentParameteriv", userptr);
    glad_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)load("glGenFramebuffers", userptr);
    glad_glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)load("glIsRenderbuffer", userptr);
    glad_glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)load("glFramebufferTexture3D", userptr);
    glad_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)load("glFramebufferTexture2D", userptr);
    glad_glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)load("glGenRenderbuffers", userptr);
    glad_glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)load("glDeleteRenderbuffers", userptr);
    glad_glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)load("glFramebufferTextureLayer", userptr);
    glad_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)load("glBindFramebuffer", userptr);
    glad_glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)load("glRenderbufferStorage", userptr);
    glad_glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)load("glIsFramebuffer", userptr);
    glad_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)load("glCheckFramebufferStatus", userptr);
    glad_glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)load("glBlitFramebuffer", userptr);
    glad_glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)load("glGenerateMipmap", userptr);
    glad_glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)load("glFramebufferTexture1D", userptr);
}
static void load_GL_ARB_draw_buffers_blend(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_draw_buffers_blend) return;
    glad_glBlendFuncSeparateiARB = (PFNGLBLENDFUNCSEPARATEIARBPROC)load("glBlendFuncSeparateiARB", userptr);
    glad_glBlendEquationiARB = (PFNGLBLENDEQUATIONIARBPROC)load("glBlendEquationiARB", userptr);
    glad_glBlendEquationSeparateiARB = (PFNGLBLENDEQUATIONSEPARATEIARBPROC)load("glBlendEquationSeparateiARB", userptr);
    glad_glBlendFunciARB = (PFNGLBLENDFUNCIARBPROC)load("glBlendFunciARB", userptr);
}
static void load_GL_ARB_direct_state_access(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_direct_state_access) return;
    glad_glTextureParameterfv = (PFNGLTEXTUREPARAMETERFVPROC)load("glTextureParameterfv", userptr);
    glad_glNamedBufferStorage = (PFNGLNAMEDBUFFERSTORAGEPROC)load("glNamedBufferStorage", userptr);
    glad_glGetVertexArrayIndexediv = (PFNGLGETVERTEXARRAYINDEXEDIVPROC)load("glGetVertexArrayIndexediv", userptr);
    glad_glCheckNamedFramebufferStatus = (PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC)load("glCheckNamedFramebufferStatus", userptr);
    glad_glTextureParameterIiv = (PFNGLTEXTUREPARAMETERIIVPROC)load("glTextureParameterIiv", userptr);
    glad_glVertexArrayElementBuffer = (PFNGLVERTEXARRAYELEMENTBUFFERPROC)load("glVertexArrayElementBuffer", userptr);
    glad_glNamedRenderbufferStorage = (PFNGLNAMEDRENDERBUFFERSTORAGEPROC)load("glNamedRenderbufferStorage", userptr);
    glad_glGetNamedFramebufferAttachmentParameteriv = (PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC)load("glGetNamedFramebufferAttachmentParameteriv", userptr);
    glad_glGetNamedBufferParameteriv = (PFNGLGETNAMEDBUFFERPARAMETERIVPROC)load("glGetNamedBufferParameteriv", userptr);
    glad_glTextureStorage3DMultisample = (PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC)load("glTextureStorage3DMultisample", userptr);
    glad_glCopyTextureSubImage3D = (PFNGLCOPYTEXTURESUBIMAGE3DPROC)load("glCopyTextureSubImage3D", userptr);
    glad_glNamedFramebufferTexture = (PFNGLNAMEDFRAMEBUFFERTEXTUREPROC)load("glNamedFramebufferTexture", userptr);
    glad_glGetTextureLevelParameteriv = (PFNGLGETTEXTURELEVELPARAMETERIVPROC)load("glGetTextureLevelParameteriv", userptr);
    glad_glCreateFramebuffers = (PFNGLCREATEFRAMEBUFFERSPROC)load("glCreateFramebuffers", userptr);
    glad_glTextureStorage3D = (PFNGLTEXTURESTORAGE3DPROC)load("glTextureStorage3D", userptr);
    glad_glCompressedTextureSubImage1D = (PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC)load("glCompressedTextureSubImage1D", userptr);
    glad_glVertexArrayVertexBuffers = (PFNGLVERTEXARRAYVERTEXBUFFERSPROC)load("glVertexArrayVertexBuffers", userptr);
    glad_glGetNamedBufferParameteri64v = (PFNGLGETNAMEDBUFFERPARAMETERI64VPROC)load("glGetNamedBufferParameteri64v", userptr);
    glad_glCreateBuffers = (PFNGLCREATEBUFFERSPROC)load("glCreateBuffers", userptr);
    glad_glGetTextureLevelParameterfv = (PFNGLGETTEXTURELEVELPARAMETERFVPROC)load("glGetTextureLevelParameterfv", userptr);
    glad_glGetNamedBufferPointerv = (PFNGLGETNAMEDBUFFERPOINTERVPROC)load("glGetNamedBufferPointerv", userptr);
    glad_glCreateProgramPipelines = (PFNGLCREATEPROGRAMPIPELINESPROC)load("glCreateProgramPipelines", userptr);
    glad_glInvalidateNamedFramebufferSubData = (PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC)load("glInvalidateNamedFramebufferSubData", userptr);
    glad_glMapNamedBufferRange = (PFNGLMAPNAMEDBUFFERRANGEPROC)load("glMapNamedBufferRange", userptr);
    glad_glVertexArrayAttribBinding = (PFNGLVERTEXARRAYATTRIBBINDINGPROC)load("glVertexArrayAttribBinding", userptr);
    glad_glDisableVertexArrayAttrib = (PFNGLDISABLEVERTEXARRAYATTRIBPROC)load("glDisableVertexArrayAttrib", userptr);
    glad_glTextureSubImage1D = (PFNGLTEXTURESUBIMAGE1DPROC)load("glTextureSubImage1D", userptr);
    glad_glTransformFeedbackBufferRange = (PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC)load("glTransformFeedbackBufferRange", userptr);
    glad_glGetTransformFeedbacki64_v = (PFNGLGETTRANSFORMFEEDBACKI64_VPROC)load("glGetTransformFeedbacki64_v", userptr);
    glad_glCreateTransformFeedbacks = (PFNGLCREATETRANSFORMFEEDBACKSPROC)load("glCreateTransformFeedbacks", userptr);
    glad_glGetNamedFramebufferParameteriv = (PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC)load("glGetNamedFramebufferParameteriv", userptr);
    glad_glVertexArrayAttribIFormat = (PFNGLVERTEXARRAYATTRIBIFORMATPROC)load("glVertexArrayAttribIFormat", userptr);
    glad_glCreateVertexArrays = (PFNGLCREATEVERTEXARRAYSPROC)load("glCreateVertexArrays", userptr);
    glad_glVertexArrayVertexBuffer = (PFNGLVERTEXARRAYVERTEXBUFFERPROC)load("glVertexArrayVertexBuffer", userptr);
    glad_glCreateRenderbuffers = (PFNGLCREATERENDERBUFFERSPROC)load("glCreateRenderbuffers", userptr);
    glad_glTransformFeedbackBufferBase = (PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC)load("glTransformFeedbackBufferBase", userptr);
    glad_glClearNamedFramebufferuiv = (PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC)load("glClearNamedFramebufferuiv", userptr);
    glad_glGetCompressedTextureImage = (PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC)load("glGetCompressedTextureImage", userptr);
    glad_glInvalidateNamedFramebufferData = (PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC)load("glInvalidateNamedFramebufferData", userptr);
    glad_glNamedFramebufferReadBuffer = (PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC)load("glNamedFramebufferReadBuffer", userptr);
    glad_glGetTransformFeedbacki_v = (PFNGLGETTRANSFORMFEEDBACKI_VPROC)load("glGetTransformFeedbacki_v", userptr);
    glad_glTextureStorage2DMultisample = (PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC)load("glTextureStorage2DMultisample", userptr);
    glad_glNamedBufferData = (PFNGLNAMEDBUFFERDATAPROC)load("glNamedBufferData", userptr);
    glad_glGetVertexArrayiv = (PFNGLGETVERTEXARRAYIVPROC)load("glGetVertexArrayiv", userptr);
    glad_glGetTransformFeedbackiv = (PFNGLGETTRANSFORMFEEDBACKIVPROC)load("glGetTransformFeedbackiv", userptr);
    glad_glBlitNamedFramebuffer = (PFNGLBLITNAMEDFRAMEBUFFERPROC)load("glBlitNamedFramebuffer", userptr);
    glad_glVertexArrayAttribLFormat = (PFNGLVERTEXARRAYATTRIBLFORMATPROC)load("glVertexArrayAttribLFormat", userptr);
    glad_glCreateQueries = (PFNGLCREATEQUERIESPROC)load("glCreateQueries", userptr);
    glad_glGetQueryBufferObjectuiv = (PFNGLGETQUERYBUFFEROBJECTUIVPROC)load("glGetQueryBufferObjectuiv", userptr);
    glad_glClearNamedFramebufferfi = (PFNGLCLEARNAMEDFRAMEBUFFERFIPROC)load("glClearNamedFramebufferfi", userptr);
    glad_glGetTextureParameterIuiv = (PFNGLGETTEXTUREPARAMETERIUIVPROC)load("glGetTextureParameterIuiv", userptr);
    glad_glClearNamedFramebufferfv = (PFNGLCLEARNAMEDFRAMEBUFFERFVPROC)load("glClearNamedFramebufferfv", userptr);
    glad_glGetVertexArrayIndexed64iv = (PFNGLGETVERTEXARRAYINDEXED64IVPROC)load("glGetVertexArrayIndexed64iv", userptr);
    glad_glTextureParameteri = (PFNGLTEXTUREPARAMETERIPROC)load("glTextureParameteri", userptr);
    glad_glClearNamedBufferSubData = (PFNGLCLEARNAMEDBUFFERSUBDATAPROC)load("glClearNamedBufferSubData", userptr);
    glad_glVertexArrayAttribFormat = (PFNGLVERTEXARRAYATTRIBFORMATPROC)load("glVertexArrayAttribFormat", userptr);
    glad_glCopyTextureSubImage2D = (PFNGLCOPYTEXTURESUBIMAGE2DPROC)load("glCopyTextureSubImage2D", userptr);
    glad_glGenerateTextureMipmap = (PFNGLGENERATETEXTUREMIPMAPPROC)load("glGenerateTextureMipmap", userptr);
    glad_glNamedBufferSubData = (PFNGLNAMEDBUFFERSUBDATAPROC)load("glNamedBufferSubData", userptr);
    glad_glGetQueryBufferObjectiv = (PFNGLGETQUERYBUFFEROBJECTIVPROC)load("glGetQueryBufferObjectiv", userptr);
    glad_glTextureStorage1D = (PFNGLTEXTURESTORAGE1DPROC)load("glTextureStorage1D", userptr);
    glad_glBindTextureUnit = (PFNGLBINDTEXTUREUNITPROC)load("glBindTextureUnit", userptr);
    glad_glNamedFramebufferDrawBuffers = (PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC)load("glNamedFramebufferDrawBuffers", userptr);
    glad_glGetNamedBufferSubData = (PFNGLGETNAMEDBUFFERSUBDATAPROC)load("glGetNamedBufferSubData", userptr);
    glad_glTextureSubImage2D = (PFNGLTEXTURESUBIMAGE2DPROC)load("glTextureSubImage2D", userptr);
    glad_glGetTextureParameterIiv = (PFNGLGETTEXTUREPARAMETERIIVPROC)load("glGetTextureParameterIiv", userptr);
    glad_glCompressedTextureSubImage3D = (PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC)load("glCompressedTextureSubImage3D", userptr);
    glad_glGetTextureParameteriv = (PFNGLGETTEXTUREPARAMETERIVPROC)load("glGetTextureParameteriv", userptr);
    glad_glCompressedTextureSubImage2D = (PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC)load("glCompressedTextureSubImage2D", userptr);
    glad_glClearNamedFramebufferiv = (PFNGLCLEARNAMEDFRAMEBUFFERIVPROC)load("glClearNamedFramebufferiv", userptr);
    glad_glMapNamedBuffer = (PFNGLMAPNAMEDBUFFERPROC)load("glMapNamedBuffer", userptr);
    glad_glTextureBuffer = (PFNGLTEXTUREBUFFERPROC)load("glTextureBuffer", userptr);
    glad_glTextureParameterIuiv = (PFNGLTEXTUREPARAMETERIUIVPROC)load("glTextureParameterIuiv", userptr);
    glad_glTextureBufferRange = (PFNGLTEXTUREBUFFERRANGEPROC)load("glTextureBufferRange", userptr);
    glad_glUnmapNamedBuffer = (PFNGLUNMAPNAMEDBUFFERPROC)load("glUnmapNamedBuffer", userptr);
    glad_glGetNamedRenderbufferParameteriv = (PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC)load("glGetNamedRenderbufferParameteriv", userptr);
    glad_glGetTextureParameterfv = (PFNGLGETTEXTUREPARAMETERFVPROC)load("glGetTextureParameterfv", userptr);
    glad_glFlushMappedNamedBufferRange = (PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC)load("glFlushMappedNamedBufferRange", userptr);
    glad_glGetTextureImage = (PFNGLGETTEXTUREIMAGEPROC)load("glGetTextureImage", userptr);
    glad_glGetQueryBufferObjectui64v = (PFNGLGETQUERYBUFFEROBJECTUI64VPROC)load("glGetQueryBufferObjectui64v", userptr);
    glad_glNamedRenderbufferStorageMultisample = (PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC)load("glNamedRenderbufferStorageMultisample", userptr);
    glad_glCopyTextureSubImage1D = (PFNGLCOPYTEXTURESUBIMAGE1DPROC)load("glCopyTextureSubImage1D", userptr);
    glad_glTextureStorage2D = (PFNGLTEXTURESTORAGE2DPROC)load("glTextureStorage2D", userptr);
    glad_glClearNamedBufferData = (PFNGLCLEARNAMEDBUFFERDATAPROC)load("glClearNamedBufferData", userptr);
    glad_glEnableVertexArrayAttrib = (PFNGLENABLEVERTEXARRAYATTRIBPROC)load("glEnableVertexArrayAttrib", userptr);
    glad_glVertexArrayBindingDivisor = (PFNGLVERTEXARRAYBINDINGDIVISORPROC)load("glVertexArrayBindingDivisor", userptr);
    glad_glCopyNamedBufferSubData = (PFNGLCOPYNAMEDBUFFERSUBDATAPROC)load("glCopyNamedBufferSubData", userptr);
    glad_glCreateSamplers = (PFNGLCREATESAMPLERSPROC)load("glCreateSamplers", userptr);
    glad_glTextureParameteriv = (PFNGLTEXTUREPARAMETERIVPROC)load("glTextureParameteriv", userptr);
    glad_glNamedFramebufferTextureLayer = (PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC)load("glNamedFramebufferTextureLayer", userptr);
    glad_glNamedFramebufferParameteri = (PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC)load("glNamedFramebufferParameteri", userptr);
    glad_glGetQueryBufferObjecti64v = (PFNGLGETQUERYBUFFEROBJECTI64VPROC)load("glGetQueryBufferObjecti64v", userptr);
    glad_glTextureSubImage3D = (PFNGLTEXTURESUBIMAGE3DPROC)load("glTextureSubImage3D", userptr);
    glad_glNamedFramebufferDrawBuffer = (PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC)load("glNamedFramebufferDrawBuffer", userptr);
    glad_glCreateTextures = (PFNGLCREATETEXTURESPROC)load("glCreateTextures", userptr);
    glad_glTextureParameterf = (PFNGLTEXTUREPARAMETERFPROC)load("glTextureParameterf", userptr);
    glad_glNamedFramebufferRenderbuffer = (PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC)load("glNamedFramebufferRenderbuffer", userptr);
}
static void load_GL_ARB_gpu_shader_int64(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_gpu_shader_int64) return;
    glad_glProgramUniform3i64vARB = (PFNGLPROGRAMUNIFORM3I64VARBPROC)load("glProgramUniform3i64vARB", userptr);
    glad_glProgramUniform1i64ARB = (PFNGLPROGRAMUNIFORM1I64ARBPROC)load("glProgramUniform1i64ARB", userptr);
    glad_glProgramUniform2i64vARB = (PFNGLPROGRAMUNIFORM2I64VARBPROC)load("glProgramUniform2i64vARB", userptr);
    glad_glProgramUniform2i64ARB = (PFNGLPROGRAMUNIFORM2I64ARBPROC)load("glProgramUniform2i64ARB", userptr);
    glad_glProgramUniform2ui64vARB = (PFNGLPROGRAMUNIFORM2UI64VARBPROC)load("glProgramUniform2ui64vARB", userptr);
    glad_glUniform2ui64ARB = (PFNGLUNIFORM2UI64ARBPROC)load("glUniform2ui64ARB", userptr);
    glad_glProgramUniform4ui64vARB = (PFNGLPROGRAMUNIFORM4UI64VARBPROC)load("glProgramUniform4ui64vARB", userptr);
    glad_glProgramUniform1ui64vARB = (PFNGLPROGRAMUNIFORM1UI64VARBPROC)load("glProgramUniform1ui64vARB", userptr);
    glad_glUniform3i64vARB = (PFNGLUNIFORM3I64VARBPROC)load("glUniform3i64vARB", userptr);
    glad_glUniform1ui64ARB = (PFNGLUNIFORM1UI64ARBPROC)load("glUniform1ui64ARB", userptr);
    glad_glGetUniformui64vARB = (PFNGLGETUNIFORMUI64VARBPROC)load("glGetUniformui64vARB", userptr);
    glad_glUniform3ui64ARB = (PFNGLUNIFORM3UI64ARBPROC)load("glUniform3ui64ARB", userptr);
    glad_glProgramUniform4ui64ARB = (PFNGLPROGRAMUNIFORM4UI64ARBPROC)load("glProgramUniform4ui64ARB", userptr);
    glad_glUniform4ui64ARB = (PFNGLUNIFORM4UI64ARBPROC)load("glUniform4ui64ARB", userptr);
    glad_glProgramUniform4i64vARB = (PFNGLPROGRAMUNIFORM4I64VARBPROC)load("glProgramUniform4i64vARB", userptr);
    glad_glUniform4i64ARB = (PFNGLUNIFORM4I64ARBPROC)load("glUniform4i64ARB", userptr);
    glad_glProgramUniform3ui64ARB = (PFNGLPROGRAMUNIFORM3UI64ARBPROC)load("glProgramUniform3ui64ARB", userptr);
    glad_glProgramUniform2ui64ARB = (PFNGLPROGRAMUNIFORM2UI64ARBPROC)load("glProgramUniform2ui64ARB", userptr);
    glad_glUniform1i64vARB = (PFNGLUNIFORM1I64VARBPROC)load("glUniform1i64vARB", userptr);
    glad_glProgramUniform3i64ARB = (PFNGLPROGRAMUNIFORM3I64ARBPROC)load("glProgramUniform3i64ARB", userptr);
    glad_glUniform4i64vARB = (PFNGLUNIFORM4I64VARBPROC)load("glUniform4i64vARB", userptr);
    glad_glUniform2i64ARB = (PFNGLUNIFORM2I64ARBPROC)load("glUniform2i64ARB", userptr);
    glad_glUniform4ui64vARB = (PFNGLUNIFORM4UI64VARBPROC)load("glUniform4ui64vARB", userptr);
    glad_glGetnUniformi64vARB = (PFNGLGETNUNIFORMI64VARBPROC)load("glGetnUniformi64vARB", userptr);
    glad_glUniform1i64ARB = (PFNGLUNIFORM1I64ARBPROC)load("glUniform1i64ARB", userptr);
    glad_glUniform3ui64vARB = (PFNGLUNIFORM3UI64VARBPROC)load("glUniform3ui64vARB", userptr);
    glad_glGetUniformi64vARB = (PFNGLGETUNIFORMI64VARBPROC)load("glGetUniformi64vARB", userptr);
    glad_glUniform1ui64vARB = (PFNGLUNIFORM1UI64VARBPROC)load("glUniform1ui64vARB", userptr);
    glad_glUniform2ui64vARB = (PFNGLUNIFORM2UI64VARBPROC)load("glUniform2ui64vARB", userptr);
    glad_glProgramUniform1ui64ARB = (PFNGLPROGRAMUNIFORM1UI64ARBPROC)load("glProgramUniform1ui64ARB", userptr);
    glad_glProgramUniform1i64vARB = (PFNGLPROGRAMUNIFORM1I64VARBPROC)load("glProgramUniform1i64vARB", userptr);
    glad_glUniform2i64vARB = (PFNGLUNIFORM2I64VARBPROC)load("glUniform2i64vARB", userptr);
    glad_glProgramUniform4i64ARB = (PFNGLPROGRAMUNIFORM4I64ARBPROC)load("glProgramUniform4i64ARB", userptr);
    glad_glProgramUniform3ui64vARB = (PFNGLPROGRAMUNIFORM3UI64VARBPROC)load("glProgramUniform3ui64vARB", userptr);
    glad_glGetnUniformui64vARB = (PFNGLGETNUNIFORMUI64VARBPROC)load("glGetnUniformui64vARB", userptr);
    glad_glUniform3i64ARB = (PFNGLUNIFORM3I64ARBPROC)load("glUniform3i64ARB", userptr);
}
static void load_GL_ARB_texture_storage(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_texture_storage) return;
    glad_glTexStorage1D = (PFNGLTEXSTORAGE1DPROC)load("glTexStorage1D", userptr);
    glad_glTexStorage2D = (PFNGLTEXSTORAGE2DPROC)load("glTexStorage2D", userptr);
    glad_glTexStorage3D = (PFNGLTEXSTORAGE3DPROC)load("glTexStorage3D", userptr);
}
static void load_GL_NV_vertex_program(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_NV_vertex_program) return;
    glad_glVertexAttribs2fvNV = (PFNGLVERTEXATTRIBS2FVNVPROC)load("glVertexAttribs2fvNV", userptr);
    glad_glBindProgramNV = (PFNGLBINDPROGRAMNVPROC)load("glBindProgramNV", userptr);
    glad_glVertexAttrib3sNV = (PFNGLVERTEXATTRIB3SNVPROC)load("glVertexAttrib3sNV", userptr);
    glad_glGetProgramParameterdvNV = (PFNGLGETPROGRAMPARAMETERDVNVPROC)load("glGetProgramParameterdvNV", userptr);
    glad_glVertexAttrib4fNV = (PFNGLVERTEXATTRIB4FNVPROC)load("glVertexAttrib4fNV", userptr);
    glad_glVertexAttrib3fvNV = (PFNGLVERTEXATTRIB3FVNVPROC)load("glVertexAttrib3fvNV", userptr);
    glad_glDeleteProgramsNV = (PFNGLDELETEPROGRAMSNVPROC)load("glDeleteProgramsNV", userptr);
    glad_glVertexAttrib4svNV = (PFNGLVERTEXATTRIB4SVNVPROC)load("glVertexAttrib4svNV", userptr);
    glad_glVertexAttrib4fvNV = (PFNGLVERTEXATTRIB4FVNVPROC)load("glVertexAttrib4fvNV", userptr);
    glad_glVertexAttribPointerNV = (PFNGLVERTEXATTRIBPOINTERNVPROC)load("glVertexAttribPointerNV", userptr);
    glad_glProgramParameters4dvNV = (PFNGLPROGRAMPARAMETERS4DVNVPROC)load("glProgramParameters4dvNV", userptr);
    glad_glVertexAttribs3svNV = (PFNGLVERTEXATTRIBS3SVNVPROC)load("glVertexAttribs3svNV", userptr);
    glad_glVertexAttrib1fNV = (PFNGLVERTEXATTRIB1FNVPROC)load("glVertexAttrib1fNV", userptr);
    glad_glGetVertexAttribivNV = (PFNGLGETVERTEXATTRIBIVNVPROC)load("glGetVertexAttribivNV", userptr);
    glad_glVertexAttribs4ubvNV = (PFNGLVERTEXATTRIBS4UBVNVPROC)load("glVertexAttribs4ubvNV", userptr);
    glad_glVertexAttrib3svNV = (PFNGLVERTEXATTRIB3SVNVPROC)load("glVertexAttrib3svNV", userptr);
    glad_glGetVertexAttribfvNV = (PFNGLGETVERTEXATTRIBFVNVPROC)load("glGetVertexAttribfvNV", userptr);
    glad_glVertexAttribs1fvNV = (PFNGLVERTEXATTRIBS1FVNVPROC)load("glVertexAttribs1fvNV", userptr);
    glad_glProgramParameter4fNV = (PFNGLPROGRAMPARAMETER4FNVPROC)load("glProgramParameter4fNV", userptr);
    glad_glVertexAttrib1fvNV = (PFNGLVERTEXATTRIB1FVNVPROC)load("glVertexAttrib1fvNV", userptr);
    glad_glProgramParameter4dNV = (PFNGLPROGRAMPARAMETER4DNVPROC)load("glProgramParameter4dNV", userptr);
    glad_glVertexAttrib2dvNV = (PFNGLVERTEXATTRIB2DVNVPROC)load("glVertexAttrib2dvNV", userptr);
    glad_glVertexAttribs1dvNV = (PFNGLVERTEXATTRIBS1DVNVPROC)load("glVertexAttribs1dvNV", userptr);
    glad_glVertexAttribs4dvNV = (PFNGLVERTEXATTRIBS4DVNVPROC)load("glVertexAttribs4dvNV", userptr);
    glad_glGetProgramivNV = (PFNGLGETPROGRAMIVNVPROC)load("glGetProgramivNV", userptr);
    glad_glVertexAttribs4fvNV = (PFNGLVERTEXATTRIBS4FVNVPROC)load("glVertexAttribs4fvNV", userptr);
    glad_glGenProgramsNV = (PFNGLGENPROGRAMSNVPROC)load("glGenProgramsNV", userptr);
    glad_glVertexAttrib3dvNV = (PFNGLVERTEXATTRIB3DVNVPROC)load("glVertexAttrib3dvNV", userptr);
    glad_glVertexAttrib2sNV = (PFNGLVERTEXATTRIB2SNVPROC)load("glVertexAttrib2sNV", userptr);
    glad_glTrackMatrixNV = (PFNGLTRACKMATRIXNVPROC)load("glTrackMatrixNV", userptr);
    glad_glAreProgramsResidentNV = (PFNGLAREPROGRAMSRESIDENTNVPROC)load("glAreProgramsResidentNV", userptr);
    glad_glVertexAttrib1dNV = (PFNGLVERTEXATTRIB1DNVPROC)load("glVertexAttrib1dNV", userptr);
    glad_glProgramParameter4dvNV = (PFNGLPROGRAMPARAMETER4DVNVPROC)load("glProgramParameter4dvNV", userptr);
    glad_glVertexAttrib4dNV = (PFNGLVERTEXATTRIB4DNVPROC)load("glVertexAttrib4dNV", userptr);
    glad_glVertexAttribs1svNV = (PFNGLVERTEXATTRIBS1SVNVPROC)load("glVertexAttribs1svNV", userptr);
    glad_glVertexAttrib1svNV = (PFNGLVERTEXATTRIB1SVNVPROC)load("glVertexAttrib1svNV", userptr);
    glad_glVertexAttribs4svNV = (PFNGLVERTEXATTRIBS4SVNVPROC)load("glVertexAttribs4svNV", userptr);
    glad_glLoadProgramNV = (PFNGLLOADPROGRAMNVPROC)load("glLoadProgramNV", userptr);
    glad_glVertexAttrib4ubNV = (PFNGLVERTEXATTRIB4UBNVPROC)load("glVertexAttrib4ubNV", userptr);
    glad_glVertexAttribs3fvNV = (PFNGLVERTEXATTRIBS3FVNVPROC)load("glVertexAttribs3fvNV", userptr);
    glad_glVertexAttrib4sNV = (PFNGLVERTEXATTRIB4SNVPROC)load("glVertexAttrib4sNV", userptr);
    glad_glGetProgramParameterfvNV = (PFNGLGETPROGRAMPARAMETERFVNVPROC)load("glGetProgramParameterfvNV", userptr);
    glad_glRequestResidentProgramsNV = (PFNGLREQUESTRESIDENTPROGRAMSNVPROC)load("glRequestResidentProgramsNV", userptr);
    glad_glGetVertexAttribdvNV = (PFNGLGETVERTEXATTRIBDVNVPROC)load("glGetVertexAttribdvNV", userptr);
    glad_glVertexAttribs3dvNV = (PFNGLVERTEXATTRIBS3DVNVPROC)load("glVertexAttribs3dvNV", userptr);
    glad_glGetTrackMatrixivNV = (PFNGLGETTRACKMATRIXIVNVPROC)load("glGetTrackMatrixivNV", userptr);
    glad_glVertexAttrib1sNV = (PFNGLVERTEXATTRIB1SNVPROC)load("glVertexAttrib1sNV", userptr);
    glad_glVertexAttrib2dNV = (PFNGLVERTEXATTRIB2DNVPROC)load("glVertexAttrib2dNV", userptr);
    glad_glVertexAttrib2fvNV = (PFNGLVERTEXATTRIB2FVNVPROC)load("glVertexAttrib2fvNV", userptr);
    glad_glVertexAttrib3dNV = (PFNGLVERTEXATTRIB3DNVPROC)load("glVertexAttrib3dNV", userptr);
    glad_glVertexAttribs2dvNV = (PFNGLVERTEXATTRIBS2DVNVPROC)load("glVertexAttribs2dvNV", userptr);
    glad_glExecuteProgramNV = (PFNGLEXECUTEPROGRAMNVPROC)load("glExecuteProgramNV", userptr);
    glad_glIsProgramNV = (PFNGLISPROGRAMNVPROC)load("glIsProgramNV", userptr);
    glad_glVertexAttrib2fNV = (PFNGLVERTEXATTRIB2FNVPROC)load("glVertexAttrib2fNV", userptr);
    glad_glVertexAttrib3fNV = (PFNGLVERTEXATTRIB3FNVPROC)load("glVertexAttrib3fNV", userptr);
    glad_glGetProgramStringNV = (PFNGLGETPROGRAMSTRINGNVPROC)load("glGetProgramStringNV", userptr);
    glad_glVertexAttrib4ubvNV = (PFNGLVERTEXATTRIB4UBVNVPROC)load("glVertexAttrib4ubvNV", userptr);
    glad_glVertexAttrib1dvNV = (PFNGLVERTEXATTRIB1DVNVPROC)load("glVertexAttrib1dvNV", userptr);
    glad_glVertexAttrib2svNV = (PFNGLVERTEXATTRIB2SVNVPROC)load("glVertexAttrib2svNV", userptr);
    glad_glGetVertexAttribPointervNV = (PFNGLGETVERTEXATTRIBPOINTERVNVPROC)load("glGetVertexAttribPointervNV", userptr);
    glad_glProgramParameter4fvNV = (PFNGLPROGRAMPARAMETER4FVNVPROC)load("glProgramParameter4fvNV", userptr);
    glad_glVertexAttrib4dvNV = (PFNGLVERTEXATTRIB4DVNVPROC)load("glVertexAttrib4dvNV", userptr);
    glad_glProgramParameters4fvNV = (PFNGLPROGRAMPARAMETERS4FVNVPROC)load("glProgramParameters4fvNV", userptr);
    glad_glVertexAttribs2svNV = (PFNGLVERTEXATTRIBS2SVNVPROC)load("glVertexAttribs2svNV", userptr);
}
static void load_GL_ARB_color_buffer_float(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_color_buffer_float) return;
    glad_glClampColorARB = (PFNGLCLAMPCOLORARBPROC)load("glClampColorARB", userptr);
}
static void load_GL_ARB_clear_buffer_object(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_clear_buffer_object) return;
    glad_glClearBufferSubData = (PFNGLCLEARBUFFERSUBDATAPROC)load("glClearBufferSubData", userptr);
    glad_glClearBufferData = (PFNGLCLEARBUFFERDATAPROC)load("glClearBufferData", userptr);
}
static void load_GL_ARB_multisample(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_multisample) return;
    glad_glSampleCoverageARB = (PFNGLSAMPLECOVERAGEARBPROC)load("glSampleCoverageARB", userptr);
}
static void load_GL_NV_conditional_render(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_NV_conditional_render) return;
    glad_glBeginConditionalRenderNV = (PFNGLBEGINCONDITIONALRENDERNVPROC)load("glBeginConditionalRenderNV", userptr);
    glad_glEndConditionalRenderNV = (PFNGLENDCONDITIONALRENDERNVPROC)load("glEndConditionalRenderNV", userptr);
}
static void load_GL_EXT_texture_filter_anisotropic(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_texture_filter_anisotropic) return;
}
static void load_GL_ARB_shader_storage_buffer_object(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_shader_storage_buffer_object) return;
    glad_glShaderStorageBlockBinding = (PFNGLSHADERSTORAGEBLOCKBINDINGPROC)load("glShaderStorageBlockBinding", userptr);
}
static void load_GL_ARB_sample_shading(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_sample_shading) return;
    glad_glMinSampleShadingARB = (PFNGLMINSAMPLESHADINGARBPROC)load("glMinSampleShadingARB", userptr);
}
static void load_GL_EXT_blend_minmax(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_blend_minmax) return;
    glad_glBlendEquationEXT = (PFNGLBLENDEQUATIONEXTPROC)load("glBlendEquationEXT", userptr);
}
static void load_GL_EXT_framebuffer_blit(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_framebuffer_blit) return;
    glad_glBlitFramebufferEXT = (PFNGLBLITFRAMEBUFFEREXTPROC)load("glBlitFramebufferEXT", userptr);
}
static void load_GL_ARB_gpu_shader_fp64(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_gpu_shader_fp64) return;
    glad_glUniform2d = (PFNGLUNIFORM2DPROC)load("glUniform2d", userptr);
    glad_glUniformMatrix3x4dv = (PFNGLUNIFORMMATRIX3X4DVPROC)load("glUniformMatrix3x4dv", userptr);
    glad_glUniform1dv = (PFNGLUNIFORM1DVPROC)load("glUniform1dv", userptr);
    glad_glUniform4dv = (PFNGLUNIFORM4DVPROC)load("glUniform4dv", userptr);
    glad_glUniformMatrix2x3dv = (PFNGLUNIFORMMATRIX2X3DVPROC)load("glUniformMatrix2x3dv", userptr);
    glad_glUniform3dv = (PFNGLUNIFORM3DVPROC)load("glUniform3dv", userptr);
    glad_glUniformMatrix3x2dv = (PFNGLUNIFORMMATRIX3X2DVPROC)load("glUniformMatrix3x2dv", userptr);
    glad_glUniformMatrix4x3dv = (PFNGLUNIFORMMATRIX4X3DVPROC)load("glUniformMatrix4x3dv", userptr);
    glad_glUniform2dv = (PFNGLUNIFORM2DVPROC)load("glUniform2dv", userptr);
    glad_glUniform4d = (PFNGLUNIFORM4DPROC)load("glUniform4d", userptr);
    glad_glUniformMatrix2x4dv = (PFNGLUNIFORMMATRIX2X4DVPROC)load("glUniformMatrix2x4dv", userptr);
    glad_glUniform3d = (PFNGLUNIFORM3DPROC)load("glUniform3d", userptr);
    glad_glUniformMatrix2dv = (PFNGLUNIFORMMATRIX2DVPROC)load("glUniformMatrix2dv", userptr);
    glad_glGetUniformdv = (PFNGLGETUNIFORMDVPROC)load("glGetUniformdv", userptr);
    glad_glUniformMatrix4dv = (PFNGLUNIFORMMATRIX4DVPROC)load("glUniformMatrix4dv", userptr);
    glad_glUniformMatrix4x2dv = (PFNGLUNIFORMMATRIX4X2DVPROC)load("glUniformMatrix4x2dv", userptr);
    glad_glUniform1d = (PFNGLUNIFORM1DPROC)load("glUniform1d", userptr);
    glad_glUniformMatrix3dv = (PFNGLUNIFORMMATRIX3DVPROC)load("glUniformMatrix3dv", userptr);
}
static void load_GL_NV_vertex_program4(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_NV_vertex_program4) return;
    glad_glVertexAttribI2ivEXT = (PFNGLVERTEXATTRIBI2IVEXTPROC)load("glVertexAttribI2ivEXT", userptr);
    glad_glVertexAttribI2uiEXT = (PFNGLVERTEXATTRIBI2UIEXTPROC)load("glVertexAttribI2uiEXT", userptr);
    glad_glVertexAttribI4ubvEXT = (PFNGLVERTEXATTRIBI4UBVEXTPROC)load("glVertexAttribI4ubvEXT", userptr);
    glad_glVertexAttribI1uiEXT = (PFNGLVERTEXATTRIBI1UIEXTPROC)load("glVertexAttribI1uiEXT", userptr);
    glad_glVertexAttribI4uivEXT = (PFNGLVERTEXATTRIBI4UIVEXTPROC)load("glVertexAttribI4uivEXT", userptr);
    glad_glVertexAttribI3uiEXT = (PFNGLVERTEXATTRIBI3UIEXTPROC)load("glVertexAttribI3uiEXT", userptr);
    glad_glVertexAttribI4svEXT = (PFNGLVERTEXATTRIBI4SVEXTPROC)load("glVertexAttribI4svEXT", userptr);
    glad_glVertexAttribI4uiEXT = (PFNGLVERTEXATTRIBI4UIEXTPROC)load("glVertexAttribI4uiEXT", userptr);
    glad_glVertexAttribI1uivEXT = (PFNGLVERTEXATTRIBI1UIVEXTPROC)load("glVertexAttribI1uivEXT", userptr);
    glad_glVertexAttribI4ivEXT = (PFNGLVERTEXATTRIBI4IVEXTPROC)load("glVertexAttribI4ivEXT", userptr);
    glad_glGetVertexAttribIivEXT = (PFNGLGETVERTEXATTRIBIIVEXTPROC)load("glGetVertexAttribIivEXT", userptr);
    glad_glVertexAttribI3uivEXT = (PFNGLVERTEXATTRIBI3UIVEXTPROC)load("glVertexAttribI3uivEXT", userptr);
    glad_glVertexAttribI4usvEXT = (PFNGLVERTEXATTRIBI4USVEXTPROC)load("glVertexAttribI4usvEXT", userptr);
    glad_glGetVertexAttribIuivEXT = (PFNGLGETVERTEXATTRIBIUIVEXTPROC)load("glGetVertexAttribIuivEXT", userptr);
    glad_glVertexAttribI3ivEXT = (PFNGLVERTEXATTRIBI3IVEXTPROC)load("glVertexAttribI3ivEXT", userptr);
    glad_glVertexAttribI1iEXT = (PFNGLVERTEXATTRIBI1IEXTPROC)load("glVertexAttribI1iEXT", userptr);
    glad_glVertexAttribI1ivEXT = (PFNGLVERTEXATTRIBI1IVEXTPROC)load("glVertexAttribI1ivEXT", userptr);
    glad_glVertexAttribI2iEXT = (PFNGLVERTEXATTRIBI2IEXTPROC)load("glVertexAttribI2iEXT", userptr);
    glad_glVertexAttribI3iEXT = (PFNGLVERTEXATTRIBI3IEXTPROC)load("glVertexAttribI3iEXT", userptr);
    glad_glVertexAttribI4bvEXT = (PFNGLVERTEXATTRIBI4BVEXTPROC)load("glVertexAttribI4bvEXT", userptr);
    glad_glVertexAttribIPointerEXT = (PFNGLVERTEXATTRIBIPOINTEREXTPROC)load("glVertexAttribIPointerEXT", userptr);
    glad_glVertexAttribI4iEXT = (PFNGLVERTEXATTRIBI4IEXTPROC)load("glVertexAttribI4iEXT", userptr);
    glad_glVertexAttribI2uivEXT = (PFNGLVERTEXATTRIBI2UIVEXTPROC)load("glVertexAttribI2uivEXT", userptr);
}
static void load_GL_ARB_base_instance(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_base_instance) return;
    glad_glDrawElementsInstancedBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC)load("glDrawElementsInstancedBaseInstance", userptr);
    glad_glDrawArraysInstancedBaseInstance = (PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)load("glDrawArraysInstancedBaseInstance", userptr);
    glad_glDrawElementsInstancedBaseVertexBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC)load("glDrawElementsInstancedBaseVertexBaseInstance", userptr);
}
static void load_GL_ARB_compute_shader(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_compute_shader) return;
    glad_glDispatchComputeIndirect = (PFNGLDISPATCHCOMPUTEINDIRECTPROC)load("glDispatchComputeIndirect", userptr);
    glad_glDispatchCompute = (PFNGLDISPATCHCOMPUTEPROC)load("glDispatchCompute", userptr);
}
static void load_GL_ARB_blend_func_extended(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_blend_func_extended) return;
    glad_glGetFragDataIndex = (PFNGLGETFRAGDATAINDEXPROC)load("glGetFragDataIndex", userptr);
    glad_glBindFragDataLocationIndexed = (PFNGLBINDFRAGDATALOCATIONINDEXEDPROC)load("glBindFragDataLocationIndexed", userptr);
}
static void load_GL_ARB_sync(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_sync) return;
    glad_glIsSync = (PFNGLISSYNCPROC)load("glIsSync", userptr);
    glad_glClientWaitSync = (PFNGLCLIENTWAITSYNCPROC)load("glClientWaitSync", userptr);
    glad_glFenceSync = (PFNGLFENCESYNCPROC)load("glFenceSync", userptr);
    glad_glGetSynciv = (PFNGLGETSYNCIVPROC)load("glGetSynciv", userptr);
    glad_glWaitSync = (PFNGLWAITSYNCPROC)load("glWaitSync", userptr);
    glad_glDeleteSync = (PFNGLDELETESYNCPROC)load("glDeleteSync", userptr);
    glad_glGetInteger64v = (PFNGLGETINTEGER64VPROC)load("glGetInteger64v", userptr);
}
static void load_GL_EXT_subtexture(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_subtexture) return;
    glad_glTexSubImage1DEXT = (PFNGLTEXSUBIMAGE1DEXTPROC)load("glTexSubImage1DEXT", userptr);
    glad_glTexSubImage2DEXT = (PFNGLTEXSUBIMAGE2DEXTPROC)load("glTexSubImage2DEXT", userptr);
}
static void load_GL_ARB_ES3_1_compatibility(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_ES3_1_compatibility) return;
    glad_glMemoryBarrierByRegion = (PFNGLMEMORYBARRIERBYREGIONPROC)load("glMemoryBarrierByRegion", userptr);
}
static void load_GL_ARB_bindless_texture(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_bindless_texture) return;
    glad_glVertexAttribL1ui64ARB = (PFNGLVERTEXATTRIBL1UI64ARBPROC)load("glVertexAttribL1ui64ARB", userptr);
    glad_glMakeImageHandleNonResidentARB = (PFNGLMAKEIMAGEHANDLENONRESIDENTARBPROC)load("glMakeImageHandleNonResidentARB", userptr);
    glad_glMakeImageHandleResidentARB = (PFNGLMAKEIMAGEHANDLERESIDENTARBPROC)load("glMakeImageHandleResidentARB", userptr);
    glad_glGetImageHandleARB = (PFNGLGETIMAGEHANDLEARBPROC)load("glGetImageHandleARB", userptr);
    glad_glUniformHandleui64ARB = (PFNGLUNIFORMHANDLEUI64ARBPROC)load("glUniformHandleui64ARB", userptr);
    glad_glGetVertexAttribLui64vARB = (PFNGLGETVERTEXATTRIBLUI64VARBPROC)load("glGetVertexAttribLui64vARB", userptr);
    glad_glUniformHandleui64vARB = (PFNGLUNIFORMHANDLEUI64VARBPROC)load("glUniformHandleui64vARB", userptr);
    glad_glGetTextureHandleARB = (PFNGLGETTEXTUREHANDLEARBPROC)load("glGetTextureHandleARB", userptr);
    glad_glIsImageHandleResidentARB = (PFNGLISIMAGEHANDLERESIDENTARBPROC)load("glIsImageHandleResidentARB", userptr);
    glad_glMakeTextureHandleNonResidentARB = (PFNGLMAKETEXTUREHANDLENONRESIDENTARBPROC)load("glMakeTextureHandleNonResidentARB", userptr);
    glad_glIsTextureHandleResidentARB = (PFNGLISTEXTUREHANDLERESIDENTARBPROC)load("glIsTextureHandleResidentARB", userptr);
    glad_glProgramUniformHandleui64vARB = (PFNGLPROGRAMUNIFORMHANDLEUI64VARBPROC)load("glProgramUniformHandleui64vARB", userptr);
    glad_glGetTextureSamplerHandleARB = (PFNGLGETTEXTURESAMPLERHANDLEARBPROC)load("glGetTextureSamplerHandleARB", userptr);
    glad_glProgramUniformHandleui64ARB = (PFNGLPROGRAMUNIFORMHANDLEUI64ARBPROC)load("glProgramUniformHandleui64ARB", userptr);
    glad_glVertexAttribL1ui64vARB = (PFNGLVERTEXATTRIBL1UI64VARBPROC)load("glVertexAttribL1ui64vARB", userptr);
    glad_glMakeTextureHandleResidentARB = (PFNGLMAKETEXTUREHANDLERESIDENTARBPROC)load("glMakeTextureHandleResidentARB", userptr);
}
static void load_GL_ARB_internalformat_query(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_internalformat_query) return;
    glad_glGetInternalformativ = (PFNGLGETINTERNALFORMATIVPROC)load("glGetInternalformativ", userptr);
}
static void load_GL_EXT_texture_integer(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_texture_integer) return;
    glad_glGetTexParameterIivEXT = (PFNGLGETTEXPARAMETERIIVEXTPROC)load("glGetTexParameterIivEXT", userptr);
    glad_glTexParameterIuivEXT = (PFNGLTEXPARAMETERIUIVEXTPROC)load("glTexParameterIuivEXT", userptr);
    glad_glClearColorIuiEXT = (PFNGLCLEARCOLORIUIEXTPROC)load("glClearColorIuiEXT", userptr);
    glad_glGetTexParameterIuivEXT = (PFNGLGETTEXPARAMETERIUIVEXTPROC)load("glGetTexParameterIuivEXT", userptr);
    glad_glClearColorIiEXT = (PFNGLCLEARCOLORIIEXTPROC)load("glClearColorIiEXT", userptr);
    glad_glTexParameterIivEXT = (PFNGLTEXPARAMETERIIVEXTPROC)load("glTexParameterIivEXT", userptr);
}
static void load_GL_ARB_compute_variable_group_size(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_compute_variable_group_size) return;
    glad_glDispatchComputeGroupSizeARB = (PFNGLDISPATCHCOMPUTEGROUPSIZEARBPROC)load("glDispatchComputeGroupSizeARB", userptr);
}
static void load_GL_ARB_texture_multisample(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_texture_multisample) return;
    glad_glSampleMaski = (PFNGLSAMPLEMASKIPROC)load("glSampleMaski", userptr);
    glad_glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC)load("glGetMultisamplefv", userptr);
    glad_glTexImage3DMultisample = (PFNGLTEXIMAGE3DMULTISAMPLEPROC)load("glTexImage3DMultisample", userptr);
    glad_glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)load("glTexImage2DMultisample", userptr);
}
static void load_GL_AMD_gpu_shader_int64(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_AMD_gpu_shader_int64) return;
    glad_glUniform2i64NV = (PFNGLUNIFORM2I64NVPROC)load("glUniform2i64NV", userptr);
    glad_glProgramUniform3i64NV = (PFNGLPROGRAMUNIFORM3I64NVPROC)load("glProgramUniform3i64NV", userptr);
    glad_glProgramUniform2ui64NV = (PFNGLPROGRAMUNIFORM2UI64NVPROC)load("glProgramUniform2ui64NV", userptr);
    glad_glUniform2ui64NV = (PFNGLUNIFORM2UI64NVPROC)load("glUniform2ui64NV", userptr);
    glad_glUniform1ui64vNV = (PFNGLUNIFORM1UI64VNVPROC)load("glUniform1ui64vNV", userptr);
    glad_glUniform1i64vNV = (PFNGLUNIFORM1I64VNVPROC)load("glUniform1i64vNV", userptr);
    glad_glUniform4i64vNV = (PFNGLUNIFORM4I64VNVPROC)load("glUniform4i64vNV", userptr);
    glad_glUniform1ui64NV = (PFNGLUNIFORM1UI64NVPROC)load("glUniform1ui64NV", userptr);
    glad_glProgramUniform3ui64NV = (PFNGLPROGRAMUNIFORM3UI64NVPROC)load("glProgramUniform3ui64NV", userptr);
    glad_glUniform3i64vNV = (PFNGLUNIFORM3I64VNVPROC)load("glUniform3i64vNV", userptr);
    glad_glGetUniformui64vNV = (PFNGLGETUNIFORMUI64VNVPROC)load("glGetUniformui64vNV", userptr);
    glad_glUniform2i64vNV = (PFNGLUNIFORM2I64VNVPROC)load("glUniform2i64vNV", userptr);
    glad_glProgramUniform1i64NV = (PFNGLPROGRAMUNIFORM1I64NVPROC)load("glProgramUniform1i64NV", userptr);
    glad_glProgramUniform4ui64vNV = (PFNGLPROGRAMUNIFORM4UI64VNVPROC)load("glProgramUniform4ui64vNV", userptr);
    glad_glUniform4ui64vNV = (PFNGLUNIFORM4UI64VNVPROC)load("glUniform4ui64vNV", userptr);
    glad_glUniform2ui64vNV = (PFNGLUNIFORM2UI64VNVPROC)load("glUniform2ui64vNV", userptr);
    glad_glProgramUniform3ui64vNV = (PFNGLPROGRAMUNIFORM3UI64VNVPROC)load("glProgramUniform3ui64vNV", userptr);
    glad_glProgramUniform4ui64NV = (PFNGLPROGRAMUNIFORM4UI64NVPROC)load("glProgramUniform4ui64NV", userptr);
    glad_glUniform3ui64vNV = (PFNGLUNIFORM3UI64VNVPROC)load("glUniform3ui64vNV", userptr);
    glad_glProgramUniform1ui64vNV = (PFNGLPROGRAMUNIFORM1UI64VNVPROC)load("glProgramUniform1ui64vNV", userptr);
    glad_glProgramUniform2i64NV = (PFNGLPROGRAMUNIFORM2I64NVPROC)load("glProgramUniform2i64NV", userptr);
    glad_glUniform4ui64NV = (PFNGLUNIFORM4UI64NVPROC)load("glUniform4ui64NV", userptr);
    glad_glUniform4i64NV = (PFNGLUNIFORM4I64NVPROC)load("glUniform4i64NV", userptr);
    glad_glGetUniformi64vNV = (PFNGLGETUNIFORMI64VNVPROC)load("glGetUniformi64vNV", userptr);
    glad_glUniform3i64NV = (PFNGLUNIFORM3I64NVPROC)load("glUniform3i64NV", userptr);
    glad_glProgramUniform4i64NV = (PFNGLPROGRAMUNIFORM4I64NVPROC)load("glProgramUniform4i64NV", userptr);
    glad_glProgramUniform3i64vNV = (PFNGLPROGRAMUNIFORM3I64VNVPROC)load("glProgramUniform3i64vNV", userptr);
    glad_glProgramUniform2i64vNV = (PFNGLPROGRAMUNIFORM2I64VNVPROC)load("glProgramUniform2i64vNV", userptr);
    glad_glProgramUniform4i64vNV = (PFNGLPROGRAMUNIFORM4I64VNVPROC)load("glProgramUniform4i64vNV", userptr);
    glad_glProgramUniform1i64vNV = (PFNGLPROGRAMUNIFORM1I64VNVPROC)load("glProgramUniform1i64vNV", userptr);
    glad_glProgramUniform2ui64vNV = (PFNGLPROGRAMUNIFORM2UI64VNVPROC)load("glProgramUniform2ui64vNV", userptr);
    glad_glProgramUniform1ui64NV = (PFNGLPROGRAMUNIFORM1UI64NVPROC)load("glProgramUniform1ui64NV", userptr);
    glad_glUniform1i64NV = (PFNGLUNIFORM1I64NVPROC)load("glUniform1i64NV", userptr);
    glad_glUniform3ui64NV = (PFNGLUNIFORM3UI64NVPROC)load("glUniform3ui64NV", userptr);
}
static void load_GL_ARB_invalidate_subdata(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_invalidate_subdata) return;
    glad_glInvalidateFramebuffer = (PFNGLINVALIDATEFRAMEBUFFERPROC)load("glInvalidateFramebuffer", userptr);
    glad_glInvalidateTexImage = (PFNGLINVALIDATETEXIMAGEPROC)load("glInvalidateTexImage", userptr);
    glad_glInvalidateBufferData = (PFNGLINVALIDATEBUFFERDATAPROC)load("glInvalidateBufferData", userptr);
    glad_glInvalidateBufferSubData = (PFNGLINVALIDATEBUFFERSUBDATAPROC)load("glInvalidateBufferSubData", userptr);
    glad_glInvalidateTexSubImage = (PFNGLINVALIDATETEXSUBIMAGEPROC)load("glInvalidateTexSubImage", userptr);
    glad_glInvalidateSubFramebuffer = (PFNGLINVALIDATESUBFRAMEBUFFERPROC)load("glInvalidateSubFramebuffer", userptr);
}
static void load_GL_EXT_framebuffer_multisample(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_framebuffer_multisample) return;
    glad_glRenderbufferStorageMultisampleEXT = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)load("glRenderbufferStorageMultisampleEXT", userptr);
}
static void load_GL_EXT_shader_image_load_store(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_shader_image_load_store) return;
    glad_glMemoryBarrierEXT = (PFNGLMEMORYBARRIEREXTPROC)load("glMemoryBarrierEXT", userptr);
    glad_glBindImageTextureEXT = (PFNGLBINDIMAGETEXTUREEXTPROC)load("glBindImageTextureEXT", userptr);
}
static void load_GL_EXT_texture3D(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_texture3D) return;
    glad_glTexImage3DEXT = (PFNGLTEXIMAGE3DEXTPROC)load("glTexImage3DEXT", userptr);
    glad_glTexSubImage3DEXT = (PFNGLTEXSUBIMAGE3DEXTPROC)load("glTexSubImage3DEXT", userptr);
}
static void load_GL_ARB_multitexture(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_multitexture) return;
    glad_glMultiTexCoord2svARB = (PFNGLMULTITEXCOORD2SVARBPROC)load("glMultiTexCoord2svARB", userptr);
    glad_glMultiTexCoord1ivARB = (PFNGLMULTITEXCOORD1IVARBPROC)load("glMultiTexCoord1ivARB", userptr);
    glad_glMultiTexCoord2iARB = (PFNGLMULTITEXCOORD2IARBPROC)load("glMultiTexCoord2iARB", userptr);
    glad_glMultiTexCoord4fARB = (PFNGLMULTITEXCOORD4FARBPROC)load("glMultiTexCoord4fARB", userptr);
    glad_glMultiTexCoord4fvARB = (PFNGLMULTITEXCOORD4FVARBPROC)load("glMultiTexCoord4fvARB", userptr);
    glad_glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)load("glClientActiveTextureARB", userptr);
    glad_glMultiTexCoord3svARB = (PFNGLMULTITEXCOORD3SVARBPROC)load("glMultiTexCoord3svARB", userptr);
    glad_glMultiTexCoord4sARB = (PFNGLMULTITEXCOORD4SARBPROC)load("glMultiTexCoord4sARB", userptr);
    glad_glMultiTexCoord4dvARB = (PFNGLMULTITEXCOORD4DVARBPROC)load("glMultiTexCoord4dvARB", userptr);
    glad_glMultiTexCoord1fARB = (PFNGLMULTITEXCOORD1FARBPROC)load("glMultiTexCoord1fARB", userptr);
    glad_glMultiTexCoord2fvARB = (PFNGLMULTITEXCOORD2FVARBPROC)load("glMultiTexCoord2fvARB", userptr);
    glad_glMultiTexCoord1dARB = (PFNGLMULTITEXCOORD1DARBPROC)load("glMultiTexCoord1dARB", userptr);
    glad_glMultiTexCoord1dvARB = (PFNGLMULTITEXCOORD1DVARBPROC)load("glMultiTexCoord1dvARB", userptr);
    glad_glMultiTexCoord3dARB = (PFNGLMULTITEXCOORD3DARBPROC)load("glMultiTexCoord3dARB", userptr);
    glad_glMultiTexCoord4svARB = (PFNGLMULTITEXCOORD4SVARBPROC)load("glMultiTexCoord4svARB", userptr);
    glad_glMultiTexCoord2sARB = (PFNGLMULTITEXCOORD2SARBPROC)load("glMultiTexCoord2sARB", userptr);
    glad_glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC)load("glMultiTexCoord3fARB", userptr);
    glad_glMultiTexCoord3sARB = (PFNGLMULTITEXCOORD3SARBPROC)load("glMultiTexCoord3sARB", userptr);
    glad_glMultiTexCoord1svARB = (PFNGLMULTITEXCOORD1SVARBPROC)load("glMultiTexCoord1svARB", userptr);
    glad_glMultiTexCoord2dvARB = (PFNGLMULTITEXCOORD2DVARBPROC)load("glMultiTexCoord2dvARB", userptr);
    glad_glMultiTexCoord1fvARB = (PFNGLMULTITEXCOORD1FVARBPROC)load("glMultiTexCoord1fvARB", userptr);
    glad_glMultiTexCoord1sARB = (PFNGLMULTITEXCOORD1SARBPROC)load("glMultiTexCoord1sARB", userptr);
    glad_glMultiTexCoord3fvARB = (PFNGLMULTITEXCOORD3FVARBPROC)load("glMultiTexCoord3fvARB", userptr);
    glad_glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)load("glMultiTexCoord2fARB", userptr);
    glad_glMultiTexCoord3ivARB = (PFNGLMULTITEXCOORD3IVARBPROC)load("glMultiTexCoord3ivARB", userptr);
    glad_glMultiTexCoord4dARB = (PFNGLMULTITEXCOORD4DARBPROC)load("glMultiTexCoord4dARB", userptr);
    glad_glMultiTexCoord4ivARB = (PFNGLMULTITEXCOORD4IVARBPROC)load("glMultiTexCoord4ivARB", userptr);
    glad_glMultiTexCoord3dvARB = (PFNGLMULTITEXCOORD3DVARBPROC)load("glMultiTexCoord3dvARB", userptr);
    glad_glMultiTexCoord1iARB = (PFNGLMULTITEXCOORD1IARBPROC)load("glMultiTexCoord1iARB", userptr);
    glad_glMultiTexCoord2ivARB = (PFNGLMULTITEXCOORD2IVARBPROC)load("glMultiTexCoord2ivARB", userptr);
    glad_glMultiTexCoord4iARB = (PFNGLMULTITEXCOORD4IARBPROC)load("glMultiTexCoord4iARB", userptr);
    glad_glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)load("glActiveTextureARB", userptr);
    glad_glMultiTexCoord2dARB = (PFNGLMULTITEXCOORD2DARBPROC)load("glMultiTexCoord2dARB", userptr);
    glad_glMultiTexCoord3iARB = (PFNGLMULTITEXCOORD3IARBPROC)load("glMultiTexCoord3iARB", userptr);
}
static void load_GL_EXT_draw_instanced(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_draw_instanced) return;
    glad_glDrawArraysInstancedEXT = (PFNGLDRAWARRAYSINSTANCEDEXTPROC)load("glDrawArraysInstancedEXT", userptr);
    glad_glDrawElementsInstancedEXT = (PFNGLDRAWELEMENTSINSTANCEDEXTPROC)load("glDrawElementsInstancedEXT", userptr);
}
static void load_GL_ARB_draw_elements_base_vertex(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_draw_elements_base_vertex) return;
    glad_glMultiDrawElementsBaseVertex = (PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC)load("glMultiDrawElementsBaseVertex", userptr);
    glad_glDrawElementsInstancedBaseVertex = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC)load("glDrawElementsInstancedBaseVertex", userptr);
    glad_glDrawElementsBaseVertex = (PFNGLDRAWELEMENTSBASEVERTEXPROC)load("glDrawElementsBaseVertex", userptr);
    glad_glDrawRangeElementsBaseVertex = (PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC)load("glDrawRangeElementsBaseVertex", userptr);
}
static void load_GL_ARB_viewport_array(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_viewport_array) return;
    glad_glViewportArrayv = (PFNGLVIEWPORTARRAYVPROC)load("glViewportArrayv", userptr);
    glad_glScissorArrayv = (PFNGLSCISSORARRAYVPROC)load("glScissorArrayv", userptr);
    glad_glGetFloati_v = (PFNGLGETFLOATI_VPROC)load("glGetFloati_v", userptr);
    glad_glScissorIndexedv = (PFNGLSCISSORINDEXEDVPROC)load("glScissorIndexedv", userptr);
    glad_glViewportIndexedf = (PFNGLVIEWPORTINDEXEDFPROC)load("glViewportIndexedf", userptr);
    glad_glGetDoublei_v = (PFNGLGETDOUBLEI_VPROC)load("glGetDoublei_v", userptr);
    glad_glDepthRangeArrayv = (PFNGLDEPTHRANGEARRAYVPROC)load("glDepthRangeArrayv", userptr);
    glad_glScissorIndexed = (PFNGLSCISSORINDEXEDPROC)load("glScissorIndexed", userptr);
    glad_glDepthRangeIndexed = (PFNGLDEPTHRANGEINDEXEDPROC)load("glDepthRangeIndexed", userptr);
    glad_glViewportIndexedfv = (PFNGLVIEWPORTINDEXEDFVPROC)load("glViewportIndexedfv", userptr);
}
static void load_GL_ARB_separate_shader_objects(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_separate_shader_objects) return;
    glad_glProgramUniformMatrix3x4dv = (PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC)load("glProgramUniformMatrix3x4dv", userptr);
    glad_glGenProgramPipelines = (PFNGLGENPROGRAMPIPELINESPROC)load("glGenProgramPipelines", userptr);
    glad_glProgramUniform4ui = (PFNGLPROGRAMUNIFORM4UIPROC)load("glProgramUniform4ui", userptr);
    glad_glProgramUniform2iv = (PFNGLPROGRAMUNIFORM2IVPROC)load("glProgramUniform2iv", userptr);
    glad_glGetProgramPipelineiv = (PFNGLGETPROGRAMPIPELINEIVPROC)load("glGetProgramPipelineiv", userptr);
    glad_glProgramUniformMatrix3x4fv = (PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC)load("glProgramUniformMatrix3x4fv", userptr);
    glad_glProgramUniform4dv = (PFNGLPROGRAMUNIFORM4DVPROC)load("glProgramUniform4dv", userptr);
    glad_glProgramUniformMatrix2x4fv = (PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC)load("glProgramUniformMatrix2x4fv", userptr);
    glad_glProgramUniform1iv = (PFNGLPROGRAMUNIFORM1IVPROC)load("glProgramUniform1iv", userptr);
    glad_glProgramUniform4uiv = (PFNGLPROGRAMUNIFORM4UIVPROC)load("glProgramUniform4uiv", userptr);
    glad_glProgramUniformMatrix4x3fv = (PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC)load("glProgramUniformMatrix4x3fv", userptr);
    glad_glProgramUniform4fv = (PFNGLPROGRAMUNIFORM4FVPROC)load("glProgramUniform4fv", userptr);
    glad_glValidateProgramPipeline = (PFNGLVALIDATEPROGRAMPIPELINEPROC)load("glValidateProgramPipeline", userptr);
    glad_glProgramUniformMatrix4fv = (PFNGLPROGRAMUNIFORMMATRIX4FVPROC)load("glProgramUniformMatrix4fv", userptr);
    glad_glProgramUniformMatrix4x2fv = (PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC)load("glProgramUniformMatrix4x2fv", userptr);
    glad_glProgramUniform1f = (PFNGLPROGRAMUNIFORM1FPROC)load("glProgramUniform1f", userptr);
    glad_glProgramUniform2fv = (PFNGLPROGRAMUNIFORM2FVPROC)load("glProgramUniform2fv", userptr);
    glad_glProgramUniform2uiv = (PFNGLPROGRAMUNIFORM2UIVPROC)load("glProgramUniform2uiv", userptr);
    glad_glProgramUniformMatrix2x3fv = (PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC)load("glProgramUniformMatrix2x3fv", userptr);
    glad_glProgramUniform3iv = (PFNGLPROGRAMUNIFORM3IVPROC)load("glProgramUniform3iv", userptr);
    glad_glProgramUniform4i = (PFNGLPROGRAMUNIFORM4IPROC)load("glProgramUniform4i", userptr);
    glad_glProgramUniform2dv = (PFNGLPROGRAMUNIFORM2DVPROC)load("glProgramUniform2dv", userptr);
    glad_glProgramUniform4f = (PFNGLPROGRAMUNIFORM4FPROC)load("glProgramUniform4f", userptr);
    glad_glProgramUniform4d = (PFNGLPROGRAMUNIFORM4DPROC)load("glProgramUniform4d", userptr);
    glad_glProgramUniformMatrix2x3dv = (PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC)load("glProgramUniformMatrix2x3dv", userptr);
    glad_glProgramUniform3fv = (PFNGLPROGRAMUNIFORM3FVPROC)load("glProgramUniform3fv", userptr);
    glad_glProgramUniform2i = (PFNGLPROGRAMUNIFORM2IPROC)load("glProgramUniform2i", userptr);
    glad_glProgramUniformMatrix2x4dv = (PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC)load("glProgramUniformMatrix2x4dv", userptr);
    glad_glIsProgramPipeline = (PFNGLISPROGRAMPIPELINEPROC)load("glIsProgramPipeline", userptr);
    glad_glProgramUniform2d = (PFNGLPROGRAMUNIFORM2DPROC)load("glProgramUniform2d", userptr);
    glad_glProgramUniform1i = (PFNGLPROGRAMUNIFORM1IPROC)load("glProgramUniform1i", userptr);
    glad_glProgramUniform2ui = (PFNGLPROGRAMUNIFORM2UIPROC)load("glProgramUniform2ui", userptr);
    glad_glProgramUniformMatrix2fv = (PFNGLPROGRAMUNIFORMMATRIX2FVPROC)load("glProgramUniformMatrix2fv", userptr);
    glad_glProgramUniform3dv = (PFNGLPROGRAMUNIFORM3DVPROC)load("glProgramUniform3dv", userptr);
    glad_glProgramUniform1ui = (PFNGLPROGRAMUNIFORM1UIPROC)load("glProgramUniform1ui", userptr);
    glad_glProgramUniform3ui = (PFNGLPROGRAMUNIFORM3UIPROC)load("glProgramUniform3ui", userptr);
    glad_glProgramUniform4iv = (PFNGLPROGRAMUNIFORM4IVPROC)load("glProgramUniform4iv", userptr);
    glad_glProgramUniformMatrix4x2dv = (PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC)load("glProgramUniformMatrix4x2dv", userptr);
    glad_glProgramUniform3uiv = (PFNGLPROGRAMUNIFORM3UIVPROC)load("glProgramUniform3uiv", userptr);
    glad_glActiveShaderProgram = (PFNGLACTIVESHADERPROGRAMPROC)load("glActiveShaderProgram", userptr);
    glad_glProgramUniform1dv = (PFNGLPROGRAMUNIFORM1DVPROC)load("glProgramUniform1dv", userptr);
    glad_glProgramUniform1fv = (PFNGLPROGRAMUNIFORM1FVPROC)load("glProgramUniform1fv", userptr);
    glad_glProgramUniformMatrix3x2dv = (PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC)load("glProgramUniformMatrix3x2dv", userptr);
    glad_glCreateShaderProgramv = (PFNGLCREATESHADERPROGRAMVPROC)load("glCreateShaderProgramv", userptr);
    glad_glProgramUniformMatrix4dv = (PFNGLPROGRAMUNIFORMMATRIX4DVPROC)load("glProgramUniformMatrix4dv", userptr);
    glad_glProgramUniformMatrix4x3dv = (PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC)load("glProgramUniformMatrix4x3dv", userptr);
    glad_glProgramUniformMatrix3fv = (PFNGLPROGRAMUNIFORMMATRIX3FVPROC)load("glProgramUniformMatrix3fv", userptr);
    glad_glGetProgramPipelineInfoLog = (PFNGLGETPROGRAMPIPELINEINFOLOGPROC)load("glGetProgramPipelineInfoLog", userptr);
    glad_glProgramUniformMatrix3x2fv = (PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC)load("glProgramUniformMatrix3x2fv", userptr);
    glad_glBindProgramPipeline = (PFNGLBINDPROGRAMPIPELINEPROC)load("glBindProgramPipeline", userptr);
    glad_glProgramUniform3i = (PFNGLPROGRAMUNIFORM3IPROC)load("glProgramUniform3i", userptr);
    glad_glUseProgramStages = (PFNGLUSEPROGRAMSTAGESPROC)load("glUseProgramStages", userptr);
    glad_glProgramUniformMatrix3dv = (PFNGLPROGRAMUNIFORMMATRIX3DVPROC)load("glProgramUniformMatrix3dv", userptr);
    glad_glProgramUniform1d = (PFNGLPROGRAMUNIFORM1DPROC)load("glProgramUniform1d", userptr);
    glad_glDeleteProgramPipelines = (PFNGLDELETEPROGRAMPIPELINESPROC)load("glDeleteProgramPipelines", userptr);
    glad_glProgramUniform2f = (PFNGLPROGRAMUNIFORM2FPROC)load("glProgramUniform2f", userptr);
    glad_glProgramUniform3f = (PFNGLPROGRAMUNIFORM3FPROC)load("glProgramUniform3f", userptr);
    glad_glProgramUniform1uiv = (PFNGLPROGRAMUNIFORM1UIVPROC)load("glProgramUniform1uiv", userptr);
    glad_glProgramUniform3d = (PFNGLPROGRAMUNIFORM3DPROC)load("glProgramUniform3d", userptr);
    glad_glProgramUniformMatrix2dv = (PFNGLPROGRAMUNIFORMMATRIX2DVPROC)load("glProgramUniformMatrix2dv", userptr);
}
static void load_GL_ATI_draw_buffers(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ATI_draw_buffers) return;
    glad_glDrawBuffersATI = (PFNGLDRAWBUFFERSATIPROC)load("glDrawBuffersATI", userptr);
}
static void load_GL_NV_transform_feedback(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_NV_transform_feedback) return;
    glad_glBindBufferRangeNV = (PFNGLBINDBUFFERRANGENVPROC)load("glBindBufferRangeNV", userptr);
    glad_glTransformFeedbackVaryingsNV = (PFNGLTRANSFORMFEEDBACKVARYINGSNVPROC)load("glTransformFeedbackVaryingsNV", userptr);
    glad_glBindBufferBaseNV = (PFNGLBINDBUFFERBASENVPROC)load("glBindBufferBaseNV", userptr);
    glad_glBindBufferOffsetNV = (PFNGLBINDBUFFEROFFSETNVPROC)load("glBindBufferOffsetNV", userptr);
    glad_glGetActiveVaryingNV = (PFNGLGETACTIVEVARYINGNVPROC)load("glGetActiveVaryingNV", userptr);
    glad_glEndTransformFeedbackNV = (PFNGLENDTRANSFORMFEEDBACKNVPROC)load("glEndTransformFeedbackNV", userptr);
    glad_glBeginTransformFeedbackNV = (PFNGLBEGINTRANSFORMFEEDBACKNVPROC)load("glBeginTransformFeedbackNV", userptr);
    glad_glGetTransformFeedbackVaryingNV = (PFNGLGETTRANSFORMFEEDBACKVARYINGNVPROC)load("glGetTransformFeedbackVaryingNV", userptr);
    glad_glTransformFeedbackAttribsNV = (PFNGLTRANSFORMFEEDBACKATTRIBSNVPROC)load("glTransformFeedbackAttribsNV", userptr);
    glad_glActiveVaryingNV = (PFNGLACTIVEVARYINGNVPROC)load("glActiveVaryingNV", userptr);
    glad_glGetVaryingLocationNV = (PFNGLGETVARYINGLOCATIONNVPROC)load("glGetVaryingLocationNV", userptr);
    glad_glTransformFeedbackStreamAttribsNV = (PFNGLTRANSFORMFEEDBACKSTREAMATTRIBSNVPROC)load("glTransformFeedbackStreamAttribsNV", userptr);
}
static void load_GL_NVX_gpu_memory_info(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_NVX_gpu_memory_info) return;
}
static void load_GL_ARB_sampler_objects(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_sampler_objects) return;
    glad_glSamplerParameterIiv = (PFNGLSAMPLERPARAMETERIIVPROC)load("glSamplerParameterIiv", userptr);
    glad_glDeleteSamplers = (PFNGLDELETESAMPLERSPROC)load("glDeleteSamplers", userptr);
    glad_glGetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIVPROC)load("glGetSamplerParameteriv", userptr);
    glad_glSamplerParameterIuiv = (PFNGLSAMPLERPARAMETERIUIVPROC)load("glSamplerParameterIuiv", userptr);
    glad_glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC)load("glSamplerParameteri", userptr);
    glad_glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC)load("glSamplerParameterf", userptr);
    glad_glGetSamplerParameterIiv = (PFNGLGETSAMPLERPARAMETERIIVPROC)load("glGetSamplerParameterIiv", userptr);
    glad_glGenSamplers = (PFNGLGENSAMPLERSPROC)load("glGenSamplers", userptr);
    glad_glSamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC)load("glSamplerParameterfv", userptr);
    glad_glBindSampler = (PFNGLBINDSAMPLERPROC)load("glBindSampler", userptr);
    glad_glIsSampler = (PFNGLISSAMPLERPROC)load("glIsSampler", userptr);
    glad_glGetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFVPROC)load("glGetSamplerParameterfv", userptr);
    glad_glGetSamplerParameterIuiv = (PFNGLGETSAMPLERPARAMETERIUIVPROC)load("glGetSamplerParameterIuiv", userptr);
    glad_glSamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC)load("glSamplerParameteriv", userptr);
}
static void load_GL_EXT_copy_texture(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_copy_texture) return;
    glad_glCopyTexImage2DEXT = (PFNGLCOPYTEXIMAGE2DEXTPROC)load("glCopyTexImage2DEXT", userptr);
    glad_glCopyTexSubImage2DEXT = (PFNGLCOPYTEXSUBIMAGE2DEXTPROC)load("glCopyTexSubImage2DEXT", userptr);
    glad_glCopyTexSubImage1DEXT = (PFNGLCOPYTEXSUBIMAGE1DEXTPROC)load("glCopyTexSubImage1DEXT", userptr);
    glad_glCopyTexImage1DEXT = (PFNGLCOPYTEXIMAGE1DEXTPROC)load("glCopyTexImage1DEXT", userptr);
    glad_glCopyTexSubImage3DEXT = (PFNGLCOPYTEXSUBIMAGE3DEXTPROC)load("glCopyTexSubImage3DEXT", userptr);
}
static void load_GL_EXT_draw_range_elements(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_EXT_draw_range_elements) return;
    glad_glDrawRangeElementsEXT = (PFNGLDRAWRANGEELEMENTSEXTPROC)load("glDrawRangeElementsEXT", userptr);
}
static void load_GL_ARB_uniform_buffer_object(GLADloadproc load, void* userptr) {
    if(!GLAD_GL_ARB_uniform_buffer_object) return;
    glad_glGetUniformIndices = (PFNGLGETUNIFORMINDICESPROC)load("glGetUniformIndices", userptr);
    glad_glGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC)load("glGetActiveUniformsiv", userptr);
    glad_glGetActiveUniformName = (PFNGLGETACTIVEUNIFORMNAMEPROC)load("glGetActiveUniformName", userptr);
    glad_glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC)load("glGetIntegeri_v", userptr);
    glad_glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)load("glBindBufferRange", userptr);
    glad_glGetActiveUniformBlockName = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC)load("glGetActiveUniformBlockName", userptr);
    glad_glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC)load("glGetUniformBlockIndex", userptr);
    glad_glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)load("glBindBufferBase", userptr);
    glad_glGetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC)load("glGetActiveUniformBlockiv", userptr);
    glad_glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC)load("glUniformBlockBinding", userptr);
}


static void resolve_aliases() {
    if (glCopyTexImage1D == NULL && glCopyTexImage1DEXT != NULL) glCopyTexImage1D = (PFNGLCOPYTEXIMAGE1DPROC)glCopyTexImage1DEXT;
    if (glGetFramebufferAttachmentParameterivEXT == NULL && glGetFramebufferAttachmentParameteriv != NULL) glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)glGetFramebufferAttachmentParameteriv;
    if (glGetIntegerIndexedvEXT == NULL && glGetIntegeri_v != NULL) glGetIntegerIndexedvEXT = (PFNGLGETINTEGERINDEXEDVEXTPROC)glGetIntegeri_v;
    if (glCompressedTexSubImage3D == NULL && glCompressedTexSubImage3DARB != NULL) glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)glCompressedTexSubImage3DARB;
    if (glUniform4uiEXT == NULL && glUniform4ui != NULL) glUniform4uiEXT = (PFNGLUNIFORM4UIEXTPROC)glUniform4ui;
    if (glVertexAttribI3uivEXT == NULL && glVertexAttribI3uiv != NULL) glVertexAttribI3uivEXT = (PFNGLVERTEXATTRIBI3UIVEXTPROC)glVertexAttribI3uiv;
    if (glIsBuffer == NULL && glIsBufferARB != NULL) glIsBuffer = (PFNGLISBUFFERPROC)glIsBufferARB;
    if (glGetMultisamplefv == NULL && glGetMultisamplefvNV != NULL) glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC)glGetMultisamplefvNV;
    if (glMinSampleShading == NULL && glMinSampleShadingARB != NULL) glMinSampleShading = (PFNGLMINSAMPLESHADINGPROC)glMinSampleShadingARB;
    if (glDebugMessageControl == NULL && glDebugMessageControlARB != NULL) glDebugMessageControl = (PFNGLDEBUGMESSAGECONTROLPROC)glDebugMessageControlARB;
    if (glProgramUniform4iEXT == NULL && glProgramUniform4i != NULL) glProgramUniform4iEXT = (PFNGLPROGRAMUNIFORM4IEXTPROC)glProgramUniform4i;
    if (glVertexAttrib2svARB == NULL && glVertexAttrib2sv != NULL) glVertexAttrib2svARB = (PFNGLVERTEXATTRIB2SVARBPROC)glVertexAttrib2sv;
    if (glVertexAttrib2svARB == NULL && glVertexAttrib2svNV != NULL) glVertexAttrib2svARB = (PFNGLVERTEXATTRIB2SVARBPROC)glVertexAttrib2svNV;
    if (glVertexAttrib4ubv == NULL && glVertexAttrib4ubvARB != NULL) glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC)glVertexAttrib4ubvARB;
    if (glVertexAttrib4NubvARB == NULL && glVertexAttrib4ubvNV != NULL) glVertexAttrib4NubvARB = (PFNGLVERTEXATTRIB4NUBVARBPROC)glVertexAttrib4ubvNV;
    if (glVertexAttrib4NubvARB == NULL && glVertexAttrib4Nubv != NULL) glVertexAttrib4NubvARB = (PFNGLVERTEXATTRIB4NUBVARBPROC)glVertexAttrib4Nubv;
    if (glProgramUniform2fEXT == NULL && glProgramUniform2f != NULL) glProgramUniform2fEXT = (PFNGLPROGRAMUNIFORM2FEXTPROC)glProgramUniform2f;
    if (glGetVertexAttribLdvEXT == NULL && glGetVertexAttribLdv != NULL) glGetVertexAttribLdvEXT = (PFNGLGETVERTEXATTRIBLDVEXTPROC)glGetVertexAttribLdv;
    if (glResumeTransformFeedback == NULL && glResumeTransformFeedbackNV != NULL) glResumeTransformFeedback = (PFNGLRESUMETRANSFORMFEEDBACKPROC)glResumeTransformFeedbackNV;
    if (glProgramUniformMatrix3x4fvEXT == NULL && glProgramUniformMatrix3x4fv != NULL) glProgramUniformMatrix3x4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC)glProgramUniformMatrix3x4fv;
    if (glVertexAttribI2i == NULL && glVertexAttribI2iEXT != NULL) glVertexAttribI2i = (PFNGLVERTEXATTRIBI2IPROC)glVertexAttribI2iEXT;
    if (glBindProgramNV == NULL && glBindProgramARB != NULL) glBindProgramNV = (PFNGLBINDPROGRAMNVPROC)glBindProgramARB;
    if (glVertexAttrib1fv == NULL && glVertexAttrib1fvARB != NULL) glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)glVertexAttrib1fvARB;
    if (glVertexAttrib1fv == NULL && glVertexAttrib1fvNV != NULL) glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)glVertexAttrib1fvNV;
    if (glFramebufferTexture2D == NULL && glFramebufferTexture2DEXT != NULL) glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)glFramebufferTexture2DEXT;
    if (glGenFramebuffers == NULL && glGenFramebuffersEXT != NULL) glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)glGenFramebuffersEXT;
    if (glTexSubImage3DEXT == NULL && glTexSubImage3D != NULL) glTexSubImage3DEXT = (PFNGLTEXSUBIMAGE3DEXTPROC)glTexSubImage3D;
    if (glDeleteVertexArrays == NULL && glDeleteVertexArraysAPPLE != NULL) glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)glDeleteVertexArraysAPPLE;
    if (glVertexAttrib2fvNV == NULL && glVertexAttrib2fvARB != NULL) glVertexAttrib2fvNV = (PFNGLVERTEXATTRIB2FVNVPROC)glVertexAttrib2fvARB;
    if (glVertexAttrib2fvNV == NULL && glVertexAttrib2fv != NULL) glVertexAttrib2fvNV = (PFNGLVERTEXATTRIB2FVNVPROC)glVertexAttrib2fv;
    if (glIsProgramNV == NULL && glIsProgramARB != NULL) glIsProgramNV = (PFNGLISPROGRAMNVPROC)glIsProgramARB;
    if (glGetUniformfv == NULL && glGetUniformfvARB != NULL) glGetUniformfv = (PFNGLGETUNIFORMFVPROC)glGetUniformfvARB;
    if (glGetUniformuiv == NULL && glGetUniformuivEXT != NULL) glGetUniformuiv = (PFNGLGETUNIFORMUIVPROC)glGetUniformuivEXT;
    if (glGetRenderbufferParameteriv == NULL && glGetRenderbufferParameterivEXT != NULL) glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)glGetRenderbufferParameterivEXT;
    if (glProgramUniform3fEXT == NULL && glProgramUniform3f != NULL) glProgramUniform3fEXT = (PFNGLPROGRAMUNIFORM3FEXTPROC)glProgramUniform3f;
    if (glGetVertexAttribIivEXT == NULL && glGetVertexAttribIiv != NULL) glGetVertexAttribIivEXT = (PFNGLGETVERTEXATTRIBIIVEXTPROC)glGetVertexAttribIiv;
    if (glPauseTransformFeedbackNV == NULL && glPauseTransformFeedback != NULL) glPauseTransformFeedbackNV = (PFNGLPAUSETRANSFORMFEEDBACKNVPROC)glPauseTransformFeedback;
    if (glGetQueryObjecti64vEXT == NULL && glGetQueryObjecti64v != NULL) glGetQueryObjecti64vEXT = (PFNGLGETQUERYOBJECTI64VEXTPROC)glGetQueryObjecti64v;
    if (glProgramUniform4uiEXT == NULL && glProgramUniform4ui != NULL) glProgramUniform4uiEXT = (PFNGLPROGRAMUNIFORM4UIEXTPROC)glProgramUniform4ui;
    if (glGenQueriesARB == NULL && glGenQueries != NULL) glGenQueriesARB = (PFNGLGENQUERIESARBPROC)glGenQueries;
    if (glDeleteVertexArraysAPPLE == NULL && glDeleteVertexArrays != NULL) glDeleteVertexArraysAPPLE = (PFNGLDELETEVERTEXARRAYSAPPLEPROC)glDeleteVertexArrays;
    if (glDrawTransformFeedback == NULL && glDrawTransformFeedbackNV != NULL) glDrawTransformFeedback = (PFNGLDRAWTRANSFORMFEEDBACKPROC)glDrawTransformFeedbackNV;
    if (glGetTexParameterIuiv == NULL && glGetTexParameterIuivEXT != NULL) glGetTexParameterIuiv = (PFNGLGETTEXPARAMETERIUIVPROC)glGetTexParameterIuivEXT;
    if (glVertexAttrib4Nbv == NULL && glVertexAttrib4NbvARB != NULL) glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC)glVertexAttrib4NbvARB;
    if (glVertexAttribL4dEXT == NULL && glVertexAttribL4d != NULL) glVertexAttribL4dEXT = (PFNGLVERTEXATTRIBL4DEXTPROC)glVertexAttribL4d;
    if (glVertexAttribL3d == NULL && glVertexAttribL3dEXT != NULL) glVertexAttribL3d = (PFNGLVERTEXATTRIBL3DPROC)glVertexAttribL3dEXT;
    if (glProgramUniform1ui == NULL && glProgramUniform1uiEXT != NULL) glProgramUniform1ui = (PFNGLPROGRAMUNIFORM1UIPROC)glProgramUniform1uiEXT;
    if (glCopyTexImage2D == NULL && glCopyTexImage2DEXT != NULL) glCopyTexImage2D = (PFNGLCOPYTEXIMAGE2DPROC)glCopyTexImage2DEXT;
    if (glProgramUniformMatrix2x4fvEXT == NULL && glProgramUniformMatrix2x4fv != NULL) glProgramUniformMatrix2x4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC)glProgramUniformMatrix2x4fv;
    if (glCopyTexSubImage3DEXT == NULL && glCopyTexSubImage3D != NULL) glCopyTexSubImage3DEXT = (PFNGLCOPYTEXSUBIMAGE3DEXTPROC)glCopyTexSubImage3D;
    if (glRenderbufferStorageEXT == NULL && glRenderbufferStorage != NULL) glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)glRenderbufferStorage;
    if (glUniform4uiv == NULL && glUniform4uivEXT != NULL) glUniform4uiv = (PFNGLUNIFORM4UIVPROC)glUniform4uivEXT;
    if (glPointParameterfvARB == NULL && glPointParameterfv != NULL) glPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC)glPointParameterfv;
    if (glPointParameterfvARB == NULL && glPointParameterfvEXT != NULL) glPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC)glPointParameterfvEXT;
    if (glPointParameterfvARB == NULL && glPointParameterfvSGIS != NULL) glPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC)glPointParameterfvSGIS;
    if (glMapBuffer == NULL && glMapBufferARB != NULL) glMapBuffer = (PFNGLMAPBUFFERPROC)glMapBufferARB;
    if (glUniform3iv == NULL && glUniform3ivARB != NULL) glUniform3iv = (PFNGLUNIFORM3IVPROC)glUniform3ivARB;
    if (glProgramUniform4iv == NULL && glProgramUniform4ivEXT != NULL) glProgramUniform4iv = (PFNGLPROGRAMUNIFORM4IVPROC)glProgramUniform4ivEXT;
    if (glVertexAttrib1dvARB == NULL && glVertexAttrib1dvNV != NULL) glVertexAttrib1dvARB = (PFNGLVERTEXATTRIB1DVARBPROC)glVertexAttrib1dvNV;
    if (glVertexAttrib1dvARB == NULL && glVertexAttrib1dv != NULL) glVertexAttrib1dvARB = (PFNGLVERTEXATTRIB1DVARBPROC)glVertexAttrib1dv;
    if (glIsVertexArrayAPPLE == NULL && glIsVertexArray != NULL) glIsVertexArrayAPPLE = (PFNGLISVERTEXARRAYAPPLEPROC)glIsVertexArray;
    if (glUseProgram == NULL && glUseProgramObjectARB != NULL) glUseProgram = (PFNGLUSEPROGRAMPROC)glUseProgramObjectARB;
    if (glUniform3fvARB == NULL && glUniform3fv != NULL) glUniform3fvARB = (PFNGLUNIFORM3FVARBPROC)glUniform3fv;
    if (glProgramUniformMatrix3x2fvEXT == NULL && glProgramUniformMatrix3x2fv != NULL) glProgramUniformMatrix3x2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC)glProgramUniformMatrix3x2fv;
    if (glDeleteBuffers == NULL && glDeleteBuffersARB != NULL) glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)glDeleteBuffersARB;
    if (glUniform2ui == NULL && glUniform2uiEXT != NULL) glUniform2ui = (PFNGLUNIFORM2UIPROC)glUniform2uiEXT;
    if (glVertexAttribIPointerEXT == NULL && glVertexAttribIPointer != NULL) glVertexAttribIPointerEXT = (PFNGLVERTEXATTRIBIPOINTEREXTPROC)glVertexAttribIPointer;
    if (glVertexAttribI3i == NULL && glVertexAttribI3iEXT != NULL) glVertexAttribI3i = (PFNGLVERTEXATTRIBI3IPROC)glVertexAttribI3iEXT;
    if (glVertexAttribI4usv == NULL && glVertexAttribI4usvEXT != NULL) glVertexAttribI4usv = (PFNGLVERTEXATTRIBI4USVPROC)glVertexAttribI4usvEXT;
    if (glGetShaderSource == NULL && glGetShaderSourceARB != NULL) glGetShaderSource = (PFNGLGETSHADERSOURCEPROC)glGetShaderSourceARB;
    if (glVertexAttrib3fv == NULL && glVertexAttrib3fvNV != NULL) glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)glVertexAttrib3fvNV;
    if (glVertexAttrib3fv == NULL && glVertexAttrib3fvARB != NULL) glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)glVertexAttrib3fvARB;
    if (glLinkProgram == NULL && glLinkProgramARB != NULL) glLinkProgram = (PFNGLLINKPROGRAMPROC)glLinkProgramARB;
    if (glDebugMessageCallbackARB == NULL && glDebugMessageCallback != NULL) glDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKARBPROC)glDebugMessageCallback;
    if (glProgramUniform3i == NULL && glProgramUniform3iEXT != NULL) glProgramUniform3i = (PFNGLPROGRAMUNIFORM3IPROC)glProgramUniform3iEXT;
    if (glEndQuery == NULL && glEndQueryARB != NULL) glEndQuery = (PFNGLENDQUERYPROC)glEndQueryARB;
    if (glVertexAttribI4ivEXT == NULL && glVertexAttribI4iv != NULL) glVertexAttribI4ivEXT = (PFNGLVERTEXATTRIBI4IVEXTPROC)glVertexAttribI4iv;
    if (glVertexAttribI2ui == NULL && glVertexAttribI2uiEXT != NULL) glVertexAttribI2ui = (PFNGLVERTEXATTRIBI2UIPROC)glVertexAttribI2uiEXT;
    if (glUniform2iARB == NULL && glUniform2i != NULL) glUniform2iARB = (PFNGLUNIFORM2IARBPROC)glUniform2i;
    if (glBeginConditionalRender == NULL && glBeginConditionalRenderNV != NULL) glBeginConditionalRender = (PFNGLBEGINCONDITIONALRENDERPROC)glBeginConditionalRenderNV;
    if (glGetQueryObjectuivARB == NULL && glGetQueryObjectuiv != NULL) glGetQueryObjectuivARB = (PFNGLGETQUERYOBJECTUIVARBPROC)glGetQueryObjectuiv;
    if (glUniform1f == NULL && glUniform1fARB != NULL) glUniform1f = (PFNGLUNIFORM1FPROC)glUniform1fARB;
    if (glGetCompressedTexImage == NULL && glGetCompressedTexImageARB != NULL) glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC)glGetCompressedTexImageARB;
    if (glMinSampleShadingARB == NULL && glMinSampleShading != NULL) glMinSampleShadingARB = (PFNGLMINSAMPLESHADINGARBPROC)glMinSampleShading;
    if (glUniform1i == NULL && glUniform1iARB != NULL) glUniform1i = (PFNGLUNIFORM1IPROC)glUniform1iARB;
    if (glFramebufferTextureEXT == NULL && glFramebufferTexture != NULL) glFramebufferTextureEXT = (PFNGLFRAMEBUFFERTEXTUREEXTPROC)glFramebufferTexture;
    if (glFramebufferTextureEXT == NULL && glFramebufferTextureARB != NULL) glFramebufferTextureEXT = (PFNGLFRAMEBUFFERTEXTUREEXTPROC)glFramebufferTextureARB;
    if (glProgramUniform4i == NULL && glProgramUniform4iEXT != NULL) glProgramUniform4i = (PFNGLPROGRAMUNIFORM4IPROC)glProgramUniform4iEXT;
    if (glProgramUniform4f == NULL && glProgramUniform4fEXT != NULL) glProgramUniform4f = (PFNGLPROGRAMUNIFORM4FPROC)glProgramUniform4fEXT;
    if (glEndQueryARB == NULL && glEndQuery != NULL) glEndQueryARB = (PFNGLENDQUERYARBPROC)glEndQuery;
    if (glVertexAttrib4uivARB == NULL && glVertexAttrib4uiv != NULL) glVertexAttrib4uivARB = (PFNGLVERTEXATTRIB4UIVARBPROC)glVertexAttrib4uiv;
    if (glResumeTransformFeedbackNV == NULL && glResumeTransformFeedback != NULL) glResumeTransformFeedbackNV = (PFNGLRESUMETRANSFORMFEEDBACKNVPROC)glResumeTransformFeedback;
    if (glVertexAttrib3fARB == NULL && glVertexAttrib3f != NULL) glVertexAttrib3fARB = (PFNGLVERTEXATTRIB3FARBPROC)glVertexAttrib3f;
    if (glVertexAttrib3fARB == NULL && glVertexAttrib3fNV != NULL) glVertexAttrib3fARB = (PFNGLVERTEXATTRIB3FARBPROC)glVertexAttrib3fNV;
    if (glGetBufferParameteriv == NULL && glGetBufferParameterivARB != NULL) glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC)glGetBufferParameterivARB;
    if (glTexParameterIuiv == NULL && glTexParameterIuivEXT != NULL) glTexParameterIuiv = (PFNGLTEXPARAMETERIUIVPROC)glTexParameterIuivEXT;
    if (glGetCompressedTexImageARB == NULL && glGetCompressedTexImage != NULL) glGetCompressedTexImageARB = (PFNGLGETCOMPRESSEDTEXIMAGEARBPROC)glGetCompressedTexImage;
    if (glVertexAttribI4ubvEXT == NULL && glVertexAttribI4ubv != NULL) glVertexAttribI4ubvEXT = (PFNGLVERTEXATTRIBI4UBVEXTPROC)glVertexAttribI4ubv;
    if (glVertexAttrib1dNV == NULL && glVertexAttrib1d != NULL) glVertexAttrib1dNV = (PFNGLVERTEXATTRIB1DNVPROC)glVertexAttrib1d;
    if (glVertexAttrib1dNV == NULL && glVertexAttrib1dARB != NULL) glVertexAttrib1dNV = (PFNGLVERTEXATTRIB1DNVPROC)glVertexAttrib1dARB;
    if (glProgramUniform1iEXT == NULL && glProgramUniform1i != NULL) glProgramUniform1iEXT = (PFNGLPROGRAMUNIFORM1IEXTPROC)glProgramUniform1i;
    if (glDeleteQueriesARB == NULL && glDeleteQueries != NULL) glDeleteQueriesARB = (PFNGLDELETEQUERIESARBPROC)glDeleteQueries;
    if (glIsTransformFeedback == NULL && glIsTransformFeedbackNV != NULL) glIsTransformFeedback = (PFNGLISTRANSFORMFEEDBACKPROC)glIsTransformFeedbackNV;
    if (glVertexAttrib4sNV == NULL && glVertexAttrib4s != NULL) glVertexAttrib4sNV = (PFNGLVERTEXATTRIB4SNVPROC)glVertexAttrib4s;
    if (glVertexAttrib4sNV == NULL && glVertexAttrib4sARB != NULL) glVertexAttrib4sNV = (PFNGLVERTEXATTRIB4SNVPROC)glVertexAttrib4sARB;
    if (glVertexAttribL2dv == NULL && glVertexAttribL2dvEXT != NULL) glVertexAttribL2dv = (PFNGLVERTEXATTRIBL2DVPROC)glVertexAttribL2dvEXT;
    if (glVertexAttribL2d == NULL && glVertexAttribL2dEXT != NULL) glVertexAttribL2d = (PFNGLVERTEXATTRIBL2DPROC)glVertexAttribL2dEXT;
    if (glGetVertexAttribdvNV == NULL && glGetVertexAttribdv != NULL) glGetVertexAttribdvNV = (PFNGLGETVERTEXATTRIBDVNVPROC)glGetVertexAttribdv;
    if (glGetVertexAttribdvNV == NULL && glGetVertexAttribdvARB != NULL) glGetVertexAttribdvNV = (PFNGLGETVERTEXATTRIBDVNVPROC)glGetVertexAttribdvARB;
    if (glGetFramebufferAttachmentParameteriv == NULL && glGetFramebufferAttachmentParameterivEXT != NULL) glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)glGetFramebufferAttachmentParameterivEXT;
    if (glVertexAttribL4d == NULL && glVertexAttribL4dEXT != NULL) glVertexAttribL4d = (PFNGLVERTEXATTRIBL4DPROC)glVertexAttribL4dEXT;
    if (glVertexAttribI4i == NULL && glVertexAttribI4iEXT != NULL) glVertexAttribI4i = (PFNGLVERTEXATTRIBI4IPROC)glVertexAttribI4iEXT;
    if (glBindAttribLocation == NULL && glBindAttribLocationARB != NULL) glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)glBindAttribLocationARB;
    if (glBlendEquationSeparate == NULL && glBlendEquationSeparateEXT != NULL) glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC)glBlendEquationSeparateEXT;
    if (glDrawElementsInstancedARB == NULL && glDrawElementsInstanced != NULL) glDrawElementsInstancedARB = (PFNGLDRAWELEMENTSINSTANCEDARBPROC)glDrawElementsInstanced;
    if (glDrawElementsInstancedARB == NULL && glDrawElementsInstancedEXT != NULL) glDrawElementsInstancedARB = (PFNGLDRAWELEMENTSINSTANCEDARBPROC)glDrawElementsInstancedEXT;
    if (glBeginQueryARB == NULL && glBeginQuery != NULL) glBeginQueryARB = (PFNGLBEGINQUERYARBPROC)glBeginQuery;
    if (glVertexAttrib2sv == NULL && glVertexAttrib2svARB != NULL) glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC)glVertexAttrib2svARB;
    if (glVertexAttrib2sv == NULL && glVertexAttrib2svNV != NULL) glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC)glVertexAttrib2svNV;
    if (glVertexAttrib1fvARB == NULL && glVertexAttrib1fvNV != NULL) glVertexAttrib1fvARB = (PFNGLVERTEXATTRIB1FVARBPROC)glVertexAttrib1fvNV;
    if (glVertexAttrib1fvARB == NULL && glVertexAttrib1fv != NULL) glVertexAttrib1fvARB = (PFNGLVERTEXATTRIB1FVARBPROC)glVertexAttrib1fv;
    if (glVertexAttribL1dEXT == NULL && glVertexAttribL1d != NULL) glVertexAttribL1dEXT = (PFNGLVERTEXATTRIBL1DEXTPROC)glVertexAttribL1d;
    if (glProgramUniform1uivEXT == NULL && glProgramUniform1uiv != NULL) glProgramUniform1uivEXT = (PFNGLPROGRAMUNIFORM1UIVEXTPROC)glProgramUniform1uiv;
    if (glProgramUniform4fv == NULL && glProgramUniform4fvEXT != NULL) glProgramUniform4fv = (PFNGLPROGRAMUNIFORM4FVPROC)glProgramUniform4fvEXT;
    if (glVertexAttrib1d == NULL && glVertexAttrib1dNV != NULL) glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC)glVertexAttrib1dNV;
    if (glVertexAttrib1d == NULL && glVertexAttrib1dARB != NULL) glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC)glVertexAttrib1dARB;
    if (glMultiDrawArraysIndirectAMD == NULL && glMultiDrawArraysIndirect != NULL) glMultiDrawArraysIndirectAMD = (PFNGLMULTIDRAWARRAYSINDIRECTAMDPROC)glMultiDrawArraysIndirect;
    if (glProgramUniform4uivEXT == NULL && glProgramUniform4uiv != NULL) glProgramUniform4uivEXT = (PFNGLPROGRAMUNIFORM4UIVEXTPROC)glProgramUniform4uiv;
    if (glPointParameterfv == NULL && glPointParameterfvARB != NULL) glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC)glPointParameterfvARB;
    if (glPointParameterfv == NULL && glPointParameterfvEXT != NULL) glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC)glPointParameterfvEXT;
    if (glPointParameterfv == NULL && glPointParameterfvSGIS != NULL) glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC)glPointParameterfvSGIS;
    if (glUniformMatrix2fvARB == NULL && glUniformMatrix2fv != NULL) glUniformMatrix2fvARB = (PFNGLUNIFORMMATRIX2FVARBPROC)glUniformMatrix2fv;
    if (glUniform2fv == NULL && glUniform2fvARB != NULL) glUniform2fv = (PFNGLUNIFORM2FVPROC)glUniform2fvARB;
    if (glBindBufferRange == NULL && glBindBufferRangeNV != NULL) glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)glBindBufferRangeNV;
    if (glBindBufferRange == NULL && glBindBufferRangeEXT != NULL) glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)glBindBufferRangeEXT;
    if (glProgramUniform3fvEXT == NULL && glProgramUniform3fv != NULL) glProgramUniform3fvEXT = (PFNGLPROGRAMUNIFORM3FVEXTPROC)glProgramUniform3fv;
    if (glIsVertexArray == NULL && glIsVertexArrayAPPLE != NULL) glIsVertexArray = (PFNGLISVERTEXARRAYPROC)glIsVertexArrayAPPLE;
    if (glPointParameterfSGIS == NULL && glPointParameterfEXT != NULL) glPointParameterfSGIS = (PFNGLPOINTPARAMETERFSGISPROC)glPointParameterfEXT;
    if (glPointParameterfSGIS == NULL && glPointParameterf != NULL) glPointParameterfSGIS = (PFNGLPOINTPARAMETERFSGISPROC)glPointParameterf;
    if (glPointParameterfSGIS == NULL && glPointParameterfARB != NULL) glPointParameterfSGIS = (PFNGLPOINTPARAMETERFSGISPROC)glPointParameterfARB;
    if (glUniform1fARB == NULL && glUniform1f != NULL) glUniform1fARB = (PFNGLUNIFORM1FARBPROC)glUniform1f;
    if (glVertexAttrib4NusvARB == NULL && glVertexAttrib4Nusv != NULL) glVertexAttrib4NusvARB = (PFNGLVERTEXATTRIB4NUSVARBPROC)glVertexAttrib4Nusv;
    if (glBlendFunciARB == NULL && glBlendFunci != NULL) glBlendFunciARB = (PFNGLBLENDFUNCIARBPROC)glBlendFunci;
    if (glBlendFunciARB == NULL && glBlendFuncIndexedAMD != NULL) glBlendFunciARB = (PFNGLBLENDFUNCIARBPROC)glBlendFuncIndexedAMD;
    if (glDebugMessageInsert == NULL && glDebugMessageInsertARB != NULL) glDebugMessageInsert = (PFNGLDEBUGMESSAGEINSERTPROC)glDebugMessageInsertARB;
    if (glCopyTexSubImage1D == NULL && glCopyTexSubImage1DEXT != NULL) glCopyTexSubImage1D = (PFNGLCOPYTEXSUBIMAGE1DPROC)glCopyTexSubImage1DEXT;
    if (glCheckFramebufferStatus == NULL && glCheckFramebufferStatusEXT != NULL) glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glCheckFramebufferStatusEXT;
    if (glDrawRangeElements == NULL && glDrawRangeElementsEXT != NULL) glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC)glDrawRangeElementsEXT;
    if (glBindBufferBase == NULL && glBindBufferBaseEXT != NULL) glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)glBindBufferBaseEXT;
    if (glBindBufferBase == NULL && glBindBufferBaseNV != NULL) glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)glBindBufferBaseNV;
    if (glMultiDrawArrays == NULL && glMultiDrawArraysEXT != NULL) glMultiDrawArrays = (PFNGLMULTIDRAWARRAYSPROC)glMultiDrawArraysEXT;
    if (glVertexAttribL1d == NULL && glVertexAttribL1dEXT != NULL) glVertexAttribL1d = (PFNGLVERTEXATTRIBL1DPROC)glVertexAttribL1dEXT;
    if (glVertexAttribI3uiv == NULL && glVertexAttribI3uivEXT != NULL) glVertexAttribI3uiv = (PFNGLVERTEXATTRIBI3UIVPROC)glVertexAttribI3uivEXT;
    if (glBufferDataARB == NULL && glBufferData != NULL) glBufferDataARB = (PFNGLBUFFERDATAARBPROC)glBufferData;
    if (glUniform2uivEXT == NULL && glUniform2uiv != NULL) glUniform2uivEXT = (PFNGLUNIFORM2UIVEXTPROC)glUniform2uiv;
    if (glVertexAttribI4ui == NULL && glVertexAttribI4uiEXT != NULL) glVertexAttribI4ui = (PFNGLVERTEXATTRIBI4UIPROC)glVertexAttribI4uiEXT;
    if (glBeginQuery == NULL && glBeginQueryARB != NULL) glBeginQuery = (PFNGLBEGINQUERYPROC)glBeginQueryARB;
    if (glBindBuffer == NULL && glBindBufferARB != NULL) glBindBuffer = (PFNGLBINDBUFFERPROC)glBindBufferARB;
    if (glGetVertexAttribIuivEXT == NULL && glGetVertexAttribIuiv != NULL) glGetVertexAttribIuivEXT = (PFNGLGETVERTEXATTRIBIUIVEXTPROC)glGetVertexAttribIuiv;
    if (glGetUniformivARB == NULL && glGetUniformiv != NULL) glGetUniformivARB = (PFNGLGETUNIFORMIVARBPROC)glGetUniformiv;
    if (glGetActiveUniformARB == NULL && glGetActiveUniform != NULL) glGetActiveUniformARB = (PFNGLGETACTIVEUNIFORMARBPROC)glGetActiveUniform;
    if (glVertexAttribI3ivEXT == NULL && glVertexAttribI3iv != NULL) glVertexAttribI3ivEXT = (PFNGLVERTEXATTRIBI3IVEXTPROC)glVertexAttribI3iv;
    if (glVertexAttribI1ivEXT == NULL && glVertexAttribI1iv != NULL) glVertexAttribI1ivEXT = (PFNGLVERTEXATTRIBI1IVEXTPROC)glVertexAttribI1iv;
    if (glVertexAttrib3svNV == NULL && glVertexAttrib3sv != NULL) glVertexAttrib3svNV = (PFNGLVERTEXATTRIB3SVNVPROC)glVertexAttrib3sv;
    if (glVertexAttrib3svNV == NULL && glVertexAttrib3svARB != NULL) glVertexAttrib3svNV = (PFNGLVERTEXATTRIB3SVNVPROC)glVertexAttrib3svARB;
    if (glVertexAttribI1uiEXT == NULL && glVertexAttribI1ui != NULL) glVertexAttribI1uiEXT = (PFNGLVERTEXATTRIBI1UIEXTPROC)glVertexAttribI1ui;
    if (glProgramUniform4ui == NULL && glProgramUniform4uiEXT != NULL) glProgramUniform4ui = (PFNGLPROGRAMUNIFORM4UIPROC)glProgramUniform4uiEXT;
    if (glCopyTexSubImage2DEXT == NULL && glCopyTexSubImage2D != NULL) glCopyTexSubImage2DEXT = (PFNGLCOPYTEXSUBIMAGE2DEXTPROC)glCopyTexSubImage2D;
    if (glVertexAttrib3fvARB == NULL && glVertexAttrib3fvNV != NULL) glVertexAttrib3fvARB = (PFNGLVERTEXATTRIB3FVARBPROC)glVertexAttrib3fvNV;
    if (glVertexAttrib3fvARB == NULL && glVertexAttrib3fv != NULL) glVertexAttrib3fvARB = (PFNGLVERTEXATTRIB3FVARBPROC)glVertexAttrib3fv;
    if (glUniform3iARB == NULL && glUniform3i != NULL) glUniform3iARB = (PFNGLUNIFORM3IARBPROC)glUniform3i;
    if (glCreateProgramObjectARB == NULL && glCreateProgram != NULL) glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)glCreateProgram;
    if (glBlendFuncSeparateINGR == NULL && glBlendFuncSeparate != NULL) glBlendFuncSeparateINGR = (PFNGLBLENDFUNCSEPARATEINGRPROC)glBlendFuncSeparate;
    if (glBlendFuncSeparateINGR == NULL && glBlendFuncSeparateEXT != NULL) glBlendFuncSeparateINGR = (PFNGLBLENDFUNCSEPARATEINGRPROC)glBlendFuncSeparateEXT;
    if (glVertexAttrib2fARB == NULL && glVertexAttrib2fNV != NULL) glVertexAttrib2fARB = (PFNGLVERTEXATTRIB2FARBPROC)glVertexAttrib2fNV;
    if (glVertexAttrib2fARB == NULL && glVertexAttrib2f != NULL) glVertexAttrib2fARB = (PFNGLVERTEXATTRIB2FARBPROC)glVertexAttrib2f;
    if (glIsFramebufferEXT == NULL && glIsFramebuffer != NULL) glIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC)glIsFramebuffer;
    if (glVertexAttribI3ui == NULL && glVertexAttribI3uiEXT != NULL) glVertexAttribI3ui = (PFNGLVERTEXATTRIBI3UIPROC)glVertexAttribI3uiEXT;
    if (glVertexAttrib4NivARB == NULL && glVertexAttrib4Niv != NULL) glVertexAttrib4NivARB = (PFNGLVERTEXATTRIB4NIVARBPROC)glVertexAttrib4Niv;
    if (glBlendEquationSeparateIndexedAMD == NULL && glBlendEquationSeparatei != NULL) glBlendEquationSeparateIndexedAMD = (PFNGLBLENDEQUATIONSEPARATEINDEXEDAMDPROC)glBlendEquationSeparatei;
    if (glBlendEquationSeparateIndexedAMD == NULL && glBlendEquationSeparateiARB != NULL) glBlendEquationSeparateIndexedAMD = (PFNGLBLENDEQUATIONSEPARATEINDEXEDAMDPROC)glBlendEquationSeparateiARB;
    if (glProgramParameteri == NULL && glProgramParameteriARB != NULL) glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC)glProgramParameteriARB;
    if (glProgramParameteri == NULL && glProgramParameteriEXT != NULL) glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC)glProgramParameteriEXT;
    if (glVertexAttribL2dEXT == NULL && glVertexAttribL2d != NULL) glVertexAttribL2dEXT = (PFNGLVERTEXATTRIBL2DEXTPROC)glVertexAttribL2d;
    if (glProgramUniform2uivEXT == NULL && glProgramUniform2uiv != NULL) glProgramUniform2uivEXT = (PFNGLPROGRAMUNIFORM2UIVEXTPROC)glProgramUniform2uiv;
    if (glGetDoubleIndexedvEXT == NULL && glGetDoublei_v != NULL) glGetDoubleIndexedvEXT = (PFNGLGETDOUBLEINDEXEDVEXTPROC)glGetDoublei_v;
    if (glGetDoubleIndexedvEXT == NULL && glGetDoublei_vEXT != NULL) glGetDoubleIndexedvEXT = (PFNGLGETDOUBLEINDEXEDVEXTPROC)glGetDoublei_vEXT;
    if (glCreateShader == NULL && glCreateShaderObjectARB != NULL) glCreateShader = (PFNGLCREATESHADERPROC)glCreateShaderObjectARB;
    if (glGenRenderbuffers == NULL && glGenRenderbuffersEXT != NULL) glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)glGenRenderbuffersEXT;
    if (glCopyTexSubImage2D == NULL && glCopyTexSubImage2DEXT != NULL) glCopyTexSubImage2D = (PFNGLCOPYTEXSUBIMAGE2DPROC)glCopyTexSubImage2DEXT;
    if (glVertexAttrib4ubvARB == NULL && glVertexAttrib4ubv != NULL) glVertexAttrib4ubvARB = (PFNGLVERTEXATTRIB4UBVARBPROC)glVertexAttrib4ubv;
    if (glBindBufferBaseEXT == NULL && glBindBufferBaseNV != NULL) glBindBufferBaseEXT = (PFNGLBINDBUFFERBASEEXTPROC)glBindBufferBaseNV;
    if (glBindBufferBaseEXT == NULL && glBindBufferBase != NULL) glBindBufferBaseEXT = (PFNGLBINDBUFFERBASEEXTPROC)glBindBufferBase;
    if (glCompressedTexImage1D == NULL && glCompressedTexImage1DARB != NULL) glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC)glCompressedTexImage1DARB;
    if (glBlendFuncSeparate == NULL && glBlendFuncSeparateINGR != NULL) glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC)glBlendFuncSeparateINGR;
    if (glBlendFuncSeparate == NULL && glBlendFuncSeparateEXT != NULL) glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC)glBlendFuncSeparateEXT;
    if (glPointParameterfARB == NULL && glPointParameterfEXT != NULL) glPointParameterfARB = (PFNGLPOINTPARAMETERFARBPROC)glPointParameterfEXT;
    if (glPointParameterfARB == NULL && glPointParameterfSGIS != NULL) glPointParameterfARB = (PFNGLPOINTPARAMETERFARBPROC)glPointParameterfSGIS;
    if (glPointParameterfARB == NULL && glPointParameterf != NULL) glPointParameterfARB = (PFNGLPOINTPARAMETERFARBPROC)glPointParameterf;
    if (glCompressedTexSubImage1DARB == NULL && glCompressedTexSubImage1D != NULL) glCompressedTexSubImage1DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC)glCompressedTexSubImage1D;
    if (glVertexAttrib4Nuiv == NULL && glVertexAttrib4NuivARB != NULL) glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC)glVertexAttrib4NuivARB;
    if (glGetUniformfvARB == NULL && glGetUniformfv != NULL) glGetUniformfvARB = (PFNGLGETUNIFORMFVARBPROC)glGetUniformfv;
    if (glUniform3i == NULL && glUniform3iARB != NULL) glUniform3i = (PFNGLUNIFORM3IPROC)glUniform3iARB;
    if (glBlendEquationSeparatei == NULL && glBlendEquationSeparateIndexedAMD != NULL) glBlendEquationSeparatei = (PFNGLBLENDEQUATIONSEPARATEIPROC)glBlendEquationSeparateIndexedAMD;
    if (glBlendEquationSeparatei == NULL && glBlendEquationSeparateiARB != NULL) glBlendEquationSeparatei = (PFNGLBLENDEQUATIONSEPARATEIPROC)glBlendEquationSeparateiARB;
    if (glUniform3f == NULL && glUniform3fARB != NULL) glUniform3f = (PFNGLUNIFORM3FPROC)glUniform3fARB;
    if (glVertexAttrib4sARB == NULL && glVertexAttrib4sNV != NULL) glVertexAttrib4sARB = (PFNGLVERTEXATTRIB4SARBPROC)glVertexAttrib4sNV;
    if (glVertexAttrib4sARB == NULL && glVertexAttrib4s != NULL) glVertexAttrib4sARB = (PFNGLVERTEXATTRIB4SARBPROC)glVertexAttrib4s;
    if (glCopyTexImage1DEXT == NULL && glCopyTexImage1D != NULL) glCopyTexImage1DEXT = (PFNGLCOPYTEXIMAGE1DEXTPROC)glCopyTexImage1D;
    if (glVertexAttribL3dvEXT == NULL && glVertexAttribL3dv != NULL) glVertexAttribL3dvEXT = (PFNGLVERTEXATTRIBL3DVEXTPROC)glVertexAttribL3dv;
    if (glDeleteFramebuffers == NULL && glDeleteFramebuffersEXT != NULL) glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glDeleteFramebuffersEXT;
    if (glDrawArrays == NULL && glDrawArraysEXT != NULL) glDrawArrays = (PFNGLDRAWARRAYSPROC)glDrawArraysEXT;
    if (glBlendFuncSeparateiARB == NULL && glBlendFuncSeparateIndexedAMD != NULL) glBlendFuncSeparateiARB = (PFNGLBLENDFUNCSEPARATEIARBPROC)glBlendFuncSeparateIndexedAMD;
    if (glBlendFuncSeparateiARB == NULL && glBlendFuncSeparatei != NULL) glBlendFuncSeparateiARB = (PFNGLBLENDFUNCSEPARATEIARBPROC)glBlendFuncSeparatei;
    if (glProgramUniform3iEXT == NULL && glProgramUniform3i != NULL) glProgramUniform3iEXT = (PFNGLPROGRAMUNIFORM3IEXTPROC)glProgramUniform3i;
    if (glVertexAttrib4Nub == NULL && glVertexAttrib4ubNV != NULL) glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC)glVertexAttrib4ubNV;
    if (glVertexAttrib4Nub == NULL && glVertexAttrib4NubARB != NULL) glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC)glVertexAttrib4NubARB;
    if (glIsRenderbufferEXT == NULL && glIsRenderbuffer != NULL) glIsRenderbufferEXT = (PFNGLISRENDERBUFFEREXTPROC)glIsRenderbuffer;
    if (glUniform3ivARB == NULL && glUniform3iv != NULL) glUniform3ivARB = (PFNGLUNIFORM3IVARBPROC)glUniform3iv;
    if (glIsRenderbuffer == NULL && glIsRenderbufferEXT != NULL) glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)glIsRenderbufferEXT;
    if (glDrawBuffersARB == NULL && glDrawBuffersATI != NULL) glDrawBuffersARB = (PFNGLDRAWBUFFERSARBPROC)glDrawBuffersATI;
    if (glDrawBuffersARB == NULL && glDrawBuffers != NULL) glDrawBuffersARB = (PFNGLDRAWBUFFERSARBPROC)glDrawBuffers;
    if (glDisableVertexAttribArray == NULL && glDisableVertexAttribArrayARB != NULL) glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)glDisableVertexAttribArrayARB;
    if (glStencilOpSeparate == NULL && glStencilOpSeparateATI != NULL) glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC)glStencilOpSeparateATI;
    if (glVertexAttribI4ubv == NULL && glVertexAttribI4ubvEXT != NULL) glVertexAttribI4ubv = (PFNGLVERTEXATTRIBI4UBVPROC)glVertexAttribI4ubvEXT;
    if (glTexImage3DEXT == NULL && glTexImage3D != NULL) glTexImage3DEXT = (PFNGLTEXIMAGE3DEXTPROC)glTexImage3D;
    if (glVertexAttrib2fNV == NULL && glVertexAttrib2fARB != NULL) glVertexAttrib2fNV = (PFNGLVERTEXATTRIB2FNVPROC)glVertexAttrib2fARB;
    if (glVertexAttrib2fNV == NULL && glVertexAttrib2f != NULL) glVertexAttrib2fNV = (PFNGLVERTEXATTRIB2FNVPROC)glVertexAttrib2f;
    if (glGetVertexAttribPointervARB == NULL && glGetVertexAttribPointervNV != NULL) glGetVertexAttribPointervARB = (PFNGLGETVERTEXATTRIBPOINTERVARBPROC)glGetVertexAttribPointervNV;
    if (glGetVertexAttribPointervARB == NULL && glGetVertexAttribPointerv != NULL) glGetVertexAttribPointervARB = (PFNGLGETVERTEXATTRIBPOINTERVARBPROC)glGetVertexAttribPointerv;
    if (glGetVertexAttribIiv == NULL && glGetVertexAttribIivEXT != NULL) glGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC)glGetVertexAttribIivEXT;
    if (glVertexAttrib4fNV == NULL && glVertexAttrib4f != NULL) glVertexAttrib4fNV = (PFNGLVERTEXATTRIB4FNVPROC)glVertexAttrib4f;
    if (glVertexAttrib4fNV == NULL && glVertexAttrib4fARB != NULL) glVertexAttrib4fNV = (PFNGLVERTEXATTRIB4FNVPROC)glVertexAttrib4fARB;
    if (glGenVertexArraysAPPLE == NULL && glGenVertexArrays != NULL) glGenVertexArraysAPPLE = (PFNGLGENVERTEXARRAYSAPPLEPROC)glGenVertexArrays;
    if (glFramebufferTextureFaceARB == NULL && glFramebufferTextureFaceEXT != NULL) glFramebufferTextureFaceARB = (PFNGLFRAMEBUFFERTEXTUREFACEARBPROC)glFramebufferTextureFaceEXT;
    if (glBlendEquationSeparateiARB == NULL && glBlendEquationSeparateIndexedAMD != NULL) glBlendEquationSeparateiARB = (PFNGLBLENDEQUATIONSEPARATEIARBPROC)glBlendEquationSeparateIndexedAMD;
    if (glBlendEquationSeparateiARB == NULL && glBlendEquationSeparatei != NULL) glBlendEquationSeparateiARB = (PFNGLBLENDEQUATIONSEPARATEIARBPROC)glBlendEquationSeparatei;
    if (glGetVertexAttribfvARB == NULL && glGetVertexAttribfv != NULL) glGetVertexAttribfvARB = (PFNGLGETVERTEXATTRIBFVARBPROC)glGetVertexAttribfv;
    if (glGetVertexAttribfvARB == NULL && glGetVertexAttribfvNV != NULL) glGetVertexAttribfvARB = (PFNGLGETVERTEXATTRIBFVARBPROC)glGetVertexAttribfvNV;
    if (glProgramUniform2iEXT == NULL && glProgramUniform2i != NULL) glProgramUniform2iEXT = (PFNGLPROGRAMUNIFORM2IEXTPROC)glProgramUniform2i;
    if (glUniform4fvARB == NULL && glUniform4fv != NULL) glUniform4fvARB = (PFNGLUNIFORM4FVARBPROC)glUniform4fv;
    if (glGetFloati_vEXT == NULL && glGetFloatIndexedvEXT != NULL) glGetFloati_vEXT = (PFNGLGETFLOATI_VEXTPROC)glGetFloatIndexedvEXT;
    if (glGetFloati_vEXT == NULL && glGetFloati_v != NULL) glGetFloati_vEXT = (PFNGLGETFLOATI_VEXTPROC)glGetFloati_v;
    if (glBindAttribLocationARB == NULL && glBindAttribLocation != NULL) glBindAttribLocationARB = (PFNGLBINDATTRIBLOCATIONARBPROC)glBindAttribLocation;
    if (glGetVertexAttribfvNV == NULL && glGetVertexAttribfvARB != NULL) glGetVertexAttribfvNV = (PFNGLGETVERTEXATTRIBFVNVPROC)glGetVertexAttribfvARB;
    if (glGetVertexAttribfvNV == NULL && glGetVertexAttribfv != NULL) glGetVertexAttribfvNV = (PFNGLGETVERTEXATTRIBFVNVPROC)glGetVertexAttribfv;
    if (glUniform2i == NULL && glUniform2iARB != NULL) glUniform2i = (PFNGLUNIFORM2IPROC)glUniform2iARB;
    if (glUniform2f == NULL && glUniform2fARB != NULL) glUniform2f = (PFNGLUNIFORM2FPROC)glUniform2fARB;
    if (glFramebufferTextureFaceEXT == NULL && glFramebufferTextureFaceARB != NULL) glFramebufferTextureFaceEXT = (PFNGLFRAMEBUFFERTEXTUREFACEEXTPROC)glFramebufferTextureFaceARB;
    if (glFramebufferTextureLayer == NULL && glFramebufferTextureLayerARB != NULL) glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)glFramebufferTextureLayerARB;
    if (glFramebufferTextureLayer == NULL && glFramebufferTextureLayerEXT != NULL) glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)glFramebufferTextureLayerEXT;
    if (glProgramUniformMatrix2x3fvEXT == NULL && glProgramUniformMatrix2x3fv != NULL) glProgramUniformMatrix2x3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC)glProgramUniformMatrix2x3fv;
    if (glProgramUniform2fv == NULL && glProgramUniform2fvEXT != NULL) glProgramUniform2fv = (PFNGLPROGRAMUNIFORM2FVPROC)glProgramUniform2fvEXT;
    if (glDrawArraysEXT == NULL && glDrawArrays != NULL) glDrawArraysEXT = (PFNGLDRAWARRAYSEXTPROC)glDrawArrays;
    if (glBlendEquationEXT == NULL && glBlendEquation != NULL) glBlendEquationEXT = (PFNGLBLENDEQUATIONEXTPROC)glBlendEquation;
    if (glVertexAttrib2dvNV == NULL && glVertexAttrib2dv != NULL) glVertexAttrib2dvNV = (PFNGLVERTEXATTRIB2DVNVPROC)glVertexAttrib2dv;
    if (glVertexAttrib2dvNV == NULL && glVertexAttrib2dvARB != NULL) glVertexAttrib2dvNV = (PFNGLVERTEXATTRIB2DVNVPROC)glVertexAttrib2dvARB;
    if (glBlendFuncIndexedAMD == NULL && glBlendFunci != NULL) glBlendFuncIndexedAMD = (PFNGLBLENDFUNCINDEXEDAMDPROC)glBlendFunci;
    if (glBlendFuncIndexedAMD == NULL && glBlendFunciARB != NULL) glBlendFuncIndexedAMD = (PFNGLBLENDFUNCINDEXEDAMDPROC)glBlendFunciARB;
    if (glBlitFramebuffer == NULL && glBlitFramebufferEXT != NULL) glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)glBlitFramebufferEXT;
    if (glIsEnabledi == NULL && glIsEnabledIndexedEXT != NULL) glIsEnabledi = (PFNGLISENABLEDIPROC)glIsEnabledIndexedEXT;
    if (glProgramUniformMatrix3fv == NULL && glProgramUniformMatrix3fvEXT != NULL) glProgramUniformMatrix3fv = (PFNGLPROGRAMUNIFORMMATRIX3FVPROC)glProgramUniformMatrix3fvEXT;
    if (glGetDebugMessageLog == NULL && glGetDebugMessageLogARB != NULL) glGetDebugMessageLog = (PFNGLGETDEBUGMESSAGELOGPROC)glGetDebugMessageLogARB;
    if (glBindFragDataLocation == NULL && glBindFragDataLocationEXT != NULL) glBindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONPROC)glBindFragDataLocationEXT;
    if (glBindBufferBaseNV == NULL && glBindBufferBaseEXT != NULL) glBindBufferBaseNV = (PFNGLBINDBUFFERBASENVPROC)glBindBufferBaseEXT;
    if (glBindBufferBaseNV == NULL && glBindBufferBase != NULL) glBindBufferBaseNV = (PFNGLBINDBUFFERBASENVPROC)glBindBufferBase;
    if (glFramebufferTextureLayerARB == NULL && glFramebufferTextureLayerEXT != NULL) glFramebufferTextureLayerARB = (PFNGLFRAMEBUFFERTEXTURELAYERARBPROC)glFramebufferTextureLayerEXT;
    if (glFramebufferTextureLayerARB == NULL && glFramebufferTextureLayer != NULL) glFramebufferTextureLayerARB = (PFNGLFRAMEBUFFERTEXTURELAYERARBPROC)glFramebufferTextureLayer;
    if (glMultiDrawElementsIndirect == NULL && glMultiDrawElementsIndirectAMD != NULL) glMultiDrawElementsIndirect = (PFNGLMULTIDRAWELEMENTSINDIRECTPROC)glMultiDrawElementsIndirectAMD;
    if (glCopyTexSubImage3D == NULL && glCopyTexSubImage3DEXT != NULL) glCopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3DPROC)glCopyTexSubImage3DEXT;
    if (glCreateShaderObjectARB == NULL && glCreateShader != NULL) glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)glCreateShader;
    if (glVertexAttrib1svARB == NULL && glVertexAttrib1svNV != NULL) glVertexAttrib1svARB = (PFNGLVERTEXATTRIB1SVARBPROC)glVertexAttrib1svNV;
    if (glVertexAttrib1svARB == NULL && glVertexAttrib1sv != NULL) glVertexAttrib1svARB = (PFNGLVERTEXATTRIB1SVARBPROC)glVertexAttrib1sv;
    if (glProgramUniform2f == NULL && glProgramUniform2fEXT != NULL) glProgramUniform2f = (PFNGLPROGRAMUNIFORM2FPROC)glProgramUniform2fEXT;
    if (glIsFramebuffer == NULL && glIsFramebufferEXT != NULL) glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)glIsFramebufferEXT;
    if (glDrawElementsInstanced == NULL && glDrawElementsInstancedARB != NULL) glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC)glDrawElementsInstancedARB;
    if (glDrawElementsInstanced == NULL && glDrawElementsInstancedEXT != NULL) glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC)glDrawElementsInstancedEXT;
    if (glEndTransformFeedbackNV == NULL && glEndTransformFeedbackEXT != NULL) glEndTransformFeedbackNV = (PFNGLENDTRANSFORMFEEDBACKNVPROC)glEndTransformFeedbackEXT;
    if (glEndTransformFeedbackNV == NULL && glEndTransformFeedback != NULL) glEndTransformFeedbackNV = (PFNGLENDTRANSFORMFEEDBACKNVPROC)glEndTransformFeedback;
    if (glProgramUniform4ivEXT == NULL && glProgramUniform4iv != NULL) glProgramUniform4ivEXT = (PFNGLPROGRAMUNIFORM4IVEXTPROC)glProgramUniform4iv;
    if (glColorMaskIndexedEXT == NULL && glColorMaski != NULL) glColorMaskIndexedEXT = (PFNGLCOLORMASKINDEXEDEXTPROC)glColorMaski;
    if (glVertexAttribL1dvEXT == NULL && glVertexAttribL1dv != NULL) glVertexAttribL1dvEXT = (PFNGLVERTEXATTRIBL1DVEXTPROC)glVertexAttribL1dv;
    if (glCompressedTexSubImage2DARB == NULL && glCompressedTexSubImage2D != NULL) glCompressedTexSubImage2DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC)glCompressedTexSubImage2D;
    if (glTexBuffer == NULL && glTexBufferARB != NULL) glTexBuffer = (PFNGLTEXBUFFERPROC)glTexBufferARB;
    if (glTexBuffer == NULL && glTexBufferEXT != NULL) glTexBuffer = (PFNGLTEXBUFFERPROC)glTexBufferEXT;
    if (glValidateProgram == NULL && glValidateProgramARB != NULL) glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)glValidateProgramARB;
    if (glProgramUniform1uiEXT == NULL && glProgramUniform1ui != NULL) glProgramUniform1uiEXT = (PFNGLPROGRAMUNIFORM1UIEXTPROC)glProgramUniform1ui;
    if (glBindTexture == NULL && glBindTextureEXT != NULL) glBindTexture = (PFNGLBINDTEXTUREPROC)glBindTextureEXT;
    if (glGetActiveAttribARB == NULL && glGetActiveAttrib != NULL) glGetActiveAttribARB = (PFNGLGETACTIVEATTRIBARBPROC)glGetActiveAttrib;
    if (glDetachShader == NULL && glDetachObjectARB != NULL) glDetachShader = (PFNGLDETACHSHADERPROC)glDetachObjectARB;
    if (glVertexAttrib4fARB == NULL && glVertexAttrib4f != NULL) glVertexAttrib4fARB = (PFNGLVERTEXATTRIB4FARBPROC)glVertexAttrib4f;
    if (glVertexAttrib4fARB == NULL && glVertexAttrib4fNV != NULL) glVertexAttrib4fARB = (PFNGLVERTEXATTRIB4FARBPROC)glVertexAttrib4fNV;
    if (glStencilOpSeparateATI == NULL && glStencilOpSeparate != NULL) glStencilOpSeparateATI = (PFNGLSTENCILOPSEPARATEATIPROC)glStencilOpSeparate;
    if (glVertexAttrib1fNV == NULL && glVertexAttrib1fARB != NULL) glVertexAttrib1fNV = (PFNGLVERTEXATTRIB1FNVPROC)glVertexAttrib1fARB;
    if (glVertexAttrib1fNV == NULL && glVertexAttrib1f != NULL) glVertexAttrib1fNV = (PFNGLVERTEXATTRIB1FNVPROC)glVertexAttrib1f;
    if (glSampleCoverage == NULL && glSampleCoverageARB != NULL) glSampleCoverage = (PFNGLSAMPLECOVERAGEPROC)glSampleCoverageARB;
    if (glVertexAttribI3iEXT == NULL && glVertexAttribI3i != NULL) glVertexAttribI3iEXT = (PFNGLVERTEXATTRIBI3IEXTPROC)glVertexAttribI3i;
    if (glGetUniformiv == NULL && glGetUniformivARB != NULL) glGetUniformiv = (PFNGLGETUNIFORMIVPROC)glGetUniformivARB;
    if (glRenderbufferStorage == NULL && glRenderbufferStorageEXT != NULL) glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)glRenderbufferStorageEXT;
    if (glUniform4ui == NULL && glUniform4uiEXT != NULL) glUniform4ui = (PFNGLUNIFORM4UIPROC)glUniform4uiEXT;
    if (glValidateProgramARB == NULL && glValidateProgram != NULL) glValidateProgramARB = (PFNGLVALIDATEPROGRAMARBPROC)glValidateProgram;
    if (glVertexAttribPointerARB == NULL && glVertexAttribPointer != NULL) glVertexAttribPointerARB = (PFNGLVERTEXATTRIBPOINTERARBPROC)glVertexAttribPointer;
    if (glDebugMessageInsertARB == NULL && glDebugMessageInsert != NULL) glDebugMessageInsertARB = (PFNGLDEBUGMESSAGEINSERTARBPROC)glDebugMessageInsert;
    if (glDeleteFramebuffersEXT == NULL && glDeleteFramebuffers != NULL) glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)glDeleteFramebuffers;
    if (glVertexAttrib1fvNV == NULL && glVertexAttrib1fvARB != NULL) glVertexAttrib1fvNV = (PFNGLVERTEXATTRIB1FVNVPROC)glVertexAttrib1fvARB;
    if (glVertexAttrib1fvNV == NULL && glVertexAttrib1fv != NULL) glVertexAttrib1fvNV = (PFNGLVERTEXATTRIB1FVNVPROC)glVertexAttrib1fv;
    if (glIsQueryARB == NULL && glIsQuery != NULL) glIsQueryARB = (PFNGLISQUERYARBPROC)glIsQuery;
    if (glGenFramebuffersEXT == NULL && glGenFramebuffers != NULL) glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)glGenFramebuffers;
    if (glGetQueryObjectiv == NULL && glGetQueryObjectivARB != NULL) glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)glGetQueryObjectivARB;
    if (glVertexAttrib3dvNV == NULL && glVertexAttrib3dv != NULL) glVertexAttrib3dvNV = (PFNGLVERTEXATTRIB3DVNVPROC)glVertexAttrib3dv;
    if (glVertexAttrib3dvNV == NULL && glVertexAttrib3dvARB != NULL) glVertexAttrib3dvNV = (PFNGLVERTEXATTRIB3DVNVPROC)glVertexAttrib3dvARB;
    if (glGenerateMipmap == NULL && glGenerateMipmapEXT != NULL) glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)glGenerateMipmapEXT;
    if (glBlendEquationIndexedAMD == NULL && glBlendEquationi != NULL) glBlendEquationIndexedAMD = (PFNGLBLENDEQUATIONINDEXEDAMDPROC)glBlendEquationi;
    if (glBlendEquationIndexedAMD == NULL && glBlendEquationiARB != NULL) glBlendEquationIndexedAMD = (PFNGLBLENDEQUATIONINDEXEDAMDPROC)glBlendEquationiARB;
    if (glPointParameteri == NULL && glPointParameteriNV != NULL) glPointParameteri = (PFNGLPOINTPARAMETERIPROC)glPointParameteriNV;
    if (glUnmapBuffer == NULL && glUnmapBufferARB != NULL) glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)glUnmapBufferARB;
    if (glVertexAttrib3sv == NULL && glVertexAttrib3svNV != NULL) glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC)glVertexAttrib3svNV;
    if (glVertexAttrib3sv == NULL && glVertexAttrib3svARB != NULL) glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC)glVertexAttrib3svARB;
    if (glPointParameterf == NULL && glPointParameterfEXT != NULL) glPointParameterf = (PFNGLPOINTPARAMETERFPROC)glPointParameterfEXT;
    if (glPointParameterf == NULL && glPointParameterfSGIS != NULL) glPointParameterf = (PFNGLPOINTPARAMETERFPROC)glPointParameterfSGIS;
    if (glPointParameterf == NULL && glPointParameterfARB != NULL) glPointParameterf = (PFNGLPOINTPARAMETERFPROC)glPointParameterfARB;
    if (glVertexAttrib1sARB == NULL && glVertexAttrib1sNV != NULL) glVertexAttrib1sARB = (PFNGLVERTEXATTRIB1SARBPROC)glVertexAttrib1sNV;
    if (glVertexAttrib1sARB == NULL && glVertexAttrib1s != NULL) glVertexAttrib1sARB = (PFNGLVERTEXATTRIB1SARBPROC)glVertexAttrib1s;
    if (glVertexAttrib4sv == NULL && glVertexAttrib4svARB != NULL) glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC)glVertexAttrib4svARB;
    if (glVertexAttrib4sv == NULL && glVertexAttrib4svNV != NULL) glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC)glVertexAttrib4svNV;
    if (glDeleteRenderbuffersEXT == NULL && glDeleteRenderbuffers != NULL) glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)glDeleteRenderbuffers;
    if (glVertexAttrib1svNV == NULL && glVertexAttrib1svARB != NULL) glVertexAttrib1svNV = (PFNGLVERTEXATTRIB1SVNVPROC)glVertexAttrib1svARB;
    if (glVertexAttrib1svNV == NULL && glVertexAttrib1sv != NULL) glVertexAttrib1svNV = (PFNGLVERTEXATTRIB1SVNVPROC)glVertexAttrib1sv;
    if (glGetDoublei_vEXT == NULL && glGetDoublei_v != NULL) glGetDoublei_vEXT = (PFNGLGETDOUBLEI_VEXTPROC)glGetDoublei_v;
    if (glGetDoublei_vEXT == NULL && glGetDoubleIndexedvEXT != NULL) glGetDoublei_vEXT = (PFNGLGETDOUBLEI_VEXTPROC)glGetDoubleIndexedvEXT;
    if (glReadnPixelsARB == NULL && glReadnPixels != NULL) glReadnPixelsARB = (PFNGLREADNPIXELSARBPROC)glReadnPixels;
    if (glFramebufferTextureARB == NULL && glFramebufferTextureEXT != NULL) glFramebufferTextureARB = (PFNGLFRAMEBUFFERTEXTUREARBPROC)glFramebufferTextureEXT;
    if (glFramebufferTextureARB == NULL && glFramebufferTexture != NULL) glFramebufferTextureARB = (PFNGLFRAMEBUFFERTEXTUREARBPROC)glFramebufferTexture;
    if (glGetVertexAttribLdv == NULL && glGetVertexAttribLdvEXT != NULL) glGetVertexAttribLdv = (PFNGLGETVERTEXATTRIBLDVPROC)glGetVertexAttribLdvEXT;
    if (glVertexAttrib3dNV == NULL && glVertexAttrib3d != NULL) glVertexAttrib3dNV = (PFNGLVERTEXATTRIB3DNVPROC)glVertexAttrib3d;
    if (glVertexAttrib3dNV == NULL && glVertexAttrib3dARB != NULL) glVertexAttrib3dNV = (PFNGLVERTEXATTRIB3DNVPROC)glVertexAttrib3dARB;
    if (glGenBuffers == NULL && glGenBuffersARB != NULL) glGenBuffers = (PFNGLGENBUFFERSPROC)glGenBuffersARB;
    if (glDeleteProgramsNV == NULL && glDeleteProgramsARB != NULL) glDeleteProgramsNV = (PFNGLDELETEPROGRAMSNVPROC)glDeleteProgramsARB;
    if (glVertexAttrib3fNV == NULL && glVertexAttrib3f != NULL) glVertexAttrib3fNV = (PFNGLVERTEXATTRIB3FNVPROC)glVertexAttrib3f;
    if (glVertexAttrib3fNV == NULL && glVertexAttrib3fARB != NULL) glVertexAttrib3fNV = (PFNGLVERTEXATTRIB3FNVPROC)glVertexAttrib3fARB;
    if (glTexImage3D == NULL && glTexImage3DEXT != NULL) glTexImage3D = (PFNGLTEXIMAGE3DPROC)glTexImage3DEXT;
    if (glVertexAttribI2iEXT == NULL && glVertexAttribI2i != NULL) glVertexAttribI2iEXT = (PFNGLVERTEXATTRIBI2IEXTPROC)glVertexAttribI2i;
    if (glClampColor == NULL && glClampColorARB != NULL) glClampColor = (PFNGLCLAMPCOLORPROC)glClampColorARB;
    if (glVertexAttribI2ivEXT == NULL && glVertexAttribI2iv != NULL) glVertexAttribI2ivEXT = (PFNGLVERTEXATTRIBI2IVEXTPROC)glVertexAttribI2iv;
    if (glProgramUniform3uiv == NULL && glProgramUniform3uivEXT != NULL) glProgramUniform3uiv = (PFNGLPROGRAMUNIFORM3UIVPROC)glProgramUniform3uivEXT;
    if (glProgramUniform2uiEXT == NULL && glProgramUniform2ui != NULL) glProgramUniform2uiEXT = (PFNGLPROGRAMUNIFORM2UIEXTPROC)glProgramUniform2ui;
    if (glProgramUniformMatrix4x3fvEXT == NULL && glProgramUniformMatrix4x3fv != NULL) glProgramUniformMatrix4x3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC)glProgramUniformMatrix4x3fv;
    if (glVertexAttrib4Nusv == NULL && glVertexAttrib4NusvARB != NULL) glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC)glVertexAttrib4NusvARB;
    if (glBlendFunci == NULL && glBlendFunciARB != NULL) glBlendFunci = (PFNGLBLENDFUNCIPROC)glBlendFunciARB;
    if (glBlendFunci == NULL && glBlendFuncIndexedAMD != NULL) glBlendFunci = (PFNGLBLENDFUNCIPROC)glBlendFuncIndexedAMD;
    if (glProgramUniform4fEXT == NULL && glProgramUniform4f != NULL) glProgramUniform4fEXT = (PFNGLPROGRAMUNIFORM4FEXTPROC)glProgramUniform4f;
    if (glGetUniformLocation == NULL && glGetUniformLocationARB != NULL) glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glGetUniformLocationARB;
    if (glUniform4fv == NULL && glUniform4fvARB != NULL) glUniform4fv = (PFNGLUNIFORM4FVPROC)glUniform4fvARB;
    if (glEndConditionalRender == NULL && glEndConditionalRenderNV != NULL) glEndConditionalRender = (PFNGLENDCONDITIONALRENDERPROC)glEndConditionalRenderNV;
    if (glEndConditionalRender == NULL && glEndConditionalRenderNVX != NULL) glEndConditionalRender = (PFNGLENDCONDITIONALRENDERPROC)glEndConditionalRenderNVX;
    if (glTexSubImage2DEXT == NULL && glTexSubImage2D != NULL) glTexSubImage2DEXT = (PFNGLTEXSUBIMAGE2DEXTPROC)glTexSubImage2D;
    if (glProgramUniform2uiv == NULL && glProgramUniform2uivEXT != NULL) glProgramUniform2uiv = (PFNGLPROGRAMUNIFORM2UIVPROC)glProgramUniform2uivEXT;
    if (glGetQueryObjectuiv == NULL && glGetQueryObjectuivARB != NULL) glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)glGetQueryObjectuivARB;
    if (glVertexAttrib4iv == NULL && glVertexAttrib4ivARB != NULL) glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC)glVertexAttrib4ivARB;
    if (glProgramUniform1uiv == NULL && glProgramUniform1uivEXT != NULL) glProgramUniform1uiv = (PFNGLPROGRAMUNIFORM1UIVPROC)glProgramUniform1uivEXT;
    if (glFramebufferTexture == NULL && glFramebufferTextureEXT != NULL) glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)glFramebufferTextureEXT;
    if (glFramebufferTexture == NULL && glFramebufferTextureARB != NULL) glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)glFramebufferTextureARB;
    if (glVertexAttribL1dv == NULL && glVertexAttribL1dvEXT != NULL) glVertexAttribL1dv = (PFNGLVERTEXATTRIBL1DVPROC)glVertexAttribL1dvEXT;
    if (glPointParameterfvSGIS == NULL && glPointParameterfv != NULL) glPointParameterfvSGIS = (PFNGLPOINTPARAMETERFVSGISPROC)glPointParameterfv;
    if (glPointParameterfvSGIS == NULL && glPointParameterfvARB != NULL) glPointParameterfvSGIS = (PFNGLPOINTPARAMETERFVSGISPROC)glPointParameterfvARB;
    if (glPointParameterfvSGIS == NULL && glPointParameterfvEXT != NULL) glPointParameterfvSGIS = (PFNGLPOINTPARAMETERFVSGISPROC)glPointParameterfvEXT;
    if (glProgramUniformMatrix2x4fv == NULL && glProgramUniformMatrix2x4fvEXT != NULL) glProgramUniformMatrix2x4fv = (PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC)glProgramUniformMatrix2x4fvEXT;
    if (glFlushMappedBufferRangeAPPLE == NULL && glFlushMappedBufferRange != NULL) glFlushMappedBufferRangeAPPLE = (PFNGLFLUSHMAPPEDBUFFERRANGEAPPLEPROC)glFlushMappedBufferRange;
    if (glUniformMatrix2fv == NULL && glUniformMatrix2fvARB != NULL) glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC)glUniformMatrix2fvARB;
    if (glIsTransformFeedbackNV == NULL && glIsTransformFeedback != NULL) glIsTransformFeedbackNV = (PFNGLISTRANSFORMFEEDBACKNVPROC)glIsTransformFeedback;
    if (glVertexAttribDivisorARB == NULL && glVertexAttribDivisor != NULL) glVertexAttribDivisorARB = (PFNGLVERTEXATTRIBDIVISORARBPROC)glVertexAttribDivisor;
    if (glVertexAttribI3iv == NULL && glVertexAttribI3ivEXT != NULL) glVertexAttribI3iv = (PFNGLVERTEXATTRIBI3IVPROC)glVertexAttribI3ivEXT;
    if (glFramebufferTextureLayerEXT == NULL && glFramebufferTextureLayerARB != NULL) glFramebufferTextureLayerEXT = (PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC)glFramebufferTextureLayerARB;
    if (glFramebufferTextureLayerEXT == NULL && glFramebufferTextureLayer != NULL) glFramebufferTextureLayerEXT = (PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC)glFramebufferTextureLayer;
    if (glRenderbufferStorageMultisample == NULL && glRenderbufferStorageMultisampleEXT != NULL) glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)glRenderbufferStorageMultisampleEXT;
    if (glIsQuery == NULL && glIsQueryARB != NULL) glIsQuery = (PFNGLISQUERYPROC)glIsQueryARB;
    if (glEnableIndexedEXT == NULL && glEnablei != NULL) glEnableIndexedEXT = (PFNGLENABLEINDEXEDEXTPROC)glEnablei;
    if (glUniform2uiEXT == NULL && glUniform2ui != NULL) glUniform2uiEXT = (PFNGLUNIFORM2UIEXTPROC)glUniform2ui;
    if (glVertexAttrib4Nubv == NULL && glVertexAttrib4ubvNV != NULL) glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC)glVertexAttrib4ubvNV;
    if (glVertexAttrib4Nubv == NULL && glVertexAttrib4NubvARB != NULL) glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC)glVertexAttrib4NubvARB;
    if (glVertexAttribI1uiv == NULL && glVertexAttribI1uivEXT != NULL) glVertexAttribI1uiv = (PFNGLVERTEXATTRIBI1UIVPROC)glVertexAttribI1uivEXT;
    if (glVertexAttribL4dvEXT == NULL && glVertexAttribL4dv != NULL) glVertexAttribL4dvEXT = (PFNGLVERTEXATTRIBL4DVEXTPROC)glVertexAttribL4dv;
    if (glGetActiveUniform == NULL && glGetActiveUniformARB != NULL) glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)glGetActiveUniformARB;
    if (glVertexAttrib3dARB == NULL && glVertexAttrib3dNV != NULL) glVertexAttrib3dARB = (PFNGLVERTEXATTRIB3DARBPROC)glVertexAttrib3dNV;
    if (glVertexAttrib3dARB == NULL && glVertexAttrib3d != NULL) glVertexAttrib3dARB = (PFNGLVERTEXATTRIB3DARBPROC)glVertexAttrib3d;
    if (glUniform3uiEXT == NULL && glUniform3ui != NULL) glUniform3uiEXT = (PFNGLUNIFORM3UIEXTPROC)glUniform3ui;
    if (glTexParameterIuivEXT == NULL && glTexParameterIuiv != NULL) glTexParameterIuivEXT = (PFNGLTEXPARAMETERIUIVEXTPROC)glTexParameterIuiv;
    if (glProgramUniformMatrix4fv == NULL && glProgramUniformMatrix4fvEXT != NULL) glProgramUniformMatrix4fv = (PFNGLPROGRAMUNIFORMMATRIX4FVPROC)glProgramUniformMatrix4fvEXT;
    if (glVertexAttribL3dEXT == NULL && glVertexAttribL3d != NULL) glVertexAttribL3dEXT = (PFNGLVERTEXATTRIBL3DEXTPROC)glVertexAttribL3d;
    if (glVertexAttribL3dv == NULL && glVertexAttribL3dvEXT != NULL) glVertexAttribL3dv = (PFNGLVERTEXATTRIBL3DVPROC)glVertexAttribL3dvEXT;
    if (glFramebufferRenderbuffer == NULL && glFramebufferRenderbufferEXT != NULL) glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)glFramebufferRenderbufferEXT;
    if (glGetUniformLocationARB == NULL && glGetUniformLocation != NULL) glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)glGetUniformLocation;
    if (glGetDoublei_v == NULL && glGetDoubleIndexedvEXT != NULL) glGetDoublei_v = (PFNGLGETDOUBLEI_VPROC)glGetDoubleIndexedvEXT;
    if (glGetDoublei_v == NULL && glGetDoublei_vEXT != NULL) glGetDoublei_v = (PFNGLGETDOUBLEI_VPROC)glGetDoublei_vEXT;
    if (glVertexAttrib1sv == NULL && glVertexAttrib1svNV != NULL) glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC)glVertexAttrib1svNV;
    if (glVertexAttrib1sv == NULL && glVertexAttrib1svARB != NULL) glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC)glVertexAttrib1svARB;
    if (glBindBufferOffsetEXT == NULL && glBindBufferOffsetNV != NULL) glBindBufferOffsetEXT = (PFNGLBINDBUFFEROFFSETEXTPROC)glBindBufferOffsetNV;
    if (glGetIntegeri_v == NULL && glGetIntegerIndexedvEXT != NULL) glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC)glGetIntegerIndexedvEXT;
    if (glGetTransformFeedbackVarying == NULL && glGetTransformFeedbackVaryingEXT != NULL) glGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC)glGetTransformFeedbackVaryingEXT;
    if (glVertexAttrib2dARB == NULL && glVertexAttrib2d != NULL) glVertexAttrib2dARB = (PFNGLVERTEXATTRIB2DARBPROC)glVertexAttrib2d;
    if (glVertexAttrib2dARB == NULL && glVertexAttrib2dNV != NULL) glVertexAttrib2dARB = (PFNGLVERTEXATTRIB2DARBPROC)glVertexAttrib2dNV;
    if (glDrawRangeElementsEXT == NULL && glDrawRangeElements != NULL) glDrawRangeElementsEXT = (PFNGLDRAWRANGEELEMENTSEXTPROC)glDrawRangeElements;
    if (glDepthRangef == NULL && glDepthRangefOES != NULL) glDepthRangef = (PFNGLDEPTHRANGEFPROC)glDepthRangefOES;
    if (glDeleteProgramsARB == NULL && glDeleteProgramsNV != NULL) glDeleteProgramsARB = (PFNGLDELETEPROGRAMSARBPROC)glDeleteProgramsNV;
    if (glEnablei == NULL && glEnableIndexedEXT != NULL) glEnablei = (PFNGLENABLEIPROC)glEnableIndexedEXT;
    if (glBindProgramARB == NULL && glBindProgramNV != NULL) glBindProgramARB = (PFNGLBINDPROGRAMARBPROC)glBindProgramNV;
    if (glSampleCoverageARB == NULL && glSampleCoverage != NULL) glSampleCoverageARB = (PFNGLSAMPLECOVERAGEARBPROC)glSampleCoverage;
    if (glPointParameteriNV == NULL && glPointParameteri != NULL) glPointParameteriNV = (PFNGLPOINTPARAMETERINVPROC)glPointParameteri;
    if (glNamedBufferStorageEXT == NULL && glNamedBufferStorage != NULL) glNamedBufferStorageEXT = (PFNGLNAMEDBUFFERSTORAGEEXTPROC)glNamedBufferStorage;
    if (glVertexAttrib2dv == NULL && glVertexAttrib2dvNV != NULL) glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC)glVertexAttrib2dvNV;
    if (glVertexAttrib2dv == NULL && glVertexAttrib2dvARB != NULL) glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC)glVertexAttrib2dvARB;
    if (glVertexAttrib4dvARB == NULL && glVertexAttrib4dvNV != NULL) glVertexAttrib4dvARB = (PFNGLVERTEXATTRIB4DVARBPROC)glVertexAttrib4dvNV;
    if (glVertexAttrib4dvARB == NULL && glVertexAttrib4dv != NULL) glVertexAttrib4dvARB = (PFNGLVERTEXATTRIB4DVARBPROC)glVertexAttrib4dv;
    if (glProgramUniformMatrix3fvEXT == NULL && glProgramUniformMatrix3fv != NULL) glProgramUniformMatrix3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC)glProgramUniformMatrix3fv;
    if (glDeleteBuffersARB == NULL && glDeleteBuffers != NULL) glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)glDeleteBuffers;
    if (glUnmapBufferARB == NULL && glUnmapBuffer != NULL) glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)glUnmapBuffer;
    if (glProgramUniform2fvEXT == NULL && glProgramUniform2fv != NULL) glProgramUniform2fvEXT = (PFNGLPROGRAMUNIFORM2FVEXTPROC)glProgramUniform2fv;
    if (glColorMaski == NULL && glColorMaskIndexedEXT != NULL) glColorMaski = (PFNGLCOLORMASKIPROC)glColorMaskIndexedEXT;
    if (glGenProgramsNV == NULL && glGenProgramsARB != NULL) glGenProgramsNV = (PFNGLGENPROGRAMSNVPROC)glGenProgramsARB;
    if (glVertexAttribI3uiEXT == NULL && glVertexAttribI3ui != NULL) glVertexAttribI3uiEXT = (PFNGLVERTEXATTRIBI3UIEXTPROC)glVertexAttribI3ui;
    if (glVertexAttrib4NsvARB == NULL && glVertexAttrib4Nsv != NULL) glVertexAttrib4NsvARB = (PFNGLVERTEXATTRIB4NSVARBPROC)glVertexAttrib4Nsv;
    if (glProgramUniformMatrix4x3fv == NULL && glProgramUniformMatrix4x3fvEXT != NULL) glProgramUniformMatrix4x3fv = (PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC)glProgramUniformMatrix4x3fvEXT;
    if (glProgramUniform2ui == NULL && glProgramUniform2uiEXT != NULL) glProgramUniform2ui = (PFNGLPROGRAMUNIFORM2UIPROC)glProgramUniform2uiEXT;
    if (glGenRenderbuffersEXT == NULL && glGenRenderbuffers != NULL) glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)glGenRenderbuffers;
    if (glBlendEquationiARB == NULL && glBlendEquationi != NULL) glBlendEquationiARB = (PFNGLBLENDEQUATIONIARBPROC)glBlendEquationi;
    if (glBlendEquationiARB == NULL && glBlendEquationIndexedAMD != NULL) glBlendEquationiARB = (PFNGLBLENDEQUATIONIARBPROC)glBlendEquationIndexedAMD;
    if (glUniform4ivARB == NULL && glUniform4iv != NULL) glUniform4ivARB = (PFNGLUNIFORM4IVARBPROC)glUniform4iv;
    if (glPointParameterivNV == NULL && glPointParameteriv != NULL) glPointParameterivNV = (PFNGLPOINTPARAMETERIVNVPROC)glPointParameteriv;
    if (glProgramUniform2iv == NULL && glProgramUniform2ivEXT != NULL) glProgramUniform2iv = (PFNGLPROGRAMUNIFORM2IVPROC)glProgramUniform2ivEXT;
    if (glGetQueryiv == NULL && glGetQueryivARB != NULL) glGetQueryiv = (PFNGLGETQUERYIVPROC)glGetQueryivARB;
    if (glUniform4iARB == NULL && glUniform4i != NULL) glUniform4iARB = (PFNGLUNIFORM4IARBPROC)glUniform4i;
    if (glPointParameteriv == NULL && glPointParameterivNV != NULL) glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC)glPointParameterivNV;
    if (glGetBufferSubData == NULL && glGetBufferSubDataARB != NULL) glGetBufferSubData = (PFNGLGETBUFFERSUBDATAPROC)glGetBufferSubDataARB;
    if (glGetVertexAttribdvARB == NULL && glGetVertexAttribdv != NULL) glGetVertexAttribdvARB = (PFNGLGETVERTEXATTRIBDVARBPROC)glGetVertexAttribdv;
    if (glGetVertexAttribdvARB == NULL && glGetVertexAttribdvNV != NULL) glGetVertexAttribdvARB = (PFNGLGETVERTEXATTRIBDVARBPROC)glGetVertexAttribdvNV;
    if (glDrawArraysInstancedEXT == NULL && glDrawArraysInstanced != NULL) glDrawArraysInstancedEXT = (PFNGLDRAWARRAYSINSTANCEDEXTPROC)glDrawArraysInstanced;
    if (glDrawArraysInstancedEXT == NULL && glDrawArraysInstancedARB != NULL) glDrawArraysInstancedEXT = (PFNGLDRAWARRAYSINSTANCEDEXTPROC)glDrawArraysInstancedARB;
    if (glShaderSourceARB == NULL && glShaderSource != NULL) glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)glShaderSource;
    if (glProvokingVertexEXT == NULL && glProvokingVertex != NULL) glProvokingVertexEXT = (PFNGLPROVOKINGVERTEXEXTPROC)glProvokingVertex;
    if (glFramebufferTexture2DEXT == NULL && glFramebufferTexture2D != NULL) glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)glFramebufferTexture2D;
    if (glAttachShader == NULL && glAttachObjectARB != NULL) glAttachShader = (PFNGLATTACHSHADERPROC)glAttachObjectARB;
    if (glProgramUniform3uivEXT == NULL && glProgramUniform3uiv != NULL) glProgramUniform3uivEXT = (PFNGLPROGRAMUNIFORM3UIVEXTPROC)glProgramUniform3uiv;
    if (glVertexAttrib4bvARB == NULL && glVertexAttrib4bv != NULL) glVertexAttrib4bvARB = (PFNGLVERTEXATTRIB4BVARBPROC)glVertexAttrib4bv;
    if (glFlushMappedBufferRange == NULL && glFlushMappedBufferRangeAPPLE != NULL) glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC)glFlushMappedBufferRangeAPPLE;
    if (glProgramUniformMatrix2fvEXT == NULL && glProgramUniformMatrix2fv != NULL) glProgramUniformMatrix2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC)glProgramUniformMatrix2fv;
    if (glGenQueries == NULL && glGenQueriesARB != NULL) glGenQueries = (PFNGLGENQUERIESPROC)glGenQueriesARB;
    if (glBlendColorEXT == NULL && glBlendColor != NULL) glBlendColorEXT = (PFNGLBLENDCOLOREXTPROC)glBlendColor;
    if (glTexSubImage3D == NULL && glTexSubImage3DEXT != NULL) glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC)glTexSubImage3DEXT;
    if (glGetBufferParameterivARB == NULL && glGetBufferParameteriv != NULL) glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)glGetBufferParameteriv;
    if (glVertexAttrib4dNV == NULL && glVertexAttrib4d != NULL) glVertexAttrib4dNV = (PFNGLVERTEXATTRIB4DNVPROC)glVertexAttrib4d;
    if (glVertexAttrib4dNV == NULL && glVertexAttrib4dARB != NULL) glVertexAttrib4dNV = (PFNGLVERTEXATTRIB4DNVPROC)glVertexAttrib4dARB;
    if (glGetMultisamplefvNV == NULL && glGetMultisamplefv != NULL) glGetMultisamplefvNV = (PFNGLGETMULTISAMPLEFVNVPROC)glGetMultisamplefv;
    if (glProgramUniformMatrix4x2fvEXT == NULL && glProgramUniformMatrix4x2fv != NULL) glProgramUniformMatrix4x2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC)glProgramUniformMatrix4x2fv;
    if (glUniform2fARB == NULL && glUniform2f != NULL) glUniform2fARB = (PFNGLUNIFORM2FARBPROC)glUniform2f;
    if (glBindBufferRangeEXT == NULL && glBindBufferRange != NULL) glBindBufferRangeEXT = (PFNGLBINDBUFFERRANGEEXTPROC)glBindBufferRange;
    if (glBindBufferRangeEXT == NULL && glBindBufferRangeNV != NULL) glBindBufferRangeEXT = (PFNGLBINDBUFFERRANGEEXTPROC)glBindBufferRangeNV;
    if (glMapBufferARB == NULL && glMapBuffer != NULL) glMapBufferARB = (PFNGLMAPBUFFERARBPROC)glMapBuffer;
    if (glGetShaderSourceARB == NULL && glGetShaderSource != NULL) glGetShaderSourceARB = (PFNGLGETSHADERSOURCEARBPROC)glGetShaderSource;
    if (glUniform3fv == NULL && glUniform3fvARB != NULL) glUniform3fv = (PFNGLUNIFORM3FVPROC)glUniform3fvARB;
    if (glMultiDrawElements == NULL && glMultiDrawElementsEXT != NULL) glMultiDrawElements = (PFNGLMULTIDRAWELEMENTSPROC)glMultiDrawElementsEXT;
    if (glVertexAttribL2dvEXT == NULL && glVertexAttribL2dv != NULL) glVertexAttribL2dvEXT = (PFNGLVERTEXATTRIBL2DVEXTPROC)glVertexAttribL2dv;
    if (glTexSubImage1DEXT == NULL && glTexSubImage1D != NULL) glTexSubImage1DEXT = (PFNGLTEXSUBIMAGE1DEXTPROC)glTexSubImage1D;
    if (glUniform1fvARB == NULL && glUniform1fv != NULL) glUniform1fvARB = (PFNGLUNIFORM1FVARBPROC)glUniform1fv;
    if (glUniform2uiv == NULL && glUniform2uivEXT != NULL) glUniform2uiv = (PFNGLUNIFORM2UIVPROC)glUniform2uivEXT;
    if (glCompressedTexSubImage1D == NULL && glCompressedTexSubImage1DARB != NULL) glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)glCompressedTexSubImage1DARB;
    if (glUniform1uiv == NULL && glUniform1uivEXT != NULL) glUniform1uiv = (PFNGLUNIFORM1UIVPROC)glUniform1uivEXT;
    if (glDisableIndexedEXT == NULL && glDisablei != NULL) glDisableIndexedEXT = (PFNGLDISABLEINDEXEDEXTPROC)glDisablei;
    if (glCompressedTexImage3DARB == NULL && glCompressedTexImage3D != NULL) glCompressedTexImage3DARB = (PFNGLCOMPRESSEDTEXIMAGE3DARBPROC)glCompressedTexImage3D;
    if (glBeginTransformFeedbackEXT == NULL && glBeginTransformFeedback != NULL) glBeginTransformFeedbackEXT = (PFNGLBEGINTRANSFORMFEEDBACKEXTPROC)glBeginTransformFeedback;
    if (glBeginTransformFeedbackEXT == NULL && glBeginTransformFeedbackNV != NULL) glBeginTransformFeedbackEXT = (PFNGLBEGINTRANSFORMFEEDBACKEXTPROC)glBeginTransformFeedbackNV;
    if (glBlendFuncSeparateEXT == NULL && glBlendFuncSeparate != NULL) glBlendFuncSeparateEXT = (PFNGLBLENDFUNCSEPARATEEXTPROC)glBlendFuncSeparate;
    if (glBlendFuncSeparateEXT == NULL && glBlendFuncSeparateINGR != NULL) glBlendFuncSeparateEXT = (PFNGLBLENDFUNCSEPARATEEXTPROC)glBlendFuncSeparateINGR;
    if (glUniform3uivEXT == NULL && glUniform3uiv != NULL) glUniform3uivEXT = (PFNGLUNIFORM3UIVEXTPROC)glUniform3uiv;
    if (glGetVertexAttribfv == NULL && glGetVertexAttribfvARB != NULL) glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC)glGetVertexAttribfvARB;
    if (glGetVertexAttribfv == NULL && glGetVertexAttribfvNV != NULL) glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC)glGetVertexAttribfvNV;
    if (glGetActiveAttrib == NULL && glGetActiveAttribARB != NULL) glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC)glGetActiveAttribARB;
    if (glTexSubImage2D == NULL && glTexSubImage2DEXT != NULL) glTexSubImage2D = (PFNGLTEXSUBIMAGE2DPROC)glTexSubImage2DEXT;
    if (glProgramUniformMatrix3x4fv == NULL && glProgramUniformMatrix3x4fvEXT != NULL) glProgramUniformMatrix3x4fv = (PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC)glProgramUniformMatrix3x4fvEXT;
    if (glFramebufferTexture1DEXT == NULL && glFramebufferTexture1D != NULL) glFramebufferTexture1DEXT = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)glFramebufferTexture1D;
    if (glVertexAttribI1uivEXT == NULL && glVertexAttribI1uiv != NULL) glVertexAttribI1uivEXT = (PFNGLVERTEXATTRIBI1UIVEXTPROC)glVertexAttribI1uiv;
    if (glProvokingVertex == NULL && glProvokingVertexEXT != NULL) glProvokingVertex = (PFNGLPROVOKINGVERTEXPROC)glProvokingVertexEXT;
    if (glUniform1iv == NULL && glUniform1ivARB != NULL) glUniform1iv = (PFNGLUNIFORM1IVPROC)glUniform1ivARB;
    if (glDrawArraysInstanced == NULL && glDrawArraysInstancedEXT != NULL) glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC)glDrawArraysInstancedEXT;
    if (glDrawArraysInstanced == NULL && glDrawArraysInstancedARB != NULL) glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC)glDrawArraysInstancedARB;
    if (glVertexAttrib2sNV == NULL && glVertexAttrib2s != NULL) glVertexAttrib2sNV = (PFNGLVERTEXATTRIB2SNVPROC)glVertexAttrib2s;
    if (glVertexAttrib2sNV == NULL && glVertexAttrib2sARB != NULL) glVertexAttrib2sNV = (PFNGLVERTEXATTRIB2SNVPROC)glVertexAttrib2sARB;
    if (glProgramUniformMatrix2fv == NULL && glProgramUniformMatrix2fvEXT != NULL) glProgramUniformMatrix2fv = (PFNGLPROGRAMUNIFORMMATRIX2FVPROC)glProgramUniformMatrix2fvEXT;
    if (glVertexAttrib4uiv == NULL && glVertexAttrib4uivARB != NULL) glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC)glVertexAttrib4uivARB;
    if (glProgramParameteriARB == NULL && glProgramParameteri != NULL) glProgramParameteriARB = (PFNGLPROGRAMPARAMETERIARBPROC)glProgramParameteri;
    if (glProgramParameteriARB == NULL && glProgramParameteriEXT != NULL) glProgramParameteriARB = (PFNGLPROGRAMPARAMETERIARBPROC)glProgramParameteriEXT;
    if (glProgramUniform1iv == NULL && glProgramUniform1ivEXT != NULL) glProgramUniform1iv = (PFNGLPROGRAMUNIFORM1IVPROC)glProgramUniform1ivEXT;
    if (glMultiDrawElementsIndirectAMD == NULL && glMultiDrawElementsIndirect != NULL) glMultiDrawElementsIndirectAMD = (PFNGLMULTIDRAWELEMENTSINDIRECTAMDPROC)glMultiDrawElementsIndirect;
    if (glVertexAttrib4fv == NULL && glVertexAttrib4fvARB != NULL) glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)glVertexAttrib4fvARB;
    if (glVertexAttrib4fv == NULL && glVertexAttrib4fvNV != NULL) glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)glVertexAttrib4fvNV;
    if (glVertexAttrib2fv == NULL && glVertexAttrib2fvARB != NULL) glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)glVertexAttrib2fvARB;
    if (glVertexAttrib2fv == NULL && glVertexAttrib2fvNV != NULL) glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)glVertexAttrib2fvNV;
    if (glBeginConditionalRenderNV == NULL && glBeginConditionalRender != NULL) glBeginConditionalRenderNV = (PFNGLBEGINCONDITIONALRENDERNVPROC)glBeginConditionalRender;
    if (glTexBufferARB == NULL && glTexBuffer != NULL) glTexBufferARB = (PFNGLTEXBUFFERARBPROC)glTexBuffer;
    if (glTexBufferARB == NULL && glTexBufferEXT != NULL) glTexBufferARB = (PFNGLTEXBUFFERARBPROC)glTexBufferEXT;
    if (glGetBufferPointervARB == NULL && glGetBufferPointerv != NULL) glGetBufferPointervARB = (PFNGLGETBUFFERPOINTERVARBPROC)glGetBufferPointerv;
    if (glProgramUniform3f == NULL && glProgramUniform3fEXT != NULL) glProgramUniform3f = (PFNGLPROGRAMUNIFORM3FPROC)glProgramUniform3fEXT;
    if (glEndConditionalRenderNV == NULL && glEndConditionalRenderNVX != NULL) glEndConditionalRenderNV = (PFNGLENDCONDITIONALRENDERNVPROC)glEndConditionalRenderNVX;
    if (glEndConditionalRenderNV == NULL && glEndConditionalRender != NULL) glEndConditionalRenderNV = (PFNGLENDCONDITIONALRENDERNVPROC)glEndConditionalRender;
    if (glGetQueryivARB == NULL && glGetQueryiv != NULL) glGetQueryivARB = (PFNGLGETQUERYIVARBPROC)glGetQueryiv;
    if (glVertexAttribI1ui == NULL && glVertexAttribI1uiEXT != NULL) glVertexAttribI1ui = (PFNGLVERTEXATTRIBI1UIPROC)glVertexAttribI1uiEXT;
    if (glGetRenderbufferParameterivEXT == NULL && glGetRenderbufferParameteriv != NULL) glGetRenderbufferParameterivEXT = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)glGetRenderbufferParameteriv;
    if (glVertexAttribI4bvEXT == NULL && glVertexAttribI4bv != NULL) glVertexAttribI4bvEXT = (PFNGLVERTEXATTRIBI4BVEXTPROC)glVertexAttribI4bv;
    if (glVertexAttrib4ubvNV == NULL && glVertexAttrib4Nubv != NULL) glVertexAttrib4ubvNV = (PFNGLVERTEXATTRIB4UBVNVPROC)glVertexAttrib4Nubv;
    if (glVertexAttrib4ubvNV == NULL && glVertexAttrib4NubvARB != NULL) glVertexAttrib4ubvNV = (PFNGLVERTEXATTRIB4UBVNVPROC)glVertexAttrib4NubvARB;
    if (glVertexAttrib4usvARB == NULL && glVertexAttrib4usv != NULL) glVertexAttrib4usvARB = (PFNGLVERTEXATTRIB4USVARBPROC)glVertexAttrib4usv;
    if (glPointParameterfEXT == NULL && glPointParameterfSGIS != NULL) glPointParameterfEXT = (PFNGLPOINTPARAMETERFEXTPROC)glPointParameterfSGIS;
    if (glPointParameterfEXT == NULL && glPointParameterf != NULL) glPointParameterfEXT = (PFNGLPOINTPARAMETERFEXTPROC)glPointParameterf;
    if (glPointParameterfEXT == NULL && glPointParameterfARB != NULL) glPointParameterfEXT = (PFNGLPOINTPARAMETERFEXTPROC)glPointParameterfARB;
    if (glVertexAttrib4Niv == NULL && glVertexAttrib4NivARB != NULL) glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC)glVertexAttrib4NivARB;
    if (glVertexAttrib4ivARB == NULL && glVertexAttrib4iv != NULL) glVertexAttrib4ivARB = (PFNGLVERTEXATTRIB4IVARBPROC)glVertexAttrib4iv;
    if (glGetAttribLocationARB == NULL && glGetAttribLocation != NULL) glGetAttribLocationARB = (PFNGLGETATTRIBLOCATIONARBPROC)glGetAttribLocation;
    if (glGenTransformFeedbacks == NULL && glGenTransformFeedbacksNV != NULL) glGenTransformFeedbacks = (PFNGLGENTRANSFORMFEEDBACKSPROC)glGenTransformFeedbacksNV;
    if (glIsEnabledIndexedEXT == NULL && glIsEnabledi != NULL) glIsEnabledIndexedEXT = (PFNGLISENABLEDINDEXEDEXTPROC)glIsEnabledi;
    if (glReadnPixels == NULL && glReadnPixelsARB != NULL) glReadnPixels = (PFNGLREADNPIXELSPROC)glReadnPixelsARB;
    if (glProgramUniform4fvEXT == NULL && glProgramUniform4fv != NULL) glProgramUniform4fvEXT = (PFNGLPROGRAMUNIFORM4FVEXTPROC)glProgramUniform4fv;
    if (glVertexAttrib2sARB == NULL && glVertexAttrib2sNV != NULL) glVertexAttrib2sARB = (PFNGLVERTEXATTRIB2SARBPROC)glVertexAttrib2sNV;
    if (glVertexAttrib2sARB == NULL && glVertexAttrib2s != NULL) glVertexAttrib2sARB = (PFNGLVERTEXATTRIB2SARBPROC)glVertexAttrib2s;
    if (glCompressedTexImage2DARB == NULL && glCompressedTexImage2D != NULL) glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)glCompressedTexImage2D;
    if (glCheckFramebufferStatusEXT == NULL && glCheckFramebufferStatus != NULL) glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)glCheckFramebufferStatus;
    if (glVertexAttrib1fARB == NULL && glVertexAttrib1fNV != NULL) glVertexAttrib1fARB = (PFNGLVERTEXATTRIB1FARBPROC)glVertexAttrib1fNV;
    if (glVertexAttrib1fARB == NULL && glVertexAttrib1f != NULL) glVertexAttrib1fARB = (PFNGLVERTEXATTRIB1FARBPROC)glVertexAttrib1f;
    if (glDisableVertexAttribArrayARB == NULL && glDisableVertexAttribArray != NULL) glDisableVertexAttribArrayARB = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)glDisableVertexAttribArray;
    if (glVertexAttribI1iEXT == NULL && glVertexAttribI1i != NULL) glVertexAttribI1iEXT = (PFNGLVERTEXATTRIBI1IEXTPROC)glVertexAttribI1i;
    if (glBlendEquation == NULL && glBlendEquationEXT != NULL) glBlendEquation = (PFNGLBLENDEQUATIONPROC)glBlendEquationEXT;
    if (glGetQueryObjectui64vEXT == NULL && glGetQueryObjectui64v != NULL) glGetQueryObjectui64vEXT = (PFNGLGETQUERYOBJECTUI64VEXTPROC)glGetQueryObjectui64v;
    if (glVertexAttribI4uivEXT == NULL && glVertexAttribI4uiv != NULL) glVertexAttribI4uivEXT = (PFNGLVERTEXATTRIBI4UIVEXTPROC)glVertexAttribI4uiv;
    if (glDeleteTransformFeedbacks == NULL && glDeleteTransformFeedbacksNV != NULL) glDeleteTransformFeedbacks = (PFNGLDELETETRANSFORMFEEDBACKSPROC)glDeleteTransformFeedbacksNV;
    if (glProgramUniform2ivEXT == NULL && glProgramUniform2iv != NULL) glProgramUniform2ivEXT = (PFNGLPROGRAMUNIFORM2IVEXTPROC)glProgramUniform2iv;
    if (glVertexAttrib1dv == NULL && glVertexAttrib1dvARB != NULL) glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC)glVertexAttrib1dvARB;
    if (glVertexAttrib1dv == NULL && glVertexAttrib1dvNV != NULL) glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC)glVertexAttrib1dvNV;
    if (glBufferSubData == NULL && glBufferSubDataARB != NULL) glBufferSubData = (PFNGLBUFFERSUBDATAPROC)glBufferSubDataARB;
    if (glBlendFuncSeparatei == NULL && glBlendFuncSeparateIndexedAMD != NULL) glBlendFuncSeparatei = (PFNGLBLENDFUNCSEPARATEIPROC)glBlendFuncSeparateIndexedAMD;
    if (glBlendFuncSeparatei == NULL && glBlendFuncSeparateiARB != NULL) glBlendFuncSeparatei = (PFNGLBLENDFUNCSEPARATEIPROC)glBlendFuncSeparateiARB;
    if (glGetBooleanIndexedvEXT == NULL && glGetBooleani_v != NULL) glGetBooleanIndexedvEXT = (PFNGLGETBOOLEANINDEXEDVEXTPROC)glGetBooleani_v;
    if (glDisablei == NULL && glDisableIndexedEXT != NULL) glDisablei = (PFNGLDISABLEIPROC)glDisableIndexedEXT;
    if (glRenderbufferStorageMultisampleEXT == NULL && glRenderbufferStorageMultisample != NULL) glRenderbufferStorageMultisampleEXT = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)glRenderbufferStorageMultisample;
    if (glProgramUniform2i == NULL && glProgramUniform2iEXT != NULL) glProgramUniform2i = (PFNGLPROGRAMUNIFORM2IPROC)glProgramUniform2iEXT;
    if (glBeginTransformFeedbackNV == NULL && glBeginTransformFeedback != NULL) glBeginTransformFeedbackNV = (PFNGLBEGINTRANSFORMFEEDBACKNVPROC)glBeginTransformFeedback;
    if (glBeginTransformFeedbackNV == NULL && glBeginTransformFeedbackEXT != NULL) glBeginTransformFeedbackNV = (PFNGLBEGINTRANSFORMFEEDBACKNVPROC)glBeginTransformFeedbackEXT;
    if (glPauseTransformFeedback == NULL && glPauseTransformFeedbackNV != NULL) glPauseTransformFeedback = (PFNGLPAUSETRANSFORMFEEDBACKPROC)glPauseTransformFeedbackNV;
    if (glTexSubImage1D == NULL && glTexSubImage1DEXT != NULL) glTexSubImage1D = (PFNGLTEXSUBIMAGE1DPROC)glTexSubImage1DEXT;
    if (glBlitFramebufferEXT == NULL && glBlitFramebuffer != NULL) glBlitFramebufferEXT = (PFNGLBLITFRAMEBUFFEREXTPROC)glBlitFramebuffer;
    if (glUniformMatrix4fvARB == NULL && glUniformMatrix4fv != NULL) glUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC)glUniformMatrix4fv;
    if (glVertexAttribI4iv == NULL && glVertexAttribI4ivEXT != NULL) glVertexAttribI4iv = (PFNGLVERTEXATTRIBI4IVPROC)glVertexAttribI4ivEXT;
    if (glVertexAttrib4fvARB == NULL && glVertexAttrib4fv != NULL) glVertexAttrib4fvARB = (PFNGLVERTEXATTRIB4FVARBPROC)glVertexAttrib4fv;
    if (glVertexAttrib4fvARB == NULL && glVertexAttrib4fvNV != NULL) glVertexAttrib4fvARB = (PFNGLVERTEXATTRIB4FVARBPROC)glVertexAttrib4fvNV;
    if (glProgramUniform3uiEXT == NULL && glProgramUniform3ui != NULL) glProgramUniform3uiEXT = (PFNGLPROGRAMUNIFORM3UIEXTPROC)glProgramUniform3ui;
    if (glProgramUniform3fv == NULL && glProgramUniform3fvEXT != NULL) glProgramUniform3fv = (PFNGLPROGRAMUNIFORM3FVPROC)glProgramUniform3fvEXT;
    if (glIsBufferARB == NULL && glIsBuffer != NULL) glIsBufferARB = (PFNGLISBUFFERARBPROC)glIsBuffer;
    if (glVertexAttrib4dvNV == NULL && glVertexAttrib4dvARB != NULL) glVertexAttrib4dvNV = (PFNGLVERTEXATTRIB4DVNVPROC)glVertexAttrib4dvARB;
    if (glVertexAttrib4dvNV == NULL && glVertexAttrib4dv != NULL) glVertexAttrib4dvNV = (PFNGLVERTEXATTRIB4DVNVPROC)glVertexAttrib4dv;
    if (glProgramUniform1fvEXT == NULL && glProgramUniform1fv != NULL) glProgramUniform1fvEXT = (PFNGLPROGRAMUNIFORM1FVEXTPROC)glProgramUniform1fv;
    if (glVertexAttrib1dvNV == NULL && glVertexAttrib1dvARB != NULL) glVertexAttrib1dvNV = (PFNGLVERTEXATTRIB1DVNVPROC)glVertexAttrib1dvARB;
    if (glVertexAttrib1dvNV == NULL && glVertexAttrib1dv != NULL) glVertexAttrib1dvNV = (PFNGLVERTEXATTRIB1DVNVPROC)glVertexAttrib1dv;
    if (glCompressedTexImage1DARB == NULL && glCompressedTexImage1D != NULL) glCompressedTexImage1DARB = (PFNGLCOMPRESSEDTEXIMAGE1DARBPROC)glCompressedTexImage1D;
    if (glVertexAttrib4usv == NULL && glVertexAttrib4usvARB != NULL) glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC)glVertexAttrib4usvARB;
    if (glGetQueryObjectui64v == NULL && glGetQueryObjectui64vEXT != NULL) glGetQueryObjectui64v = (PFNGLGETQUERYOBJECTUI64VPROC)glGetQueryObjectui64vEXT;
    if (glProgramUniform1fv == NULL && glProgramUniform1fvEXT != NULL) glProgramUniform1fv = (PFNGLPROGRAMUNIFORM1FVPROC)glProgramUniform1fvEXT;
    if (glUniformMatrix4fv == NULL && glUniformMatrix4fvARB != NULL) glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)glUniformMatrix4fvARB;
    if (glProgramUniformMatrix4fvEXT == NULL && glProgramUniformMatrix4fv != NULL) glProgramUniformMatrix4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC)glProgramUniformMatrix4fv;
    if (glMultiDrawArraysEXT == NULL && glMultiDrawArrays != NULL) glMultiDrawArraysEXT = (PFNGLMULTIDRAWARRAYSEXTPROC)glMultiDrawArrays;
    if (glUniform4uivEXT == NULL && glUniform4uiv != NULL) glUniform4uivEXT = (PFNGLUNIFORM4UIVEXTPROC)glUniform4uiv;
    if (glVertexAttribI4uiEXT == NULL && glVertexAttribI4ui != NULL) glVertexAttribI4uiEXT = (PFNGLVERTEXATTRIBI4UIEXTPROC)glVertexAttribI4ui;
    if (glCompileShader == NULL && glCompileShaderARB != NULL) glCompileShader = (PFNGLCOMPILESHADERPROC)glCompileShaderARB;
    if (glUniform1uivEXT == NULL && glUniform1uiv != NULL) glUniform1uivEXT = (PFNGLUNIFORM1UIVEXTPROC)glUniform1uiv;
    if (glUniformMatrix3fv == NULL && glUniformMatrix3fvARB != NULL) glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)glUniformMatrix3fvARB;
    if (glGetUniformuivEXT == NULL && glGetUniformuiv != NULL) glGetUniformuivEXT = (PFNGLGETUNIFORMUIVEXTPROC)glGetUniformuiv;
    if (glGetQueryObjecti64v == NULL && glGetQueryObjecti64vEXT != NULL) glGetQueryObjecti64v = (PFNGLGETQUERYOBJECTI64VPROC)glGetQueryObjecti64vEXT;
    if (glVertexAttribIPointer == NULL && glVertexAttribIPointerEXT != NULL) glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)glVertexAttribIPointerEXT;
    if (glDepthRangefOES == NULL && glDepthRangef != NULL) glDepthRangefOES = (PFNGLDEPTHRANGEFOESPROC)glDepthRangef;
    if (glCompressedTexImage2D == NULL && glCompressedTexImage2DARB != NULL) glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)glCompressedTexImage2DARB;
    if (glDrawBuffers == NULL && glDrawBuffersATI != NULL) glDrawBuffers = (PFNGLDRAWBUFFERSPROC)glDrawBuffersATI;
    if (glDrawBuffers == NULL && glDrawBuffersARB != NULL) glDrawBuffers = (PFNGLDRAWBUFFERSPROC)glDrawBuffersARB;
    if (glVertexAttrib1s == NULL && glVertexAttrib1sNV != NULL) glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC)glVertexAttrib1sNV;
    if (glVertexAttrib1s == NULL && glVertexAttrib1sARB != NULL) glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC)glVertexAttrib1sARB;
    if (glUniform1iARB == NULL && glUniform1i != NULL) glUniform1iARB = (PFNGLUNIFORM1IARBPROC)glUniform1i;
    if (glProgramUniform3ivEXT == NULL && glProgramUniform3iv != NULL) glProgramUniform3ivEXT = (PFNGLPROGRAMUNIFORM3IVEXTPROC)glProgramUniform3iv;
    if (glGenVertexArrays == NULL && glGenVertexArraysAPPLE != NULL) glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glGenVertexArraysAPPLE;
    if (glGetVertexAttribdv == NULL && glGetVertexAttribdvARB != NULL) glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC)glGetVertexAttribdvARB;
    if (glGetVertexAttribdv == NULL && glGetVertexAttribdvNV != NULL) glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC)glGetVertexAttribdvNV;
    if (glGetFloatIndexedvEXT == NULL && glGetFloati_v != NULL) glGetFloatIndexedvEXT = (PFNGLGETFLOATINDEXEDVEXTPROC)glGetFloati_v;
    if (glGetFloatIndexedvEXT == NULL && glGetFloati_vEXT != NULL) glGetFloatIndexedvEXT = (PFNGLGETFLOATINDEXEDVEXTPROC)glGetFloati_vEXT;
    if (glGetTexParameterIivEXT == NULL && glGetTexParameterIiv != NULL) glGetTexParameterIivEXT = (PFNGLGETTEXPARAMETERIIVEXTPROC)glGetTexParameterIiv;
    if (glUniform1uiEXT == NULL && glUniform1ui != NULL) glUniform1uiEXT = (PFNGLUNIFORM1UIEXTPROC)glUniform1ui;
    if (glVertexAttrib4NubARB == NULL && glVertexAttrib4Nub != NULL) glVertexAttrib4NubARB = (PFNGLVERTEXATTRIB4NUBARBPROC)glVertexAttrib4Nub;
    if (glVertexAttrib4NubARB == NULL && glVertexAttrib4ubNV != NULL) glVertexAttrib4NubARB = (PFNGLVERTEXATTRIB4NUBARBPROC)glVertexAttrib4ubNV;
    if (glUniform1ui == NULL && glUniform1uiEXT != NULL) glUniform1ui = (PFNGLUNIFORM1UIPROC)glUniform1uiEXT;
    if (glVertexAttrib2fvARB == NULL && glVertexAttrib2fvNV != NULL) glVertexAttrib2fvARB = (PFNGLVERTEXATTRIB2FVARBPROC)glVertexAttrib2fvNV;
    if (glVertexAttrib2fvARB == NULL && glVertexAttrib2fv != NULL) glVertexAttrib2fvARB = (PFNGLVERTEXATTRIB2FVARBPROC)glVertexAttrib2fv;
    if (glClearDepthf == NULL && glClearDepthfOES != NULL) glClearDepthf = (PFNGLCLEARDEPTHFPROC)glClearDepthfOES;
    if (glVertexAttribI4svEXT == NULL && glVertexAttribI4sv != NULL) glVertexAttribI4svEXT = (PFNGLVERTEXATTRIBI4SVEXTPROC)glVertexAttribI4sv;
    if (glMemoryBarrier == NULL && glMemoryBarrierEXT != NULL) glMemoryBarrier = (PFNGLMEMORYBARRIERPROC)glMemoryBarrierEXT;
    if (glIsProgramARB == NULL && glIsProgramNV != NULL) glIsProgramARB = (PFNGLISPROGRAMARBPROC)glIsProgramNV;
    if (glGetFragDataLocation == NULL && glGetFragDataLocationEXT != NULL) glGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC)glGetFragDataLocationEXT;
    if (glVertexAttribI4iEXT == NULL && glVertexAttribI4i != NULL) glVertexAttribI4iEXT = (PFNGLVERTEXATTRIBI4IEXTPROC)glVertexAttribI4i;
    if (glVertexAttrib4NbvARB == NULL && glVertexAttrib4Nbv != NULL) glVertexAttrib4NbvARB = (PFNGLVERTEXATTRIB4NBVARBPROC)glVertexAttrib4Nbv;
    if (glDetachObjectARB == NULL && glDetachShader != NULL) glDetachObjectARB = (PFNGLDETACHOBJECTARBPROC)glDetachShader;
    if (glUniform3fARB == NULL && glUniform3f != NULL) glUniform3fARB = (PFNGLUNIFORM3FARBPROC)glUniform3f;
    if (glEnableVertexAttribArrayARB == NULL && glEnableVertexAttribArray != NULL) glEnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)glEnableVertexAttribArray;
    if (glGetVertexAttribivARB == NULL && glGetVertexAttribivNV != NULL) glGetVertexAttribivARB = (PFNGLGETVERTEXATTRIBIVARBPROC)glGetVertexAttribivNV;
    if (glGetVertexAttribivARB == NULL && glGetVertexAttribiv != NULL) glGetVertexAttribivARB = (PFNGLGETVERTEXATTRIBIVARBPROC)glGetVertexAttribiv;
    if (glDeleteQueries == NULL && glDeleteQueriesARB != NULL) glDeleteQueries = (PFNGLDELETEQUERIESPROC)glDeleteQueriesARB;
    if (glUseProgramObjectARB == NULL && glUseProgram != NULL) glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)glUseProgram;
    if (glVertexAttrib4dv == NULL && glVertexAttrib4dvNV != NULL) glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC)glVertexAttrib4dvNV;
    if (glVertexAttrib4dv == NULL && glVertexAttrib4dvARB != NULL) glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC)glVertexAttrib4dvARB;
    if (glVertexAttrib4f == NULL && glVertexAttrib4fNV != NULL) glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC)glVertexAttrib4fNV;
    if (glVertexAttrib4f == NULL && glVertexAttrib4fARB != NULL) glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC)glVertexAttrib4fARB;
    if (glVertexAttrib4d == NULL && glVertexAttrib4dNV != NULL) glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC)glVertexAttrib4dNV;
    if (glVertexAttrib4d == NULL && glVertexAttrib4dARB != NULL) glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC)glVertexAttrib4dARB;
    if (glProgramUniform3ui == NULL && glProgramUniform3uiEXT != NULL) glProgramUniform3ui = (PFNGLPROGRAMUNIFORM3UIPROC)glProgramUniform3uiEXT;
    if (glVertexAttrib3svARB == NULL && glVertexAttrib3sv != NULL) glVertexAttrib3svARB = (PFNGLVERTEXATTRIB3SVARBPROC)glVertexAttrib3sv;
    if (glVertexAttrib3svARB == NULL && glVertexAttrib3svNV != NULL) glVertexAttrib3svARB = (PFNGLVERTEXATTRIB3SVARBPROC)glVertexAttrib3svNV;
    if (glProgramUniformMatrix2x3fv == NULL && glProgramUniformMatrix2x3fvEXT != NULL) glProgramUniformMatrix2x3fv = (PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC)glProgramUniformMatrix2x3fvEXT;
    if (glGetVertexAttribPointervNV == NULL && glGetVertexAttribPointerv != NULL) glGetVertexAttribPointervNV = (PFNGLGETVERTEXATTRIBPOINTERVNVPROC)glGetVertexAttribPointerv;
    if (glGetVertexAttribPointervNV == NULL && glGetVertexAttribPointervARB != NULL) glGetVertexAttribPointervNV = (PFNGLGETVERTEXATTRIBPOINTERVNVPROC)glGetVertexAttribPointervARB;
    if (glGenerateMipmapEXT == NULL && glGenerateMipmap != NULL) glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC)glGenerateMipmap;
    if (glVertexAttribI2uivEXT == NULL && glVertexAttribI2uiv != NULL) glVertexAttribI2uivEXT = (PFNGLVERTEXATTRIBI2UIVEXTPROC)glVertexAttribI2uiv;
    if (glProgramUniform1i == NULL && glProgramUniform1iEXT != NULL) glProgramUniform1i = (PFNGLPROGRAMUNIFORM1IPROC)glProgramUniform1iEXT;
    if (glProgramUniform1f == NULL && glProgramUniform1fEXT != NULL) glProgramUniform1f = (PFNGLPROGRAMUNIFORM1FPROC)glProgramUniform1fEXT;
    if (glProgramParameteriEXT == NULL && glProgramParameteriARB != NULL) glProgramParameteriEXT = (PFNGLPROGRAMPARAMETERIEXTPROC)glProgramParameteriARB;
    if (glProgramParameteriEXT == NULL && glProgramParameteri != NULL) glProgramParameteriEXT = (PFNGLPROGRAMPARAMETERIEXTPROC)glProgramParameteri;
    if (glBeginTransformFeedback == NULL && glBeginTransformFeedbackEXT != NULL) glBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC)glBeginTransformFeedbackEXT;
    if (glBeginTransformFeedback == NULL && glBeginTransformFeedbackNV != NULL) glBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC)glBeginTransformFeedbackNV;
    if (glProgramUniform3iv == NULL && glProgramUniform3ivEXT != NULL) glProgramUniform3iv = (PFNGLPROGRAMUNIFORM3IVPROC)glProgramUniform3ivEXT;
    if (glBindFragDataLocationEXT == NULL && glBindFragDataLocation != NULL) glBindFragDataLocationEXT = (PFNGLBINDFRAGDATALOCATIONEXTPROC)glBindFragDataLocation;
    if (glNamedBufferSubData == NULL && glNamedBufferSubDataEXT != NULL) glNamedBufferSubData = (PFNGLNAMEDBUFFERSUBDATAPROC)glNamedBufferSubDataEXT;
    if (glGenBuffersARB == NULL && glGenBuffers != NULL) glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)glGenBuffers;
    if (glBlendEquationi == NULL && glBlendEquationiARB != NULL) glBlendEquationi = (PFNGLBLENDEQUATIONIPROC)glBlendEquationiARB;
    if (glBlendEquationi == NULL && glBlendEquationIndexedAMD != NULL) glBlendEquationi = (PFNGLBLENDEQUATIONIPROC)glBlendEquationIndexedAMD;
    if (glVertexAttrib4s == NULL && glVertexAttrib4sNV != NULL) glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC)glVertexAttrib4sNV;
    if (glVertexAttrib4s == NULL && glVertexAttrib4sARB != NULL) glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC)glVertexAttrib4sARB;
    if (glVertexAttrib4fvNV == NULL && glVertexAttrib4fv != NULL) glVertexAttrib4fvNV = (PFNGLVERTEXATTRIB4FVNVPROC)glVertexAttrib4fv;
    if (glVertexAttrib4fvNV == NULL && glVertexAttrib4fvARB != NULL) glVertexAttrib4fvNV = (PFNGLVERTEXATTRIB4FVNVPROC)glVertexAttrib4fvARB;
    if (glGetBufferSubDataARB == NULL && glGetBufferSubData != NULL) glGetBufferSubDataARB = (PFNGLGETBUFFERSUBDATAARBPROC)glGetBufferSubData;
    if (glGetAttribLocation == NULL && glGetAttribLocationARB != NULL) glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)glGetAttribLocationARB;
    if (glClearDepthfOES == NULL && glClearDepthf != NULL) glClearDepthfOES = (PFNGLCLEARDEPTHFOESPROC)glClearDepthf;
    if (glBlendFuncSeparateIndexedAMD == NULL && glBlendFuncSeparatei != NULL) glBlendFuncSeparateIndexedAMD = (PFNGLBLENDFUNCSEPARATEINDEXEDAMDPROC)glBlendFuncSeparatei;
    if (glBlendFuncSeparateIndexedAMD == NULL && glBlendFuncSeparateiARB != NULL) glBlendFuncSeparateIndexedAMD = (PFNGLBLENDFUNCSEPARATEINDEXEDAMDPROC)glBlendFuncSeparateiARB;
    if (glGetVertexAttribivNV == NULL && glGetVertexAttribiv != NULL) glGetVertexAttribivNV = (PFNGLGETVERTEXATTRIBIVNVPROC)glGetVertexAttribiv;
    if (glGetVertexAttribivNV == NULL && glGetVertexAttribivARB != NULL) glGetVertexAttribivNV = (PFNGLGETVERTEXATTRIBIVNVPROC)glGetVertexAttribivARB;
    if (glVertexAttrib2dvARB == NULL && glVertexAttrib2dv != NULL) glVertexAttrib2dvARB = (PFNGLVERTEXATTRIB2DVARBPROC)glVertexAttrib2dv;
    if (glVertexAttrib2dvARB == NULL && glVertexAttrib2dvNV != NULL) glVertexAttrib2dvARB = (PFNGLVERTEXATTRIB2DVARBPROC)glVertexAttrib2dvNV;
    if (glVertexAttribPointer == NULL && glVertexAttribPointerARB != NULL) glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)glVertexAttribPointerARB;
    if (glDrawBuffersATI == NULL && glDrawBuffers != NULL) glDrawBuffersATI = (PFNGLDRAWBUFFERSATIPROC)glDrawBuffers;
    if (glDrawBuffersATI == NULL && glDrawBuffersARB != NULL) glDrawBuffersATI = (PFNGLDRAWBUFFERSATIPROC)glDrawBuffersARB;
    if (glGetFloati_v == NULL && glGetFloatIndexedvEXT != NULL) glGetFloati_v = (PFNGLGETFLOATI_VPROC)glGetFloatIndexedvEXT;
    if (glGetFloati_v == NULL && glGetFloati_vEXT != NULL) glGetFloati_v = (PFNGLGETFLOATI_VPROC)glGetFloati_vEXT;
    if (glProgramUniform1ivEXT == NULL && glProgramUniform1iv != NULL) glProgramUniform1ivEXT = (PFNGLPROGRAMUNIFORM1IVEXTPROC)glProgramUniform1iv;
    if (glVertexAttribI4bv == NULL && glVertexAttribI4bvEXT != NULL) glVertexAttribI4bv = (PFNGLVERTEXATTRIBI4BVPROC)glVertexAttribI4bvEXT;
    if (glEndTransformFeedbackEXT == NULL && glEndTransformFeedbackNV != NULL) glEndTransformFeedbackEXT = (PFNGLENDTRANSFORMFEEDBACKEXTPROC)glEndTransformFeedbackNV;
    if (glEndTransformFeedbackEXT == NULL && glEndTransformFeedback != NULL) glEndTransformFeedbackEXT = (PFNGLENDTRANSFORMFEEDBACKEXTPROC)glEndTransformFeedback;
    if (glEndConditionalRenderNVX == NULL && glEndConditionalRenderNV != NULL) glEndConditionalRenderNVX = (PFNGLENDCONDITIONALRENDERNVXPROC)glEndConditionalRenderNV;
    if (glEndConditionalRenderNVX == NULL && glEndConditionalRender != NULL) glEndConditionalRenderNVX = (PFNGLENDCONDITIONALRENDERNVXPROC)glEndConditionalRender;
    if (glUniform2iv == NULL && glUniform2ivARB != NULL) glUniform2iv = (PFNGLUNIFORM2IVPROC)glUniform2ivARB;
    if (glFramebufferTexture1D == NULL && glFramebufferTexture1DEXT != NULL) glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)glFramebufferTexture1DEXT;
    if (glNamedBufferStorage == NULL && glNamedBufferStorageEXT != NULL) glNamedBufferStorage = (PFNGLNAMEDBUFFERSTORAGEPROC)glNamedBufferStorageEXT;
    if (glDebugMessageControlARB == NULL && glDebugMessageControl != NULL) glDebugMessageControlARB = (PFNGLDEBUGMESSAGECONTROLARBPROC)glDebugMessageControl;
    if (glLinkProgramARB == NULL && glLinkProgram != NULL) glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)glLinkProgram;
    if (glCompressedTexImage3D == NULL && glCompressedTexImage3DARB != NULL) glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC)glCompressedTexImage3DARB;
    if (glClampColorARB == NULL && glClampColor != NULL) glClampColorARB = (PFNGLCLAMPCOLORARBPROC)glClampColor;
    if (glTransformFeedbackVaryingsEXT == NULL && glTransformFeedbackVaryings != NULL) glTransformFeedbackVaryingsEXT = (PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC)glTransformFeedbackVaryings;
    if (glVertexAttrib2svNV == NULL && glVertexAttrib2sv != NULL) glVertexAttrib2svNV = (PFNGLVERTEXATTRIB2SVNVPROC)glVertexAttrib2sv;
    if (glVertexAttrib2svNV == NULL && glVertexAttrib2svARB != NULL) glVertexAttrib2svNV = (PFNGLVERTEXATTRIB2SVNVPROC)glVertexAttrib2svARB;
    if (glMemoryBarrierEXT == NULL && glMemoryBarrier != NULL) glMemoryBarrierEXT = (PFNGLMEMORYBARRIEREXTPROC)glMemoryBarrier;
    if (glGetTexParameterIiv == NULL && glGetTexParameterIivEXT != NULL) glGetTexParameterIiv = (PFNGLGETTEXPARAMETERIIVPROC)glGetTexParameterIivEXT;
    if (glVertexAttribI4usvEXT == NULL && glVertexAttribI4usv != NULL) glVertexAttribI4usvEXT = (PFNGLVERTEXATTRIBI4USVEXTPROC)glVertexAttribI4usv;
    if (glTransformFeedbackVaryings == NULL && glTransformFeedbackVaryingsEXT != NULL) glTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC)glTransformFeedbackVaryingsEXT;
    if (glVertexAttrib4svARB == NULL && glVertexAttrib4sv != NULL) glVertexAttrib4svARB = (PFNGLVERTEXATTRIB4SVARBPROC)glVertexAttrib4sv;
    if (glVertexAttrib4svARB == NULL && glVertexAttrib4svNV != NULL) glVertexAttrib4svARB = (PFNGLVERTEXATTRIB4SVARBPROC)glVertexAttrib4svNV;
    if (glDebugMessageCallback == NULL && glDebugMessageCallbackARB != NULL) glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)glDebugMessageCallbackARB;
    if (glBufferSubDataARB == NULL && glBufferSubData != NULL) glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)glBufferSubData;
    if (glGetBooleani_v == NULL && glGetBooleanIndexedvEXT != NULL) glGetBooleani_v = (PFNGLGETBOOLEANI_VPROC)glGetBooleanIndexedvEXT;
    if (glBindBufferRangeNV == NULL && glBindBufferRange != NULL) glBindBufferRangeNV = (PFNGLBINDBUFFERRANGENVPROC)glBindBufferRange;
    if (glBindBufferRangeNV == NULL && glBindBufferRangeEXT != NULL) glBindBufferRangeNV = (PFNGLBINDBUFFERRANGENVPROC)glBindBufferRangeEXT;
    if (glMultiDrawArraysIndirect == NULL && glMultiDrawArraysIndirectAMD != NULL) glMultiDrawArraysIndirect = (PFNGLMULTIDRAWARRAYSINDIRECTPROC)glMultiDrawArraysIndirectAMD;
    if (glVertexAttrib4svNV == NULL && glVertexAttrib4svARB != NULL) glVertexAttrib4svNV = (PFNGLVERTEXATTRIB4SVNVPROC)glVertexAttrib4svARB;
    if (glVertexAttrib4svNV == NULL && glVertexAttrib4sv != NULL) glVertexAttrib4svNV = (PFNGLVERTEXATTRIB4SVNVPROC)glVertexAttrib4sv;
    if (glVertexAttrib3fvNV == NULL && glVertexAttrib3fv != NULL) glVertexAttrib3fvNV = (PFNGLVERTEXATTRIB3FVNVPROC)glVertexAttrib3fv;
    if (glVertexAttrib3fvNV == NULL && glVertexAttrib3fvARB != NULL) glVertexAttrib3fvNV = (PFNGLVERTEXATTRIB3FVNVPROC)glVertexAttrib3fvARB;
    if (glVertexAttribL4dv == NULL && glVertexAttribL4dvEXT != NULL) glVertexAttribL4dv = (PFNGLVERTEXATTRIBL4DVPROC)glVertexAttribL4dvEXT;
    if (glGetVertexAttribPointerv == NULL && glGetVertexAttribPointervNV != NULL) glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC)glGetVertexAttribPointervNV;
    if (glGetVertexAttribPointerv == NULL && glGetVertexAttribPointervARB != NULL) glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC)glGetVertexAttribPointervARB;
    if (glDrawArraysInstancedARB == NULL && glDrawArraysInstanced != NULL) glDrawArraysInstancedARB = (PFNGLDRAWARRAYSINSTANCEDARBPROC)glDrawArraysInstanced;
    if (glDrawArraysInstancedARB == NULL && glDrawArraysInstancedEXT != NULL) glDrawArraysInstancedARB = (PFNGLDRAWARRAYSINSTANCEDARBPROC)glDrawArraysInstancedEXT;
    if (glVertexAttribI2uiEXT == NULL && glVertexAttribI2ui != NULL) glVertexAttribI2uiEXT = (PFNGLVERTEXATTRIBI2UIEXTPROC)glVertexAttribI2ui;
    if (glProgramUniform4uiv == NULL && glProgramUniform4uivEXT != NULL) glProgramUniform4uiv = (PFNGLPROGRAMUNIFORM4UIVPROC)glProgramUniform4uivEXT;
    if (glFramebufferRenderbufferEXT == NULL && glFramebufferRenderbuffer != NULL) glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)glFramebufferRenderbuffer;
    if (glBindBufferOffsetNV == NULL && glBindBufferOffsetEXT != NULL) glBindBufferOffsetNV = (PFNGLBINDBUFFEROFFSETNVPROC)glBindBufferOffsetEXT;
    if (glGetVertexAttribiv == NULL && glGetVertexAttribivNV != NULL) glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC)glGetVertexAttribivNV;
    if (glGetVertexAttribiv == NULL && glGetVertexAttribivARB != NULL) glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC)glGetVertexAttribivARB;
    if (glVertexAttribI2uiv == NULL && glVertexAttribI2uivEXT != NULL) glVertexAttribI2uiv = (PFNGLVERTEXATTRIBI2UIVPROC)glVertexAttribI2uivEXT;
    if (glUniform2ivARB == NULL && glUniform2iv != NULL) glUniform2ivARB = (PFNGLUNIFORM2IVARBPROC)glUniform2iv;
    if (glVertexAttrib4NuivARB == NULL && glVertexAttrib4Nuiv != NULL) glVertexAttrib4NuivARB = (PFNGLVERTEXATTRIB4NUIVARBPROC)glVertexAttrib4Nuiv;
    if (glVertexAttrib3f == NULL && glVertexAttrib3fARB != NULL) glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC)glVertexAttrib3fARB;
    if (glVertexAttrib3f == NULL && glVertexAttrib3fNV != NULL) glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC)glVertexAttrib3fNV;
    if (glVertexAttrib3d == NULL && glVertexAttrib3dNV != NULL) glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC)glVertexAttrib3dNV;
    if (glVertexAttrib3d == NULL && glVertexAttrib3dARB != NULL) glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC)glVertexAttrib3dARB;
    if (glBlendColor == NULL && glBlendColorEXT != NULL) glBlendColor = (PFNGLBLENDCOLORPROC)glBlendColorEXT;
    if (glVertexAttrib3s == NULL && glVertexAttrib3sARB != NULL) glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC)glVertexAttrib3sARB;
    if (glVertexAttrib3s == NULL && glVertexAttrib3sNV != NULL) glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC)glVertexAttrib3sNV;
    if (glDrawElementsInstancedEXT == NULL && glDrawElementsInstanced != NULL) glDrawElementsInstancedEXT = (PFNGLDRAWELEMENTSINSTANCEDEXTPROC)glDrawElementsInstanced;
    if (glDrawElementsInstancedEXT == NULL && glDrawElementsInstancedARB != NULL) glDrawElementsInstancedEXT = (PFNGLDRAWELEMENTSINSTANCEDEXTPROC)glDrawElementsInstancedARB;
    if (glUniform4i == NULL && glUniform4iARB != NULL) glUniform4i = (PFNGLUNIFORM4IPROC)glUniform4iARB;
    if (glActiveTexture == NULL && glActiveTextureARB != NULL) glActiveTexture = (PFNGLACTIVETEXTUREPROC)glActiveTextureARB;
    if (glEnableVertexAttribArray == NULL && glEnableVertexAttribArrayARB != NULL) glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glEnableVertexAttribArrayARB;
    if (glUniform4f == NULL && glUniform4fARB != NULL) glUniform4f = (PFNGLUNIFORM4FPROC)glUniform4fARB;
    if (glVertexAttribI4sv == NULL && glVertexAttribI4svEXT != NULL) glVertexAttribI4sv = (PFNGLVERTEXATTRIBI4SVPROC)glVertexAttribI4svEXT;
    if (glVertexAttribLPointer == NULL && glVertexAttribLPointerEXT != NULL) glVertexAttribLPointer = (PFNGLVERTEXATTRIBLPOINTERPROC)glVertexAttribLPointerEXT;
    if (glDrawTransformFeedbackNV == NULL && glDrawTransformFeedback != NULL) glDrawTransformFeedbackNV = (PFNGLDRAWTRANSFORMFEEDBACKNVPROC)glDrawTransformFeedback;
    if (glProgramUniformMatrix3x2fv == NULL && glProgramUniformMatrix3x2fvEXT != NULL) glProgramUniformMatrix3x2fv = (PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC)glProgramUniformMatrix3x2fvEXT;
    if (glUniform2fvARB == NULL && glUniform2fv != NULL) glUniform2fvARB = (PFNGLUNIFORM2FVARBPROC)glUniform2fv;
    if (glFramebufferTexture3DEXT == NULL && glFramebufferTexture3D != NULL) glFramebufferTexture3DEXT = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)glFramebufferTexture3D;
    if (glDeleteTransformFeedbacksNV == NULL && glDeleteTransformFeedbacks != NULL) glDeleteTransformFeedbacksNV = (PFNGLDELETETRANSFORMFEEDBACKSNVPROC)glDeleteTransformFeedbacks;
    if (glVertexAttribLPointerEXT == NULL && glVertexAttribLPointer != NULL) glVertexAttribLPointerEXT = (PFNGLVERTEXATTRIBLPOINTEREXTPROC)glVertexAttribLPointer;
    if (glCompileShaderARB == NULL && glCompileShader != NULL) glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)glCompileShader;
    if (glNamedBufferSubDataEXT == NULL && glNamedBufferSubData != NULL) glNamedBufferSubDataEXT = (PFNGLNAMEDBUFFERSUBDATAEXTPROC)glNamedBufferSubData;
    if (glTexBufferEXT == NULL && glTexBuffer != NULL) glTexBufferEXT = (PFNGLTEXBUFFEREXTPROC)glTexBuffer;
    if (glTexBufferEXT == NULL && glTexBufferARB != NULL) glTexBufferEXT = (PFNGLTEXBUFFEREXTPROC)glTexBufferARB;
    if (glCreateProgram == NULL && glCreateProgramObjectARB != NULL) glCreateProgram = (PFNGLCREATEPROGRAMPROC)glCreateProgramObjectARB;
    if (glBlendEquationSeparateEXT == NULL && glBlendEquationSeparate != NULL) glBlendEquationSeparateEXT = (PFNGLBLENDEQUATIONSEPARATEEXTPROC)glBlendEquationSeparate;
    if (glVertexAttrib2dNV == NULL && glVertexAttrib2dARB != NULL) glVertexAttrib2dNV = (PFNGLVERTEXATTRIB2DNVPROC)glVertexAttrib2dARB;
    if (glVertexAttrib2dNV == NULL && glVertexAttrib2d != NULL) glVertexAttrib2dNV = (PFNGLVERTEXATTRIB2DNVPROC)glVertexAttrib2d;
    if (glUniform4iv == NULL && glUniform4ivARB != NULL) glUniform4iv = (PFNGLUNIFORM4IVPROC)glUniform4ivARB;
    if (glUniform3uiv == NULL && glUniform3uivEXT != NULL) glUniform3uiv = (PFNGLUNIFORM3UIVPROC)glUniform3uivEXT;
    if (glGetQueryObjectivARB == NULL && glGetQueryObjectiv != NULL) glGetQueryObjectivARB = (PFNGLGETQUERYOBJECTIVARBPROC)glGetQueryObjectiv;
    if (glVertexAttrib3sNV == NULL && glVertexAttrib3sARB != NULL) glVertexAttrib3sNV = (PFNGLVERTEXATTRIB3SNVPROC)glVertexAttrib3sARB;
    if (glVertexAttrib3sNV == NULL && glVertexAttrib3s != NULL) glVertexAttrib3sNV = (PFNGLVERTEXATTRIB3SNVPROC)glVertexAttrib3s;
    if (glUniform3ui == NULL && glUniform3uiEXT != NULL) glUniform3ui = (PFNGLUNIFORM3UIPROC)glUniform3uiEXT;
    if (glVertexAttribI4uiv == NULL && glVertexAttribI4uivEXT != NULL) glVertexAttribI4uiv = (PFNGLVERTEXATTRIBI4UIVPROC)glVertexAttribI4uivEXT;
    if (glVertexAttrib4bv == NULL && glVertexAttrib4bvARB != NULL) glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC)glVertexAttrib4bvARB;
    if (glVertexAttrib1dARB == NULL && glVertexAttrib1dNV != NULL) glVertexAttrib1dARB = (PFNGLVERTEXATTRIB1DARBPROC)glVertexAttrib1dNV;
    if (glVertexAttrib1dARB == NULL && glVertexAttrib1d != NULL) glVertexAttrib1dARB = (PFNGLVERTEXATTRIB1DARBPROC)glVertexAttrib1d;
    if (glProgramUniform1fEXT == NULL && glProgramUniform1f != NULL) glProgramUniform1fEXT = (PFNGLPROGRAMUNIFORM1FEXTPROC)glProgramUniform1f;
    if (glVertexAttrib3sARB == NULL && glVertexAttrib3s != NULL) glVertexAttrib3sARB = (PFNGLVERTEXATTRIB3SARBPROC)glVertexAttrib3s;
    if (glVertexAttrib3sARB == NULL && glVertexAttrib3sNV != NULL) glVertexAttrib3sARB = (PFNGLVERTEXATTRIB3SARBPROC)glVertexAttrib3sNV;
    if (glGetVertexAttribIuiv == NULL && glGetVertexAttribIuivEXT != NULL) glGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC)glGetVertexAttribIuivEXT;
    if (glGetTexParameterIuivEXT == NULL && glGetTexParameterIuiv != NULL) glGetTexParameterIuivEXT = (PFNGLGETTEXPARAMETERIUIVEXTPROC)glGetTexParameterIuiv;
    if (glCompressedTexSubImage2D == NULL && glCompressedTexSubImage2DARB != NULL) glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)glCompressedTexSubImage2DARB;
    if (glCompressedTexSubImage3DARB == NULL && glCompressedTexSubImage3D != NULL) glCompressedTexSubImage3DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC)glCompressedTexSubImage3D;
    if (glVertexAttrib1f == NULL && glVertexAttrib1fARB != NULL) glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)glVertexAttrib1fARB;
    if (glVertexAttrib1f == NULL && glVertexAttrib1fNV != NULL) glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)glVertexAttrib1fNV;
    if (glTexParameterIiv == NULL && glTexParameterIivEXT != NULL) glTexParameterIiv = (PFNGLTEXPARAMETERIIVPROC)glTexParameterIivEXT;
    if (glEndTransformFeedback == NULL && glEndTransformFeedbackNV != NULL) glEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC)glEndTransformFeedbackNV;
    if (glEndTransformFeedback == NULL && glEndTransformFeedbackEXT != NULL) glEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC)glEndTransformFeedbackEXT;
    if (glBindTextureEXT == NULL && glBindTexture != NULL) glBindTextureEXT = (PFNGLBINDTEXTUREEXTPROC)glBindTexture;
    if (glVertexAttribI1iv == NULL && glVertexAttribI1ivEXT != NULL) glVertexAttribI1iv = (PFNGLVERTEXATTRIBI1IVPROC)glVertexAttribI1ivEXT;
    if (glVertexAttribDivisor == NULL && glVertexAttribDivisorARB != NULL) glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)glVertexAttribDivisorARB;
    if (glActiveTextureARB == NULL && glActiveTexture != NULL) glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)glActiveTexture;
    if (glTexParameterIivEXT == NULL && glTexParameterIiv != NULL) glTexParameterIivEXT = (PFNGLTEXPARAMETERIIVEXTPROC)glTexParameterIiv;
    if (glVertexAttrib2f == NULL && glVertexAttrib2fARB != NULL) glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC)glVertexAttrib2fARB;
    if (glVertexAttrib2f == NULL && glVertexAttrib2fNV != NULL) glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC)glVertexAttrib2fNV;
    if (glVertexAttrib4Nsv == NULL && glVertexAttrib4NsvARB != NULL) glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC)glVertexAttrib4NsvARB;
    if (glVertexAttrib3dv == NULL && glVertexAttrib3dvARB != NULL) glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC)glVertexAttrib3dvARB;
    if (glVertexAttrib3dv == NULL && glVertexAttrib3dvNV != NULL) glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC)glVertexAttrib3dvNV;
    if (glProgramUniformMatrix4x2fv == NULL && glProgramUniformMatrix4x2fvEXT != NULL) glProgramUniformMatrix4x2fv = (PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC)glProgramUniformMatrix4x2fvEXT;
    if (glVertexAttrib3dvARB == NULL && glVertexAttrib3dv != NULL) glVertexAttrib3dvARB = (PFNGLVERTEXATTRIB3DVARBPROC)glVertexAttrib3dv;
    if (glVertexAttrib3dvARB == NULL && glVertexAttrib3dvNV != NULL) glVertexAttrib3dvARB = (PFNGLVERTEXATTRIB3DVARBPROC)glVertexAttrib3dvNV;
    if (glGenProgramsARB == NULL && glGenProgramsNV != NULL) glGenProgramsARB = (PFNGLGENPROGRAMSARBPROC)glGenProgramsNV;
    if (glGenTransformFeedbacksNV == NULL && glGenTransformFeedbacks != NULL) glGenTransformFeedbacksNV = (PFNGLGENTRANSFORMFEEDBACKSNVPROC)glGenTransformFeedbacks;
    if (glVertexAttribI2iv == NULL && glVertexAttribI2ivEXT != NULL) glVertexAttribI2iv = (PFNGLVERTEXATTRIBI2IVPROC)glVertexAttribI2ivEXT;
    if (glVertexAttrib4ubNV == NULL && glVertexAttrib4Nub != NULL) glVertexAttrib4ubNV = (PFNGLVERTEXATTRIB4UBNVPROC)glVertexAttrib4Nub;
    if (glVertexAttrib4ubNV == NULL && glVertexAttrib4NubARB != NULL) glVertexAttrib4ubNV = (PFNGLVERTEXATTRIB4UBNVPROC)glVertexAttrib4NubARB;
    if (glGetFragDataLocationEXT == NULL && glGetFragDataLocation != NULL) glGetFragDataLocationEXT = (PFNGLGETFRAGDATALOCATIONEXTPROC)glGetFragDataLocation;
    if (glShaderSource == NULL && glShaderSourceARB != NULL) glShaderSource = (PFNGLSHADERSOURCEPROC)glShaderSourceARB;
    if (glDeleteRenderbuffers == NULL && glDeleteRenderbuffersEXT != NULL) glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)glDeleteRenderbuffersEXT;
    if (glCopyTexImage2DEXT == NULL && glCopyTexImage2D != NULL) glCopyTexImage2DEXT = (PFNGLCOPYTEXIMAGE2DEXTPROC)glCopyTexImage2D;
    if (glBufferData == NULL && glBufferDataARB != NULL) glBufferData = (PFNGLBUFFERDATAPROC)glBufferDataARB;
    if (glAttachObjectARB == NULL && glAttachShader != NULL) glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)glAttachShader;
    if (glCopyTexSubImage1DEXT == NULL && glCopyTexSubImage1D != NULL) glCopyTexSubImage1DEXT = (PFNGLCOPYTEXSUBIMAGE1DEXTPROC)glCopyTexSubImage1D;
    if (glUniformMatrix3fvARB == NULL && glUniformMatrix3fv != NULL) glUniformMatrix3fvARB = (PFNGLUNIFORMMATRIX3FVARBPROC)glUniformMatrix3fv;
    if (glPointParameterfvEXT == NULL && glPointParameterfv != NULL) glPointParameterfvEXT = (PFNGLPOINTPARAMETERFVEXTPROC)glPointParameterfv;
    if (glPointParameterfvEXT == NULL && glPointParameterfvARB != NULL) glPointParameterfvEXT = (PFNGLPOINTPARAMETERFVEXTPROC)glPointParameterfvARB;
    if (glPointParameterfvEXT == NULL && glPointParameterfvSGIS != NULL) glPointParameterfvEXT = (PFNGLPOINTPARAMETERFVEXTPROC)glPointParameterfvSGIS;
    if (glUniform4fARB == NULL && glUniform4f != NULL) glUniform4fARB = (PFNGLUNIFORM4FARBPROC)glUniform4f;
    if (glVertexAttribI1i == NULL && glVertexAttribI1iEXT != NULL) glVertexAttribI1i = (PFNGLVERTEXATTRIBI1IPROC)glVertexAttribI1iEXT;
    if (glGetDebugMessageLogARB == NULL && glGetDebugMessageLog != NULL) glGetDebugMessageLogARB = (PFNGLGETDEBUGMESSAGELOGARBPROC)glGetDebugMessageLog;
    if (glGetBufferPointerv == NULL && glGetBufferPointervARB != NULL) glGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC)glGetBufferPointervARB;
    if (glFramebufferTexture3D == NULL && glFramebufferTexture3DEXT != NULL) glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)glFramebufferTexture3DEXT;
    if (glBindBufferARB == NULL && glBindBuffer != NULL) glBindBufferARB = (PFNGLBINDBUFFERARBPROC)glBindBuffer;
    if (glVertexAttrib1sNV == NULL && glVertexAttrib1s != NULL) glVertexAttrib1sNV = (PFNGLVERTEXATTRIB1SNVPROC)glVertexAttrib1s;
    if (glVertexAttrib1sNV == NULL && glVertexAttrib1sARB != NULL) glVertexAttrib1sNV = (PFNGLVERTEXATTRIB1SNVPROC)glVertexAttrib1sARB;
    if (glUniform1fv == NULL && glUniform1fvARB != NULL) glUniform1fv = (PFNGLUNIFORM1FVPROC)glUniform1fvARB;
    if (glUniform1ivARB == NULL && glUniform1iv != NULL) glUniform1ivARB = (PFNGLUNIFORM1IVARBPROC)glUniform1iv;
    if (glVertexAttrib2d == NULL && glVertexAttrib2dARB != NULL) glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC)glVertexAttrib2dARB;
    if (glVertexAttrib2d == NULL && glVertexAttrib2dNV != NULL) glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC)glVertexAttrib2dNV;
    if (glMultiDrawElementsEXT == NULL && glMultiDrawElements != NULL) glMultiDrawElementsEXT = (PFNGLMULTIDRAWELEMENTSEXTPROC)glMultiDrawElements;
    if (glGetTransformFeedbackVaryingEXT == NULL && glGetTransformFeedbackVarying != NULL) glGetTransformFeedbackVaryingEXT = (PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC)glGetTransformFeedbackVarying;
    if (glVertexAttrib2s == NULL && glVertexAttrib2sNV != NULL) glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC)glVertexAttrib2sNV;
    if (glVertexAttrib2s == NULL && glVertexAttrib2sARB != NULL) glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC)glVertexAttrib2sARB;
    if (glVertexAttrib4dARB == NULL && glVertexAttrib4dNV != NULL) glVertexAttrib4dARB = (PFNGLVERTEXATTRIB4DARBPROC)glVertexAttrib4dNV;
    if (glVertexAttrib4dARB == NULL && glVertexAttrib4d != NULL) glVertexAttrib4dARB = (PFNGLVERTEXATTRIB4DARBPROC)glVertexAttrib4d;
}

#if defined(GL_ES_VERSION_3_0) || defined(GL_VERSION_3_0)
#define _GLAD_GL_IS_SOME_NEW_VERSION 1
#endif

static int get_exts( int version, const char **out_exts, int *out_num_exts_i, const char ***out_exts_i) {
#ifdef _GLAD_GL_IS_SOME_NEW_VERSION
    if(version < 30) {
#endif
        if (glGetString == NULL) {
            return 0;
        }
        *out_exts = (const char *)glGetString(GL_EXTENSIONS);
#ifdef _GLAD_GL_IS_SOME_NEW_VERSION
    } else {
        int index;
        int num_exts_i = 0;
        const char **exts_i;
        if (glGetStringi == NULL || glGetIntegerv == NULL) {
            return 0;
        }
        glGetIntegerv(GL_NUM_EXTENSIONS, &num_exts_i);
        if (num_exts_i > 0) {
            exts_i = (const char **)malloc(num_exts_i * sizeof *exts_i);
        }
        if (exts_i == NULL) {
            return 0;
        }
        for(index = 0; index < num_exts_i; index++) {
            const char *gl_str_tmp = (const char*)glGetStringi(GL_EXTENSIONS, index);
            size_t len = strlen(gl_str_tmp);

            char *local_str = (char*)malloc((len+1) * sizeof(char));

            if(local_str != NULL) {
#if _MSC_VER >= 1400
                strncpy_s(local_str, len+1, gl_str_tmp, len);
#else
                strncpy(local_str, gl_str_tmp, len+1);
#endif
            }

            exts_i[index] = local_str;
        }

        *out_num_exts_i = num_exts_i;
        *out_exts_i = exts_i;
    }
#endif
    return 1;
}
static void free_exts(const char **exts_i) {
    if (exts_i != NULL) {
        free((char **)exts_i);
        exts_i = NULL;
    }
}
static int has_ext(int version, const char *exts, int num_exts_i, const char **exts_i, const char *ext) {
    if(version < 30) {
        const char *extensions;
        const char *loc;
        const char *terminator;
        extensions = exts;
        if(extensions == NULL || ext == NULL) {
            return 0;
        }
        while(1) {
            loc = strstr(extensions, ext);
            if(loc == NULL) {
                return 0;
            }
            terminator = loc + strlen(ext);
            if((loc == extensions || *(loc - 1) == ' ') &&
                (*terminator == ' ' || *terminator == '\0')) {
                return 1;
            }
            extensions = terminator;
        }
    } else {
        int index;
        for(index = 0; index < num_exts_i; index++) {
            const char *e = exts_i[index];
            if(strcmp(e, ext) == 0) {
                return 1;
            }
        }
    }
    return 0;
}

static int find_extensionsGL( int version) {
    const char *exts = NULL;
    int num_exts_i = 0;
    const char **exts_i = NULL;
    if (!get_exts(version, &exts, &num_exts_i, &exts_i)) return 0;

    GLAD_GL_NV_transform_feedback2 = has_ext(version, exts, num_exts_i, exts_i, "GL_NV_transform_feedback2");
    GLAD_GL_ARB_texture_compression = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_texture_compression");
    GLAD_GL_NV_point_sprite = has_ext(version, exts, num_exts_i, exts_i, "GL_NV_point_sprite");
    GLAD_GL_ARB_get_texture_sub_image = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_get_texture_sub_image");
    GLAD_GL_AMD_multi_draw_indirect = has_ext(version, exts, num_exts_i, exts_i, "GL_AMD_multi_draw_indirect");
    GLAD_GL_EXT_blend_color = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_blend_color");
    GLAD_GL_ARB_texture_storage_multisample = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_texture_storage_multisample");
    GLAD_GL_ARB_clip_control = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_clip_control");
    GLAD_GL_ARB_instanced_arrays = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_instanced_arrays");
    GLAD_GL_EXT_vertex_attrib_64bit = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_vertex_attrib_64bit");
    GLAD_GL_ARB_vertex_attrib_binding = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_vertex_attrib_binding");
    GLAD_GL_ARB_framebuffer_no_attachments = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_framebuffer_no_attachments");
    GLAD_GL_ARB_texture_view = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_texture_view");
    GLAD_GL_ATI_separate_stencil = has_ext(version, exts, num_exts_i, exts_i, "GL_ATI_separate_stencil");
    GLAD_GL_EXT_texture_buffer_object = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_texture_buffer_object");
    GLAD_GL_NV_shader_buffer_store = has_ext(version, exts, num_exts_i, exts_i, "GL_NV_shader_buffer_store");
    GLAD_GL_ARB_shader_subroutine = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_shader_subroutine");
    GLAD_GL_EXT_direct_state_access = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_direct_state_access");
    GLAD_GL_ARB_program_interface_query = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_program_interface_query");
    GLAD_GL_ARB_copy_image = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_copy_image");
    GLAD_GL_EXT_texture_object = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_texture_object");
    GLAD_GL_NVX_conditional_render = has_ext(version, exts, num_exts_i, exts_i, "GL_NVX_conditional_render");
    GLAD_GL_ARB_copy_buffer = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_copy_buffer");
    GLAD_GL_EXT_blend_func_separate = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_blend_func_separate");
    GLAD_GL_ARB_buffer_storage = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_buffer_storage");
    GLAD_GL_ARB_occlusion_query = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_occlusion_query");
    GLAD_GL_ARB_multi_bind = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_multi_bind");
    GLAD_GL_KHR_robustness = has_ext(version, exts, num_exts_i, exts_i, "GL_KHR_robustness");
    GLAD_GL_ARB_provoking_vertex = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_provoking_vertex");
    GLAD_GL_EXT_gpu_shader4 = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_gpu_shader4");
    GLAD_GL_NV_geometry_program4 = has_ext(version, exts, num_exts_i, exts_i, "GL_NV_geometry_program4");
    GLAD_GL_NV_path_rendering_shared_edge = has_ext(version, exts, num_exts_i, exts_i, "GL_NV_path_rendering_shared_edge");
    GLAD_GL_NV_uniform_buffer_unified_memory = has_ext(version, exts, num_exts_i, exts_i, "GL_NV_uniform_buffer_unified_memory");
    GLAD_GL_ARB_internalformat_query2 = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_internalformat_query2");
    GLAD_GL_NV_shader_buffer_load = has_ext(version, exts, num_exts_i, exts_i, "GL_NV_shader_buffer_load");
    GLAD_GL_EXT_vertex_array = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_vertex_array");
    GLAD_GL_NV_vertex_buffer_unified_memory = has_ext(version, exts, num_exts_i, exts_i, "GL_NV_vertex_buffer_unified_memory");
    GLAD_GL_EXT_blend_equation_separate = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_blend_equation_separate");
    GLAD_GL_EXT_provoking_vertex = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_provoking_vertex");
    GLAD_GL_ARB_point_parameters = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_point_parameters");
    GLAD_GL_ARB_clear_texture = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_clear_texture");
    GLAD_GL_ARB_shader_image_load_store = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_shader_image_load_store");
    GLAD_GL_EXT_multi_draw_arrays = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_multi_draw_arrays");
    GLAD_GL_ARB_shader_atomic_counters = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_shader_atomic_counters");
    GLAD_GL_ARB_texture_barrier = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_texture_barrier");
    GLAD_GL_NV_bindless_multi_draw_indirect = has_ext(version, exts, num_exts_i, exts_i, "GL_NV_bindless_multi_draw_indirect");
    GLAD_GL_EXT_transform_feedback = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_transform_feedback");
    GLAD_GL_ARB_transform_feedback2 = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_transform_feedback2");
    GLAD_GL_ARB_transform_feedback3 = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_transform_feedback3");
    GLAD_GL_ARB_vertex_program = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_vertex_program");
    GLAD_GL_ARB_robustness = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_robustness");
    GLAD_GL_ARB_timer_query = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_timer_query");
    GLAD_GL_ARB_vertex_array_object = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_vertex_array_object");
    GLAD_GL_ARB_vertex_buffer_object = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_vertex_buffer_object");
    GLAD_GL_ARB_map_buffer_range = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_map_buffer_range");
    GLAD_GL_ARB_geometry_shader4 = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_geometry_shader4");
    GLAD_GL_ARB_texture_buffer_object = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_texture_buffer_object");
    GLAD_GL_EXT_timer_query = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_timer_query");
    GLAD_GL_KHR_debug = has_ext(version, exts, num_exts_i, exts_i, "GL_KHR_debug");
    GLAD_GL_ARB_get_program_binary = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_get_program_binary");
    GLAD_GL_ARB_transform_feedback_instanced = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_transform_feedback_instanced");
    GLAD_GL_SGIS_point_parameters = has_ext(version, exts, num_exts_i, exts_i, "GL_SGIS_point_parameters");
    GLAD_GL_EXT_geometry_shader4 = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_geometry_shader4");
    GLAD_GL_EXT_framebuffer_object = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_framebuffer_object");
    GLAD_GL_ARB_draw_indirect = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_draw_indirect");
    GLAD_GL_NV_bindless_multi_draw_indirect_count = has_ext(version, exts, num_exts_i, exts_i, "GL_NV_bindless_multi_draw_indirect_count");
    GLAD_GL_ARB_ES2_compatibility = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_ES2_compatibility");
    GLAD_GL_NV_command_list = has_ext(version, exts, num_exts_i, exts_i, "GL_NV_command_list");
    GLAD_GL_ARB_draw_buffers = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_draw_buffers");
    GLAD_GL_ARB_multi_draw_indirect = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_multi_draw_indirect");
    GLAD_GL_ARB_debug_output = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_debug_output");
    GLAD_GL_AMD_draw_buffers_blend = has_ext(version, exts, num_exts_i, exts_i, "GL_AMD_draw_buffers_blend");
    GLAD_GL_OES_single_precision = has_ext(version, exts, num_exts_i, exts_i, "GL_OES_single_precision");
    GLAD_GL_ARB_texture_buffer_range = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_texture_buffer_range");
    GLAD_GL_NV_explicit_multisample = has_ext(version, exts, num_exts_i, exts_i, "GL_NV_explicit_multisample");
    GLAD_GL_EXT_texture_array = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_texture_array");
    GLAD_GL_EXT_point_parameters = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_point_parameters");
    GLAD_GL_APPLE_vertex_array_object = has_ext(version, exts, num_exts_i, exts_i, "GL_APPLE_vertex_array_object");
    GLAD_GL_ARB_vertex_type_2_10_10_10_rev = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_vertex_type_2_10_10_10_rev");
    GLAD_GL_ARB_vertex_shader = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_vertex_shader");
    GLAD_GL_APPLE_flush_buffer_range = has_ext(version, exts, num_exts_i, exts_i, "GL_APPLE_flush_buffer_range");
    GLAD_GL_ARB_draw_instanced = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_draw_instanced");
    GLAD_GL_ARB_shader_objects = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_shader_objects");
    GLAD_GL_ARB_tessellation_shader = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_tessellation_shader");
    GLAD_GL_EXT_draw_buffers2 = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_draw_buffers2");
    GLAD_GL_ARB_vertex_attrib_64bit = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_vertex_attrib_64bit");
    GLAD_GL_ARB_imaging = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_imaging");
    GLAD_GL_INGR_blend_func_separate = has_ext(version, exts, num_exts_i, exts_i, "GL_INGR_blend_func_separate");
    GLAD_GL_NV_path_rendering = has_ext(version, exts, num_exts_i, exts_i, "GL_NV_path_rendering");
    GLAD_GL_ARB_framebuffer_object = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_framebuffer_object");
    GLAD_GL_ARB_draw_buffers_blend = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_draw_buffers_blend");
    GLAD_GL_ARB_direct_state_access = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_direct_state_access");
    GLAD_GL_ARB_gpu_shader_int64 = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_gpu_shader_int64");
    GLAD_GL_ARB_texture_storage = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_texture_storage");
    GLAD_GL_NV_vertex_program = has_ext(version, exts, num_exts_i, exts_i, "GL_NV_vertex_program");
    GLAD_GL_ARB_color_buffer_float = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_color_buffer_float");
    GLAD_GL_ARB_clear_buffer_object = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_clear_buffer_object");
    GLAD_GL_ARB_multisample = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_multisample");
    GLAD_GL_NV_conditional_render = has_ext(version, exts, num_exts_i, exts_i, "GL_NV_conditional_render");
    GLAD_GL_EXT_texture_filter_anisotropic = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_texture_filter_anisotropic");
    GLAD_GL_ARB_shader_storage_buffer_object = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_shader_storage_buffer_object");
    GLAD_GL_ARB_sample_shading = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_sample_shading");
    GLAD_GL_EXT_blend_minmax = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_blend_minmax");
    GLAD_GL_EXT_framebuffer_blit = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_framebuffer_blit");
    GLAD_GL_ARB_gpu_shader_fp64 = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_gpu_shader_fp64");
    GLAD_GL_NV_vertex_program4 = has_ext(version, exts, num_exts_i, exts_i, "GL_NV_vertex_program4");
    GLAD_GL_ARB_base_instance = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_base_instance");
    GLAD_GL_ARB_compute_shader = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_compute_shader");
    GLAD_GL_ARB_blend_func_extended = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_blend_func_extended");
    GLAD_GL_ARB_sync = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_sync");
    GLAD_GL_EXT_subtexture = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_subtexture");
    GLAD_GL_ARB_ES3_1_compatibility = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_ES3_1_compatibility");
    GLAD_GL_ARB_bindless_texture = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_bindless_texture");
    GLAD_GL_ARB_internalformat_query = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_internalformat_query");
    GLAD_GL_EXT_texture_integer = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_texture_integer");
    GLAD_GL_ARB_compute_variable_group_size = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_compute_variable_group_size");
    GLAD_GL_ARB_texture_multisample = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_texture_multisample");
    GLAD_GL_AMD_gpu_shader_int64 = has_ext(version, exts, num_exts_i, exts_i, "GL_AMD_gpu_shader_int64");
    GLAD_GL_ARB_invalidate_subdata = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_invalidate_subdata");
    GLAD_GL_EXT_framebuffer_multisample = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_framebuffer_multisample");
    GLAD_GL_EXT_shader_image_load_store = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_shader_image_load_store");
    GLAD_GL_EXT_texture3D = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_texture3D");
    GLAD_GL_ARB_multitexture = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_multitexture");
    GLAD_GL_EXT_draw_instanced = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_draw_instanced");
    GLAD_GL_ARB_draw_elements_base_vertex = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_draw_elements_base_vertex");
    GLAD_GL_ARB_viewport_array = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_viewport_array");
    GLAD_GL_ARB_separate_shader_objects = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_separate_shader_objects");
    GLAD_GL_ATI_draw_buffers = has_ext(version, exts, num_exts_i, exts_i, "GL_ATI_draw_buffers");
    GLAD_GL_NV_transform_feedback = has_ext(version, exts, num_exts_i, exts_i, "GL_NV_transform_feedback");
    GLAD_GL_NVX_gpu_memory_info = has_ext(version, exts, num_exts_i, exts_i, "GL_NVX_gpu_memory_info");
    GLAD_GL_ARB_sampler_objects = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_sampler_objects");
    GLAD_GL_EXT_copy_texture = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_copy_texture");
    GLAD_GL_EXT_draw_range_elements = has_ext(version, exts, num_exts_i, exts_i, "GL_EXT_draw_range_elements");
    GLAD_GL_ARB_uniform_buffer_object = has_ext(version, exts, num_exts_i, exts_i, "GL_ARB_uniform_buffer_object");

    free_exts(exts_i);
    return 1;
}

static int find_coreGL() {
    /* Thank you @elmindreda
     * https://github.com/elmindreda/greg/blob/master/templates/greg.c.in#L176
     * https://github.com/glfw/glfw/blob/master/src/context.c#L36
     */
    int i, major, minor;
    const char* version;
    const char* prefixes[] = {
        "OpenGL ES-CM ",
        "OpenGL ES-CL ",
        "OpenGL ES ",
        NULL
    };
    version = (const char*) glGetString(GL_VERSION);
    if (!version) return 0;
    for (i = 0;  prefixes[i];  i++) {
        const size_t length = strlen(prefixes[i]);
        if (strncmp(version, prefixes[i], length) == 0) {
            version += length;
            break;
        }
    }
/* PR #18 */
#ifdef _MSC_VER
    sscanf_s(version, "%d.%d", &major, &minor);
#else
    sscanf(version, "%d.%d", &major, &minor);
#endif

    GLAD_GL_VERSION_1_0 = (major == 1 && minor >= 0) || major > 1;
    GLAD_GL_VERSION_1_1 = (major == 1 && minor >= 1) || major > 1;
    GLAD_GL_VERSION_1_2 = (major == 1 && minor >= 2) || major > 1;
    GLAD_GL_VERSION_1_3 = (major == 1 && minor >= 3) || major > 1;
    GLAD_GL_VERSION_1_4 = (major == 1 && minor >= 4) || major > 1;
    GLAD_GL_VERSION_1_5 = (major == 1 && minor >= 5) || major > 1;
    GLAD_GL_VERSION_2_0 = (major == 2 && minor >= 0) || major > 2;
    GLAD_GL_VERSION_2_1 = (major == 2 && minor >= 1) || major > 2;
    GLAD_GL_VERSION_3_0 = (major == 3 && minor >= 0) || major > 3;
    GLAD_GL_VERSION_3_1 = (major == 3 && minor >= 1) || major > 3;
    GLAD_GL_VERSION_3_2 = (major == 3 && minor >= 2) || major > 3;
    GLAD_GL_VERSION_3_3 = (major == 3 && minor >= 3) || major > 3;
    GLAD_GL_VERSION_4_0 = (major == 4 && minor >= 0) || major > 4;
    GLAD_GL_VERSION_4_1 = (major == 4 && minor >= 1) || major > 4;
    GLAD_GL_VERSION_4_2 = (major == 4 && minor >= 2) || major > 4;
    GLAD_GL_VERSION_4_3 = (major == 4 && minor >= 3) || major > 4;
    GLAD_GL_VERSION_4_4 = (major == 4 && minor >= 4) || major > 4;
    GLAD_GL_VERSION_4_5 = (major == 4 && minor >= 5) || major > 4;

    return major * 10 + minor;
}

int gladLoadGL( GLADloadproc load, void* userptr) {
    int version;
    glGetString = (PFNGLGETSTRINGPROC)load("glGetString", userptr);
    if(glGetString == NULL) return 0;
    if(glGetString(GL_VERSION) == NULL) return 0;
    version = find_coreGL();

    load_GL_VERSION_1_0(load, userptr);
    load_GL_VERSION_1_1(load, userptr);
    load_GL_VERSION_1_2(load, userptr);
    load_GL_VERSION_1_3(load, userptr);
    load_GL_VERSION_1_4(load, userptr);
    load_GL_VERSION_1_5(load, userptr);
    load_GL_VERSION_2_0(load, userptr);
    load_GL_VERSION_2_1(load, userptr);
    load_GL_VERSION_3_0(load, userptr);
    load_GL_VERSION_3_1(load, userptr);
    load_GL_VERSION_3_2(load, userptr);
    load_GL_VERSION_3_3(load, userptr);
    load_GL_VERSION_4_0(load, userptr);
    load_GL_VERSION_4_1(load, userptr);
    load_GL_VERSION_4_2(load, userptr);
    load_GL_VERSION_4_3(load, userptr);
    load_GL_VERSION_4_4(load, userptr);
    load_GL_VERSION_4_5(load, userptr);

    if (!find_extensionsGL(version)) return 0;
    load_GL_NV_transform_feedback2(load, userptr);
    load_GL_ARB_texture_compression(load, userptr);
    load_GL_NV_point_sprite(load, userptr);
    load_GL_ARB_get_texture_sub_image(load, userptr);
    load_GL_AMD_multi_draw_indirect(load, userptr);
    load_GL_EXT_blend_color(load, userptr);
    load_GL_ARB_texture_storage_multisample(load, userptr);
    load_GL_ARB_clip_control(load, userptr);
    load_GL_ARB_instanced_arrays(load, userptr);
    load_GL_EXT_vertex_attrib_64bit(load, userptr);
    load_GL_ARB_vertex_attrib_binding(load, userptr);
    load_GL_ARB_framebuffer_no_attachments(load, userptr);
    load_GL_ARB_texture_view(load, userptr);
    load_GL_ATI_separate_stencil(load, userptr);
    load_GL_EXT_texture_buffer_object(load, userptr);
    load_GL_NV_shader_buffer_store(load, userptr);
    load_GL_ARB_shader_subroutine(load, userptr);
    load_GL_EXT_direct_state_access(load, userptr);
    load_GL_ARB_program_interface_query(load, userptr);
    load_GL_ARB_copy_image(load, userptr);
    load_GL_EXT_texture_object(load, userptr);
    load_GL_NVX_conditional_render(load, userptr);
    load_GL_ARB_copy_buffer(load, userptr);
    load_GL_EXT_blend_func_separate(load, userptr);
    load_GL_ARB_buffer_storage(load, userptr);
    load_GL_ARB_occlusion_query(load, userptr);
    load_GL_ARB_multi_bind(load, userptr);
    load_GL_KHR_robustness(load, userptr);
    load_GL_ARB_provoking_vertex(load, userptr);
    load_GL_EXT_gpu_shader4(load, userptr);
    load_GL_NV_geometry_program4(load, userptr);
    load_GL_NV_path_rendering_shared_edge(load, userptr);
    load_GL_NV_uniform_buffer_unified_memory(load, userptr);
    load_GL_ARB_internalformat_query2(load, userptr);
    load_GL_NV_shader_buffer_load(load, userptr);
    load_GL_EXT_vertex_array(load, userptr);
    load_GL_NV_vertex_buffer_unified_memory(load, userptr);
    load_GL_EXT_blend_equation_separate(load, userptr);
    load_GL_EXT_provoking_vertex(load, userptr);
    load_GL_ARB_point_parameters(load, userptr);
    load_GL_ARB_clear_texture(load, userptr);
    load_GL_ARB_shader_image_load_store(load, userptr);
    load_GL_EXT_multi_draw_arrays(load, userptr);
    load_GL_ARB_shader_atomic_counters(load, userptr);
    load_GL_ARB_texture_barrier(load, userptr);
    load_GL_NV_bindless_multi_draw_indirect(load, userptr);
    load_GL_EXT_transform_feedback(load, userptr);
    load_GL_ARB_transform_feedback2(load, userptr);
    load_GL_ARB_transform_feedback3(load, userptr);
    load_GL_ARB_vertex_program(load, userptr);
    load_GL_ARB_robustness(load, userptr);
    load_GL_ARB_timer_query(load, userptr);
    load_GL_ARB_vertex_array_object(load, userptr);
    load_GL_ARB_vertex_buffer_object(load, userptr);
    load_GL_ARB_map_buffer_range(load, userptr);
    load_GL_ARB_geometry_shader4(load, userptr);
    load_GL_ARB_texture_buffer_object(load, userptr);
    load_GL_EXT_timer_query(load, userptr);
    load_GL_KHR_debug(load, userptr);
    load_GL_ARB_get_program_binary(load, userptr);
    load_GL_ARB_transform_feedback_instanced(load, userptr);
    load_GL_SGIS_point_parameters(load, userptr);
    load_GL_EXT_geometry_shader4(load, userptr);
    load_GL_EXT_framebuffer_object(load, userptr);
    load_GL_ARB_draw_indirect(load, userptr);
    load_GL_NV_bindless_multi_draw_indirect_count(load, userptr);
    load_GL_ARB_ES2_compatibility(load, userptr);
    load_GL_NV_command_list(load, userptr);
    load_GL_ARB_draw_buffers(load, userptr);
    load_GL_ARB_multi_draw_indirect(load, userptr);
    load_GL_ARB_debug_output(load, userptr);
    load_GL_AMD_draw_buffers_blend(load, userptr);
    load_GL_OES_single_precision(load, userptr);
    load_GL_ARB_texture_buffer_range(load, userptr);
    load_GL_NV_explicit_multisample(load, userptr);
    load_GL_EXT_texture_array(load, userptr);
    load_GL_EXT_point_parameters(load, userptr);
    load_GL_APPLE_vertex_array_object(load, userptr);
    load_GL_ARB_vertex_type_2_10_10_10_rev(load, userptr);
    load_GL_ARB_vertex_shader(load, userptr);
    load_GL_APPLE_flush_buffer_range(load, userptr);
    load_GL_ARB_draw_instanced(load, userptr);
    load_GL_ARB_shader_objects(load, userptr);
    load_GL_ARB_tessellation_shader(load, userptr);
    load_GL_EXT_draw_buffers2(load, userptr);
    load_GL_ARB_vertex_attrib_64bit(load, userptr);
    load_GL_ARB_imaging(load, userptr);
    load_GL_INGR_blend_func_separate(load, userptr);
    load_GL_NV_path_rendering(load, userptr);
    load_GL_ARB_framebuffer_object(load, userptr);
    load_GL_ARB_draw_buffers_blend(load, userptr);
    load_GL_ARB_direct_state_access(load, userptr);
    load_GL_ARB_gpu_shader_int64(load, userptr);
    load_GL_ARB_texture_storage(load, userptr);
    load_GL_NV_vertex_program(load, userptr);
    load_GL_ARB_color_buffer_float(load, userptr);
    load_GL_ARB_clear_buffer_object(load, userptr);
    load_GL_ARB_multisample(load, userptr);
    load_GL_NV_conditional_render(load, userptr);
    load_GL_EXT_texture_filter_anisotropic(load, userptr);
    load_GL_ARB_shader_storage_buffer_object(load, userptr);
    load_GL_ARB_sample_shading(load, userptr);
    load_GL_EXT_blend_minmax(load, userptr);
    load_GL_EXT_framebuffer_blit(load, userptr);
    load_GL_ARB_gpu_shader_fp64(load, userptr);
    load_GL_NV_vertex_program4(load, userptr);
    load_GL_ARB_base_instance(load, userptr);
    load_GL_ARB_compute_shader(load, userptr);
    load_GL_ARB_blend_func_extended(load, userptr);
    load_GL_ARB_sync(load, userptr);
    load_GL_EXT_subtexture(load, userptr);
    load_GL_ARB_ES3_1_compatibility(load, userptr);
    load_GL_ARB_bindless_texture(load, userptr);
    load_GL_ARB_internalformat_query(load, userptr);
    load_GL_EXT_texture_integer(load, userptr);
    load_GL_ARB_compute_variable_group_size(load, userptr);
    load_GL_ARB_texture_multisample(load, userptr);
    load_GL_AMD_gpu_shader_int64(load, userptr);
    load_GL_ARB_invalidate_subdata(load, userptr);
    load_GL_EXT_framebuffer_multisample(load, userptr);
    load_GL_EXT_shader_image_load_store(load, userptr);
    load_GL_EXT_texture3D(load, userptr);
    load_GL_ARB_multitexture(load, userptr);
    load_GL_EXT_draw_instanced(load, userptr);
    load_GL_ARB_draw_elements_base_vertex(load, userptr);
    load_GL_ARB_viewport_array(load, userptr);
    load_GL_ARB_separate_shader_objects(load, userptr);
    load_GL_ATI_draw_buffers(load, userptr);
    load_GL_NV_transform_feedback(load, userptr);
    load_GL_NVX_gpu_memory_info(load, userptr);
    load_GL_ARB_sampler_objects(load, userptr);
    load_GL_EXT_copy_texture(load, userptr);
    load_GL_EXT_draw_range_elements(load, userptr);
    load_GL_ARB_uniform_buffer_object(load, userptr);


    resolve_aliases();

    return version;
}

static void* glad_gl_get_proc_from_userptr(const char* name, void *userptr) {
    return ((void* (*)(const char *name))userptr)(name);
}

int gladLoadGLSimple( GLADsimpleloadproc load) {
    return gladLoadGL( glad_gl_get_proc_from_userptr, (void*) load);
}



#ifdef GLAD_GL

#ifndef __glad_loader_library_c_
#define __glad_loader_library_c_

#include <stddef.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif


static void* glad_get_dlopen_handle(const char *lib_names[], int length) {
    int i;
    void *handle;

    for (i = 0; i < length; ++i) {
#ifdef _WIN32
        handle = LoadLibraryA(lib_names[i]);
#else
        handle = dlopen(lib_names[i], RTLD_LAZY | RTLD_GLOBAL);
#endif
        if (handle != NULL) {
            return handle;
        }
    }

    return NULL;
}

static void glad_close_dlopen_handle(void* handle) {
    if (handle != NULL) {
#ifdef _WIN32
        FreeLibrary((HMODULE) handle);
#else
        dlclose(handle);
#endif
    }
}

static void* glad_dlsym_handle(void* handle, const char *name) {
    if (handle == NULL) {
        return NULL;
    }

#ifdef _WIN32
    return (void*) GetProcAddress((HMODULE) handle, name);
#else
    return dlsym(handle, name);
#endif
}

#endif /* __glad_loader_library_c_ */
typedef void* (APIENTRYP GLAD_PFNGETPROCADDRESSPROC_PRIVATE)(const char*);
struct _glad_gl_userptr {
    void *gl_handle;
    GLAD_PFNGETPROCADDRESSPROC_PRIVATE gl_get_proc_address_ptr;
};

static void* glad_gl_get_proc(const char *name, void *vuserptr) {
    struct _glad_gl_userptr userptr = *(struct _glad_gl_userptr*) vuserptr;
    void* result = NULL;

#ifndef __APPLE__
    if(userptr.gl_get_proc_address_ptr != NULL) {
        result = userptr.gl_get_proc_address_ptr(name);
    }
#endif
    if(result == NULL) {
        result = glad_dlsym_handle(userptr.gl_handle, name);
    }

    return result;
}

int gladLoadGLInternalLoader() {
#ifdef __APPLE__
    static const char *NAMES[] = {
        "../Frameworks/OpenGL.framework/OpenGL",
        "/Library/Frameworks/OpenGL.framework/OpenGL",
        "/System/Library/Frameworks/OpenGL.framework/OpenGL",
        "/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL"
    };
#elif defined _WIN32
    static const char *NAMES[] = {"opengl32.dll"};
#else
    static const char *NAMES[] = {
#if defined __CYGWIN__
        "libGL-1.so",
#endif
        "libGL.so.1",
        "libGL.so"
    };
#endif

    int version = 0;
    void *handle;
    struct _glad_gl_userptr userptr;

    handle = glad_get_dlopen_handle(NAMES, sizeof(NAMES) / sizeof(NAMES[0]));
    if (handle) {
        userptr.gl_handle = handle;
#ifdef __APPLE__
        userptr.gl_get_proc_address_ptr = NULL;
#elif defined _WIN32
        userptr.gl_get_proc_address_ptr =
            (GLAD_PFNGETPROCADDRESSPROC_PRIVATE)glad_dlsym_handle(handle, "wglGetProcAddress");
#else
        userptr.gl_get_proc_address_ptr =
            (GLAD_PFNGETPROCADDRESSPROC_PRIVATE) glad_dlsym_handle(handle, "glXGetProcAddressARB");
#endif
        version = gladLoadGL( (GLADloadproc) glad_gl_get_proc, &userptr);

        glad_close_dlopen_handle(handle);
    }

    return version;
}

#endif /* GLAD_GL */
#endif /* GLAD_GL_IMPLEMENTATION */
