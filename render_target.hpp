//
// Created by Александр Чори on 21.10.2020.
//

#ifndef OPENGL_IMGUI_SAMPLE_RENDER_TARGET_HPP
#define OPENGL_IMGUI_SAMPLE_RENDER_TARGET_HPP

#include <GL/glew.h>

struct render_target_t {
    render_target_t(int res_x, int res_y, GLuint depth, GLuint layer);

    render_target_t(int res_x, int res_y);

    GLuint fbo_{};
    GLuint color_{};
    GLuint depth_{};
    int width_, height_;
};

#endif //OPENGL_IMGUI_SAMPLE_RENDER_TARGET_HPP
