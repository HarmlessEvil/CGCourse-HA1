#version 330 core

out vec4 o_frag_color;

struct vx_output_t
{
    vec3 normal;
    vec3 color;

    vec2 texture_coord;
};

in vx_output_t v_out;

uniform vec3 u_color;
uniform float u_time;
uniform sampler2D u_tex;

void main()
{
    vec3 texture = texture(u_tex, v_out.texture_coord).rgb;

    o_frag_color = vec4(texture, 1.0);
}
