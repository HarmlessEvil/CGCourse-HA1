//
// Created by Александр Чори on 02.10.2020.
//

#include "camera.hpp"

#include <glm/gtx/transform.hpp>

glm::vec3 camera::position() const {
    return position(glm::vec4(0, 0, _zoom, 0));
}

const glm::vec3 &camera::target() const {
    return _target;
}

void camera::rotate(glm::vec2 const &rotation) {
    _current_rotation = glm::clamp(_current_rotation + rotation, _rotation_constraints.first, _rotation_constraints.second);
}

const glm::vec3 &camera::up() const {
    return _up;
}

void camera::zoom_in(float delta) {
    _zoom = glm::clamp(_zoom / delta, _zoom_constraints.first, _zoom_constraints.second);
}

void camera::zoom_out(float delta) {
    _zoom = glm::clamp(_zoom * delta, _zoom_constraints.first, _zoom_constraints.second);
}

glm::vec3 camera::position_unscaled() const {
    return position(glm::vec4(0, 0, 1, 0));
}

glm::vec3 camera::position(glm::vec4 const &base_position) const {
    auto const rotation_matrix_y = glm::rotate(glm::mat4(1), _current_rotation.y, glm::vec3(0, 1, 0));
    auto const rotation_matrix_x = glm::vec3(rotation_matrix_y * glm::vec4(1, 0, 0, 1));

    auto result = rotation_matrix_y * base_position;
    return glm::rotate(glm::mat4(1), _current_rotation.x, rotation_matrix_x) * result;
}
