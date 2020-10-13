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

    using quad = std::array<triangle_face, 2>;

    model(std::size_t width, std::size_t height);

    [[nodiscard]] const std::vector<std::vector<glm::vec3>> &coordinates() const;

    [[nodiscard]] const std::vector<std::vector<glm::vec3>> &normals() const;

    [[nodiscard]] const std::vector<std::vector<glm::vec2>> &texture_coordinates() const;

    [[nodiscard]] const std::vector<std::vector<quad>> &quads() const;

    [[nodiscard]] std::size_t width() const;

    [[nodiscard]] std::size_t height() const;

    [[nodiscard]] std::size_t quads_count() const;

protected:
    std::vector<std::vector<glm::vec3>> coordinates_;
    std::vector<std::vector<glm::vec3>> normals_;
    std::vector<std::vector<glm::vec2>> texture_coordinates_;
    std::vector<std::vector<quad>> quads_;

    std::size_t width_;
    std::size_t height_;
};

#endif //OPENGL_IMGUI_SAMPLE_MODEL_HPP
