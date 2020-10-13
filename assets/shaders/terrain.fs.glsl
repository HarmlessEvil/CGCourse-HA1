#version 330

out vec4 o_frag_color;

struct vx_output_t {
    vec3 position;
    vec3 normal;
    vec3 texture_coordinates;
};
in vx_output_t v_out;

uniform sampler2DArray u_tex;

void main() {
    vec3 texture_color = texture(u_tex, v_out.texture_coordinates).rgb;

    o_frag_color = vec4(texture_color, 1.0);
}
