#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texture_coord;

struct vx_output_t
{
    vec3 normal;
    vec3 color;

    vec2 texture_coord;
};
out vx_output_t v_out;

uniform float u_rotation;
uniform vec2 u_translation;
uniform mat4 u_mvp;

void main()
{
    vec2 rotated_pos = in_position.xy;
    //rotated_pos.x = u_translation.x + in_position.x * cos(u_rotation) - in_position.y * sin(u_rotation);
    //rotated_pos.y = u_translation.y + in_position.x * sin(u_rotation) + in_position.y * cos(u_rotation);

    v_out.normal = in_normal;
    v_out.color = vec3(1.0, 1.0, 1.0);

    v_out.texture_coord = in_texture_coord;

    gl_Position = u_mvp * vec4(rotated_pos.x, rotated_pos.y, in_position.z, 1.0);
}
