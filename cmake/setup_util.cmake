set(GFX_LIB_DEFAULTS -DCMAKE_VERBOSE_MAKEFILE=OFF -DCMAKE_INSTALL_MESSAGE=NEVER -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} 
    -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> -DCMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM} -DCMAKE_GENERATOR=${CMAKE_GENERATOR})
unset(GFX_LIB_ARGS)

macro(compile_submodule lib_name)
    file(REMOVE_RECURSE ${GFX_UTILITIES_FOLDER}/ext/${lib_name})
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/submodules/${lib_name}/build)
    execute_process(COMMAND ${CMAKE_COMMAND} ${GFX_LIB_DEFAULTS} ${GFX_LIB_ARGS} -DCMAKE_INSTALL_PREFIX:PATH=${GFX_UTILITIES_FOLDER}/ext/${lib_name}
        ${PROJECT_SOURCE_DIR}/submodules/${lib_name} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/submodules/${lib_name}/build)
    execute_process(COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/submodules/${lib_name}/build --target install WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/submodules/${lib_name}/build)
endmacro(compile_submodule)