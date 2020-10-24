#version 330 core

out vec4 o_frag_color;

struct vx_output_t
{
    vec3 position_world;
    vec4 shadow_coordinates;
};

in vx_output_t v_out;

struct directional_light {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct point_light {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct spotlight {
    vec3 position;
    vec3 direction;
    float cut_off;
    float outer_cut_off;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 u_camera_position;
uniform directional_light u_sun;
uniform spotlight u_flashlight;

uniform sampler2DArrayShadow u_directional_light_shadow_map;

vec3 calculate_directional_light(directional_light light, vec3 normal, vec3 view_direction);
vec3 calculate_point_light(point_light light, vec3 normal, vec3 position, vec3 view_direction);
vec3 calculate_spotlight(spotlight light, vec3 normal, vec3 position, vec3 view_direction);

void main()
{
  vec3 vec_x = dFdx(v_out.position_world);
  vec3 vec_y = dFdy(v_out.position_world);

  vec3 normal = normalize(cross(vec_x, vec_y));
  vec3 view_direction = normalize(u_camera_position - v_out.position_world);

  vec3 color = calculate_directional_light(u_sun, normal, view_direction);
  color += calculate_spotlight(u_flashlight, normal, v_out.position_world, view_direction);

  o_frag_color = vec4(color, 1);
}

vec3 calculate_directional_light(directional_light light, vec3 normal, vec3 view_direction) {
    vec3 light_direction = normalize(-light.direction);

    float diffuse_intensity = 0.1 + 0.9 * clamp(dot(normal, light_direction), 0, 1);

    vec3 reflect_direction = reflect(-light_direction, normal);
    float specular_intensity = pow(max(dot(normal, reflect_direction), 0.0), 32);

    vec3 ambient = light.ambient;
    vec3 diffuse = light.diffuse * diffuse_intensity;
    vec3 specular = light.specular * specular_intensity;

    float shadow = texture(u_directional_light_shadow_map, v_out.shadow_coordinates);
    return ambient + shadow * (diffuse + specular);
}

vec3 calculate_point_light(point_light light, vec3 normal, vec3 position, vec3 view_direction) {
    vec3 light_direction = normalize(light.position - position);

    float diffuse_intensity = 0.1 + 0.9 * clamp(dot(normal, light_direction), 0, 1);

    vec3 reflect_direction = reflect(-light_direction, normal);
    float specular_intensity = pow(max(dot(normal, reflect_direction), 0.0), 32);

    float distance = length(light.position - position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient;
    vec3 diffuse = light.diffuse * diffuse_intensity;
    vec3 specular = light.specular * specular_intensity;

    return (ambient + diffuse + specular) * attenuation;
}

vec3 calculate_spotlight(spotlight light, vec3 normal, vec3 position, vec3 view_direction) {
    vec3 light_direction = normalize(light.position - position);

    float diffuse_intensity = 0.1 + 0.9 * clamp(dot(normal, light_direction), 0, 1);

    vec3 reflect_direction = reflect(-light_direction, normal);
    float specular_intensity = pow(max(dot(normal, reflect_direction), 0.0), 32);

    float distance = length(light.position - position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    float theta = dot(light_direction, normalize(-light.direction));
    float epsilon = light.cut_off - light.outer_cut_off;
    float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient;
    vec3 diffuse = light.diffuse * diffuse_intensity;
    vec3 specular = light.specular * specular_intensity;

    return (ambient + diffuse + specular) * attenuation * intensity;
}
