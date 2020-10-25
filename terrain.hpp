//
// Created by Александр Чори on 12.10.2020.
//

#ifndef OPENGL_IMGUI_SAMPLE_TERRAIN_HPP
#define OPENGL_IMGUI_SAMPLE_TERRAIN_HPP

#include <functional>

#include <glm/vec3.hpp>

#include "image.hpp"
#include "rectangular_model.hpp"

class terrain : public rectangular_model {
public:
    using coordinate_to_texture_level_mapper_t = std::function<float(glm::vec3 const &, glm::vec3 const &)>;

    explicit terrain(
            image const &height_map,
            bool normalize_coordinates = false,
            bool smooth_normals = true,
            coordinate_to_texture_level_mapper_t coordinate_to_texture_level_mapper = [](
                    glm::vec3 const &coordinate,
                    glm::vec3 const &normal
            ) {
                return 0;
            }
    );

    terrain(
            image const &height_map,
            std::size_t width,
            std::size_t height,
            bool normalize_coordinates,
            bool smooth_normals,
            coordinate_to_texture_level_mapper_t coordinate_to_texture_level_mapper
    );

    [[nodiscard]] bool has_normalized_coordinates() const;

    [[nodiscard]] virtual glm::vec3 at(glm::vec2 const &position) const;

    [[nodiscard]] virtual glm::vec3 normal_at(glm::vec2 const &position) const;

    [[nodiscard]] virtual glm::vec4 tangent_at(glm::vec2 const &position) const;

    [[nodiscard]] float scale() const;

    void set_scale(float scale);

protected:
    const float height_ratio_;
    const float width_ratio_;

    coordinate_to_texture_level_mapper_t coordinate_to_texture_level_mapper_;

    void map_coordinates_to_texture_levels();

    float scale_ = 1.0f;

private:
    bool has_normalized_coordinates_ = false;
};

#endif //OPENGL_IMGUI_SAMPLE_TERRAIN_HPP
