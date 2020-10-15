//
// Created by Александр Чори on 15.10.2020.
//

#include "player.hpp"

#include <utility>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace {
    glm::quat rotation_between_vectors(glm::vec3 start, glm::vec3 dest) {
        start = normalize(start);
        dest = normalize(dest);

        float cos_theta = dot(start, dest);
        glm::vec3 rotation_axis;

        if (cos_theta < -1 + 0.001f) {
            // special case when vectors in opposite directions:
            // there is no "ideal" rotation axis
            // So guess one; any will do as long as it's perpendicular to start
            rotation_axis = cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
            if (glm::length2(rotation_axis) < 0.01) // bad luck, they were parallel, try again!
                rotation_axis = cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

            rotation_axis = normalize(rotation_axis);
            return glm::angleAxis(glm::radians(180.0f), rotation_axis);
        }

        rotation_axis = cross(start, dest);

        float s = sqrt((1 + cos_theta) * 2);
        float inverted_s = 1 / s;

        return glm::quat(
                s * 0.5f,
                rotation_axis.x * inverted_s,
                rotation_axis.y * inverted_s,
                rotation_axis.z * inverted_s
        );

    }
}

player::player(
        std::shared_ptr<IObjModel> model,
        const shader_t &shader,
        std::shared_ptr<terrain> terrain,
        std::shared_ptr<third_person_camera> camera
) : model_(std::move(model)), shader_(shader), terrain_(std::move(terrain)), camera_(std::move(camera)) {

}

void player::draw() {
    glm::vec3 world_position = terrain_->at(position_);
    glm::vec3 normal = terrain_->normalAt(position_);
    camera_->set_target(world_position);
    camera_->set_up(normal);

    auto model = glm::translate(world_position)
                 * glm::toMat4(rotation_between_vectors(glm::vec3(0, 1, 0), normal))
                 * glm::scale(glm::vec3(7, 7, 7));
    auto mvp = camera_->get_vp() * model;

    shader_.use();
    shader_.set_uniform("u_mvp", glm::value_ptr(mvp));
    shader_.set_uniform("u_model", glm::value_ptr(model));

    auto light_dir = glm::vec3(1, 0, 0);
    shader_.set_uniform<float>("u_color", 0.83, 0.64, 0.31);
    shader_.set_uniform<float>("u_light", light_dir.x, light_dir.y, light_dir.z);
    model_->draw();
}

void player::move(const glm::vec2 &direction) {
    position_ += direction;

    if (position_.x >= terrain_->width()) {
        position_.x -= terrain_->width();
    } else if (position_.x < 0) {
        position_.x += terrain_->width();
    }

    if (position_.y >= terrain_->height()) {
        position_.y -= terrain_->height();
    } else if (position_.y < 0) {
        position_.y += terrain_->height();
    }
}

void player::set_position(const glm::vec2 &position) {
    position_ = position;
}
