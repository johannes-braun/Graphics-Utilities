glslc frag.frag -o frag.frag.spv-opengl --target-env=opengl
glslc vert.vert -o vert.vert.spv-opengl --target-env=opengl
glslc frag.frag -o frag.frag.spv-vulkan --target-env=vulkan
glslc vert.vert -o vert.vert.spv-vulkan --target-env=vulkan -Dgl_VertexID=gl_VertexIndex
