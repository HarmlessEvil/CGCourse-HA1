//
// Created by Александр Чори on 12.10.2020.
//

#ifndef OPENGL_IMGUI_SAMPLE_TERRAIN_HPP
#define OPENGL_IMGUI_SAMPLE_TERRAIN_HPP

#include <functional>

#include <glm/vec3.hpp>

#include "image.hpp"
#include "model.hpp"

class terrain : public model {
public:
    explicit terrain(
            image const &height_map,
            bool normalize_coordinates = false,
            std::function<float(glm::vec3 const &, glm::vec3 const &)> const &coordinate_to_texture_level_mapper = [](
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
            std::function<float(glm::vec3 const &, glm::vec3 const &)> const &coordinate_to_texture_level_mapper
    );
};


#endif //OPENGL_IMGUI_SAMPLE_TERRAIN_HPP
