//
// Created by Александр Чори on 11.10.2020.
//

#ifndef OPENGL_IMGUI_SAMPLE_IMAGE_HPP
#define OPENGL_IMGUI_SAMPLE_IMAGE_HPP

#include <cstddef>
#include <functional>
#include <memory>

class image {
private:
    using deleter_t = std::function<void(unsigned char*)>;

public:
    image(size_t width, size_t height, int channels, unsigned char *data, deleter_t const &deleter);

    explicit operator bool() const;

    unsigned char operator()(std::size_t i, std::size_t j, std::size_t channel) const;

    [[nodiscard]] std::size_t width() const;

    [[nodiscard]] std::size_t height() const;

    [[nodiscard]] unsigned char const *data() const;

private:
    std::size_t width_{};
    std::size_t height_{};
    int channels_{};

    std::unique_ptr<unsigned char, deleter_t> data_;
};

#endif //OPENGL_IMGUI_SAMPLE_IMAGE_HPP
