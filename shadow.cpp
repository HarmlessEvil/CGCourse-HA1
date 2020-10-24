//
// Created by Александр Чори on 21.10.2020.
//

#include "shadow.hpp"

#include <exception>
#include <utility>

#include <glm/gtx/transform.hpp>

shadow::shadow(const render_target_t &render_target) : render_target_(render_target) {

}

const render_target_t &shadow::render_target() const {
    return render_target_;
}

glm::mat4 directional_light_shadow::light_space_matrix() const {
    auto target = fetch_target_position_();
    glm::mat4 light_view = glm::lookAt(target - direction_ * distance_, target, glm::vec3(0, 1, 0));

    return light_projection_ * light_view;
}

directional_light_shadow::directional_light_shadow(
        const render_target_t &render_target,
        const glm::vec3 &direction,
        const glm::mat4 &light_projection,
        float distance,
        std::function<glm::vec3()> fetch_target_position
) : shadow(render_target),
    direction_(direction),
    light_projection_(light_projection),
    distance_(distance),
    fetch_target_position_(std::move(fetch_target_position)) {

}
