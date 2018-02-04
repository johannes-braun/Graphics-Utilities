layout(location = 0) out vec4 fColor;

layout(set=0, binding=0) uniform sampler2D image_texture;

layout(location = 0) in struct {
    vec4 color;
    vec2 texcoords;
} fragment_input;

void main() 
{
    fColor = fragment_input.color * textureLod(image_texture, fragment_input.texcoords.st, 0);
}