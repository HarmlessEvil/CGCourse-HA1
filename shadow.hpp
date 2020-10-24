//
// Created by Александр Чори on 21.10.2020.
//

#ifndef OPENGL_IMGUI_SAMPLE_SHADOW_HPP
#define OPENGL_IMGUI_SAMPLE_SHADOW_HPP

#include <functional>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "render_target.hpp"

class shadow {
public:
    explicit shadow(const render_target_t &render_target);

    [[nodiscard]] const render_target_t &render_target() const;

    [[nodiscard]] virtual glm::mat4 light_space_matrix() const = 0;

    virtual ~shadow() = default;

private:
    const render_target_t render_target_;
};

class directional_light_shadow : public shadow {
public:
    directional_light_shadow(
            const render_target_t &render_target,
            const glm::vec3 &direction,
            const glm::mat4 &light_projection,
            float distance,
            std::function<glm::vec3()> fetch_target_position
    );

    [[nodiscard]] glm::mat4 light_space_matrix() const override;

private:
    const glm::vec3 direction_;
    const glm::mat4 light_projection_;
    const float distance_;
    std::function<glm::vec3()> fetch_target_position_;
};

#endif //OPENGL_IMGUI_SAMPLE_SHADOW_HPP
