//
// Created by Александр Чори on 02.10.2020.
//

#ifndef OPENGL_IMGUI_SAMPLE_CAMERA_HPP
#define OPENGL_IMGUI_SAMPLE_CAMERA_HPP


#include <ostream>
#include <utility>

#include <glm/glm.hpp>

class camera {
public:
    [[nodiscard]] const glm::vec3 &up() const;

    [[nodiscard]] glm::vec3 position() const;

    [[nodiscard]] glm::vec3 position_unscaled() const;

    [[nodiscard]] const glm::vec3 &target() const;

    void rotate(glm::vec2 const &rotation);

    void zoom_in(float delta);

    void zoom_out(float delta);

private:
    float _zoom = 1.0f;
    std::pair<float, float> _zoom_constraints{0.01f, 100.0f};

    glm::vec2 _current_rotation{0.0f, 0.0f};
    std::pair<glm::vec2, glm::vec2> _rotation_constraints{
            {glm::radians(-89.9f), glm::radians(-180.0f)},
            {glm::radians(89.9f),  glm::radians(180.0f)}
    };

    const glm::vec3 _target{0.0f, 0.0f, 0.0f};
    const glm::vec3 _up{0.0f, 1.0f, 0.0f};

    [[nodiscard]] glm::vec3 position(glm::vec4 const &base_position) const;
};


#endif //OPENGL_IMGUI_SAMPLE_CAMERA_HPP
