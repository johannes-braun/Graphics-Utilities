compile_spirv(frag.frag opengl)
compile_spirv(vert.vert opengl)

compile_spirv(frag.frag vulkan)
compile_spirv(vert.vert vulkan)

create_shaders_target(shaders_05_implementation_tests)