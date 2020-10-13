//
// Created by Александр Чори on 12.10.2020.
//

#include "terrain.hpp"

#include <cmath>
#include <vector>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

terrain::terrain(const image &height_map) : terrain(height_map, height_map.width(), height_map.height()) {}

terrain::terrain(const image &height_map, std::size_t width, std::size_t height) : model(
        std::vector<std::vector<glm::vec3>>(height, std::vector<glm::vec3>(width)),
        std::vector<std::vector<glm::vec2>>(height, std::vector<glm::vec2>(width))
) {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Width or height should be greater than 0");
    }

    const float height_ratio = static_cast<float>(height_map.height()) / height;
    const float width_ratio = static_cast<float>(height_map.width()) / width;

    for (std::size_t i = 0; i < height; ++i) {
        float index_i = i * height_ratio;

        float prev_i_buffer;
        float height_shift = std::modf(index_i, &prev_i_buffer);

        std::size_t prev_i = prev_i_buffer;
        std::size_t next_i = std::min(prev_i + 1, height_map.height() - 1);

        for (std::size_t j = 0; j < width; ++j) {
            float index_j = j * width_ratio;

            float prev_j_buffer;
            float width_shift = std::modf(index_j, &prev_j_buffer);

            std::size_t prev_j = prev_j_buffer;
            std::size_t next_j = std::min(prev_j + 1, height_map.width() - 1);

            unsigned char nearest_heights[2][2] = {
                    {
                            height_map(prev_i, prev_j, 0),
                            height_map(prev_i, next_j, 0)
                    },
                    {
                            height_map(next_i, prev_j, 0),
                            height_map(next_i, next_j, 0)
                    }
            };

            coordinates_[i][j] = glm::vec3{
                    j,
                    i,
                    glm::mix(
                            glm::mix(nearest_heights[0][0], nearest_heights[0][1], width_shift),
                            glm::mix(nearest_heights[1][0], nearest_heights[1][1], width_shift),
                            height_shift
                    )
            };
            texture_coordinates_[i][j] = glm::vec2{
                    static_cast<float>(j) / width,
                    static_cast<float>(i) / height
            };
        }
    }

    glm::vec<2, char> triangle_vertex_shifts[2][3] = {
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
            // Iterate over triangles of quad
            for (const auto &triangle_vertex_shift : triangle_vertex_shifts) {
                model::triangle_face triangle{};
                glm::vec3 vertices[3] = {};

                for (unsigned char vertex = 0; vertex < 3; ++vertex) {
                    std::size_t y = i + triangle_vertex_shift[vertex].y;
                    std::size_t x = j + triangle_vertex_shift[vertex].x;

                    triangle.indices[vertex] = {y, x};
                    vertices[vertex] = coordinates_[y][x];
                }

                triangle.normal = glm::normalize(glm::cross(
                        vertices[0] - vertices[1],
                        vertices[0] - vertices[2]
                ));

                triangles_.push_back(triangle);
            }
        }
    }
}
