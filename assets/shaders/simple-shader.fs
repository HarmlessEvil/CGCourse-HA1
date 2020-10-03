#version 330 core

out vec4 o_frag_color;

struct vx_output_t
{
    vec3 position;
    vec3 normal;
    vec3 ambient;
    vec3 diffuse;

    vec2 texture_coord;
};
in vx_output_t v_out;

uniform vec3 u_camera_pos;
uniform vec3 u_ambient;

uniform float u_reflectivity;
uniform float u_refractive_index;
uniform float u_refraction_coefficient;

uniform sampler2D u_tex;
uniform samplerCube u_skybox;

void main()
{
    vec3 unit_normal = normalize(v_out.normal);

    vec3 I = normalize(v_out.position - u_camera_pos);
    vec3 R = reflect(I, unit_normal);
    vec3 reflection = texture(u_skybox, R).rgb;

    vec3 refraction_vector = refract(I, unit_normal, 1.0/u_refractive_index);
    vec3 refraction = texture(u_skybox, refraction_vector).rgb;
    vec3 environment = mix(reflection, refraction, u_refraction_coefficient);

    vec3 texture_color = texture(u_tex, v_out.texture_coord).rgb;
    vec3 color = texture_color * (u_ambient * v_out.ambient + v_out.diffuse);

    o_frag_color = vec4(mix(color, environment, u_reflectivity), 1.0);
}
