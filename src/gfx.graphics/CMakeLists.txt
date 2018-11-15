set(gfx_current_module graphics)
file(GLOB_RECURSE gfx_current_sources src/*.cpp src/*.c src/*.cc)
file(GLOB_RECURSE gfx_current_headers src/*.hpp src/*.h)
file(GLOB_RECURSE gfx_current_visuals src/*.natvis)

# Set sources
add_library(gfx.${gfx_current_module} STATIC)
target_sources(gfx.${gfx_current_module} PRIVATE ${gfx_current_sources}) # Sources
target_sources(gfx.${gfx_current_module} PRIVATE ${gfx_current_headers}) # Headers
include(${PROJECT_SOURCE_DIR}/cmake/natvis.cmake)
target_add_natvis(gfx.${gfx_current_module} PUBLIC ${gfx_current_visuals})

# Features
target_compile_features(gfx.${gfx_current_module} PRIVATE cxx_std_17)
target_compile_options(gfx.${gfx_current_module} PRIVATE ${compile_options})
target_include_directories(gfx.${gfx_current_module} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/gfx.${gfx_current_module}/)
target_include_directories(gfx.${gfx_current_module} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/)
target_include_directories(gfx.${gfx_current_module} PUBLIC $<INSTALL_INTERFACE:include>)
target_compile_definitions(gfx.${gfx_current_module} PUBLIC GLFW_INCLUDE_NONE)
target_compile_definitions(gfx.${gfx_current_module} PUBLIC GLM_FORCE_RADIANS)
target_compile_definitions(gfx.${gfx_current_module} PUBLIC GLM_ENABLE_EXPERIMENTAL)
target_compile_definitions(gfx.${gfx_current_module} PUBLIC GLM_FORCE_PURE)
target_compile_definitions(gfx.${gfx_current_module} PUBLIC STBI_MSC_SECURE_CRT)
target_compile_definitions(gfx.${gfx_current_module} PUBLIC _SCL_SECURE_NO_WARNINGS)
target_compile_definitions(gfx.${gfx_current_module} PUBLIC _WIN32_WINNT=0x0501)
target_compile_definitions(gfx.${gfx_current_module} PUBLIC NOMINMAX)

find_package(range-v3 REQUIRED)
target_link_libraries(gfx.${gfx_current_module} PUBLIC jbraun::gfx.core)
target_link_libraries(gfx.${gfx_current_module} PUBLIC jbraun::gfx.math)
target_link_libraries(gfx.${gfx_current_module} PUBLIC jbraun::gfx.file)
target_link_libraries(gfx.${gfx_current_module} PUBLIC jbraun::gfx.shaders)
target_link_libraries(gfx.${gfx_current_module} PUBLIC jbraun::mygl)
target_link_libraries(gfx.${gfx_current_module} PUBLIC jbraun::glsp)
target_link_libraries(gfx.${gfx_current_module} PUBLIC vulkan::vulkan)
target_link_libraries(gfx.${gfx_current_module} PUBLIC range-v3)

add_library(jbraun::gfx.${gfx_current_module} ALIAS gfx.${gfx_current_module})