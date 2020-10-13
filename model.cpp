//
// Created by Александр Чори on 11.10.2020.
//

#include "model.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

const std::vector<std::vector<glm::vec3>> &model::coordinates() const {
    return coordinates_;
}

const std::vector<std::vector<glm::vec3>> &model::texture_coordinates() const {
    return texture_coordinates_;
}

const std::vector<std::vector<model::quad>> &model::quads() const {
    return quads_;
}

model::model(std::size_t width, std::size_t height)
        : width_(width),
          height_(height),
          coordinates_(height, std::vector<glm::vec3>(width)),
          normals_(height, std::vector<glm::vec3>(width)),
          texture_coordinates_(height, std::vector<glm::vec3>(width)),
          quads_(height - 1, std::vector<quad>(width - 1)) {

}

std::size_t model::width() const {
    return width_;
}

std::size_t model::height() const {
    return height_;
}

std::size_t model::quads_count() const {
    return width_ * height_;
}

const std::vector<std::vector<glm::vec3>> &model::normals() const {
    return normals_;
}
