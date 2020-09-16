#version 330 core

out vec4 o_frag_color;

struct vx_output_t
{
    vec3 position;
};

in vx_output_t v_out;

uniform float u_max_radius;
uniform int u_max_iterations;

uniform vec2 u_shift;
uniform float u_scale;

uniform sampler1D colormap;

void main()
{
    vec2 position = v_out.position.xy * u_scale + u_shift;

    float x = position.x;
    float y = position.y;
    float ix = 0;
    float iy = 0;
    int n = 0;

    while ((ix * ix + iy * iy < u_max_radius) && (n < u_max_iterations)) {
        ix = x * x - y * y + position.x;
        iy = 2 * x * y + position.y;

        x = ix;
        y = iy;

        n++;
    }

    o_frag_color = texture(colormap, float(n) / float(u_max_iterations) - 0.01);
}
