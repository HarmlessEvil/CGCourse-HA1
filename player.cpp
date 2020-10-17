//
// Created by Александр Чори on 15.10.2020.
//

#include "player.hpp"

#include <utility>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

player::player(
        std::shared_ptr<IObjModel> model,
        const shader_t &shader,
        std::shared_ptr<terrain> terrain,
        std::shared_ptr<third_person_camera> camera,
        std::shared_ptr<ambient_light> ambient,
        std::shared_ptr<directional_light> sun
) : model_(std::move(model)),
    shader_(shader),
    terrain_(std::move(terrain)),
    camera_(std::move(camera)),
    sun_(std::move(sun)),
    ambient_(std::move(ambient)) {

}

void player::draw() {
    glm::vec3 world_position = terrain_->at(position_);
    glm::vec3 normal = terrain_->normalAt(position_);
    camera_->set_target(world_position);
    camera_->set_up(normal);

    glm::vec2 camera_terrain_position = position_
                                        - glm::rotate(glm::vec2(1, 0), angle_) *
                                          glm::vec2(camera_->shift().x, camera_->shift().y);
    glm::vec3 camera_position = terrain_->at(camera_terrain_position);
    camera_position += terrain_->normalAt(camera_terrain_position) * camera_->shift().z;
    camera_->set_position(camera_position);

    auto translation = glm::translate(world_position);
    auto rotation_y = glm::rotate(angle_, glm::vec3(0, 1, 0));
    auto rotation = glm::orientation(normal, glm::vec3(0, 1, 0));
    auto model = translation * rotation * rotation_y * glm::scale(glm::mat4(1), glm::vec3(7, 7, 7));
    auto mvp = camera_->get_vp() * model;

    shader_.use();
    shader_.set_uniform("u_mvp", glm::value_ptr(mvp));
    shader_.set_uniform("u_model", glm::value_ptr(model));

    shader_.set_uniform("u_ambient_color", ambient_->color().x, ambient_->color().y, ambient_->color().z);
    shader_.set_uniform("u_ambient_intensity", ambient_->intensity());

    shader_.set_uniform<float>("u_directional_light_color", sun_->color().x, sun_->color().y, sun_->color().z);
    shader_.set_uniform<float>(
            "u_directional_light_direction",
            sun_->direction().x,
            sun_->direction().y,
            sun_->direction().z
    );
    model_->draw();
}

void player::move(float speed, float angle) {
    angle_ += glm::radians(angle);

    glm::vec2 direction = glm::rotate(glm::vec2(1, 0), angle_);
    position_ += direction * speed;
}

void player::set_position(const glm::vec2 &position) {
    position_ = position;
}

const glm::vec2 &player::position() const {
    return position_;
}

glm::vec3 player::world_position() const {
    return terrain_->at(position_);
}
