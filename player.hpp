//
// Created by Александр Чори on 15.10.2020.
//

#ifndef OPENGL_IMGUI_SAMPLE_PLAYER_HPP
#define OPENGL_IMGUI_SAMPLE_PLAYER_HPP

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include "camera.hpp"
#include "light_source.hpp"
#include "obj_model.h"
#include "opengl_shader.h"
#include "shadow.hpp"
#include "terrain.hpp"

class player {
public:
    player(
            std::shared_ptr<IObjModel> model,
            const shader_t &shader,
            std::shared_ptr<terrain> terrain,
            std::shared_ptr<third_person_camera> camera,
            std::shared_ptr<directional_light> sun,
            std::shared_ptr<std::vector<std::shared_ptr<shadow>>> shadow_casters
    );

    void draw();

    void move(float speed, float angle);

    void set_position(const glm::vec2 &position);

    [[nodiscard]] const glm::vec2 &position() const;

    [[nodiscard]] glm::vec3 world_position() const;

    [[nodiscard]] glm::vec2 direction() const;

    [[nodiscard]] std::vector<std::pair<std::string, std::shared_ptr<light_source>>> light_casters() const;

    [[nodiscard]] glm::mat4 model() const;

private:
    void update();

    glm::mat4 model_matrix_{};

    std::shared_ptr<IObjModel> model_;
    shader_t shader_;
    std::shared_ptr<terrain> terrain_;

    glm::vec2 position_{};
    glm::vec3 world_position_{};
    glm::vec3 normal_{};
    std::shared_ptr<third_person_camera> camera_;

    float angle_{};

    std::shared_ptr<directional_light> sun_;
    std::shared_ptr<spotlight> flashlight_;

    std::shared_ptr<std::vector<std::shared_ptr<shadow>>> shadow_casters_;
};


#endif //OPENGL_IMGUI_SAMPLE_PLAYER_HPP
