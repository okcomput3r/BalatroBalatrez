#version 300 es

    #ifdef GL_ES
    precision highp float;
    #endif
    
    vec3 outColor;

    in vec2 o_uv;


    out vec4 fragColor;


    uniform sampler2D BaseTexture;
    uniform sampler2D AtlasTexture;

    uniform vec2 uv_offset;
    uniform vec2 uv_scale;

    uniform float visible;

    void main()
    {
        vec4 baseColor = texture(BaseTexture, o_uv);
        vec4 atlasColor = texture(AtlasTexture, (o_uv * uv_scale) + uv_offset);
        if (atlasColor.xyz == vec3(0,0,0))
        {
            outColor = baseColor.xyz;
        }
        else 
        {
            outColor = mix(baseColor.xyz, atlasColor.xyz, visible);
        }   
        fragColor = vec4(outColor,baseColor.a);
    }