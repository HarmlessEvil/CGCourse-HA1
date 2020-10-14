//
// Created by Александр Чори on 14.10.2020.
//

#include "toric_terrain.hpp"

#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

toric_terrain::toric_terrain(const terrain &flat_terrain, float radius, float thickness)
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
            float height_corrected_thickness = thickness + coordinates_[i][j].z * radius / 2;

            coordinates_[i][j] = {
                    (radius + height_corrected_thickness * std::cos(phi)) * std::cos(psi),
                    (radius + height_corrected_thickness * std::cos(phi)) * std::sin(psi),
                    height_corrected_thickness * std::sin(phi)
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

    for (std::size_t i = 0; i < width_ - 1; ++i) {
        quad quad{};

        quad[0].indices[0] = {height_ - 1, i};
        quad[0].indices[1] = {height_ - 1, i + 1};
        quad[0].indices[2] = {0, i};

        quad[1].indices[0] = {0, i};
        quad[1].indices[1] = {height_ - 1, i + 1};
        quad[1].indices[2] = {0, i + 1};

        quads_[i].push_back(quad);
    }

    for (auto &quad_row : quads_) {
        for (auto &quad : quad_row) {
            for (auto &triangle : quad) {
                triangle.normal = glm::normalize(glm::cross(
                        coordinates_[triangle.indices[0].first][triangle.indices[0].second] -
                        coordinates_[triangle.indices[1].first][triangle.indices[1].second],
                        coordinates_[triangle.indices[0].first][triangle.indices[0].second] -
                        coordinates_[triangle.indices[2].first][triangle.indices[2].second]
                ));
            }
        }
    }

    smooth_normals();
}
