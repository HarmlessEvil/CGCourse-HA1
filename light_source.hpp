//
// Created by Александр Чори on 17.10.2020.
//

#ifndef OPENGL_IMGUI_SAMPLE_LIGHT_SOURCE_HPP
#define OPENGL_IMGUI_SAMPLE_LIGHT_SOURCE_HPP

#include <memory>
#include <string>

#include <glm/vec3.hpp>

#include "opengl_shader.h"

class light_source {
public:
    [[nodiscard]] const glm::vec3 &ambient() const;

    [[nodiscard]] const glm::vec3 &diffuse() const;

    [[nodiscard]] const glm::vec3 &specular() const;

    virtual void to_shader(shader_t &shader, const std::string &uniform_name) const;

protected:
    light_source(glm::vec3 const &ambient, glm::vec3 const &diffuse, glm::vec3 const &specular);

    glm::vec3 ambient_;
    glm::vec3 diffuse_;
    glm::vec3 specular_;
};

class directional_light : public light_source {
public:
    directional_light(
            glm::vec3 const &direction,
            glm::vec3 const &ambient,
            glm::vec3 const &diffuse,
            glm::vec3 const &specular
    );

    [[nodiscard]] const glm::vec3 &direction() const;

    void to_shader(shader_t &shader, const std::string &uniform_name) const override;

private:
    glm::vec3 direction_;
};

class point_light : public light_source {
public:
    point_light(
            const glm::vec3 &position,
            const glm::vec3 &ambient,
            const glm::vec3 &diffuse,
            const glm::vec3 &specular,
            float constant,
            float linear,
            float quadratic
    );

    [[nodiscard]] const glm::vec3 &position() const;

    [[nodiscard]] float constant() const;

    [[nodiscard]] float linear() const;

    [[nodiscard]] float quadratic() const;

    void set_position(const glm::vec3 &position);

    void to_shader(shader_t &shader, const std::string &uniform_name) const override;

protected:
    glm::vec3 position_;

    float constant_;
    float linear_;
    float quadratic_;
};

class spotlight : public point_light {
public:
    spotlight(
            const glm::vec3 &position,
            const glm::vec3 &direction,
            const glm::vec3 &ambient,
            const glm::vec3 &diffuse,
            const glm::vec3 &specular,
            float constant,
            float linear,
            float quadratic,
            float cutOff,
            float outerCutOff
    );

    void set_direction(const glm::vec3 &direction);

    void to_shader(shader_t &shader, const std::string &uniform_name) const override;

private:
    glm::vec3 direction_;
    float cut_off_;
    float outer_cut_off_;
};

#endif //OPENGL_IMGUI_SAMPLE_LIGHT_SOURCE_HPP
