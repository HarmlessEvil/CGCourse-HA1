//
// Created by Александр Чори on 21.10.2020.
//

#ifndef OPENGL_IMGUI_SAMPLE_SHADOW_HPP
#define OPENGL_IMGUI_SAMPLE_SHADOW_HPP

#include "render_target.hpp"

#include <memory>

#include <glm/mat4x4.hpp>

class shadow {
public:
    shadow(std::shared_ptr<render_target_t> render_target, std::shared_ptr<glm::mat4> light_space_matrix);

    [[nodiscard]] std::shared_ptr<render_target_t> render_target() const;

    std::shared_ptr<glm::mat4> light_space_matrix() const;

private:
    std::shared_ptr<render_target_t> render_target_;
    std::shared_ptr<glm::mat4> light_space_matrix_;
};


#endif //OPENGL_IMGUI_SAMPLE_SHADOW_HPP
