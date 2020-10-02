#version 330 core

out vec4 o_frag_color;

struct vx_output_t
{
    vec3 normal;
    vec3 ambient;
    vec3 diffuse;

    vec2 texture_coord;
};

in vx_output_t v_out;

uniform vec3 u_ambient;
uniform float u_time;
uniform sampler2D u_tex;

void main()
{
    vec3 texture_color = texture(u_tex, v_out.texture_coord).rgb;

    o_frag_color = vec4(texture_color * (u_ambient * v_out.ambient + v_out.diffuse), 1.0);
}
