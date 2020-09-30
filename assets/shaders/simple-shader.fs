#version 330 core

out vec4 o_frag_color;

struct vx_output_t
{
    vec3 color;
};

in vx_output_t v_out;

uniform vec3 u_color;
uniform float u_time;
uniform sampler2D u_tex;

void main()
{
    vec3 texture = texture(u_tex, v_out.color.xy).rgb;
    //o_frag_color = vec4(v_out.color.xy,0,1.0);

    //if ((int(gl_FragCoord.x / 30) % 2 == 0) ^^ (int(gl_FragCoord.y / 30) % 2 == 0))
    //  discard;

    o_frag_color = vec4(texture,1.0);

    //gl_FragDepth = 0;
}
