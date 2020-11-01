//
// Created by Александр Чори on 11.10.2020.
//

#include "rectangular_model.hpp"

#include <glm/glm.hpp>

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
          tangents_(height, std::vector<glm::vec4>(width)),
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

void rectangular_model::smooth_normals() {
    assert(!quads_.empty());

    std::vector<std::vector<char>> used(quads_.size(), std::vector<char>(quads_[0].size(), false));
    const glm::vec<2, char> quad_shifts[4][4] = {
            {
                    {-1, -1},
                    {0, -1},
                    {-1, 0},
                    {0, 0}
            },
            {
                    {0,  -1},
                    {1, -1},
                    {0,  0},
                    {1, 0}
            },
            {
                    {-1, 0},
                    {0, 0},
                    {-1, 1},
                    {0, 1}
            },
            {
                    {0,  0},
                    {1, 0},
                    {0,  1},
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

                    if (shift_i < 0 || shift_j < 0 || shift_i >= quads_.size() - 1 || shift_j >= quads_row.size() - 1) {
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

// Lengyel, Eric. “Computing Tangent Space Basis Vectors for an Arbitrary Mesh”.
// https://www.cs.upc.edu/~virtual/G/1.%20Teoria/06.%20Textures/Tangent%20Space%20Calculation.pdf
void rectangular_model::calculate_tangents() {
    std::vector<std::vector<glm::vec3>> tangents(height_, std::vector<glm::vec3>(width_));
    std::vector<std::vector<glm::vec3>> bi_tangents(height_, std::vector<glm::vec3>(width_));

    for (const auto &quad_row : quads_) {
        for (const auto &quad : quad_row) {
            for (const auto &triangle : quad) {
                auto index_1 = triangle.indices[0];
                auto index_2 = triangle.indices[1];
                auto index_3 = triangle.indices[2];

                glm::vec3 const &v1 = coordinates_[index_1.first][index_1.second];
                glm::vec3 const &v2 = coordinates_[index_2.first][index_2.second];
                glm::vec3 const &v3 = coordinates_[index_3.first][index_3.second];
                glm::vec2 const &w1 = texture_coordinates_[index_1.first][index_1.second];
                glm::vec2 const &w2 = texture_coordinates_[index_2.first][index_2.second];
                glm::vec2 const &w3 = texture_coordinates_[index_3.first][index_3.second];

                float x1 = v2.x - v1.x;
                float x2 = v3.x - v1.x;
                float y1 = v2.y - v1.y;
                float y2 = v3.y - v1.y;
                float z1 = v2.z - v1.z;
                float z2 = v3.z - v1.z;

                float s1 = w2.x - w1.x;
                float s2 = w3.x - w1.x;
                float t1 = w2.y - w1.y;
                float t2 = w3.y - w1.y;

                float r = 1.0f / (s1 * t2 - s2 * t1);
                glm::vec3 s_direction{
                        (t2 * x1 - t1 * x2) * r,
                        (t2 * y1 - t1 * y2) * r,
                        (t2 * z1 - t1 * z2) * r
                };
                glm::vec3 t_direction{
                        (s1 * x2 - s2 * x1) * r,
                        (s1 * y2 - s2 * y1) * r,
                        (s1 * z2 - s2 * z1) * r
                };

                tangents[index_1.first][index_1.second] += s_direction;
                tangents[index_2.first][index_2.second] += s_direction;
                tangents[index_3.first][index_3.second] += s_direction;

                bi_tangents[index_1.first][index_1.second] += t_direction;
                bi_tangents[index_2.first][index_2.second] += t_direction;
                bi_tangents[index_3.first][index_3.second] += t_direction;
            }
        }
    }

    for (std::size_t i = 0; i < tangents.size(); ++i) {
        for (std::size_t j = 0; j < tangents[i].size(); ++j) {
            glm::vec3 const &normal = normals_[i][j];
            glm::vec3 const &tangent = tangents[i][j];

            tangents_[i][j] = glm::vec4(glm::normalize(tangent - normal * glm::dot(normal, tangent)), 0);
            tangents_[i][j].w = (glm::dot(glm::cross(normal, tangent), bi_tangents[i][j]) < 0) ? -1 : 1;
        }
    }
}
