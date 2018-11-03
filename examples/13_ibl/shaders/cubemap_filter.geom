#version 460 core

layout(points) in;
layout(triangle_strip, max_vertices = 3) out;

layout(location = 0) in int id[1];

in gl_PerVertex
{
  vec4 gl_Position;
} gl_in[1];

out gl_PerVertex
{
  vec4 gl_Position;
};

layout(location = 0) out vec3 direction;

void main()
{
	gl_Layer = id[0];

	gl_Position = vec4(-1, -1, 0.5f, 1.f);

#ifdef VULKAN
	if(id[0] == 0)
		direction = vec3(1, 1, 1);
	else if(id[0] == 1)
		direction = vec3(-1, 1, -1);
	else if(id[0] == 2)
		direction = vec3(-1, 1, -1);
	else if(id[0] == 3)
		direction = vec3(-1, -1, 1);
	else if(id[0] == 4)
		direction = vec3(-1, 1, 1);
	else if(id[0] == 5)
		direction = vec3(1, 1, -1);
	EmitVertex();
	
	gl_Position = vec4(3, -1, 0.5f, 1.f);
	if(id[0] == 0)
		direction = vec3(1, 1, -3);
	else if(id[0] == 1)
		direction = vec3(-1, 1, 3);
	else if(id[0] == 2)
		direction = vec3(3, 1, -1);
	else if(id[0] == 3)
		direction = vec3(3, -1, 1);
	else if(id[0] == 4)
		direction = vec3(3, 1, 1);
	else if(id[0] == 5)
		direction = vec3(-3, 1, -1);
	EmitVertex();
	
	gl_Position = vec4(-1, 3, 0.5f, 1.f);
	if(id[0] == 0)
		direction = vec3(1, -3, 1);
	else if(id[0] == 1)
		direction = vec3(-1, -3, -1);
	else if(id[0] == 2)
		direction = vec3(-1, 1, 3);
	else if(id[0] == 3)
		direction = vec3(-1, -1, -3);
	else if(id[0] == 4)
		direction = vec3(-1, -3, 1);
	else if(id[0] == 5)
		direction = vec3(1, -3, -1);
	EmitVertex();
	EndPrimitive();
#else
	if(id[0] == 0)
		direction = vec3(1, -1, 1);
	else if(id[0] == 1)
		direction = vec3(-1, -1, -1);
	else if(id[0] == 2)
		direction = vec3(-1, 1, 1);
	else if(id[0] == 3)
		direction = vec3(-1, -1, -1);
	else if(id[0] == 4)
		direction = vec3(-1, -1, 1);
	else if(id[0] == 5)
		direction = vec3(1, -1, -1);
	EmitVertex();
	
	gl_Position = vec4(3, -1, 0.5f, 1.f);
	if(id[0] == 0)
		direction = vec3(1, -1, -3);
	else if(id[0] == 1)
		direction = vec3(-1, -1, 3);
	else if(id[0] == 2)
		direction = vec3(3, 1, 1);
	else if(id[0] == 3)
		direction = vec3(3, -1, -1);
	else if(id[0] == 4)
		direction = vec3(3, -1, 1);
	else if(id[0] == 5)
		direction = vec3(-3, -1, -1);
	EmitVertex();
	
	gl_Position = vec4(-1, 3, 0.5f, 1.f);
	if(id[0] == 0)
		direction = vec3(1, 3, 1);
	else if(id[0] == 1)
		direction = vec3(-1, 3, -1);
	else if(id[0] == 2)
		direction = vec3(-1, 1, -3);
	else if(id[0] == 3)
		direction = vec3(-1, -1, 3);
	else if(id[0] == 4)
		direction = vec3(-1, 3, 1);
	else if(id[0] == 5)
		direction = vec3(1, 3, -1);
	EmitVertex();
	EndPrimitive();
#endif
}