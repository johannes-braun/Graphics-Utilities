#[[

Dependendencies:

gfx.core depends on:
	<none>
	
gfx.shaders depends on:
	<none>

gfx.math depends on:
	glm::glm
	eigen::eigen
	microsoft::gsl
	gfx.core

gfx.ecs depends on:
	glm::glm
	gfx.core

gfx.ecs.defaults depends on:
	glfw::glfw
	gfx.core
	gfx.math
	gfx.legacy
	gfx.ecs

gfx.file depends on:
	glm::glm
	assimp::assimp
	gfx.math
	gfx.core

gfx.legacy.executable depends on:
	gfx.legacy
	gfx.ecs
	gfx.ecs.defaults

gfx.legacy depends on:
	glfw::glfw
	jbraun::mygl
	jbraun::glsp
	opengl32
	vulkan::vulkan
	microsoft::gsl
	gfx.core
	gfx.math
	gfx.file
	gfx.shaders

]]

add_subdirectory(gfx.core)
add_subdirectory(gfx.math)
add_subdirectory(gfx.file)
add_subdirectory(gfx.ecs)
add_subdirectory(gfx.legacy)
add_subdirectory(gfx.legacy.executable)
add_subdirectory(gfx.ecs.defaults)
add_subdirectory(gfx.shaders)
add_subdirectory(gfx.graphics)
add_subdirectory(gfx.ecs.defaults2)