function(message)
    if (NOT MESSAGE_QUIET OR "${ARGV0}" MATCHES "FATAL_ERROR")
        _message(${ARGV})
    endif()
endfunction()

function (add_subdirectory_log dir)
	set(MESSAGE_QUIET ON)
		add_subdirectory(${dir})
	unset(MESSAGE_QUIET)
	gfx_msg("Add external library: ${dir}")
endfunction()

add_subdirectory_log(gsl)
add_library(microsoft::gsl ALIAS GSL)
add_subdirectory_log(mygl)
add_subdirectory_log(glshader)
add_subdirectory_log(vulkan)
add_subdirectory_log(soloud)

# Additional:
# glfw3:x64-windows glm:x64-windows eigen3:x64-windows assimp:x64-windows pugixml:x64-windows range-v3:x64-windows
