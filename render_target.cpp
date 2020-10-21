//
// Created by Александр Чори on 21.10.2020.
//

#include "render_target.hpp"

#include <memory>

render_target_t::render_target_t(int res_x, int res_y) {
    width_ = res_x;
    height_ = res_y;

    glGenTextures(1, &depth_);
    glBindTexture(GL_TEXTURE_2D, depth_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_DEPTH_COMPONENT,
            res_x,
            res_y,
            0,
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            nullptr
    );

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &fbo_);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Framebuffer incomplete");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

render_target_t::~render_target_t() {
    glDeleteFramebuffers(1, &fbo_);
    glDeleteTextures(1, &depth_);
    glDeleteTextures(1, &color_);
}