//
// Created by Александр Чори on 15.10.2020.
//

#include "player.hpp"

#include <utility>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

player::player(std::shared_ptr<IObjModel> model, const shader_t &shader, std::shared_ptr<terrain> terrain)
        : model_(std::move(model)), shader_(shader), terrain_(std::move(terrain)) {

}

glm::mat4 player::draw(float display_w, float display_h) {
    glm::vec3 world_position = terrain_->at(position_);

    auto model = glm::translate(world_position) * glm::scale(glm::vec3(7, 7, 7));
    auto view = glm::lookAt<float>(world_position + camera_shift_, world_position, glm::vec3(0, 1, 0));
    auto projection = glm::perspective<float>(90, display_w / display_h, 0.1, 100);
    auto vp = projection * view;
    auto mvp = vp * model;

    shader_.use();
    shader_.set_uniform("u_mvp", glm::value_ptr(mvp));
    shader_.set_uniform("u_model", glm::value_ptr(model));

    auto light_dir = glm::vec3(1, 0, 0);
    shader_.set_uniform<float>("u_color", 0.83, 0.64, 0.31);
    shader_.set_uniform<float>("u_light", light_dir.x, light_dir.y, light_dir.z);
    model_->draw();

    return vp;
}

void player::set_position(const glm::vec2 &position) {
    position_ = position;
}

const glm::vec2 &player::position() const {
    return position_;
}
