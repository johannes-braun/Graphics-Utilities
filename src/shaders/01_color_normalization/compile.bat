glslc center_point.frag -o center_point.frag.spv-opengl --target-env=opengl
glslc center_point.vert -o center_point.vert.spv-opengl --target-env=opengl
glslc center_point.frag -o center_point.frag.spv-vulkan --target-env=vulkan
glslc center_point.vert -o center_point.vert.spv-vulkan --target-env=vulkan -Dgl_VertexID=gl_VertexIndex

glslc cube.frag -o cube.frag.spv-opengl --target-env=opengl
glslc cube.vert -o cube.vert.spv-opengl --target-env=opengl
glslc cube.frag -o cube.frag.spv-vulkan --target-env=vulkan
glslc cube.vert -o cube.vert.spv-vulkan --target-env=vulkan -Dgl_VertexID=gl_VertexIndex

glslc cube_front.frag -o cube_front.frag.spv-opengl --target-env=opengl
glslc cube_front.frag -o cube_front.frag.spv-vulkan --target-env=vulkan

glslc gizmo.frag -o gizmo.frag.spv-opengl --target-env=opengl
glslc gizmo.vert -o gizmo.vert.spv-opengl --target-env=opengl
glslc gizmo.frag -o gizmo.frag.spv-vulkan --target-env=vulkan
glslc gizmo.vert -o gizmo.vert.spv-vulkan --target-env=vulkan -Dgl_VertexID=gl_VertexIndex

glslc image.frag -o image.frag.spv-opengl --target-env=opengl
glslc image.frag -o image.frag.spv-vulkan --target-env=vulkan

glslc points.frag -o points.frag.spv-opengl --target-env=opengl
glslc points.vert -o points.vert.spv-opengl --target-env=opengl
glslc points.frag -o points.frag.spv-vulkan --target-env=vulkan
glslc points.vert -o points.vert.spv-vulkan --target-env=vulkan -Dgl_VertexID=gl_VertexIndex