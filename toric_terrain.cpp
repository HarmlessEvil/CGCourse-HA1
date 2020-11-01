//
// Created by Александр Чори on 14.10.2020.
//

#include "toric_terrain.hpp"

#include <cmath>

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

        auto &vertexNW = coordinates_[quad[0].indices[0].first][quad[0].indices[0].second];
        auto &vertexSW = coordinates_[quad[0].indices[2].first][quad[0].indices[2].second];
        auto averageW = (vertexNW + vertexSW) / 2.0f;

        vertexNW = glm::mix(vertexNW, averageW, 0.99f);
        vertexSW = glm::mix(vertexSW, averageW, 0.99f);

        quad[1].indices[0] = {i + 1, width_ - 1};
        quad[1].indices[1] = {i, 0};
        quad[1].indices[2] = {i + 1, 0};

        auto &vertexNE = coordinates_[quad[1].indices[0].first][quad[1].indices[0].second];
        auto &vertexSE = coordinates_[quad[1].indices[2].first][quad[1].indices[2].second];
        auto averageE = (vertexNE + vertexSE) / 2.0f;

        vertexNE = glm::mix(vertexNE, averageE, 0.99f);
        vertexSE = glm::mix(vertexSE, averageE, 0.99f);

        quads_[i].push_back(quad);
    }

    quads_.emplace_back();
    for (std::size_t i = 0; i < width_ - 1; ++i) {
        quad quad{};

        quad[0].indices[0] = {height_ - 1, i};
        quad[0].indices[1] = {height_ - 1, i + 1};
        quad[0].indices[2] = {0, i};

        auto &vertexNW = coordinates_[quad[0].indices[0].first][quad[0].indices[0].second];
        auto &vertexNE = coordinates_[quad[0].indices[2].first][quad[0].indices[2].second];
        auto averageN = (vertexNE + vertexNW) / 2.0f;

        vertexNW = glm::mix(vertexNW, averageN, 0.99f);
        vertexNE = glm::mix(vertexNE, averageN, 0.99f);

        quad[1].indices[0] = {0, i};
        quad[1].indices[1] = {height_ - 1, i + 1};
        quad[1].indices[2] = {0, i + 1};

        auto &vertexSW = coordinates_[quad[1].indices[0].first][quad[1].indices[0].second];
        auto &vertexSE = coordinates_[quad[1].indices[2].first][quad[1].indices[2].second];
        auto averageS = (vertexSE + vertexSW) / 2.0f;

        vertexSW = glm::mix(vertexSW, averageS, 0.99f);
        vertexSE = glm::mix(vertexSE, averageS, 0.99f);

        quads_.back().push_back(quad);
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
    calculate_tangents();
}

glm::vec3 toric_terrain::at(const glm::vec2 &position) const {
    return get_with_interpolation([this](auto const &x) { return terrain::at(x); }, position);
}

glm::vec3 toric_terrain::normal_at(const glm::vec2 &position) const {
    return get_with_interpolation([this](auto const &x) { return terrain::normal_at(x); }, position);
}

glm::vec4 toric_terrain::tangent_at(const glm::vec2 &position) const {
    return get_with_interpolation([this](auto const &x) { return terrain::tangent_at(x); }, position);
}
