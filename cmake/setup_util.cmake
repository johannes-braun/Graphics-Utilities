set(LAST_CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})

macro(compile_submodule lib_name)
    file(REMOVE_RECURSE ${GFX_UTILITIES_FOLDER}/ext/${lib_name})
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/submodules/${lib_name}/build)
    execute_process(COMMAND ${CMAKE_COMMAND} ${GFX_LIB_DEFAULTS} ${GFX_LIB_ARGS} -DCMAKE_INSTALL_PREFIX:PATH=${GFX_UTILITIES_FOLDER}/ext/${lib_name}
        ${PROJECT_SOURCE_DIR}/submodules/${lib_name} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/submodules/${lib_name}/build)
    execute_process(COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/submodules/${lib_name}/build --target install WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/submodules/${lib_name}/build)
endmacro(compile_submodule)

macro(compile_all)
    compile_submodule(glm)
    compile_submodule(pugixml)
    compile_submodule(glshader)

    set(GFX_LIB_ARGS -DCMAKE_BUILD_TYPE=Release -DMYGL_SETTINGS_PATH="\"${GFX_GLOBAL_FOLDER}/src/mygl_settings.xml\"")
    compile_submodule(mygl)
    set(GFX_LIB_ARGS -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_EXAMPLES=OFF)
    compile_submodule(glfw3)
    set(GFX_LIB_ARGS -DALSOFT_UTILS=OFF -DALSOFT_EXAMPLES=OFF -DALSOFT_TESTS=OFF)
    compile_submodule(openal-soft)
    set(GFX_LIB_ARGS -DCMAKE_CXX_FLAGS=${ASSIMP_CXX_FLAGS} -DASSIMP_BUILD_ASSIMP_TOOLS=OFF -DASSIMP_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release)
    compile_submodule(assimp)
endmacro(compile_all)

set(GFX_LIB_DEFAULTS -DCMAKE_VERBOSE_MAKEFILE=OFF -DCMAKE_INSTALL_MESSAGE=NEVER -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} 
    -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> -DCMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM} -DCMAKE_GENERATOR=${CMAKE_GENERATOR})
unset(GFX_LIB_ARGS)

compile_all()

set(CMAKE_BUILD_TYPE ${LAST_CMAKE_BUILD_TYPE})