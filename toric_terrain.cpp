//
// Created by Александр Чори on 14.10.2020.
//

#include "toric_terrain.hpp"

#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

toric_terrain::toric_terrain(const terrain &flat_terrain, std::size_t radius, std::size_t thickness)
        : terrain(flat_terrain) {
    if (!flat_terrain.has_normalized_coordinates()) {
        throw std::logic_error("Terrain should have normalized coordinates");
    }

    const float step_v = 2 * glm::pi<float>() / (height_ - 1.0f);
    const float step_h = 2 * glm::pi<float>() / (width_ - 1.0f);

    for (std::size_t i = 0; i < height_; ++i) {
        float phi = step_v * i;

        for (std::size_t j = 0; j < width_; ++j) {
            float psi = -glm::pi<float>() + step_h * j;

            coordinates_[i][j] = {
                    (radius + thickness * std::cos(phi)) * std::cos(psi),
                    (radius + thickness * std::cos(phi)) * std::sin(psi),
                    (thickness + coordinates_[i][j].z * radius) * std::sin(phi)
            };
        }
    }

    for (std::size_t i = 0; i < height_ - 1; ++i) {
        quad quad{};

        quad[0].indices[0] = {i, width_ - 1};
        quad[0].indices[1] = {i, 0};
        quad[0].indices[2] = {i + 1, width_ - 1};

        quad[1].indices[0] = {i + 1, width_ - 1};
        quad[1].indices[1] = {i, 0};
        quad[1].indices[2] = {i + 1, 0};

        quads_[i].push_back(quad);
    }
}
