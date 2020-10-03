#version 330 core
layout(location = 0) in vec3 in_position;

struct vx_output_t {
    vec3 texture_coordinates;
};
out vx_output_t v_out;

uniform mat4 u_projection;
uniform mat4 u_view;

void main() {
    v_out.texture_coordinates = in_position;

    vec4 pos = u_projection * u_view * vec4(in_position, 1.0f);
    gl_Position = pos.xyww;
}
