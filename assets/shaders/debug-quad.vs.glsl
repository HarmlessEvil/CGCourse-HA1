#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texture_coordinates;

struct vx_output_t {
    vec2 texture_coordinates;
};
out vx_output_t v_out;

void main()
{
    v_out.texture_coordinates = in_texture_coordinates;

    gl_Position = vec4(in_position, 1);
}
