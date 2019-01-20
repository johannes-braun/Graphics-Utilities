set(current_executable vk_and_gl)

file(GLOB_RECURSE CPPS src/*.cpp src/*.natvis)
add_executable(${current_executable} ${CPPS})
target_include_directories(${current_executable} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${current_executable} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/shaders)

include(spv/spv_to_dll)
add_shader(${current_executable}.shaders_vk
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/vk_vs.vert
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/vk_fs.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/vk_vs_shadow.vert)
target_compile_options(${current_executable}.shaders_vk PRIVATE --target-env=vulkan)
	
add_shader(${current_executable}.shaders_gl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/gl_vs.vert
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/gl_fs.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/gl_vs_shadow.vert)
target_compile_options(${current_executable}.shaders_gl PRIVATE --target-env=opengl)

add_shader(${current_executable}.blur_shaders_vk
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/screen.vert
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/blur.frag)
target_compile_options(${current_executable}.blur_shaders_vk PRIVATE --target-env=vulkan)
target_compile_definitions(${current_executable}.blur_shaders_vk PRIVATE -Dgl_VertexID=gl_VertexIndex)

add_dependencies(${current_executable} ${current_executable}.shaders_vk ${current_executable}.shaders_gl ${current_executable}.blur_shaders_vk)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.default_app)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.core)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.math)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.ecs)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.ecs.components)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.file)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.graphics)

target_link_libraries(${current_executable} PRIVATE gfx_ext::qt5_core)
target_link_libraries(${current_executable} PRIVATE gfx_ext::qt5_widgets)
target_link_libraries(${current_executable} PRIVATE gfx_ext::qt5_gui)
target_link_libraries(${current_executable} PRIVATE gfx_ext::qt5_concurrent)
target_link_libraries(${current_executable} PRIVATE gfx_ext::qt5_charts)
target_link_libraries(${current_executable} PRIVATE gfx_ext::glfw)
target_link_libraries(${current_executable} PRIVATE gfx_ext::mygl)

target_compile_definitions(${current_executable} PRIVATE QT_NO_OPENGL)
find_package(OpenMP REQUIRED)
target_link_libraries(${current_executable} PRIVATE OpenMP::OpenMP_CXX)
