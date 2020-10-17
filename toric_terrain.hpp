//
// Created by Александр Чори on 14.10.2020.
//

#ifndef OPENGL_IMGUI_SAMPLE_TORIC_TERRAIN_HPP
#define OPENGL_IMGUI_SAMPLE_TORIC_TERRAIN_HPP

#include "terrain.hpp"

class toric_terrain : public terrain {
public:
    toric_terrain(terrain const &flat_terrain, float radius, float thickness);

    [[nodiscard]] glm::vec3 at(const glm::vec2 &position) const override;

    [[nodiscard]] glm::vec3 normalAt(const glm::vec2 &position) const override;
};


#endif //OPENGL_IMGUI_SAMPLE_TORIC_TERRAIN_HPP
