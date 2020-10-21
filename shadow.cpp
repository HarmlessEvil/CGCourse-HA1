//
// Created by Александр Чори on 21.10.2020.
//

#include "shadow.hpp"

#include <utility>

shadow::shadow(std::shared_ptr<render_target_t> render_target, std::shared_ptr<glm::mat4> light_space_matrix)
        : render_target_(std::move(render_target)), light_space_matrix_(std::move(light_space_matrix)) {

}

std::shared_ptr<render_target_t> shadow::render_target() const {
    return render_target_;
}

std::shared_ptr<glm::mat4> shadow::light_space_matrix() const {
    return light_space_matrix_;
}
