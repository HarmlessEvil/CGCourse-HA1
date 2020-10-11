#version 330 core

layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_st;

struct vx_output_t
{
    vec2 st;
};
out vx_output_t v_out;

uniform mat4 u_mvp;

void main()
{
    v_out.st = in_st;

    gl_Position = u_mvp * vec4(in_position.xy, 0.0, 1.0);

    //gl_Position.xyz *= 0;

    //gl_Position = u_mvp * vec4(3 * rotated_pos.x, 2 * rotated_pos.y, in_position.z, 1.0);
    //gl_Position /= gl_Position.w;
}
