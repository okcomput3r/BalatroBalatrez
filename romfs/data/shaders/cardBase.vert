#version 300 es

    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aUV;

    uniform mat4 transform;

    uniform float u_time;
    uniform float u_maxAngle;

    out vec2 o_uv;


    void main()
    {
        gl_Position = transform * vec4(aPos, 1.0f);
        o_uv = aUV;
    }