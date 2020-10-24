#include "opengl_shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include <fmt/format.h>

#ifndef __WIN32
#include <spdlog/spdlog.h>
#endif

namespace {
    std::string read_shader_code(const std::string &fname) {
        std::stringstream file_stream;
        std::ifstream file(fname.c_str());
        file_stream << file.rdbuf();
        return file_stream.str();
    }
}

shader_t::shader_t(const std::string &vertex_code_fname, const std::string &fragment_code_fname) {
#ifdef _WIN32
    std::cout << "Creating shader, vertex: " << vertex_code_fname << ", fragment: " << fragment_code_fname << std::endl;
#else
    spdlog::info("Creating shader, vertex: {}, fragment: {}", vertex_code_fname, fragment_code_fname);
#endif

    const auto vertex_code = read_shader_code(vertex_code_fname);
    const auto fragment_code = read_shader_code(fragment_code_fname);
    compile(vertex_code, fragment_code);
    link();
}

shader_t::~shader_t() {
}

void shader_t::compile(const std::string &vertex_code, const std::string &fragment_code) {
    const char *vcode = vertex_code.c_str();
    vertex_id_ = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_id_, 1, &vcode, NULL);
    glCompileShader(vertex_id_);

    const char *fcode = fragment_code.c_str();
    fragment_id_ = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_id_, 1, &fcode, NULL);
    glCompileShader(fragment_id_);
    check_compile_error();
}

void shader_t::link() {
    program_id_ = glCreateProgram();
    glAttachShader(program_id_, vertex_id_);
    glAttachShader(program_id_, fragment_id_);
    glLinkProgram(program_id_);
    check_linking_error();
    glDeleteShader(vertex_id_);
    glDeleteShader(fragment_id_);
}

void shader_t::use() {
    glUseProgram(program_id_);
}

template<>
void shader_t::set_uniform<int>(const std::string &name, int val) {
    glUniform1i(uniforms_[name], val);
}

template<>
void shader_t::set_uniform<bool>(const std::string &name, bool val) {
    glUniform1i(uniforms_[name], val);
}

template<>
void shader_t::set_uniform<float>(const std::string &name, float val) {
    glUniform1f(uniforms_[name], val);
}

template<>
void shader_t::set_uniform<float>(const std::string &name, float val1, float val2) {
    glUniform2f(uniforms_[name], val1, val2);
}

template<>
void shader_t::set_uniform<float>(const std::string &name, float val1, float val2, float val3) {
    glUniform3f(uniforms_[name], val1, val2, val3);
}

template<>
void shader_t::set_uniform<float *>(const std::string &name, float *val) {
    glUniformMatrix4fv(uniforms_[name], 1, GL_FALSE, val);
}

template<>
void shader_t::set_uniform<const float *>(const std::string &name, const float *val) {
    glUniformMatrix4fv(uniforms_[name], 1, GL_FALSE, val);
}

void shader_t::check_compile_error() {
    int success;
    char infoLog[1024];
    glGetShaderiv(vertex_id_, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_id_, 1024, NULL, infoLog);
        throw std::runtime_error(fmt::format("Error compiling Vertex shader: {}", infoLog));
    }
    glGetShaderiv(fragment_id_, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_id_, 1024, NULL, infoLog);
        throw std::runtime_error(fmt::format("Error compiling Fragment shader: {}", infoLog));
    }
}

void shader_t::check_linking_error() {
    int success;
    char infoLog[1024];
    glGetProgramiv(program_id_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program_id_, 1024, NULL, infoLog);
        throw std::runtime_error(fmt::format("Error Linking Program: {}", infoLog));
    }
}

shader_t::shader_t(
        const std::string &vertex_code_fname,
        const std::string &fragment_code_fname,
        const std::vector<std::string> &uniforms
) : shader_t(vertex_code_fname, fragment_code_fname) {
    for (const auto &uniform : uniforms) {
        uniforms_[uniform] = glGetUniformLocation(program_id_, uniform.c_str());
    }
}
