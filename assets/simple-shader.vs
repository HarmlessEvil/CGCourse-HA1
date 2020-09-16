#version 330 core

layout (location = 0) in vec3 in_position;

struct vx_output_t
{
    vec3 position;
};
out vx_output_t v_out;

void main()
{
    gl_Position = vec4(in_position, 1.0);
    v_out.position = in_position;
}
