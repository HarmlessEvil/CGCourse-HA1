//
// Created by Александр Чори on 11.10.2020.
//

#include "image.hpp"

image::operator bool() const {
    return data_.operator bool();
}

unsigned char image::operator()(std::size_t i, std::size_t j, std::size_t channel) const {
    return (data_.get() + i * width_ * 4 + j * 4)[channel];
}

image::image(size_t width, size_t height, int channels, unsigned char *data, deleter_t const &deleter)
        : width_(width), height_(height), channels_(channels), data_(data, deleter) {}

std::size_t image::width() const {
    return width_;
}

std::size_t image::height() const {
    return height_;
}

unsigned char const *image::data() const {
    return data_.get();
}
