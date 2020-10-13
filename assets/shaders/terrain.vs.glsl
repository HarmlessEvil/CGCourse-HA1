#version 330

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texture_coordinates;

struct vx_output_t {
    vec3 position;
    vec3 normal;
};
out vx_output_t v_out;

uniform mat4 u_mvp;

void main() {
    v_out.position = in_position;
    v_out.normal = in_normal;

    gl_Position = u_mvp * vec4(in_position, 1.0);
}
