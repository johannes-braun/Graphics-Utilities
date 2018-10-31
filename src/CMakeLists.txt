add_subdirectory(gfx.core)
add_subdirectory(gfx.math)
add_subdirectory(gfx.file)
add_subdirectory(gfx.ecs)
add_subdirectory(gfx.legacy)
add_subdirectory(gfx.ecs.defaults)

add_subdirectory(gfx.shaders)

#add_subdirectory(shaders)

#add_library(gfx.shader.test STATIC test.frag test.vert)
#set_target_properties(gfx.shader.test PROPERTIES LINKER_LANGUAGE SPV)

#[[
set(gfx_dir ${var_source_dir}/gfx)

file(GLOB_RECURSE gfx_sources *.cpp *.c *.h *.hpp )
add_library(gfx ${gfx_sources})
target_include_directories(gfx 
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_compile_features(gfx PUBLIC cxx_std_17)
target_link_libraries(gfx
	PUBLIC 
		glm::glm 
		jbraun::mygl 
		jbraun::glsp 
		glfw::glfw 
		assimp::assimp 
		eigen::eigen 
		vulkan::vulkan
		soloud::soloud
		zeux::pugixml)
target_compile_options(gfx PRIVATE ${compile_options})

if(WIN32)
    target_link_libraries(gfx PUBLIC opengl32) 
	if(MSVC)
		set_target_properties(gfx PROPERTIES LINK_FLAGS "/NATVIS")
	endif()
endif()

target_compile_definitions(gfx 
    PUBLIC 
        GLFW_INCLUDE_NONE
        GLM_FORCE_RADIANS
        GLM_ENABLE_EXPERIMENTAL
		GLM_FORCE_PURE)

if(MSVC)
    target_compile_definitions(gfx 
        PUBLIC 
            STBI_MSC_SECURE_CRT
            _SCL_SECURE_NO_WARNINGS
            _WIN32_WINNT=0x0501
            NOMINMAX)
endif()

target_compile_definitions(gfx
	PRIVATE
		$<$<CONFIG:debug>:GFX_DEBUG=1>
		$<$<CONFIG:relwithdebinfo>:GFX_RELWIDTHDEBINFO=1>
		$<$<CONFIG:release>:GFX_RELEASE=1>)

add_library(jbraun::gfx ALIAS gfx)

if(GFX_USE_INSTALL)
	install(TARGETS gfx EXPORT gfxTargets
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		RUNTIME DESTINATION bin
	)
	function(register_public_headers ROOT)
		list(REMOVE_AT ARGV 0)
		foreach(file IN LISTS ARGV)
			list(APPEND HEADERS ${ROOT}/${file})
		endforeach()
		install(FILES ${HEADERS} DESTINATION include/${ROOT})
	endfunction()

	register_public_headers(gfx 
		file.hpp
		flags.hpp
		flags.inl
		geometry.hpp
		log.hpp
		spline.hpp
		input.hpp
		context.hpp
		callbacks.hpp
		imgui.hpp)

	register_public_headers(gfx/imgui
		imconfig.h
		imgui.h)

	register_public_headers(gfx/input
		button_press.inl)
	register_public_headers(gfx/interface
		imgui_handler.hpp)

	register_public_headers(gfx/graphics
		device_buffer.hpp
		device_buffer.inl
		host_buffer.hpp
		host_buffer.inl
		device_image.hpp
		host_image.hpp
		host_image.inl
		swapchain.hpp
		sampler.hpp
		image_view.hpp
		vertex_input.hpp)

	register_public_headers(gfx/data
		bvh.hpp
		gpu_data.hpp
		grid_line_space.hpp
		line_space.hpp)
    
	register_public_headers(gfx/data/impl
		bvh.inl)

	register_public_headers(gfx/geometry
		bounds.inl
		projection.inl
		transform.inl
		vertex.inl)
    
	register_public_headers(opengl
		buffer.hpp
		command_list.hpp
		context.hpp
		enums.hpp
		framebuffer.hpp
		opengl.hpp
		pipeline.hpp
		query.hpp
		shader.hpp
		state.hpp
		texture.hpp
		uniform.hpp
		vertex_array.hpp)
    
	register_public_headers(opengl/impl
		bounded_buffer_data.inl
		bounded_buffer_iterator.inl
		buffer.inl
		command_list.inl
		framebuffer.inl
		pipeline.inl
		shader.inl
		uniform.inl
		vertex_array.inl)
endif()

configure_file(gfx/file/file_config.hpp.in gfx/file/file_config.hpp @ONLY)
set(PUBLIC_HEADERS_FILE
    ${CMAKE_CURRENT_BINARY_DIR}/gfx/file/file_config.hpp)

if(GFX_USE_INSTALL)
	install(FILES ${PUBLIC_HEADERS_FILE} DESTINATION include/gfx/file)
endif()
configure_file(gfx/graphics/shader_include.hpp.in gfx/graphics/shader_include.hpp @ONLY)

add_subdirectory(shaders)
add_dependencies(gfx shaders_gfx)

if(GFX_USE_INSTALL)
	install(DIRECTORY shaders DESTINATION . 
		FILES_MATCHING
			PATTERN *
			PATTERN *.txt EXCLUDE
			PATTERN *.bat EXCLUDE
			PATTERN .git* EXCLUDE)
	
	install(EXPORT gfxTargets
		FILE gfxTargets.cmake
		NAMESPACE jbraun::
		DESTINATION lib/cmake/gfx
	)
endif()
]]