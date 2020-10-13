//
// Created by Александр Чори on 12.10.2020.
//

#ifndef OPENGL_IMGUI_SAMPLE_TERRAIN_HPP
#define OPENGL_IMGUI_SAMPLE_TERRAIN_HPP

#include "image.hpp"
#include "model.hpp"

class terrain : public model {
public:
    explicit terrain(image const &height_map);
};


#endif //OPENGL_IMGUI_SAMPLE_TERRAIN_HPP
