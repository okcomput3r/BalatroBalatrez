#version 300 es

#ifdef GL_ES
precision highp float;
#endif

in vec2 o_uv;
out vec4 fragColor;

uniform sampler2D fontTexture;
uniform vec2 uv_offset;
uniform vec2 uv_scale;

void main()
{
    vec2 final_uv = (o_uv * uv_scale) + uv_offset;
    
    vec4 letterColor = texture(fontTexture, final_uv);
    
    fragColor = letterColor;
}