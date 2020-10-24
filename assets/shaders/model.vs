#version 330 core

layout (location = 0) in vec3 in_position;

struct vx_output_t
{
    vec3 position_world;
    vec4 shadow_coordinates;
};
out vx_output_t v_out;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform mat4 u_light_space_matrices[5];

vec4 compute_shadow_coordinates() {
    int index = 0;

    // find the appropriate depth map to look up in based on the depth of this fragment
    //    if(gl_FragCoord.z < farbounds.x) {
    //        index = 0;
    //    } else if(gl_FragCoord.z < farbounds.y) {
    //        index = 1;
    //    } else if(gl_FragCoord.z < farbounds.z) {
    //        index = 2;
    //    }

    vec4 shadow_coord = u_light_space_matrices[index] * vec4(v_out.position_world, 1);
    shadow_coord.z *= 0.98;
    shadow_coord *= 0.5;
    shadow_coord += 0.5;

    // shadow_coord.w = shadow_coord.z;
    // shadow_coord.z = float(index);
    shadow_coord.w = float(index);

    return shadow_coord;
}

void main()
{
  gl_Position = u_mvp * vec4(in_position, 1.0);

  v_out.position_world = (u_model * vec4(in_position, 1.0)).xyz;
  v_out.shadow_coordinates = compute_shadow_coordinates();
}
