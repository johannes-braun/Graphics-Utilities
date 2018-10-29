set(gfx_current_module core)

# Set sources
add_library(gfx.${gfx_current_module} INTERFACE)

# Features
target_compile_features(gfx.${gfx_current_module} INTERFACE cxx_std_17)
target_include_directories(gfx.${gfx_current_module} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/)
target_include_directories(gfx.${gfx_current_module} INTERFACE $<INSTALL_INTERFACE:include>)
target_compile_definitions(gfx.${gfx_current_module} INTERFACE _SCL_SECURE_NO_WARNINGS)
target_compile_definitions(gfx.${gfx_current_module} INTERFACE _WIN32_WINNT=0x0501)
target_compile_definitions(gfx.${gfx_current_module} INTERFACE NOMINMAX)

add_library(jbraun::gfx.${gfx_current_module} ALIAS gfx.${gfx_current_module})