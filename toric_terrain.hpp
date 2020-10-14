//
// Created by Александр Чори on 14.10.2020.
//

#ifndef OPENGL_IMGUI_SAMPLE_TORIC_TERRAIN_HPP
#define OPENGL_IMGUI_SAMPLE_TORIC_TERRAIN_HPP

#include "terrain.hpp"

class toric_terrain : public terrain {
public:
    toric_terrain(terrain const &flat_terrain, float radius, float thickness);
};


#endif //OPENGL_IMGUI_SAMPLE_TORIC_TERRAIN_HPP
