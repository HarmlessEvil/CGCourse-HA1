#version 330 core

layout (location = 0) in vec3 in_position;

struct vx_output_t
{
    vec3 position_world;
    vec3 position_in_directional_light_space;
};
out vx_output_t v_out;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform mat4 u_directional_light_space;

void main()
{
  gl_Position = u_mvp * vec4(in_position, 1.0);

  v_out.position_world = (u_model * vec4(in_position, 1.0)).xyz;

  vec4 position_in_directional_light_space = u_directional_light_space * vec4(v_out.position_world, 1.0);
  position_in_directional_light_space.z *= 0.98;

  v_out.position_in_directional_light_space = position_in_directional_light_space.xyz / position_in_directional_light_space.w;
  v_out.position_in_directional_light_space *= 0.5;
  v_out.position_in_directional_light_space += 0.5;
}
