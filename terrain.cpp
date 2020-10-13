//
// Created by Александр Чори on 12.10.2020.
//

#include "terrain.hpp"

#include <cmath>
#include <vector>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

terrain::terrain(const image &height_map) : terrain(height_map, height_map.width(), height_map.height()) {}

terrain::terrain(const image &height_map, std::size_t width, std::size_t height) : model(width, height) {
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
                model::triangle_face triangle{};
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

    std::vector<std::vector<char>> used(height, std::vector<char>(width, false));
    const glm::vec<2, char> quad_shifts[4][4] = {
            {
                    {-1, -1},
                    {0, -1},
                    {-1, 0},
                    {0, 0}
            },
            {
                    {0, -1},
                    {1, -1},
                    {0, 0},
                    {1, 0}
            },
            {
                    {-1, 0},
                    {0, 0},
                    {-1, 1},
                    {0, 1}
            },
            {
                    {0, 0},
                    {1, 0},
                    {0, 1},
                    {1, 1}
            }
    };

    for (long i = 0; i < quads_.size(); ++i) {
        auto quads_row = quads_[i];

        for (long j = 0; j < quads_row.size(); ++j) {
            auto quad = quads_row[j];
            std::pair<std::size_t, std::size_t> vertices[4] = {
                    quad[0].indices[0],
                    quad[0].indices[1],
                    quad[0].indices[2],
                    quad[1].indices[2]
            };

            for (unsigned char k = 0; k < 4; ++k) {
                auto index = vertices[k];
                if (used[index.first][index.second]) {
                    continue;
                }

                used[index.first][index.second] = true;

                glm::vec3 sum_of_normals{};
                float count{};

                for (unsigned char l = 0; l < 4; ++l) {
                    long shift_i = i + quad_shifts[k][l].y;
                    long shift_j = j + quad_shifts[k][l].x;

                    if (shift_i < 0 || shift_j < 0 || shift_i >= height - 1 || shift_j >= width - 1) {
                        continue;
                    }

                    auto current_quad = quads_[shift_i][shift_j];
                    if (l != 0) {
                        sum_of_normals += current_quad[0].normal;
                        ++count;
                    }
                    if (l != 3) {
                        sum_of_normals += current_quad[1].normal;
                        ++count;
                    }
                }

                normals_[index.first][index.second] = sum_of_normals / count;
            }
        }
    }
}
