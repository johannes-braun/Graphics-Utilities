#define COUNT 8
#define FARCLIP 1.0
#define BIAS .4f
#define RADIUS 0.04f

layout(location=0) in vec2 uv;

uniform sampler2D albedo_texture;
uniform sampler2D random_texture;
uniform sampler2DMS normal_depth_texture;
uniform float time;

layout(location=0) out vec4 color;

void main()
{
    vec2 resolution = vec2(textureSize(normal_depth_texture));
	vec2 uv = gl_FragCoord.xy / resolution;    

    vec4 norz = texelFetch(normal_depth_texture, ivec2(uv * resolution), 0);
    bool is_bg = norz.w == 1;

    vec4 col = texture(albedo_texture, uv);

    if(is_bg) {
        color = col;
        return;
    }

    float depth = norz.w;
    float radius = float(RADIUS);
    float scale = radius / depth;
    vec3 albedo = col.rgb;
    
    ivec2 rnd_res = ivec2(textureSize(random_texture, 0));

    vec3 ao = vec3(0.0);
    for(int i = 0; i < COUNT; i++)
    {
        ivec2 randUv = clamp(ivec2(gl_FragCoord.xy + 27.71 * float(i) + vec2(34232.f * sin(time), 51314.f * cos(time))) % rnd_res, 
            ivec2(1), rnd_res-1);
        vec3 randNor = texelFetch(random_texture, randUv, 0).rgb * 2.0 - 1.0;
        
        if(dot(norz.xyz, randNor) < 0.0)
            randNor *= -1.0;
        
        vec2 off = randNor.xy * scale;

        vec4 sampleNorz = texelFetch(normal_depth_texture, ivec2((uv + off) * resolution), 0);
        is_bg = sampleNorz.w == 1;
        if(is_bg) continue;

        float depthDelta = depth - sampleNorz.w;
        
        vec3 otherColor = texture(albedo_texture, (uv + off)).rgb;
        
        vec3 sampleDir = vec3(randNor.xy * radius, depthDelta);       
        float occ = max(0.0, dot(normalize(norz.xyz), normalize(sampleDir)) - BIAS) / (length(sampleDir) + 1.0);
        ao += (occ) * (vec3(1.0)-otherColor.xyz);
    }    
    
    vec3 base = vec3(col);
    color =  vec4((1.0 - 2.f*length(ao)/float(COUNT)) * vec3(base), col.a);
}