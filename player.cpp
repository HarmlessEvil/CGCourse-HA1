//
// Created by Александр Чори on 15.10.2020.
//

#include "player.hpp"

#include <sstream>
#include <utility>

#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/vec3.hpp>

player::player(
        std::shared_ptr<IObjModel> model,
        const shader_t &shader,
        std::shared_ptr<terrain> terrain,
        std::shared_ptr<third_person_camera> camera,
        std::shared_ptr<directional_light> sun,
        std::shared_ptr<std::vector<std::shared_ptr<shadow>>> shadow_casters
) : model_(std::move(model)),
    shader_(shader),
    terrain_(std::move(terrain)),
    camera_(std::move(camera)),
    sun_(std::move(sun)),
    flashlight_(std::make_shared<spotlight>(
            glm::vec3(),
            glm::vec3(-1, 0, 0),
            glm::vec3(),
            glm::vec3(1),
            glm::vec3(1),
            1,
            0.03,
            0.016,
            glm::cos(glm::radians(12.5)),
            glm::cos(glm::radians(17.5))
    )),
    shadow_casters_(std::move(shadow_casters)) {

}

void player::draw() {
    shader_.use();
    shader_.set_uniform("u_mvp", glm::value_ptr(camera_->get_vp() * model()));
    shader_.set_uniform("u_model", glm::value_ptr(model()));
    shader_.set_uniform("u_directional_light_shadow_map", int(1));

    for (std::size_t i = 0; i < shadow_casters_->size(); ++i) {
        shader_.set_uniform(
                fmt::format("u_light_space_matrices[{}]", i),
                glm::value_ptr((*shadow_casters_)[i]->light_space_matrix())
        );
    }

    shader_.set_uniform("u_camera_position", camera_->position().x, camera_->position().y, camera_->position().z);

    sun_->to_shader(shader_, "u_sun");
    flashlight_->to_shader(shader_, "u_flashlight");

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadow_casters_->front()->render_target().depth_);

    model_->draw();

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

glm::vec2 player::direction() const {
    return glm::rotate(glm::vec2(1, 0), this->angle_);
}

void player::move(float speed, float angle) {
    angle_ += glm::radians(angle);

    glm::vec2 direction = glm::rotate(glm::vec2(1, 0), angle_);
    set_position(position_ + direction * speed);

    update();
}

void player::set_position(const glm::vec2 &position) {
    position_ = position;
    flashlight_->set_position(terrain_->at(position_));

    update();
}

const glm::vec2 &player::position() const {
    return position_;
}

glm::vec3 player::world_position() const {
    return world_position_;
}

std::vector<std::pair<std::string, std::shared_ptr<light_source>>> player::light_casters() const {
    std::vector<std::pair<std::string, std::shared_ptr<light_source>>> lights{{"u_player_flashlight", flashlight_}};

    return lights;
}

glm::mat4 player::model() const {
    return model_matrix_;
}

void player::update() {
    world_position_ = terrain_->at(position_);
    normal_ = terrain_->normal_at(position_);
    camera_->set_target(world_position_);
    camera_->set_up(normal_);

    glm::vec2 camera_terrain_position = position_ - direction() * glm::vec2(camera_->shift().x, camera_->shift().y);
    glm::vec3 camera_ground_position = terrain_->at(camera_terrain_position);
    glm::vec3 camera_position = camera_ground_position + terrain_->normal_at(
            camera_terrain_position
    ) * camera_->shift().z;
    camera_->set_position(camera_position);
    flashlight_->set_direction(world_position_ - camera_ground_position);

    auto translation = glm::translate(world_position_);
    auto rotation_y = glm::rotate(angle_, glm::vec3(0, 1, 0));
    auto rotation = glm::orientation(normal_, glm::vec3(0, 1, 0));
    auto model = translation * rotation * rotation_y * glm::scale(glm::mat4(1), glm::vec3(7, 7, 7));

    model_matrix_ = model;
}
