//
// Created by Александр Чори on 17.10.2020.
//

#include "light_source.hpp"

directional_light::directional_light(const glm::vec3 &direction, const glm::vec3 &color)
        : light_source(color), direction_(direction) {}

const glm::vec3 &directional_light::direction() const {
    return direction_;
}

const glm::vec3 &light_source::color() const {
    return color_;
}

light_source::light_source(const glm::vec3 &color) : color_(color) {}

ambient_light::ambient_light(const glm::vec3 &color, float intensity) : light_source(color), intensity_(intensity) {

}

float ambient_light::intensity() const {
    return intensity_;
}
