//
// Created by Александр Чори on 11.10.2020.
//

#ifndef OPENGL_IMGUI_SAMPLE_MODEL_HPP
#define OPENGL_IMGUI_SAMPLE_MODEL_HPP

#include <array>
#include <utility>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class model {
public:
    struct triangle_face {
        std::array<std::pair<std::size_t, std::size_t>, 3> indices;
        glm::vec3 normal;
    };

    explicit model(
            std::vector<std::vector<glm::vec3>>  coordinates = {},
            std::vector<std::vector<glm::vec2>>  texture_coordinates = {},
            std::vector<triangle_face>  triangles = {}
    );

    [[nodiscard]] const std::vector<std::vector<glm::vec3>> &coordinates() const;

    [[nodiscard]] const std::vector<std::vector<glm::vec2>> &texture_coordinates() const;

    [[nodiscard]] const std::vector<triangle_face> &triangles() const;

protected:
    std::vector<std::vector<glm::vec3>> coordinates_;
    std::vector<std::vector<glm::vec2>> texture_coordinates_;
    std::vector<triangle_face> triangles_;
};

#endif //OPENGL_IMGUI_SAMPLE_MODEL_HPP
