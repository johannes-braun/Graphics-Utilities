layout(location=0) in vec2 uv;

uniform sampler2D src_textures[];
#define this_texture (src_textures[0])

layout(location=0) out vec4 color;

const vec3 colors[10] = vec3[](
    vec3(1.f, 0.3f, 0.1f),
    vec3(0.95f, 0.5f, 0.22f),
    vec3(0.8f, 0.6f, 0.4f),
    vec3(0.7f, 0.65f, 0.6f),
    vec3(0.3f, 0.7f, 0.6f),
    vec3(0.4f, 0.9f, 0.5f),
    vec3(0.3f, 0.8f, 0.7f),
    vec3(0.25f, 0.65f, 0.85f),
    vec3(0.2f, 0.55f, 0.9f),
    vec3(0.12f, 0.4f, 1.f)
);

void main()
{
    vec4 main_color = texture(this_texture, uv);

    color = main_color;

    const float centered_ness = 0.95f;

    int idx1 = 0;
    int idx2 = 0;
    for(float scale = 1.75f; scale > 0.f; scale -= 0.4f)
    {
        if(scale < 1.f){
            vec2 tmp_uv = uv - 0.5f;
            tmp_uv /= scale;
            tmp_uv += 0.5f ;
            vec2 new_uv = tmp_uv;

            if(new_uv.x < 0 || new_uv.y < 0 || new_uv.x > 1 || new_uv.y > 1)
                continue;

            vec2 vx = new_uv - vec2(0.5f);
            float factor = clamp(smoothstep(1.f-centered_ness, 0.f, dot(vx, vx)), 0, 1);

            vec4 col = max(texture(this_texture, new_uv), vec4(0));
            color += factor * col * vec4(colors[idx2++], 1);
        }

        {   
            vec2 tmp_uv = uv - 0.5f;
            tmp_uv /= -pow(scale, 3) * 2.f;
            tmp_uv += 0.5f ;

            vec2 vx = tmp_uv - vec2(0.5f);
            float factor = clamp(smoothstep(1.f-centered_ness, 0.f, dot(vx, vx)), 0, 1);

            vec2 new_uv = tmp_uv;

            if(new_uv.x < 0 || new_uv.y < 0 || new_uv.x > 1 || new_uv.y > 1)
                continue;

            vec4 col = max(texture(this_texture, new_uv), vec4(0));
            color += factor * col * vec4(colors[9-idx1], 1);
            ++idx1;
        }
    }
}
