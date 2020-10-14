//
// Created by Александр Чори on 11.10.2020.
//

#include "rectangular_model.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

const std::vector<std::vector<glm::vec3>> &rectangular_model::coordinates() const {
    return coordinates_;
}

const std::vector<std::vector<glm::vec3>> &rectangular_model::texture_coordinates() const {
    return texture_coordinates_;
}

const std::vector<std::vector<rectangular_model::quad>> &rectangular_model::quads() const {
    return quads_;
}

rectangular_model::rectangular_model(std::size_t width, std::size_t height)
        : width_(width),
          height_(height),
          coordinates_(height, std::vector<glm::vec3>(width)),
          normals_(height, std::vector<glm::vec3>(width)),
          texture_coordinates_(height, std::vector<glm::vec3>(width)),
          quads_(height - 1, std::vector<quad>(width - 1)) {

}

std::size_t rectangular_model::width() const {
    return width_;
}

std::size_t rectangular_model::height() const {
    return height_;
}

std::size_t rectangular_model::quads_count() const {
    assert(!quads_.empty());

    return quads_.size() * quads_[0].size();
}

const std::vector<std::vector<glm::vec3>> &rectangular_model::normals() const {
    return normals_;
}
