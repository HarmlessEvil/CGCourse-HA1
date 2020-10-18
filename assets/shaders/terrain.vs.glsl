#version 330

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_texture_coordinates;

struct vx_output_t {
    vec3 position;
    vec3 normal;
    vec3 texture_coordinates;
};
out vx_output_t v_out;

uniform mat4 u_mvp;
uniform mat4 u_model;

void main() {
    v_out.position = (u_model * vec4(in_position, 1.0)).xyz;
    v_out.normal = in_normal;
    v_out.texture_coordinates = in_texture_coordinates;

    gl_Position = u_mvp * vec4(in_position, 1.0);
}
