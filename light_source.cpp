//
// Created by Александр Чори on 17.10.2020.
//

#include "light_source.hpp"

const glm::vec3 &directional_light::direction() const {
    return direction_;
}

directional_light::directional_light(
        const glm::vec3 &direction,
        const glm::vec3 &ambient,
        const glm::vec3 &diffuse,
        const glm::vec3 &specular
) : light_source(ambient, diffuse, specular), direction_(direction) {

}

void directional_light::to_shader(shader_t &shader, const std::string &uniform_name) const {
    shader.set_uniform(uniform_name + ".direction", direction_.x, direction_.y, direction_.z);

    light_source::to_shader(shader, uniform_name);
}

const glm::vec3 &light_source::ambient() const {
    return ambient_;
}

const glm::vec3 &light_source::diffuse() const {
    return diffuse_;
}

const glm::vec3 &light_source::specular() const {
    return specular_;
}

light_source::light_source(const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular)
        : ambient_(ambient), diffuse_(diffuse), specular_(specular) {

}

void light_source::to_shader(shader_t &shader, const std::string &uniform_name) const {
    shader.set_uniform(uniform_name + ".ambient", ambient_.x, ambient_.y, ambient_.z);
    shader.set_uniform(uniform_name + ".diffuse", diffuse_.x, diffuse_.y, diffuse_.z);
    shader.set_uniform(uniform_name + ".specular", specular_.x, specular_.y, specular_.z);
}

point_light::point_light(
        const glm::vec3 &position,
        const glm::vec3 &ambient,
        const glm::vec3 &diffuse,
        const glm::vec3 &specular,
        float constant,
        float linear,
        float quadratic
) : light_source(ambient, diffuse, specular),
    position_(position),
    constant_(constant),
    linear_(linear),
    quadratic_(quadratic) {

}

const glm::vec3 &point_light::position() const {
    return position_;
}

float point_light::constant() const {
    return constant_;
}

float point_light::linear() const {
    return linear_;
}

float point_light::quadratic() const {
    return quadratic_;
}

void point_light::set_position(const glm::vec3 &position) {
    position_ = position;
}

void point_light::to_shader(shader_t &shader, const std::string &uniform_name) const {
    shader.set_uniform(uniform_name + ".position", position_.x, position_.y, position_.z);
    shader.set_uniform(uniform_name + ".constant", constant_);
    shader.set_uniform(uniform_name + ".linear", linear_);
    shader.set_uniform(uniform_name + ".quadratic", quadratic_);

    light_source::to_shader(shader, uniform_name);
}

spotlight::spotlight(
        const glm::vec3 &position,
        const glm::vec3 &direction,
        const glm::vec3 &ambient,
        const glm::vec3 &diffuse,
        const glm::vec3 &specular,
        float constant,
        float linear,
        float quadratic,
        float cutOff,
        float outerCutOff
) : point_light(position, ambient, diffuse, specular, constant, linear, quadratic),
    direction_(direction),
    cut_off_(cutOff),
    outer_cut_off_(outerCutOff) {

}

void spotlight::set_direction(const glm::vec3 &direction) {
    direction_ = direction;
}

void spotlight::to_shader(shader_t &shader, const std::string &uniform_name) const {
    point_light::to_shader(shader, uniform_name);

    shader.set_uniform(uniform_name + ".direction", direction_.x, direction_.y, direction_.z);
    shader.set_uniform(uniform_name + ".cut_off", cut_off_);
    shader.set_uniform(uniform_name + ".outer_cut_off", outer_cut_off_);
}
