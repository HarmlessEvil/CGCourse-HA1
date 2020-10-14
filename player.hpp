//
// Created by Александр Чори on 15.10.2020.
//

#ifndef OPENGL_IMGUI_SAMPLE_PLAYER_HPP
#define OPENGL_IMGUI_SAMPLE_PLAYER_HPP

#include <memory>

#include <glm/glm.hpp>

#include "obj_model.h"
#include "opengl_shader.h"
#include "terrain.hpp"

class player {
public:
    player(std::shared_ptr<IObjModel> model, const shader_t &shader, std::shared_ptr<terrain> terrain_);

    glm::mat4 draw(float display_w, float display_h);

    [[nodiscard]] const glm::vec2 &position() const;

    void set_position(const glm::vec2 &position);

private:
    std::shared_ptr<IObjModel> model_;
    shader_t shader_;
    std::shared_ptr<terrain> terrain_;

    glm::vec2 position_{};

public:
    glm::vec3 camera_shift_{1.5, 1.8, 0};
};


#endif //OPENGL_IMGUI_SAMPLE_PLAYER_HPP
