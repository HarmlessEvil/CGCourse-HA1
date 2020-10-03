#version 330 core
out vec4 o_frag_color;

struct vx_output_t {
    vec3 texture_coordinates;
};

in vx_output_t v_out;

uniform samplerCube u_skybox;

void main() {
    o_frag_color = texture(u_skybox, v_out.texture_coordinates);
}
