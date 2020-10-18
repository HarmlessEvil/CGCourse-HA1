//
// Created by Александр Чори on 15.10.2020.
//

#include "player.hpp"

#include <utility>

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
        std::shared_ptr<directional_light> sun
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
    )) {

}

void player::draw() {
    glm::vec3 world_position = terrain_->at(position_);
    glm::vec3 normal = terrain_->normalAt(position_);
    camera_->set_target(world_position);
    camera_->set_up(normal);

    glm::vec2 camera_terrain_position = position_ - direction() * glm::vec2(camera_->shift().x, camera_->shift().y);
    glm::vec3 camera_ground_position = terrain_->at(camera_terrain_position);
    glm::vec3 camera_position = camera_ground_position + terrain_->normalAt(
            camera_terrain_position
    ) * camera_->shift().z;
    camera_->set_position(camera_position);
    flashlight_->set_direction(world_position - camera_ground_position);

    auto translation = glm::translate(world_position);
    auto rotation_y = glm::rotate(angle_, glm::vec3(0, 1, 0));
    auto rotation = glm::orientation(normal, glm::vec3(0, 1, 0));
    auto model = translation * rotation * rotation_y * glm::scale(glm::mat4(1), glm::vec3(7, 7, 7));
    auto mvp = camera_->get_vp() * model;

    shader_.use();
    shader_.set_uniform("u_mvp", glm::value_ptr(mvp));
    shader_.set_uniform("u_model", glm::value_ptr(model));

    shader_.set_uniform("u_camera_position", camera_position.x, camera_position.y, camera_position.z);

    sun_->to_shader(shader_, "u_sun");
    flashlight_->to_shader(shader_, "u_flashlight");

    model_->draw();
}

glm::vec2 player::direction() const {
    return glm::rotate(glm::vec2(1, 0), this->angle_);
}

void player::move(float speed, float angle) {
    angle_ += glm::radians(angle);

    glm::vec2 direction = glm::rotate(glm::vec2(1, 0), angle_);
    set_position(position_ + direction * speed);
}

void player::set_position(const glm::vec2 &position) {
    position_ = position;
    flashlight_->set_position(terrain_->at(position_));
}

const glm::vec2 &player::position() const {
    return position_;
}

glm::vec3 player::world_position() const {
    return terrain_->at(position_);
}

std::vector<std::pair<std::string, std::shared_ptr<light_source>>> player::light_casters() const {
    std::vector<std::pair<std::string, std::shared_ptr<light_source>>> lights{{"u_player_flashlight", flashlight_}};

    return lights;
}

glm::mat4 player::model() const {
    glm::vec3 world_position = terrain_->at(position_);
    glm::vec3 normal = terrain_->normalAt(position_);

    auto translation = glm::translate(world_position);
    auto rotation_y = glm::rotate(angle_, glm::vec3(0, 1, 0));
    auto rotation = glm::orientation(normal, glm::vec3(0, 1, 0));
    auto model = translation * rotation * rotation_y * glm::scale(glm::mat4(1), glm::vec3(7, 7, 7));

    return model;
}
