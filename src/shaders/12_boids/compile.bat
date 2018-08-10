glslc render_points.frag -o render_points.frag.spv-opengl --target-env=opengl
glslc render_points.geom -o render_points.geom.spv-opengl --target-env=opengl
glslc render_points.vert -o render_points.vert.spv-opengl --target-env=opengl
glslc render_points.frag -o render_points.frag.spv-vulkan --target-env=vulkan
glslc render_points.geom -o render_points.geom.spv-vulkan --target-env=vulkan
glslc render_points.vert -o render_points.vert.spv-vulkan --target-env=vulkan -Dgl_VertexID=gl_VertexIndex

glslc update.comp -o update.comp.spv-opengl --target-env=opengl
glslc update.comp -o update.comp.spv-vulkan --target-env=vulkan