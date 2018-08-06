compile_spirv(frag.frag opengl)
compile_spirv(vert.vert opengl)
compile_spirv(comp.comp vulkan)

compile_spirv(frag.frag vulkan)
compile_spirv(vert.vert vulkan)
compile_spirv(comp.comp vulkan)

create_shaders_target(shaders_05_implementation_tests)