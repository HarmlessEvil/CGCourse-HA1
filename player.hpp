//
// Created by Александр Чори on 15.10.2020.
//

#ifndef OPENGL_IMGUI_SAMPLE_PLAYER_HPP
#define OPENGL_IMGUI_SAMPLE_PLAYER_HPP

#include <memory>

#include <glm/glm.hpp>

#include "camera.hpp"
#include "obj_model.h"
#include "opengl_shader.h"
#include "terrain.hpp"

class player {
public:
    player(
            std::shared_ptr<IObjModel> model,
            const shader_t &shader,
            std::shared_ptr<terrain> terrain,
            std::shared_ptr<third_person_camera> camera
    );

    void draw();

    void move(float speed, float angle);

    void set_position(const glm::vec2 &position);

    [[nodiscard]] const glm::vec2 &position() const;

    [[nodiscard]] glm::vec3 world_position() const;

private:
    std::shared_ptr<IObjModel> model_;
    shader_t shader_;
    std::shared_ptr<terrain> terrain_;

    glm::vec2 position_{};
    std::shared_ptr<third_person_camera> camera_;

    float angle_{};
};


#endif //OPENGL_IMGUI_SAMPLE_PLAYER_HPP
