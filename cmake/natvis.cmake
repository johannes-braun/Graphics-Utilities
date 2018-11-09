function(target_add_natvis target access)
	list(REMOVE_AT ARGV 0 ${ARGV})
	list(REMOVE_AT ARGV 0 ${ARGV})

	if(ARGV)
		target_sources(${target} ${access} ${ARGV}) # Natvis

		# Needed for Ninja :(
		unset(gfx_current_link_flags)
		foreach(natvis ${ARGV})
			if(gfx_current_link_flags)
				set(gfx_current_link_flags "${gfx_current_link_flags} -natvis:${natvis}")
			else()
				set(gfx_current_link_flags "-natvis:${natvis}")
			endif()
		endforeach()
		target_link_libraries(${target} ${access} "${gfx_current_link_flags}")
	endif()
endfunction()