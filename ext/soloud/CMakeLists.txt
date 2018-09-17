add_library(soloud INTERFACE)

if(MSVC)
	target_link_libraries(soloud INTERFACE debug		${CMAKE_CURRENT_SOURCE_DIR}/lib/soloud_static_x64_d.lib)
	target_link_libraries(soloud INTERFACE optimized	${CMAKE_CURRENT_SOURCE_DIR}/lib/soloud_static_x64.lib)
endif()

target_include_directories(soloud INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_library(soloud::soloud ALIAS soloud)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION include FILES_MATCHING PATTERN *.h)
install (TARGETS soloud EXPORT soloudTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib 
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include)