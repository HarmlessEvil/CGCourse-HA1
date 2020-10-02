#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_ambient;
layout (location = 3) in vec3 in_diffuse;
layout (location = 4) in vec2 in_texture_coord;

struct vx_output_t
{
    vec3 normal;
    vec3 ambient;
    vec3 diffuse;

    vec2 texture_coord;
};
out vx_output_t v_out;

uniform mat4 u_mvp;

void main()
{
    v_out.normal = in_normal;
    v_out.ambient = in_ambient;
    v_out.diffuse = in_diffuse;
    v_out.texture_coord = in_texture_coord;

    gl_Position = u_mvp * vec4(in_position, 1.0);
}
