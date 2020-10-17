//
// Created by Александр Чори on 17.10.2020.
//

#ifndef OPENGL_IMGUI_SAMPLE_LIGHT_SOURCE_HPP
#define OPENGL_IMGUI_SAMPLE_LIGHT_SOURCE_HPP

#include <glm/vec3.hpp>

class light_source {
public:
    [[nodiscard]] const glm::vec3 &color() const;

protected:
    explicit light_source(const glm::vec3 &color);

    glm::vec3 color_;
};

class ambient_light : public light_source {
public:
    ambient_light(glm::vec3 const &color, float intensity);

    [[nodiscard]] float intensity() const;

private:
    float intensity_ = 0.1;
};

class directional_light : public light_source {
public:
    directional_light(const glm::vec3 &direction, const glm::vec3 &color);

    [[nodiscard]] const glm::vec3 &direction() const;

private:
    glm::vec3 direction_;
};


#endif //OPENGL_IMGUI_SAMPLE_LIGHT_SOURCE_HPP
