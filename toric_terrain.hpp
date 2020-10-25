//
// Created by Александр Чори on 14.10.2020.
//

#ifndef OPENGL_IMGUI_SAMPLE_TORIC_TERRAIN_HPP
#define OPENGL_IMGUI_SAMPLE_TORIC_TERRAIN_HPP

#include "terrain.hpp"

#include <cmath>

#include <glm/glm.hpp>

class toric_terrain : public terrain {
public:
    toric_terrain(terrain const &flat_terrain, float radius, float thickness);

    [[nodiscard]] glm::vec3 at(const glm::vec2 &position) const override;

    [[nodiscard]] glm::vec3 normal_at(const glm::vec2 &position) const override;

    [[nodiscard]] glm::vec4 tangent_at(const glm::vec2 &position) const override;

private:
    template<typename F>
    auto get_with_interpolation(F get, glm::vec2 const &position) const {
        float prev_i;
        float i_shift = std::modf(position.x, &prev_i);
        float next_i = prev_i + 1;

        std::size_t int_prev_i = ((static_cast<long>(prev_i) % width_) + width_) % width_;
        std::size_t int_next_i = ((static_cast<long>(next_i) % width_) + width_) % width_;

        float prev_j;
        float j_shift = std::modf(position.y, &prev_j);
        float next_j = prev_j + 1;

        std::size_t int_prev_j = ((static_cast<long>(prev_j) % height_) + height_) % height_;
        std::size_t int_next_j = ((static_cast<long>(next_j) % height_) + height_) % height_;

        auto upper_coordinates = glm::mix(
                get(glm::vec2(int_prev_i, int_prev_j)),
                get(glm::vec2(int_prev_i, int_next_j)),
                j_shift
        );
        auto lower_coordinates = glm::mix(
                get(glm::vec2(int_next_i, int_prev_j)),
                get(glm::vec2(int_next_i, int_next_j)),
                j_shift
        );
        return glm::mix(upper_coordinates, lower_coordinates, i_shift);
    }
};


#endif //OPENGL_IMGUI_SAMPLE_TORIC_TERRAIN_HPP
