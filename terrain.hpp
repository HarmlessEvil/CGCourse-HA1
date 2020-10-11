//
// Created by Александр Чори on 11.10.2020.
//

#ifndef OPENGL_IMGUI_SAMPLE_TERRAIN_HPP
#define OPENGL_IMGUI_SAMPLE_TERRAIN_HPP

#include <utility>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "image.hpp"

class terrain {
public:
    explicit terrain(image const &height_map);

private:
    std::vector<std::vector<glm::vec3>> coordinates_;
    std::vector<std::pair<std::size_t, std::size_t>> indices_;
    std::vector<glm::vec3> normals_;
    std::vector<std::vector<glm::vec2>> texture_coordinates_;
};


#endif //OPENGL_IMGUI_SAMPLE_TERRAIN_HPP
