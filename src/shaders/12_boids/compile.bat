@echo off

for %%e in (*.frag *.vert *.tese *.tesc *.geom *.comp) do (
	for %%f in (%%e) do (
		echo %%f [GL]
		glslc %%f -o %%f.spv-opengl --target-env=opengl
		echo %%f [VK]
		glslc %%f -o %%f.spv-vulkan --target-env=vulkan -Dgl_VertexID=gl_VertexIndex
	)
)