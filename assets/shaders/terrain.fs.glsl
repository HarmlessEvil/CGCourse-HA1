#version 330

out vec4 o_frag_color;

struct vx_output_t {
    vec3 position;
    vec3 normal;
};
in vx_output_t v_out;

void main() {
    o_frag_color = vec4(v_out.normal, 1.0);
}
