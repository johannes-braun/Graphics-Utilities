uniform mat4 cubemap_matrix;

out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) out vec3 uv;

const float z = 0.0f;

void main()
{
  gl_Position = vec4(mix(-1, 3, gl_VertexID & 0x1), mix(-1, 3, (gl_VertexID >> 1) & 0x1), z, 1.f);
  uv = (cubemap_matrix * gl_Position).xyz;
}