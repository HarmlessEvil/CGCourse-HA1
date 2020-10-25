//
// Created by Александр Чори on 12.10.2020.
//

#include "terrain.hpp"

#include <cmath>
#include <vector>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

terrain::terrain(
        const image &height_map,
        bool normalize_coordinates,
        bool smooth_normals,
        coordinate_to_texture_level_mapper_t coordinate_to_texture_level_mapper
) : terrain(
        height_map,
        height_map.width(),
        height_map.height(),
        normalize_coordinates,
        smooth_normals,
        std::move(coordinate_to_texture_level_mapper)
) {}

terrain::terrain(
        const image &height_map,
        std::size_t width,
        std::size_t height,
        bool normalize_coordinates,
        bool smooth_normals,
        coordinate_to_texture_level_mapper_t coordinate_to_texture_level_mapper
) : rectangular_model(width, height),
    has_normalized_coordinates_(normalize_coordinates),
    height_ratio_(static_cast<float>(height_map.height()) / height),
    width_ratio_(static_cast<float>(height_map.width()) / width),
    coordinate_to_texture_level_mapper_(std::move(coordinate_to_texture_level_mapper)) {
    for (std::size_t i = 0; i < height; ++i) {
        float index_i = i * height_ratio_;

        float prev_i_buffer;
        float height_shift = std::modf(index_i, &prev_i_buffer);

        std::size_t prev_i = prev_i_buffer;
        std::size_t next_i = std::min(prev_i + 1, height_map.height() - 1);

        for (std::size_t j = 0; j < width; ++j) {
            float index_j = j * width_ratio_;

            float prev_j_buffer;
            float width_shift = std::modf(index_j, &prev_j_buffer);

            std::size_t prev_j = prev_j_buffer;
            std::size_t next_j = std::min(prev_j + 1, height_map.width() - 1);

            float nearest_heights[2][2] = {
                    {
                            static_cast<float>(height_map(prev_i, prev_j, 0)),
                            static_cast<float>(height_map(prev_i, next_j, 0))
                    },
                    {
                            static_cast<float>(height_map(next_i, prev_j, 0)),
                            static_cast<float>(height_map(next_i, next_j, 0))
                    }
            };

            if (has_normalized_coordinates_) {
                for (auto &item : nearest_heights) {
                    item[0] /= 255;
                    item[1] /= 255;
                }
            }

            texture_coordinates_[i][j] = {j / 15.0f, i / 15.0f, 0.0f};

            glm::vec3 coordinate;
            if (has_normalized_coordinates_) {
                coordinate = glm::vec3(static_cast<float>(j) / width, static_cast<float>(i) / height, 0);
            } else {
                coordinate = glm::vec3(j, i, 0);
            }
            coordinate.z = glm::mix(
                    glm::mix(nearest_heights[0][0], nearest_heights[0][1], width_shift),
                    glm::mix(nearest_heights[1][0], nearest_heights[1][1], width_shift),
                    height_shift
            );

            coordinates_[i][j] = coordinate;
        }
    }

    const glm::vec<2, char> triangle_vertex_shifts[2][3] = {
            {
                    {0, 0},
                    {1, 0},
                    {0, 1}
            },
            {
                    {0, 1},
                    {1, 0},
                    {1, 1}
            }
    };

    for (std::size_t i = 0; i < height - 1; ++i) {
        for (std::size_t j = 0; j < width - 1; ++j) {
            quad quad{};

            // Iterate over triangles of quad
            for (unsigned char shift = 0; shift < 2; ++shift) {
                rectangular_model::triangle_face triangle{};
                glm::vec3 vertices[3] = {};

                for (unsigned char vertex = 0; vertex < 3; ++vertex) {
                    std::size_t y = i + triangle_vertex_shifts[shift][vertex].y;
                    std::size_t x = j + triangle_vertex_shifts[shift][vertex].x;

                    triangle.indices[vertex] = {y, x};
                    vertices[vertex] = coordinates_[y][x];
                }

                triangle.normal = glm::normalize(glm::cross(
                        vertices[0] - vertices[1],
                        vertices[0] - vertices[2]
                ));

                quad[shift] = triangle;
            }

            quads_[i][j] = quad;
        }
    }

    if (smooth_normals) {
        this->smooth_normals();
        calculate_tangents();
    }

    map_coordinates_to_texture_levels();
}

void terrain::map_coordinates_to_texture_levels() {
    for (auto &quad_row : quads_) {
        for (auto &quad : quad_row) {
            for (auto &triangle : quad) {
                for (auto &index : triangle.indices) {
                    texture_coordinates_[index.first][index.second].z = coordinate_to_texture_level_mapper_(
                            coordinates_[index.first][index.second], normals_[index.first][index.second]
                    );
                }
            }
        }
    }
}

bool terrain::has_normalized_coordinates() const {
    return has_normalized_coordinates_;
}

glm::vec3 terrain::at(glm::vec2 const &position) const {
    return coordinates_[position.y][position.x] * scale_;
}

glm::vec3 terrain::normal_at(const glm::vec2 &position) const {
    return normals_[position.y][position.x];
}

glm::vec3 terrain::tangent_at(glm::vec2 const &position) const {
    return tangents_[position.y][position.x];
}

float terrain::scale() const {
    return scale_;
}

void terrain::set_scale(float scale) {
    scale_ = scale;
}