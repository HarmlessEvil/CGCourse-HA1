//
// Created by Александр Чори on 11.10.2020.
//

#include "model.hpp"

#include <glm/glm.hpp>
#include <utility>

const std::vector<std::vector<glm::vec3>> &model::coordinates() const {
    return coordinates_;
}

const std::vector<std::vector<glm::vec2>> &model::texture_coordinates() const {
    return texture_coordinates_;
}

model::model(
        std::vector<std::vector<glm::vec3>> coordinates,
        std::vector<std::vector<glm::vec2>> texture_coordinates,
        std::vector<triangle_face> triangles
) : coordinates_(std::move(coordinates)),
    texture_coordinates_(std::move(texture_coordinates)),
    triangles_(std::move(triangles)) {

}

const std::vector<model::triangle_face> &model::triangles() const {
    return triangles_;
}
