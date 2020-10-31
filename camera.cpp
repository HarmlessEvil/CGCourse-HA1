//
// Created by Александр Чори on 15.10.2020.
//

#include "camera.hpp"

#include <algorithm>
#include <iostream>

#include <glm/gtx/rotate_vector.hpp>

perspective_camera::perspective_camera(float fov, float z_near, float z_far, float ascpect)
        : fov_(fov), z_near_(z_near), z_far_(z_far), aspect_(ascpect) {}

glm::mat4 perspective_camera::get_vp(const glm::vec3 &eye, const glm::vec3 &target) const {
    return glm::perspective(fov_, aspect_, z_near_, z_far_) * glm::lookAt(eye, target, up_);
}

void perspective_camera::set_fov(float fov) {
    fov_ = fov;
}

void perspective_camera::set_z_near(float z_near) {
    z_near_ = z_near;
}

void perspective_camera::set_z_far(float z_far) {
    z_far_ = z_far;
}

void perspective_camera::set_aspect(float aspect) {
    aspect_ = aspect;
}

void perspective_camera::set_up(const glm::vec3 &up) {
    up_ = up;
}

third_person_camera::third_person_camera(float fov, float z_near, float z_far, float aspect, glm::vec3 const &shift)
        : perspective_camera(fov, z_near, z_far, aspect), shift_(shift) {

}

glm::mat4 third_person_camera::get_vp() {
    return perspective_camera::get_vp(position(), target_);
}

void third_person_camera::set_target(const glm::vec3 &target) {
    target_ = target;
}

glm::vec3 third_person_camera::position() {
    current_position_ = glm::mix(current_position_, desired_position_, 0.002f);
    return current_position_;
}

void third_person_camera::set_position(const glm::vec3 &position) {
    desired_position_ = position;
}

const glm::vec3 &third_person_camera::shift() const {
    return shift_;
}
