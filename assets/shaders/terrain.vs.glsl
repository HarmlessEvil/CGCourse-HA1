#version 330

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_texture_coordinates;

struct vx_output_t {
    vec3 position;
    vec3 normal;
    vec3 texture_coordinates;
    vec4 shadow_coordinates;
    vec4 screen_space_position;
};
out vx_output_t v_out;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform mat4 u_light_space_matrices[3];

vec4 compute_shadow_coordinates() {
    vec4 shadow_coord;

    for (int i = 0; i < 3; i++) {
        shadow_coord = u_light_space_matrices[i] * vec4(v_out.position, 1);

        if (shadow_coord.x <= -1 || shadow_coord.x >= 1 || shadow_coord.y <= -1 || shadow_coord.y >= 1) {
            continue;
        }

        shadow_coord.z *= 0.98;
        shadow_coord *= 0.5;
        shadow_coord += 0.5;

        shadow_coord.w = shadow_coord.z;
        shadow_coord.z = float(i);
        break;
    }

    return shadow_coord;
}

void main() {
    v_out.position = (u_model * vec4(in_position, 1.0)).xyz;
    v_out.normal = in_normal;
    v_out.texture_coordinates = in_texture_coordinates;
    v_out.shadow_coordinates = compute_shadow_coordinates();

    v_out.screen_space_position = u_mvp * vec4(in_position, 1.0);
    gl_Position = v_out.screen_space_position;
}
