//
// Created by Александр Чори on 15.10.2020.
//

#ifndef OPENGL_IMGUI_SAMPLE_CAMERA_HPP
#define OPENGL_IMGUI_SAMPLE_CAMERA_HPP

#include <cstddef>
#include <memory>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class perspective_camera {
public:
    perspective_camera(float fov, float z_near, float z_far, float aspect);

    [[nodiscard]] glm::mat4 get_vp(glm::vec3 const &eye, glm::vec3 const &target) const;

    void set_fov(float fov);

    void set_z_near(float z_near);

    void set_z_far(float z_far);

    void set_aspect(float aspect);

    void set_up(const glm::vec3 &up);

protected:
    float fov_;
    float z_near_;
    float z_far_;
    float aspect_;

    glm::vec3 up_{0, 1, 0};
};

class third_person_camera : public perspective_camera {
public:
    third_person_camera(float fov, float z_near, float z_far, float aspect, glm::vec3 const &shift);

    [[nodiscard]] glm::mat4 get_vp();

    void set_target(const glm::vec3 &target);

    void set_position(const glm::vec3 &position);

    [[nodiscard]] glm::vec3 position();

    [[nodiscard]] const glm::vec3 &shift() const;

private:
    glm::vec3 current_position_{};
    glm::vec3 desired_position_{};
    glm::vec3 target_{};
    glm::vec3 shift_{};
};

#endif //OPENGL_IMGUI_SAMPLE_CAMERA_HPP
