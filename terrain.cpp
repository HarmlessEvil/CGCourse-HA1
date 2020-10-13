//
// Created by Александр Чори on 12.10.2020.
//

#include "terrain.hpp"

#include <vector>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

terrain::terrain(const image &height_map)
        : model(
        std::vector<std::vector<glm::vec3>>(
                height_map.height(),
                std::vector<glm::vec3>(height_map.width())
        ),
        std::vector<std::vector<glm::vec2>>(
                height_map.height(),
                std::vector<glm::vec2>(height_map.width())
        )
) {
    for (std::size_t i = 0; i < height_map.height(); ++i) {
        for (std::size_t j = 0; j < height_map.width(); ++j) {
            coordinates_[i][j] = glm::vec3{j, i, height_map(i, j, 0)};
            texture_coordinates_[i][j] = glm::vec2{
                    static_cast<float>(j) / static_cast<float>(height_map.width()),
                    static_cast<float>(i) / static_cast<float>(height_map.height())
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

    for (std::size_t i = 0; i < height_map.height() - 1; ++i) {
        for (std::size_t j = 0; j < height_map.width() - 1; ++j) {
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
