//! #version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

layout(location=0) in vec3 position[3];
layout(location=1) in vec2 uv[3];
layout(location=2) in vec3 normal[3];
layout(location=3) flat in uint in_draw_id[3];

in gl_PerVertex
{
  vec4 gl_Position;
} gl_in[];

out gl_PerVertex
{
	vec4 gl_Position;
	int gl_Layer;
};

layout(binding = 3) restrict readonly buffer Info
{
	mat4 view;
};

layout(binding = 2) uniform Layer
{
	int layer;
};

mat4 cubemapViewMat(vec3 lookDir,vec3 up){
    vec3 sideDir = normalize(cross(lookDir,up));
    return mat4(transpose(mat3(sideDir,up,-lookDir)));
}

const mat4 cubemapViewMatrices[6] = {
    cubemapViewMat(vec3(1,0,0),vec3(0,-1,0)),
    cubemapViewMat(vec3(-1,0,0),vec3(0,-1,0)),
    cubemapViewMat(vec3(0,1,0),vec3(0,0,1)),
    cubemapViewMat(vec3(0,-1,0),vec3(0,0,-1)),
    cubemapViewMat(vec3(0,0,1),vec3(0,-1,0)),
    cubemapViewMat(vec3(0,0,-1),vec3(0,-1,0)),
};

const float cnear = 0.001;
const float cfar = 1000.0;
const mat4 cubemapProjMat = mat4(
    1,	0,	0,	0,
    0,	1,  0,	0,
    0,	0,	-(cfar)/(cfar-cnear), -1,
    0,	0,	-(cfar*cnear)/(cfar-cnear),	0
);

layout(location=0) out vec3 out_position;
layout(location=1) out vec2 out_uv;
layout(location=2) out vec3 out_normal;
layout(location=3) flat out uint draw_id;

const mat4 inv_z_mat = transpose(mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 1, 0, 0, 0, 1));

void main()
{
	gl_Layer = layer;
	for(int i = 0; i < 3; i++){
		gl_Position = (inv_z_mat * cubemapProjMat) * cubemapViewMatrices[layer] * view * gl_in[i].gl_Position;
		out_position = position[i];
		out_uv = uv[i];
		out_normal = normal[i];
		draw_id = in_draw_id[i];
		EmitVertex();
	}
	EndPrimitive();

}