#version 330

out vec4 o_frag_color;

struct vx_output_t {
    vec3 position;
    vec3 normal;
    vec3 texture_coordinates;
};
in vx_output_t v_out;

uniform sampler2DArray u_tex;

uniform vec3 u_ambient_color;
uniform float u_ambient_intensity;

uniform vec3 u_directional_light_color;
uniform vec3 u_directional_light_direction;

void main() {
    vec3 texture_color = texture(u_tex, v_out.texture_coordinates).rgb;

    vec3 ambient = u_ambient_intensity * u_ambient_color * texture_color;

    vec3 directional_light_direction = normalize(-u_directional_light_direction);
    float dot_val = 0.1 + 0.9 * clamp(dot(v_out.normal, u_directional_light_direction), 0, 1);
    vec3 diffuse = u_directional_light_color * dot_val * texture_color;

    o_frag_color = vec4(ambient + diffuse, 1.0);
}
