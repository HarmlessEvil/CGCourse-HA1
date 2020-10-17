#version 330 core

out vec4 o_frag_color;

struct vx_output_t
{
    vec3 position_world;
};

in vx_output_t v_out;

uniform vec3 u_ambient_color;
uniform float u_ambient_intensity;

uniform vec3 u_directional_light_color;
uniform vec3 u_directional_light_direction;

void main()
{
  vec3 vec_x = dFdx(v_out.position_world);
  vec3 vec_y = dFdy(v_out.position_world);

  vec3 ambient = u_ambient_intensity * u_ambient_color;

  vec3 normal = normalize(cross(vec_x, vec_y));
  vec3 directional_light_direction = normalize(-u_directional_light_direction);
  float dot_val = 0.1 + 0.9 * clamp(dot(normal, u_directional_light_direction), 0, 1);
  vec3 diffuse = u_directional_light_color * dot_val;

  o_frag_color = vec4(ambient + diffuse, 1);
}
