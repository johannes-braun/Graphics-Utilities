#include "scene.h"

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 model_matrix;
uniform mat4 normal_matrix;

layout(binding = 0) uniform SceneBuffer { scene current_scene; };

layout(location = 0) out vec4 out_position;
layout(location = 1) out vec4 out_uv;
layout(location = 2) out vec4 out_normal; 

out gl_PerVertex
{
  vec4 gl_Position; 
};  

void main()
{ 
    out_position = model_matrix * vec4(position, 1);
    out_uv = vec4(uv, 0, 0);
    out_normal = normalize(normal_matrix * vec4(normal, 0));
    gl_Position = current_scene.view_projection * out_position;
}
 